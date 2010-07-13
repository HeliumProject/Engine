///////////////////////////////////////////////////////////////////////////////
// Name:        TreeWndCtrl.cpp
// Purpose:     TreeWndCtrl implementation.
// Author:      Giac Veltri (while at Insomniac Games)
// Created:     03/23/03
// Modified by:
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "TreeWndCtrl.h"

using namespace Nocturnal;

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------

/* XPM */
static char* s_treeWndCtrlExpandedXpm[] = {
"9 9 5 1",
"X c #000000",
". c #ffffff",
"- c #808080",
"| c #808080",
"+ c #808080",

"+-------+",
"|.......|",
"|.......|",
"|.......|",
"|.XXXXX.|",
"|.......|",
"|.......|",
"|.......|",
"+-------+",
};

/* XPM */
static char* s_treeWndCtrlCollapsedXpm[] = {
"9 9 5 1",
"X c #000000",
". c #ffffff",
"- c #808080",
"| c #808080",
"+ c #808080",

"+-------+",
"|.......|",
"|...X...|",
"|...X...|",
"|.XXXXX.|",
"|...X...|",
"|...X...|",
"|.......|",
"+-------+",
};


const wxTreeItemId Nocturnal::TreeWndCtrlItemIdInvalid(NULL);
wxBitmap Nocturnal::TreeWndCtrlDefaultExpand( s_treeWndCtrlExpandedXpm );
wxBitmap Nocturnal::TreeWndCtrlDefaultCollapse( s_treeWndCtrlCollapsedXpm );
wxPen Nocturnal::TreeWndCtrlDefaultPen( wxColour( 0x80, 0x80, 0x80 ), 1, wxSOLID );

// ----------------------------------------------------------------------------
// TreeWndCtrl
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(TreeWndCtrl, wxScrolledWindow)

TreeWndCtrl::TreeWndCtrl(wxWindow *parent,
                         wxWindowID winid,
                         const wxPoint &pos,
                         const wxSize &size,
                         long style,
                         const wxString &name,
                         int treeStyle,
                         unsigned int columnSize,
                         wxBitmap expandedBitmap,
                         wxBitmap collapsedBitmap,
                         wxPen pen,
                         unsigned int clickTolerance)
                         : wxScrolledWindow(parent, winid, pos, size, style, name),
                         m_columnSize(columnSize),
                         m_expandedBitmap(expandedBitmap),
                         m_collapsedBitmap(collapsedBitmap),
                         m_pen(pen),
                         m_clickTolerance(clickTolerance),
                         m_dashMode(wxTWC_DASH_DEFAULT),
                         m_lineMode(wxTWC_LINE_CENTER),
                         m_lineDrawMode(wxTWC_LINEDRAW_ALL),
                         m_toggleMode(wxTWC_TOGGLE_AUTOMATIC),
                         m_imageListNormal(NULL),
                         m_ownsImageListNormal(false),
                         m_imageListState(NULL),
                         m_ownsImageListState(false),
                         m_dirty(true),
                         m_root(TreeWndCtrlItemIdInvalid)
{
    if ( treeStyle & wxTR_USE_PEN_DASHES )
        m_dashMode = wxTWC_DASH_CUSTOM;

    if ( treeStyle & wxTR_LINES_TOP )
        m_lineMode = wxTWC_LINE_TOP;
    else if ( treeStyle & wxTR_LINES_BOTTOM )
        m_lineMode = wxTWC_LINE_BOTTOM;
    else if ( treeStyle & wxTR_LINES_CENTER )
        m_lineMode = wxTWC_LINE_CENTER;

    if ( treeStyle & wxTR_NO_LINES )
        m_lineDrawMode = wxTWC_LINEDRAW_NONE;
    else if ( treeStyle & wxTR_LINES_AT_ROOT )
        m_lineDrawMode = wxTWC_LINEDRAW_ROOT_ONLY;
    else if ( treeStyle & wxTWC_LINEDRAW_ALL )
        m_lineDrawMode = wxTWC_LINEDRAW_ALL;

    if ( treeStyle & wxTR_NO_BUTTONS )
        m_toggleMode = wxTWC_TOGGLE_DISABLED;

    m_hideRoot = ( treeStyle & wxTR_HIDE_ROOT ) ? true : false;

    SetSizer(new wxBoxSizer(wxVERTICAL));
}

TreeWndCtrl::~TreeWndCtrl()
{
    DeleteAllItems();

    if (m_ownsImageListNormal)
        delete m_imageListNormal;

    if (m_ownsImageListState)
        delete m_imageListState;
}

bool TreeWndCtrl::Layout()
{
    bool returnVal = false;
    wxSizer* sizer = GetSizer();

    if ( m_dirty )
    {
        Freeze();

        sizer->Clear();
        if ( m_root != TreeWndCtrlItemIdInvalid )
            AddNodeToLayout(sizer, m_root);

        returnVal = __super::Layout();
        AdjustTreeScrollbars();
        Thaw();
        m_dirty = false;
    }
    else
    {
        returnVal =  __super::Layout();
    }

    return returnVal;
}

size_t TreeWndCtrl::GetCount() const
{
    if ( m_root == TreeWndCtrlItemIdInvalid )
        return 0;

    return GetChildrenCount(m_root, true) + 1;
}

void TreeWndCtrl::SetDashMode(TreeWndCtrlDashMode dashMode)
{
    if ( m_dashMode == dashMode )
        return;

    m_dashMode = dashMode;
    Refresh();
}

void TreeWndCtrl::SetLineMode(TreeWndCtrlLineMode lineMode)
{
    if ( m_lineMode == lineMode )
        return;

    m_lineMode = lineMode;
    Refresh();
}

void TreeWndCtrl::SetLineDrawMode(TreeWndCtrlLineDrawMode lineDrawMode)
{
    if ( m_lineDrawMode == lineDrawMode )
        return;

    m_lineDrawMode = lineDrawMode;
    Refresh();
}

void TreeWndCtrl::SetToggleMode(TreeWndCtrlToggleMode toggleMode)
{
    if ( m_toggleMode == toggleMode )
        return;

    m_toggleMode = toggleMode;
    Refresh();
}

void TreeWndCtrl::SetColumnSize(unsigned int columnSize)
{
    if ( m_columnSize == columnSize )
        return;

    m_columnSize = columnSize;
    Refresh();
}

void TreeWndCtrl::SetHideRoot(bool hideRoot)
{
    if ( m_hideRoot == hideRoot )
        return;

    m_hideRoot = hideRoot;

    if ( m_root == TreeWndCtrlItemIdInvalid )
        return;

    UpdateNumColumns(m_root, m_hideRoot ? 0 : 1);

    Layout();
    Refresh();
}

