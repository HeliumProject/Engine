///////////////////////////////////////////////////////////////////////////////
// Name:        TreeWndCtrl.cpp
// Purpose:     TreeWndCtrl implementation.
// Author:      Insomniac Games
// Created:     03/23/03
// Modified by:
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "TreeWndCtrl.h"
#include "TreeWndCtrlItem.h"
#include "TreeWndCtrlNode.h"
#include "TreeWndCtrlSpacer.h"
#include "TreeWndCtrlCollapsed.xpm"
#include "TreeWndCtrlExpanded.xpm"

using namespace Nocturnal;

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------

APPLICATION_API const wxTreeItemId Nocturnal::TreeWndCtrlItemIdInvalid(NULL);
APPLICATION_API wxBitmap Nocturnal::TreeWndCtrlDefaultExpand( s_treeWndCtrlExpandedXpm );
APPLICATION_API wxBitmap Nocturnal::TreeWndCtrlDefaultCollapse( s_treeWndCtrlCollapsedXpm );
APPLICATION_API wxPen Nocturnal::TreeWndCtrlDefaultPen( wxColour( 0x80, 0x80, 0x80 ), 1, wxSOLID );

// ----------------------------------------------------------------------------
// TreeWndCtrl
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(TreeWndCtrl, wxScrolledWindow)

TreeWndCtrl::TreeWndCtrl()
: wxScrolledWindow(),
m_clickTolerance(WXTWC_DEFAULT_CLICK_TOLERANCE),
m_dashMode(wxTWC_DASH_DEFAULT),
m_lineMode(wxTWC_LINE_CENTER),
m_lineDrawMode(wxTWC_LINEDRAW_ALL),
m_toggleMode(wxTWC_TOGGLE_AUTOMATIC),
m_expandedBitmap(TreeWndCtrlDefaultExpand),
m_collapsedBitmap(TreeWndCtrlDefaultCollapse),
m_pen(TreeWndCtrlDefaultPen),
m_imageListNormal(NULL),
m_ownsImageListNormal(false),
m_imageListState(NULL),
m_ownsImageListState(false),
m_columnSize(WXTWC_DEFAULT_COLUMN_SIZE),
m_hideRoot(false),
m_dirty(true),
m_lastToggledItem(TreeWndCtrlItemIdInvalid),
m_root(TreeWndCtrlItemIdInvalid)
{
}

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

    m_expandedBitmap = expandedBitmap;

    m_collapsedBitmap = collapsedBitmap;

    m_pen = pen;

    m_columnSize = columnSize;

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