wxTreeItemId TreeWndCtrl::AddRoot(wxWindow *window,
                                  wxTreeItemData *data)
{
    wxCHECK_MSG((m_root == TreeWndCtrlItemIdInvalid), m_root, wxT("The TreeWndCtrl can only have one root!"));
    wxCHECK_MSG((window->GetParent() == this), TreeWndCtrlItemIdInvalid, wxT("The window must be parented to the TreeWndCtrl object!"));

    TreeWndCtrlNode *root = new TreeWndCtrlNode(this, TreeWndCtrlItemIdInvalid, window, m_hideRoot ? 0 : 1, data);

    m_dirty = true;
    m_root = wxTreeItemId(root);
    return m_root;
}

wxTreeItemId TreeWndCtrl::AddRoot(const wxString& text,
                                  int image,
                                  int selImage,
                                  wxTreeItemData *data)
{
    wxCHECK_MSG((m_root == TreeWndCtrlItemIdInvalid), m_root, wxT("The TreeWndCtrl can only have one root!"));

    TreeWndCtrlItem* window = CreateTreeWndCtrlItem(text, image, selImage);
    wxTreeItemId item = AddRoot(window, data);
    window->SetItem(item);

    return item;
}

wxWindow* TreeWndCtrl::GetItemWindow(const wxTreeItemId &item) const
{
    wxCHECK_MSG((item != TreeWndCtrlItemIdInvalid), NULL, wxT("Invalid item!"));
    TreeWndCtrlNode *node = (TreeWndCtrlNode *)item.m_pItem;
    return node->GetWindow();
}

wxTreeItemId TreeWndCtrl::GetItemParent(const wxTreeItemId& item) const
{
    wxCHECK_MSG((item != TreeWndCtrlItemIdInvalid), TreeWndCtrlItemIdInvalid, wxT("Invalid item!"));

    TreeWndCtrlNode *node = (TreeWndCtrlNode *)item.m_pItem;
    return node->GetParent();
}

wxTreeItemId TreeWndCtrl::GetFirstChild(const wxTreeItemId& item) const
{
    wxCHECK_MSG((item != TreeWndCtrlItemIdInvalid), TreeWndCtrlItemIdInvalid, wxT("Invalid item!"));

    TreeWndCtrlNode *node = (TreeWndCtrlNode *)item.m_pItem;
    if ( node->m_children.GetCount() <= 0 )
        return TreeWndCtrlItemIdInvalid;

    return node->m_children.Item(0);
}

wxTreeItemId TreeWndCtrl::GetNextChild(const wxTreeItemId& item,
                                       const wxTreeItemId& lastChild) const
{
    wxCHECK_MSG((item != TreeWndCtrlItemIdInvalid), TreeWndCtrlItemIdInvalid, wxT("Invalid item!"));

    TreeWndCtrlNode *node = (TreeWndCtrlNode *)item.m_pItem;

    int index = node->m_children.Index(lastChild.m_pItem);
    if ( index == wxNOT_FOUND )
        return TreeWndCtrlItemIdInvalid;

    ++index;
    if ( (size_t) index >= node->m_children.GetCount() )
        return TreeWndCtrlItemIdInvalid;

    return node->m_children.Item(index);
}

wxTreeItemId TreeWndCtrl::GetPrevChild(const wxTreeItemId& item,
                                       const wxTreeItemId& lastChild) const
{
    wxCHECK_MSG((item != TreeWndCtrlItemIdInvalid), TreeWndCtrlItemIdInvalid, wxT("Invalid item!"));

    TreeWndCtrlNode *node = (TreeWndCtrlNode *)item.m_pItem;

    int index = node->m_children.Index(lastChild.m_pItem);
    if ( index == wxNOT_FOUND )
        return TreeWndCtrlItemIdInvalid;

    --index;
    if ( index < 0 )
        return TreeWndCtrlItemIdInvalid;

    return node->m_children.Item(index);
}

wxTreeItemId TreeWndCtrl::GetLastChild(const wxTreeItemId& item) const
{
    wxCHECK_MSG((item != TreeWndCtrlItemIdInvalid), TreeWndCtrlItemIdInvalid, wxT("Invalid item!"));

    TreeWndCtrlNode *node = (TreeWndCtrlNode *)item.m_pItem;
    size_t count = node->m_children.GetCount();
    if ( count <= 0 )
        return TreeWndCtrlItemIdInvalid;

    return node->m_children.Item(count - 1);
}

wxTreeItemId TreeWndCtrl::GetNextSibling(const wxTreeItemId& item) const
{
    wxCHECK_MSG((item != TreeWndCtrlItemIdInvalid), TreeWndCtrlItemIdInvalid, wxT("Invalid item!"));

    TreeWndCtrlNode *node = (TreeWndCtrlNode *)item.m_pItem;
    return GetNextChild(node->m_parent, item);
}

wxTreeItemId TreeWndCtrl::GetPrevSibling(const wxTreeItemId& item) const
{
    wxCHECK_MSG((item != TreeWndCtrlItemIdInvalid), TreeWndCtrlItemIdInvalid, wxT("Invalid item!"));

    TreeWndCtrlNode *node = (TreeWndCtrlNode *)item.m_pItem;
    return GetPrevChild(node->m_parent, item);
}

wxTreeItemId TreeWndCtrl::GetFirstVisibleItem()
{
    if ( m_root == TreeWndCtrlItemIdInvalid )
        return TreeWndCtrlItemIdInvalid;

    wxArrayTreeItemIds visibleItemList;
    GetVisibleItems(visibleItemList, m_root);

    if ( visibleItemList.GetCount() <= 0 )
        return TreeWndCtrlItemIdInvalid;

    return visibleItemList.Item(0);
}

wxTreeItemId TreeWndCtrl::GetNextVisible(const wxTreeItemId& item)
{
    if ( m_root == TreeWndCtrlItemIdInvalid )
        return TreeWndCtrlItemIdInvalid;

    wxArrayTreeItemIds visibleItemList;
    GetVisibleItems(visibleItemList, m_root);

    int index = visibleItemList.Index(item.m_pItem);
    if ( index == wxNOT_FOUND )
        return TreeWndCtrlItemIdInvalid;

    ++index;
    if ( (size_t) index >= visibleItemList.GetCount() )
        return TreeWndCtrlItemIdInvalid;

    return visibleItemList.Item(index);
}

wxTreeItemId TreeWndCtrl::GetPrevVisible(const wxTreeItemId& item)
{
    if ( m_root == TreeWndCtrlItemIdInvalid )
        return TreeWndCtrlItemIdInvalid;

    wxArrayTreeItemIds visibleItemList;
    GetVisibleItems(visibleItemList, m_root);

    int index = visibleItemList.Index(item.m_pItem);
    if ( index == wxNOT_FOUND )
        return TreeWndCtrlItemIdInvalid;

    --index;
    if ( index < 0 )
        return TreeWndCtrlItemIdInvalid;

    return visibleItemList.Item(index);
}

size_t TreeWndCtrl::GetChildrenCount(const wxTreeItemId& item,
                                     bool recursively) const
{
    wxCHECK_MSG((item != TreeWndCtrlItemIdInvalid), 0, wxT("Invalid item!"));

    TreeWndCtrlNode *node = (TreeWndCtrlNode *)item.m_pItem;
    size_t childrenCount = node->m_children.GetCount();

    if ( recursively )
    {
        size_t count = childrenCount;
        for ( size_t i = 0; i < count; ++i )
            childrenCount += GetChildrenCount(node->m_children.Item(i), true);
    }

    return childrenCount;
}

wxBitmap TreeWndCtrl::GetToggleBitmap(const wxTreeItemId& item)
{
    return IsExpanded(item) ? m_expandedBitmap : m_collapsedBitmap;
}

bool TreeWndCtrl::IsToggleVisible(const wxTreeItemId& item)
{
    switch ( m_toggleMode )
    {
    case wxTWC_TOGGLE_DISABLED:
        return false;

    case wxTWC_TOGGLE_ENABLED:
        return true;

    case wxTWC_TOGGLE_AUTOMATIC:
    default:
        return GetChildrenCount(item, false) > 0 ? true : false;
    }
}

void TreeWndCtrl::Delete(const wxTreeItemId& item)
{
    wxASSERT_MSG((item != TreeWndCtrlItemIdInvalid), wxT("Invalid item!"));

    wxTreeEvent deleteEvent(wxEVT_COMMAND_TREE_DELETE_ITEM);
    deleteEvent.SetItem(item);
    ProcessEvent( deleteEvent );

    DeleteChildren(item);

    TreeWndCtrlNode *node = (TreeWndCtrlNode *)item.m_pItem;
    wxTreeItemId parent = node->GetParent();
    if ( parent != TreeWndCtrlItemIdInvalid )
    {
        TreeWndCtrlNode *parentNode = (TreeWndCtrlNode *)parent.m_pItem;
        int index = parentNode->m_children.Index(item.m_pItem);
        if ( index != wxNOT_FOUND )
            parentNode->m_children.RemoveAt(index);
    }

    if ( item == m_lastToggledItem )
        m_lastToggledItem = TreeWndCtrlItemIdInvalid;

    if ( item == m_root )
        m_root = TreeWndCtrlItemIdInvalid;

    wxWindow* window = node->GetWindow();
    if ( m_allocatedWindows.Find(window) != NULL )
    {
        m_allocatedWindows.DeleteObject(window);
        RemoveChild(window);
        delete window;
    }

    wxWindow* spacer = node->GetSpacer();
    RemoveChild(spacer);
    delete spacer;

    m_dirty = true;
    delete node;
}

void TreeWndCtrl::DeleteChildren(const wxTreeItemId& item)
{
    wxASSERT_MSG((item != TreeWndCtrlItemIdInvalid), wxT("Invalid item!"));

    TreeWndCtrlNode *node = (TreeWndCtrlNode *)item.m_pItem;
    while ( node->m_children.GetCount() > 0 )
    {
        Delete(node->m_children.Item(0));
    }
}

void TreeWndCtrl::DeleteAllItems()
{
    if ( m_root == TreeWndCtrlItemIdInvalid )
        return;

    Delete(m_root);
}

int TreeWndCtrl::GetItemImage(const wxTreeItemId& item, wxTreeItemIcon which) const
{
    wxASSERT_MSG((item != TreeWndCtrlItemIdInvalid), wxT("Invalid item!"));
    TreeWndCtrlNode *node = (TreeWndCtrlNode *)item.m_pItem;

    TreeWndCtrlItem* control = wxDynamicCast(node->GetWindow(), TreeWndCtrlItem);
    if ( control == NULL )
        return -1;

    switch ( which )
    {
    case wxTreeItemIcon_Normal:
        return control->GetImage();

    default:
        return control->GetStateImage();
    }
}

wxTreeItemId TreeWndCtrl::FindItem(const wxWindow* window) const
{
    if ( m_root == TreeWndCtrlItemIdInvalid )
        return TreeWndCtrlItemIdInvalid;

    return FindItem(m_root, window);
}

wxTreeItemId TreeWndCtrl::FindItem(const wxTreeItemId &item, const wxWindow* window) const
{
    wxASSERT_MSG((item != TreeWndCtrlItemIdInvalid), wxT("Invalid item!"));
    TreeWndCtrlNode *node = (TreeWndCtrlNode *)item.m_pItem;

    if ( node->GetWindow() == window )
        return item;

    size_t count = node->m_children.GetCount();
    for ( size_t i = 0; i < count; ++i )
    {
        wxTreeItemId childItem = FindItem(node->m_children.Item(i), window);
        if ( childItem != TreeWndCtrlItemIdInvalid )
            return childItem;
    }

    return TreeWndCtrlItemIdInvalid;
}

wxString TreeWndCtrl::GetItemText(const wxTreeItemId& item) const
{
    wxASSERT_MSG((item != TreeWndCtrlItemIdInvalid), wxT("Invalid item!"));
    TreeWndCtrlNode *node = (TreeWndCtrlNode *)item.m_pItem;

    TreeWndCtrlItem* control = wxDynamicCast(node->GetWindow(), TreeWndCtrlItem);
    if ( control == NULL )
        return wxEmptyString;

    return control->GetText();
}

wxTreeItemData *TreeWndCtrl::GetItemData(const wxTreeItemId& item) const
{
    wxASSERT_MSG((item != TreeWndCtrlItemIdInvalid), wxT("Invalid item!"));
    TreeWndCtrlNode *node = (TreeWndCtrlNode *)item.m_pItem;

    return (node->GetData());
}

wxColour TreeWndCtrl::GetItemTextColour(const wxTreeItemId& item) const
{
    wxASSERT_MSG((item != TreeWndCtrlItemIdInvalid), wxT("Invalid item!"));
    TreeWndCtrlNode *node = (TreeWndCtrlNode *)item.m_pItem;

    TreeWndCtrlItem* control = wxDynamicCast(node->GetWindow(), TreeWndCtrlItem);
    if ( control == NULL )
        return wxNullColour;

    return control->GetForegroundColour();
}

wxColour TreeWndCtrl::GetItemBackgroundColour(const wxTreeItemId& item) const
{
    wxASSERT_MSG((item != TreeWndCtrlItemIdInvalid), wxT("Invalid item!"));
    TreeWndCtrlNode *node = (TreeWndCtrlNode *)item.m_pItem;

    TreeWndCtrlItem* control = wxDynamicCast(node->GetWindow(), TreeWndCtrlItem);
    if ( control == NULL )
        return wxNullColour;

    return control->GetBackgroundColour();
}

wxFont TreeWndCtrl::GetItemFont(const wxTreeItemId& item) const
{
    wxASSERT_MSG((item != TreeWndCtrlItemIdInvalid), wxT("Invalid item!"));
    TreeWndCtrlNode *node = (TreeWndCtrlNode *)item.m_pItem;

    TreeWndCtrlItem* control = wxDynamicCast(node->GetWindow(), TreeWndCtrlItem);
    if ( control == NULL )
        return wxNullFont;

    return control->GetFont();
}

void TreeWndCtrl::SetItemText(const wxTreeItemId& item, const wxString& text)
{
    wxASSERT_MSG((item != TreeWndCtrlItemIdInvalid), wxT("Invalid item!"));
    TreeWndCtrlNode *node = (TreeWndCtrlNode *)item.m_pItem;

    TreeWndCtrlItem* control = wxDynamicCast(node->GetWindow(), TreeWndCtrlItem);
    if ( control )
        control->SetText(text);
}

void TreeWndCtrl::SetItemImage(const wxTreeItemId& item, int image, wxTreeItemIcon which)
{
    wxASSERT_MSG((item != TreeWndCtrlItemIdInvalid), wxT("Invalid item!"));
    TreeWndCtrlNode *node = (TreeWndCtrlNode *)item.m_pItem;

    TreeWndCtrlItem* control = wxDynamicCast(node->GetWindow(), TreeWndCtrlItem);
    if ( control )
    {
        switch ( which )
        {
        case wxTreeItemIcon_Normal:
            return control->SetImage(image);

        default:
            return control->SetStateImage(image);
        }
    }
}

void TreeWndCtrl::SetItemData(const wxTreeItemId& item, wxTreeItemData *data)
{
    wxASSERT_MSG((item != TreeWndCtrlItemIdInvalid), wxT("Invalid item!"));
    TreeWndCtrlNode *node = (TreeWndCtrlNode *)item.m_pItem;

    node->SetData(data);
}

void TreeWndCtrl::SetItemTextColour(const wxTreeItemId& item, const wxColour& col)
{
    wxASSERT_MSG((item != TreeWndCtrlItemIdInvalid), wxT("Invalid item!"));
    TreeWndCtrlNode *node = (TreeWndCtrlNode *)item.m_pItem;

    TreeWndCtrlItem* control = wxDynamicCast(node->GetWindow(), TreeWndCtrlItem);
    if ( control  )
        control->SetForegroundColour(col);
}

void TreeWndCtrl::SetItemBackgroundColour(const wxTreeItemId& item, const wxColour& col)
{
    wxASSERT_MSG((item != TreeWndCtrlItemIdInvalid), wxT("Invalid item!"));
    TreeWndCtrlNode *node = (TreeWndCtrlNode *)item.m_pItem;

    TreeWndCtrlItem* control = wxDynamicCast(node->GetWindow(), TreeWndCtrlItem);
    if ( control  )
        control->SetBackgroundColour(col);
}

void TreeWndCtrl::SetItemFont(const wxTreeItemId& item, const wxFont& font)
{
    wxASSERT_MSG((item != TreeWndCtrlItemIdInvalid), wxT("Invalid item!"));
    TreeWndCtrlNode *node = (TreeWndCtrlNode *)item.m_pItem;

    TreeWndCtrlItem* control = wxDynamicCast(node->GetWindow(), TreeWndCtrlItem);
    if ( control  )
        control->SetFont(font);
}

bool TreeWndCtrl::IsExpanded(const wxTreeItemId& item) const
{
    wxCHECK_MSG((item != TreeWndCtrlItemIdInvalid), false, wxT("Invalid item!"));

    TreeWndCtrlNode *node = (TreeWndCtrlNode *)item.m_pItem;
    return node->m_expanded;
}

bool TreeWndCtrl::IsVisible(const wxTreeItemId& item) const
{
    if ( item == TreeWndCtrlItemIdInvalid )
        return false;

    wxTreeItemId currentItem = GetItemParent(item);
    while ( currentItem != TreeWndCtrlItemIdInvalid )
    {
        if ( !IsExpanded(currentItem) )
            return false;

        currentItem = GetItemParent(currentItem);
    }

    int scrollSizeX = 0;
    int scrollSizeY = 0;
    GetScrollPixelsPerUnit(&scrollSizeX, &scrollSizeY);

    int viewStartX = 0;
    int viewStartY = 0;
    GetViewStart(&viewStartX, &viewStartY);

    wxSize windowSize = GetSize();
    wxRect scrollRect(viewStartX * scrollSizeX, viewStartY * scrollSizeY, windowSize.GetWidth(), windowSize.GetHeight());
    wxRect visibleRect = GetItemBounds(item);

    if ( visibleRect.GetRight() < scrollRect.GetLeft() )
        return false;

    if ( visibleRect.GetLeft() > scrollRect.GetRight() )
        return false;

    if ( visibleRect.GetBottom() < scrollRect.GetTop() )
        return false;

    if ( visibleRect.GetTop() > scrollRect.GetBottom() )
        return false;

    return true;
}

void TreeWndCtrl::Expand(const wxTreeItemId& item)
{
    wxASSERT_MSG((item != TreeWndCtrlItemIdInvalid), wxT("Invalid item!"));
    TreeWndCtrlNode *node = (TreeWndCtrlNode *)item.m_pItem;

    wxTreeEvent expandEvent(wxEVT_COMMAND_TREE_ITEM_EXPANDING);
    expandEvent.SetItem(item);

    if ( ProcessEvent( expandEvent ) && !expandEvent.IsAllowed() )
        return;

    m_lastToggledItem = item;
    node->m_expanded = true;

    m_dirty = true;
    Layout();

    expandEvent.SetEventType(wxEVT_COMMAND_TREE_ITEM_EXPANDED);
    ProcessEvent( expandEvent );
}

void TreeWndCtrl::ExpandAllChildren(const wxTreeItemId& item)
{
    wxASSERT_MSG((item != TreeWndCtrlItemIdInvalid), wxT("Invalid item!"));

    TreeWndCtrlNode *node = (TreeWndCtrlNode *)item.m_pItem;
    size_t count = node->m_children.GetCount();
    for ( size_t i = 0; i < count; ++i )
    {
        wxTreeItemId child = node->m_children.Item(i);
        Expand(child);
        ExpandAllChildren(child);
    }
}

void TreeWndCtrl::ExpandAll()
{
    if (m_root == TreeWndCtrlItemIdInvalid)
        return;

    ExpandAllChildren(m_root);
    Expand(m_root);
}

void TreeWndCtrl::Collapse(const wxTreeItemId& item)
{
    wxASSERT_MSG((item != TreeWndCtrlItemIdInvalid), wxT("Invalid item!"));
    TreeWndCtrlNode *node = (TreeWndCtrlNode *)item.m_pItem;

    wxTreeEvent expandEvent(wxEVT_COMMAND_TREE_ITEM_COLLAPSING);
    expandEvent.SetItem(item);

    if ( ProcessEvent( expandEvent ) && !expandEvent.IsAllowed() )
        return;

    m_lastToggledItem = item;
    node->m_expanded = false;

    m_dirty = true;
    Layout();

    expandEvent.SetEventType(wxEVT_COMMAND_TREE_ITEM_COLLAPSED);
    ProcessEvent( expandEvent );
}

void TreeWndCtrl::CollapseAllChildren(const wxTreeItemId& item)
{
    wxASSERT_MSG((item != TreeWndCtrlItemIdInvalid), wxT("Invalid item!"));

    TreeWndCtrlNode *node = (TreeWndCtrlNode *)item.m_pItem;
    size_t count = node->m_children.GetCount();
    for ( size_t i = 0; i < count; ++i )
    {
        wxTreeItemId child = node->m_children.Item(i);
        Collapse(child);
        CollapseAllChildren(child);
    }
}

void TreeWndCtrl::CollapseAll()
{
    if (m_root == TreeWndCtrlItemIdInvalid)
        return;

    CollapseAllChildren(m_root);
    Collapse(m_root);
}

void TreeWndCtrl::Toggle(const wxTreeItemId& item)
{
    wxASSERT_MSG((item != TreeWndCtrlItemIdInvalid), wxT("Invalid item!"));

    SetExpanded(item, !IsExpanded(item));
}

void TreeWndCtrl::SetExpanded(const wxTreeItemId& item, bool expanded)
{
    wxASSERT_MSG((item != TreeWndCtrlItemIdInvalid), wxT("Invalid item!"));

    if ( expanded )
        Expand(item);
    else
        Collapse(item);
}

void TreeWndCtrl::SetImageList(wxImageList *imageList)
{
    if (m_ownsImageListNormal)
        delete m_imageListNormal;

    m_imageListNormal = imageList;
    m_ownsImageListNormal = false;
}

void TreeWndCtrl::SetStateImageList(wxImageList *imageList)
{
    if (m_ownsImageListState)
        delete m_imageListState;

    m_imageListState = imageList;
    m_ownsImageListState = false;
}

void TreeWndCtrl::AdjustTreeScrollbars()
{
    wxSizer* sizer = GetSizer();
    wxSize sizerSize = sizer->GetMinSize();
    wxSize windowSize = GetSize();

    int scrollSizeX = 0;
    int scrollSizeY = 0;
    GetScrollPixelsPerUnit(&scrollSizeX, &scrollSizeY);

    int viewStartX = 0;
    int viewStartY = 0;
    GetViewStart(&viewStartX, &viewStartY);

    int numScrollsX = scrollSizeX ? ( ( sizerSize.GetWidth() + scrollSizeX - 1 ) / scrollSizeX ) : 0;
    int numScrollsY = scrollSizeY ? ( ( sizerSize.GetHeight() + scrollSizeY - 1 ) / scrollSizeY ) : 0;

    wxRect scrollRect(viewStartX * scrollSizeX, viewStartY * scrollSizeY, windowSize.GetWidth(), windowSize.GetHeight());
    wxRect visibleRect = ( m_lastToggledItem == TreeWndCtrlItemIdInvalid ) ? wxRect(0, 0, 0, 0) : GetItemBounds(m_lastToggledItem);
    visibleRect.x += scrollRect.x;
    visibleRect.y += scrollRect.y;

    if ( ( scrollSizeX == 0 ) || ( sizerSize.GetWidth() < windowSize.GetWidth() ) )
    {
        numScrollsX = 0;
        viewStartX = 0;
    }
    else if ( ( visibleRect.GetLeft() < scrollRect.GetLeft() ) || ( visibleRect.GetRight() > scrollRect.GetRight() ) )
    {
        if ( visibleRect.GetWidth() > windowSize.GetWidth() )
            viewStartX = visibleRect.x / scrollSizeX;
        else
            viewStartX = ( visibleRect.x + visibleRect.GetWidth() - windowSize.GetWidth() + scrollSizeX - 1 ) / scrollSizeX;
    }

    if ( ( scrollSizeY == 0 ) || ( sizerSize.GetHeight() < windowSize.GetHeight() ) )
    {
        numScrollsY = 0;
        viewStartY = 0;
    }
    else if ( ( visibleRect.GetTop() < scrollRect.GetTop() ) || ( visibleRect.GetBottom() > scrollRect.GetBottom() ) )
    {
        if ( visibleRect.GetHeight() > windowSize.GetHeight() )
            viewStartY = visibleRect.y / scrollSizeY;
        else
            viewStartY = ( visibleRect.y + visibleRect.GetHeight() - windowSize.GetHeight() + scrollSizeY - 1 ) / scrollSizeY;
    }

    bool autoLayout = m_autoLayout;
    SetAutoLayout(false);
    Scroll(0, 0);
    SetScrollbars(scrollSizeX, scrollSizeY, numScrollsX, numScrollsY, 0, 0);
    Scroll(viewStartX, viewStartY);
    SetAutoLayout(autoLayout);
}

wxTreeItemId TreeWndCtrl::DoInsertItem(const wxTreeItemId& parent,
                                       size_t pos,
                                       const wxString& text,
                                       int image,
                                       int selImage,
                                       wxTreeItemData *data)
{
    TreeWndCtrlItem* window = CreateTreeWndCtrlItem(text, image, selImage);
    wxTreeItemId item = DoInsertItem(parent, pos, window, data);
    window->SetItem(item);
    return item;
}

wxTreeItemId TreeWndCtrl::DoInsertAfter(const wxTreeItemId& parent,
                                        const wxTreeItemId& previous,
                                        const wxString& text,
                                        int image,
                                        int selImage,
                                        wxTreeItemData *data)
{
    TreeWndCtrlItem* window = CreateTreeWndCtrlItem(text, image, selImage);
    wxTreeItemId item = DoInsertAfter(parent, previous, window, data);
    window->SetItem(item);
    return item;
}

wxTreeItemId TreeWndCtrl::DoInsertItem(const wxTreeItemId& parent,
                                       size_t pos,
                                       wxWindow *window,
                                       wxTreeItemData *data)
{
    wxCHECK_MSG((parent != TreeWndCtrlItemIdInvalid), TreeWndCtrlItemIdInvalid, wxT("Node must have a valid parent!"));
    wxCHECK_MSG((window->GetParent() == this), TreeWndCtrlItemIdInvalid, wxT("The window must be parented to the TreeWndCtrl object!"));

    TreeWndCtrlNode *newNode = new TreeWndCtrlNode(this, parent, window, 0, data);

    TreeWndCtrlNode *parentNode = (TreeWndCtrlNode *)parent.m_pItem;
    TreeWndCtrlSpacer *parentSpacer = parentNode->GetSpacer();

    TreeWndCtrlSpacer *newSpacer = newNode->GetSpacer();
    newSpacer->SetNumColumns(parentSpacer->GetNumColumns() + 1);

    wxTreeItemId node(newNode);

    if ( pos < 0 )
        parentNode->m_children.Insert(node, 0);
    else if ( pos >= parentNode->m_children.GetCount() )
        parentNode->m_children.Add(node);
    else
        parentNode->m_children.Insert(node, pos);

    if ( data != NULL )
        data->SetId(node);

    m_dirty = true;
    return node;
}

wxTreeItemId TreeWndCtrl::DoInsertAfter(const wxTreeItemId& parent,
                                        const wxTreeItemId& previous,
                                        wxWindow *window,
                                        wxTreeItemData *data)
{
    wxCHECK_MSG((parent != TreeWndCtrlItemIdInvalid), TreeWndCtrlItemIdInvalid, wxT("Node must have a valid parent!"));
    wxCHECK_MSG((previous != TreeWndCtrlItemIdInvalid), TreeWndCtrlItemIdInvalid, wxT("Previous node must be valid!"));
    wxCHECK_MSG((window->GetParent() == this), TreeWndCtrlItemIdInvalid, wxT("The window must be parented to the TreeWndCtrl object!"));

    TreeWndCtrlNode *parentNode = (TreeWndCtrlNode *)parent.m_pItem;
    size_t previousIndex = parentNode->m_children.Index(previous.m_pItem);
    if ( previousIndex == wxNOT_FOUND )
        return TreeWndCtrlItemIdInvalid;

    return DoInsertItem(parent, previousIndex, window, data);
}

TreeWndCtrlItem* TreeWndCtrl::CreateTreeWndCtrlItem(const wxString& text, int image, int selImage)
{
    TreeWndCtrlItem *window = new TreeWndCtrlItem(this, text, image, selImage);
    m_allocatedWindows.Append(window);
    window->Layout();
    return window;
}

void TreeWndCtrl::AddNodeToLayout(wxSizer* treeSizer, const wxTreeItemId& item, bool parentExpanded)
{
    wxASSERT_MSG((item != TreeWndCtrlItemIdInvalid), wxT("Invalid item!"));

    TreeWndCtrlNode *node = (TreeWndCtrlNode *)item.m_pItem;

    bool visible = true;
    if ( m_hideRoot && ( item == m_root ) )
    {
        visible = false;
        parentExpanded = true;
    }
    else
    {
        visible = parentExpanded;
        parentExpanded &= node->m_expanded;
    }

    wxWindow *window = node->GetWindow();
    wxWindow *spacerWindow = (wxWindow *)node->GetSpacer();
    if ( visible )
    {
        window->Show();
        spacerWindow->Show();

        wxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
        sizer->Add(spacerWindow, 0, wxEXPAND, 0);
        sizer->Add(window, 1, wxALIGN_CENTER_VERTICAL, 0);
        treeSizer->Add(sizer, 0, wxEXPAND, 0);
    }
    else
    {
        window->Hide();
        spacerWindow->Hide();
    }

    size_t count = node->m_children.GetCount();
    for ( size_t i = 0; i < count; ++i )
        AddNodeToLayout(treeSizer, node->m_children.Item(i), parentExpanded);
}

void TreeWndCtrl::UpdateNumColumns(const wxTreeItemId& item, unsigned int numColumns)
{
    wxASSERT_MSG((item != TreeWndCtrlItemIdInvalid), wxT("Invalid item!"));

    TreeWndCtrlNode *node = (TreeWndCtrlNode *)item.m_pItem;
    node->GetSpacer()->SetNumColumns(numColumns);

    ++numColumns;
    size_t count = node->m_children.GetCount();
    for ( size_t i = 0; i < count; ++i )
        UpdateNumColumns(node->m_children.Item(i), numColumns);
}

void TreeWndCtrl::GetVisibleItems(wxArrayTreeItemIds& visibleItemList, const wxTreeItemId& item)
{
    wxASSERT_MSG((item != TreeWndCtrlItemIdInvalid), wxT("Invalid item!"));

    if ( IsVisible(item) )
        visibleItemList.Add(item);

    TreeWndCtrlNode *node = (TreeWndCtrlNode *)item.m_pItem;
    size_t count = node->m_children.GetCount();
    for ( size_t i = 0; i < count; ++i )
        GetVisibleItems(visibleItemList, node->m_children.Item(i));
}

wxRect TreeWndCtrl::GetItemBounds(const wxTreeItemId& item, bool recursive) const
{
    wxASSERT_MSG((item != TreeWndCtrlItemIdInvalid), wxT("Invalid item!"));

    TreeWndCtrlNode *node = (TreeWndCtrlNode *)item.m_pItem;
    wxRect windowRect = node->GetWindow()->GetRect();
    if ( recursive && IsExpanded(item) )
    {
        size_t count = node->m_children.GetCount();
        for ( size_t i = 0; i < count; ++i )
        {
            wxRect childRect = GetItemBounds(node->m_children.Item(i), true);

            if ( childRect.x < windowRect.x )
                windowRect.x = childRect.x;

            if ( childRect.y < windowRect.y )
                windowRect.y = childRect.y;

            if ( childRect.GetRight() > windowRect.GetRight() )
                windowRect.width = childRect.width + childRect.x - windowRect.x;

            if ( childRect.GetBottom() > windowRect.GetBottom() )
                windowRect.height = childRect.height + childRect.y - windowRect.y;
        }
    }

    return windowRect;
}

// ----------------------------------------------------------------------------
// TreeWndCtrlItem
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(TreeWndCtrlItem, wxPanel)

BEGIN_EVENT_TABLE( TreeWndCtrlItem, wxPanel )
EVT_PAINT( TreeWndCtrlItem::OnPaint )
EVT_LEFT_DCLICK( TreeWndCtrlItem::OnDoubleClick )
END_EVENT_TABLE()

TreeWndCtrlItem::TreeWndCtrlItem()
: wxPanel(),
m_bitmapTextWidth(-1),
m_image(-1),
m_stateImage(-1),
m_spacing(WXTWC_DEFAULT_ITEM_SPACING),
m_dirty(true),
m_bitmapPoint(0, 0),
m_textPoint(0, 0),
m_item(TreeWndCtrlItemIdInvalid),
m_text( wxT( "" ) ),
m_treeWndCtrl(NULL)
{
}

TreeWndCtrlItem::TreeWndCtrlItem(TreeWndCtrl *parent,
                                 const wxString& text,
                                 int image,
                                 int stateImage)
                                 : wxPanel(parent),
                                 m_bitmapTextWidth(-1),
                                 m_image(image),
                                 m_stateImage(stateImage),
                                 m_spacing(WXTWC_DEFAULT_ITEM_SPACING),
                                 m_dirty(true),
                                 m_bitmapPoint(0, 0),
                                 m_textPoint(0, 0),
                                 m_item(TreeWndCtrlItemIdInvalid),
                                 m_text(text),
                                 m_treeWndCtrl(parent)
{
}

bool TreeWndCtrlItem::Layout()
{
    if ( m_dirty )
    {
        Freeze();

        m_bitmapPoint = wxPoint(0, 0);
        m_textPoint = wxPoint(0, 0);

        wxCoord width = 0;
        wxCoord height = 0;

        int bitmapWidth = 0;
        int bitmapHeight = 0;
        wxBitmap bitmap = GetBitmap();
        if ( bitmap.Ok() )
        {
            bitmapWidth = bitmap.GetWidth();
            bitmapHeight = bitmap.GetHeight();
        }

        if ( bitmapWidth )
            width = bitmapWidth + m_spacing;

        wxCoord textWidth = 0;
        wxCoord textHeight = 0;

        wxClientDC dc(this);
        wxFont oldFont = dc.GetFont();
        dc.SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
        dc.GetTextExtent(m_text, &textWidth, &textHeight);
        dc.SetFont(oldFont);

        m_textPoint.x = width;
        width += textWidth;

        if ( bitmapHeight > textHeight )
        {
            height = bitmapHeight;
            m_textPoint.y = (bitmapHeight - textHeight) / 2;
        }
        else
        {
            height = textHeight;
            m_bitmapPoint.y = (textHeight - bitmapHeight) / 2;
        }

        SetSize(width, height);
        m_bitmapTextWidth = width;

        Thaw();
        m_dirty = false;
    }

    return __super::Layout();
}

void TreeWndCtrlItem::OnPaint(wxPaintEvent& e)
{
    wxPaintDC dc(this);

    wxBitmap bitmap = GetBitmap();
    if ( bitmap.Ok() )
    {
        dc.DrawBitmap(bitmap, m_bitmapPoint.x, m_bitmapPoint.y, true);
    }

    wxFont oldFont = dc.GetFont();
    dc.SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
    dc.DrawText(m_text, m_textPoint.x, m_textPoint.y);
    dc.SetFont(oldFont);

    e.Skip();
}

void TreeWndCtrlItem::OnDoubleClick(wxMouseEvent& e)
{
    if ( m_item == TreeWndCtrlItemIdInvalid )
        return;

    if ( ( m_bitmapTextWidth >= 0 ) && ( e.GetPosition().x > m_bitmapTextWidth ) )
        return;

    m_treeWndCtrl->Toggle(m_item);
}

void TreeWndCtrlItem::SetSpacing(int spacing)
{
    m_spacing = spacing;
    m_dirty = true;
    Layout();
}

void TreeWndCtrlItem::SetItem(const wxTreeItemId& item)
{
    m_item = item;
    m_dirty = true;
    Layout();
}

void TreeWndCtrlItem::SetText(const wxString& text)
{
    m_text = text;
    m_dirty = true;
    Layout();
}    

void TreeWndCtrlItem::SetImage(int image)
{
    m_image = image;
    m_dirty = true;
    Layout();
}

void TreeWndCtrlItem::SetStateImage(int stateImage)
{
    m_stateImage = stateImage;
    m_dirty = true;
    Layout();
}

wxBitmap TreeWndCtrlItem::GetBitmap()
{
    wxBitmap bitmap;
    if ( m_item == TreeWndCtrlItemIdInvalid )
        return bitmap;

    int image = -1;
    wxImageList* imageList = NULL;
    if ( m_treeWndCtrl->ToggleVisible(m_item) && m_treeWndCtrl->IsExpanded(m_item) )
    {
        image = m_stateImage;
        imageList = m_treeWndCtrl->GetStateImageList();
    }
    else
    {
        image = m_image;
        imageList = m_treeWndCtrl->GetImageList();
    }

    if ( imageList && ( image >= 0 ) )
    {
        bitmap = imageList->GetBitmap(image);
    }

    return bitmap;
}

// ----------------------------------------------------------------------------
// TreeWndCtrlNode
// ----------------------------------------------------------------------------

TreeWndCtrlNode::TreeWndCtrlNode(TreeWndCtrl* treeWndCtrl,
                                 const wxTreeItemId& parent,
                                 wxWindow *window,
                                 unsigned int numColumns,
                                 wxTreeItemData *data,
                                 bool expanded)
                                 : m_expanded(expanded),
                                 m_window(window),
                                 m_data(data),
                                 m_id(this),
                                 m_parent(parent)
{
    m_spacer = new TreeWndCtrlSpacer(treeWndCtrl, m_id, numColumns);
}

TreeWndCtrlNode::~TreeWndCtrlNode()
{
}

// ----------------------------------------------------------------------------
// TreeWndCtrlSpacer
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(TreeWndCtrlSpacer, wxPanel)

BEGIN_EVENT_TABLE( TreeWndCtrlSpacer, wxPanel )
EVT_LEFT_DOWN( TreeWndCtrlSpacer::OnMouseDown )
EVT_PAINT( TreeWndCtrlSpacer::OnPaint )
END_EVENT_TABLE()

TreeWndCtrlSpacer::TreeWndCtrlSpacer()
: wxPanel(),
m_numColumns(0),
m_node(TreeWndCtrlItemIdInvalid),
m_treeWndCtrl(NULL)
{
}

TreeWndCtrlSpacer::TreeWndCtrlSpacer(TreeWndCtrl *parent,
                                     const wxTreeItemId& node,
                                     unsigned int numColumns)
                                     : wxPanel(parent),
                                     m_numColumns(numColumns),
                                     m_node(node),
                                     m_treeWndCtrl(parent)
{
    Layout();
}

bool TreeWndCtrlSpacer::Layout()
{
    SetSize(m_numColumns * m_treeWndCtrl->GetColumnSize(), wxDefaultSize.GetHeight());
    return __super::Layout();
}

void TreeWndCtrlSpacer::SetNumColumns(unsigned int numColumns)
{
    m_numColumns = numColumns;
    Layout();
}

int TreeWndCtrlSpacer::GetVerticalMidpoint(wxBitmap bitmap)
{
    switch ( m_treeWndCtrl->GetLineMode() )
    {
    case wxTWC_LINE_TOP:
        return bitmap.GetHeight()/2;

    case wxTWC_LINE_BOTTOM:
        return GetSize().GetHeight() - 1 - bitmap.GetHeight()/2;

    case wxTWC_LINE_CENTER:
    default:
        return GetSize().GetHeight()/2;
    }
}

wxPoint TreeWndCtrlSpacer::GetBitmapPosition(wxBitmap bitmap, unsigned int columnIndex)
{
    unsigned int columnSize = m_treeWndCtrl->GetColumnSize();
    unsigned int columnStartX = columnIndex * columnSize;
    unsigned int columnCenterX = columnStartX + (columnSize / 2);
    unsigned int columnCenterY = GetVerticalMidpoint(bitmap);

    return wxPoint(columnCenterX - bitmap.GetWidth() / 2, columnCenterY - bitmap.GetHeight() / 2);
}

void TreeWndCtrlSpacer::DrawHorizontalLine(wxPaintDC& dc,
                                           unsigned int x1,
                                           unsigned int x2,
                                           unsigned int y)
{
    switch ( m_treeWndCtrl->GetDashMode() )
    {
    case wxTWC_DASH_CUSTOM:
        dc.DrawLine( x1, y, x2, y );
        break;

    case wxTWC_DASH_DEFAULT:
    default:
        {
            int originX = 0;
            int originY = 0;
            ClientToScreen( &originX, &originY );
            int origin = originX + originY + y;

            for ( unsigned int x = x1; x <= x2; ++x )
            {
                if ( ( origin + x ) % 2 )
                    dc.DrawPoint( x, y );
            }
        }
        break;
    }
}

void TreeWndCtrlSpacer::DrawVerticalLine(wxPaintDC& dc,
                                         unsigned int y1,
                                         unsigned int y2,
                                         unsigned int x)
{
    switch ( m_treeWndCtrl->GetDashMode() )
    {
    case wxTWC_DASH_CUSTOM:
        dc.DrawLine( x, y1, x, y2 );
        break;

    case wxTWC_DASH_DEFAULT:
    default:
        {
            int originX = 0;
            int originY = 0;
            ClientToScreen( &originX, &originY );
            int origin = originX + originY + x;

            for ( unsigned int y = y1; y <= y2; ++y )
            {
                if ( ( origin + y ) % 2 )
                    dc.DrawPoint( x, y );
            }
        }
        break;
    }
}

void TreeWndCtrlSpacer::DrawLines(wxPaintDC& dc,
                                  const wxTreeItemId& node,
                                  int columnIndex,
                                  bool leaf)
{
    wxTreeItemId parent = m_treeWndCtrl->GetItemParent(node);
    if ( columnIndex > 0 )
    {
        if ( parent != TreeWndCtrlItemIdInvalid )
            DrawLines(dc, parent, columnIndex - 1, false);

        if ( m_treeWndCtrl->GetLineDrawMode() == wxTWC_LINEDRAW_ROOT_ONLY )
            return;
    }

    unsigned int columnSize = m_treeWndCtrl->GetColumnSize();

    int columnStartX = columnIndex * columnSize;
    int columnCenterX = columnStartX + columnSize/2;
    int columnCenterY = GetVerticalMidpoint(m_treeWndCtrl->GetToggleBitmap(m_node));
    int columnStartY = 0;

    bool hasSibling = false;
    if ( parent == TreeWndCtrlItemIdInvalid )
    {
        columnStartY = columnCenterY;
    }
    else
    {
        hasSibling = m_treeWndCtrl->GetNextSibling(node) != TreeWndCtrlItemIdInvalid;
        if ( ( columnIndex == 0 ) && ( m_node == node ) && ( m_treeWndCtrl->GetPrevSibling(node) == TreeWndCtrlItemIdInvalid ) )
        {
            columnStartY = columnCenterY;
        }
    }

    wxPen oldPen = dc.GetPen();
    dc.SetPen(m_treeWndCtrl->GetPen());

    if ( leaf )
    {
        int columnEndX = columnStartX + columnSize - 1;
        int columnEndY = hasSibling ? GetSize().GetHeight() : columnCenterY;

        DrawHorizontalLine( dc, columnCenterX, columnEndX, columnCenterY );
        if ( columnStartY != columnEndY )
            DrawVerticalLine( dc, columnStartY, columnEndY, columnCenterX );
    }
    else if ( hasSibling )
    {
        int columnEndY = GetSize().GetHeight();
        DrawVerticalLine( dc, columnStartY, columnEndY, columnCenterX );
    }

    dc.SetPen(oldPen);
}

void TreeWndCtrlSpacer::DrawToggle(wxPaintDC& dc, unsigned int columnIndex)
{
    wxBitmap bitmap = m_treeWndCtrl->GetToggleBitmap(m_node);
    wxPoint bitmapPosition = GetBitmapPosition(bitmap, columnIndex);
    dc.DrawBitmap(bitmap, bitmapPosition.x, bitmapPosition.y, true);
}

void TreeWndCtrlSpacer::OnMouseDown(wxMouseEvent& e)
{
    e.Skip();

    if ( !m_numColumns )
        return;

    if ( !m_treeWndCtrl->ToggleVisible(m_node) )
        return;

    wxBitmap bitmap = m_treeWndCtrl->GetToggleBitmap(m_node);
    wxPoint bitmapStart = GetBitmapPosition(bitmap, m_numColumns - 1);

    int clickTolerance = m_treeWndCtrl->GetClickTolerance();

    if ( e.GetPosition().x < bitmapStart.x - clickTolerance )
        return;

    if ( e.GetPosition().x > bitmapStart.x + bitmap.GetWidth() + clickTolerance )
        return;

    if ( e.GetPosition().y < bitmapStart.y - clickTolerance )
        return;

    if ( e.GetPosition().y > bitmapStart.y + bitmap.GetHeight() + clickTolerance )
        return;

    m_treeWndCtrl->Toggle(m_node);
}

void TreeWndCtrlSpacer::OnPaint(wxPaintEvent& e)
{
    wxPaintDC dc(this);

    if ( m_numColumns == 0 )
        return;

    if ( m_treeWndCtrl->GetLineDrawMode() != wxTWC_LINEDRAW_NONE )
        DrawLines(dc, m_node, m_numColumns - 1, true);

    if ( m_treeWndCtrl->ToggleVisible(m_node) )
        DrawToggle( dc, m_numColumns - 1 );

    e.Skip();
}
