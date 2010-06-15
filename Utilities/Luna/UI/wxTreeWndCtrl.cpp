///////////////////////////////////////////////////////////////////////////////
// Name:        wxTreeWndCtrl.cpp
// Purpose:     wxTreeWndCtrl implementation.
// Author:      Insomniac Games
// Created:     03/23/03
// Modified by:
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "Precompile.h"
#include "wxTreeWndCtrl.h"
#include "wxTreeWndCtrlItem.h"
#include "wxTreeWndCtrlNode.h"
#include "wxTreeWndCtrlSpacer.h"
#include "wxTreeWndCtrlCollapsed.xpm"
#include "wxTreeWndCtrlExpanded.xpm"

using namespace Luna;

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------

LUNA_CORE_API const wxTreeItemId Luna::wxTreeWndCtrlItemIdInvalid(NULL);
LUNA_CORE_API wxBitmap Luna::wxTreeWndCtrlDefaultExpand( s_treeWndCtrlExpandedXpm );
LUNA_CORE_API wxBitmap Luna::wxTreeWndCtrlDefaultCollapse( s_treeWndCtrlCollapsedXpm );
LUNA_CORE_API wxPen Luna::wxTreeWndCtrlDefaultPen( wxColour( 0x80, 0x80, 0x80 ), 1, wxSOLID );

// ----------------------------------------------------------------------------
// wxTreeWndCtrl
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxTreeWndCtrl, wxScrolledWindow)

wxTreeWndCtrl::wxTreeWndCtrl()
             : wxScrolledWindow(),
               m_clickTolerance(WXTWC_DEFAULT_CLICK_TOLERANCE),
               m_dashMode(wxTWC_DASH_DEFAULT),
               m_lineMode(wxTWC_LINE_CENTER),
               m_lineDrawMode(wxTWC_LINEDRAW_ALL),
               m_toggleMode(wxTWC_TOGGLE_AUTOMATIC),
               m_expandedBitmap(wxTreeWndCtrlDefaultExpand),
               m_collapsedBitmap(wxTreeWndCtrlDefaultCollapse),
               m_pen(wxTreeWndCtrlDefaultPen),
               m_imageListNormal(NULL),
               m_ownsImageListNormal(false),
               m_imageListState(NULL),
               m_ownsImageListState(false),
               m_columnSize(WXTWC_DEFAULT_COLUMN_SIZE),
               m_hideRoot(false),
               m_dirty(true),
               m_lastToggledItem(wxTreeWndCtrlItemIdInvalid),
               m_root(wxTreeWndCtrlItemIdInvalid)
{
}

wxTreeWndCtrl::wxTreeWndCtrl(wxWindow *parent,
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
               m_root(wxTreeWndCtrlItemIdInvalid)
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

wxTreeWndCtrl::~wxTreeWndCtrl()
{
    DeleteAllItems();

    if (m_ownsImageListNormal)
        delete m_imageListNormal;

    if (m_ownsImageListState)
        delete m_imageListState;
}

bool wxTreeWndCtrl::Layout()
{
    bool returnVal = false;
    wxSizer* sizer = GetSizer();

    if ( m_dirty )
    {
        Freeze();
        
        sizer->Clear();
        if ( m_root != wxTreeWndCtrlItemIdInvalid )
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

size_t wxTreeWndCtrl::GetCount() const
{
    if ( m_root == wxTreeWndCtrlItemIdInvalid )
        return 0;
    
    return GetChildrenCount(m_root, true) + 1;
}

void wxTreeWndCtrl::SetDashMode(wxTreeWndCtrlDashMode dashMode)
{
    if ( m_dashMode == dashMode )
        return;

    m_dashMode = dashMode;
    Refresh();
}

void wxTreeWndCtrl::SetLineMode(wxTreeWndCtrlLineMode lineMode)
{
    if ( m_lineMode == lineMode )
        return;
    
    m_lineMode = lineMode;
    Refresh();
}

void wxTreeWndCtrl::SetLineDrawMode(wxTreeWndCtrlLineDrawMode lineDrawMode)
{
    if ( m_lineDrawMode == lineDrawMode )
        return;
    
    m_lineDrawMode = lineDrawMode;
    Refresh();
}

void wxTreeWndCtrl::SetToggleMode(wxTreeWndCtrlToggleMode toggleMode)
{
    if ( m_toggleMode == toggleMode )
        return;

    m_toggleMode = toggleMode;
    Refresh();
}

void wxTreeWndCtrl::SetColumnSize(unsigned int columnSize)
{
    if ( m_columnSize == columnSize )
        return;
    
    m_columnSize = columnSize;
    Refresh();
}

void wxTreeWndCtrl::SetHideRoot(bool hideRoot)
{
  if ( m_hideRoot == hideRoot )
      return;
  
  m_hideRoot = hideRoot;

  if ( m_root == wxTreeWndCtrlItemIdInvalid )
      return;

  UpdateNumColumns(m_root, m_hideRoot ? 0 : 1);

  Layout();
  Refresh();
}

wxTreeItemId wxTreeWndCtrl::AddRoot(wxWindow *window,
                                    wxTreeItemData *data)
{
    wxCHECK_MSG((m_root == wxTreeWndCtrlItemIdInvalid), m_root, wxT("The wxTreeWndCtrl can only have one root!"));
    wxCHECK_MSG((window->GetParent() == this), wxTreeWndCtrlItemIdInvalid, wxT("The window must be parented to the wxTreeWndCtrl object!"));

    wxTreeWndCtrlNode *root = new wxTreeWndCtrlNode(this, wxTreeWndCtrlItemIdInvalid, window, m_hideRoot ? 0 : 1, data);

    m_dirty = true;
    m_root = wxTreeItemId(root);
    return m_root;
}

wxTreeItemId wxTreeWndCtrl::AddRoot(const wxString& text,
                                    int image,
                                    int selImage,
                                    wxTreeItemData *data)
{
    wxCHECK_MSG((m_root == wxTreeWndCtrlItemIdInvalid), m_root, wxT("The wxTreeWndCtrl can only have one root!"));

    wxTreeWndCtrlItem* window = CreateTreeWndCtrlItem(text, image, selImage);
    wxTreeItemId item = AddRoot(window, data);
    window->SetItem(item);

    return item;
}

wxWindow* wxTreeWndCtrl::GetItemWindow(const wxTreeItemId &item) const
{
    wxCHECK_MSG((item != wxTreeWndCtrlItemIdInvalid), NULL, wxT("Invalid item!"));
    wxTreeWndCtrlNode *node = (wxTreeWndCtrlNode *)item.m_pItem;
    return node->GetWindow();
}

wxTreeItemId wxTreeWndCtrl::GetItemParent(const wxTreeItemId& item) const
{
    wxCHECK_MSG((item != wxTreeWndCtrlItemIdInvalid), wxTreeWndCtrlItemIdInvalid, wxT("Invalid item!"));

    wxTreeWndCtrlNode *node = (wxTreeWndCtrlNode *)item.m_pItem;
    return node->GetParent();
}

wxTreeItemId wxTreeWndCtrl::GetFirstChild(const wxTreeItemId& item) const
{
    wxCHECK_MSG((item != wxTreeWndCtrlItemIdInvalid), wxTreeWndCtrlItemIdInvalid, wxT("Invalid item!"));

    wxTreeWndCtrlNode *node = (wxTreeWndCtrlNode *)item.m_pItem;
    if ( node->m_children.GetCount() <= 0 )
        return wxTreeWndCtrlItemIdInvalid;

    return node->m_children.Item(0);
}

wxTreeItemId wxTreeWndCtrl::GetNextChild(const wxTreeItemId& item,
                                         const wxTreeItemId& lastChild) const
{
    wxCHECK_MSG((item != wxTreeWndCtrlItemIdInvalid), wxTreeWndCtrlItemIdInvalid, wxT("Invalid item!"));

    wxTreeWndCtrlNode *node = (wxTreeWndCtrlNode *)item.m_pItem;

    int index = node->m_children.Index(lastChild.m_pItem);
    if ( index == wxNOT_FOUND )
        return wxTreeWndCtrlItemIdInvalid;
    
    ++index;
    if ( (size_t) index >= node->m_children.GetCount() )
        return wxTreeWndCtrlItemIdInvalid;
    
    return node->m_children.Item(index);
}

wxTreeItemId wxTreeWndCtrl::GetPrevChild(const wxTreeItemId& item,
                                         const wxTreeItemId& lastChild) const
{
    wxCHECK_MSG((item != wxTreeWndCtrlItemIdInvalid), wxTreeWndCtrlItemIdInvalid, wxT("Invalid item!"));

    wxTreeWndCtrlNode *node = (wxTreeWndCtrlNode *)item.m_pItem;

    int index = node->m_children.Index(lastChild.m_pItem);
    if ( index == wxNOT_FOUND )
        return wxTreeWndCtrlItemIdInvalid;
    
    --index;
    if ( index < 0 )
        return wxTreeWndCtrlItemIdInvalid;
    
    return node->m_children.Item(index);
}

wxTreeItemId wxTreeWndCtrl::GetLastChild(const wxTreeItemId& item) const
{
    wxCHECK_MSG((item != wxTreeWndCtrlItemIdInvalid), wxTreeWndCtrlItemIdInvalid, wxT("Invalid item!"));

    wxTreeWndCtrlNode *node = (wxTreeWndCtrlNode *)item.m_pItem;
    size_t count = node->m_children.GetCount();
    if ( count <= 0 )
        return wxTreeWndCtrlItemIdInvalid;

    return node->m_children.Item(count - 1);
}

wxTreeItemId wxTreeWndCtrl::GetNextSibling(const wxTreeItemId& item) const
{
    wxCHECK_MSG((item != wxTreeWndCtrlItemIdInvalid), wxTreeWndCtrlItemIdInvalid, wxT("Invalid item!"));

    wxTreeWndCtrlNode *node = (wxTreeWndCtrlNode *)item.m_pItem;
    return GetNextChild(node->m_parent, item);
}

wxTreeItemId wxTreeWndCtrl::GetPrevSibling(const wxTreeItemId& item) const
{
    wxCHECK_MSG((item != wxTreeWndCtrlItemIdInvalid), wxTreeWndCtrlItemIdInvalid, wxT("Invalid item!"));

    wxTreeWndCtrlNode *node = (wxTreeWndCtrlNode *)item.m_pItem;
    return GetPrevChild(node->m_parent, item);
}

wxTreeItemId wxTreeWndCtrl::GetFirstVisibleItem()
{
    if ( m_root == wxTreeWndCtrlItemIdInvalid )
        return wxTreeWndCtrlItemIdInvalid;

    wxArrayTreeItemIds visibleItemList;
    GetVisibleItems(visibleItemList, m_root);
    
    if ( visibleItemList.GetCount() <= 0 )
        return wxTreeWndCtrlItemIdInvalid;
    
    return visibleItemList.Item(0);
}

wxTreeItemId wxTreeWndCtrl::GetNextVisible(const wxTreeItemId& item)
{
    if ( m_root == wxTreeWndCtrlItemIdInvalid )
        return wxTreeWndCtrlItemIdInvalid;

    wxArrayTreeItemIds visibleItemList;
    GetVisibleItems(visibleItemList, m_root);
    
    int index = visibleItemList.Index(item.m_pItem);
    if ( index == wxNOT_FOUND )
        return wxTreeWndCtrlItemIdInvalid;
    
    ++index;
    if ( (size_t) index >= visibleItemList.GetCount() )
        return wxTreeWndCtrlItemIdInvalid;

    return visibleItemList.Item(index);
}

wxTreeItemId wxTreeWndCtrl::GetPrevVisible(const wxTreeItemId& item)
{
    if ( m_root == wxTreeWndCtrlItemIdInvalid )
        return wxTreeWndCtrlItemIdInvalid;

    wxArrayTreeItemIds visibleItemList;
    GetVisibleItems(visibleItemList, m_root);
    
    int index = visibleItemList.Index(item.m_pItem);
    if ( index == wxNOT_FOUND )
        return wxTreeWndCtrlItemIdInvalid;
    
    --index;
    if ( index < 0 )
        return wxTreeWndCtrlItemIdInvalid;

    return visibleItemList.Item(index);
}

size_t wxTreeWndCtrl::GetChildrenCount(const wxTreeItemId& item,
                                       bool recursively) const
{
    wxCHECK_MSG((item != wxTreeWndCtrlItemIdInvalid), 0, wxT("Invalid item!"));

    wxTreeWndCtrlNode *node = (wxTreeWndCtrlNode *)item.m_pItem;
    size_t childrenCount = node->m_children.GetCount();

    if ( recursively )
    {
        size_t count = childrenCount;
        for ( size_t i = 0; i < count; ++i )
            childrenCount += GetChildrenCount(node->m_children.Item(i), true);
    }

    return childrenCount;
}

wxBitmap wxTreeWndCtrl::GetToggleBitmap(const wxTreeItemId& item)
{
    return IsExpanded(item) ? m_expandedBitmap : m_collapsedBitmap;
}

bool wxTreeWndCtrl::IsToggleVisible(const wxTreeItemId& item)
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

void wxTreeWndCtrl::Delete(const wxTreeItemId& item)
{
    wxASSERT_MSG((item != wxTreeWndCtrlItemIdInvalid), wxT("Invalid item!"));

    wxTreeEvent deleteEvent(wxEVT_COMMAND_TREE_DELETE_ITEM);
    deleteEvent.SetItem(item);
    ProcessEvent( deleteEvent );

    DeleteChildren(item);
    
    wxTreeWndCtrlNode *node = (wxTreeWndCtrlNode *)item.m_pItem;
    wxTreeItemId parent = node->GetParent();
    if ( parent != wxTreeWndCtrlItemIdInvalid )
    {
        wxTreeWndCtrlNode *parentNode = (wxTreeWndCtrlNode *)parent.m_pItem;
        int index = parentNode->m_children.Index(item.m_pItem);
        if ( index != wxNOT_FOUND )
            parentNode->m_children.RemoveAt(index);
    }
    
    if ( item == m_lastToggledItem )
        m_lastToggledItem = wxTreeWndCtrlItemIdInvalid;
       
    if ( item == m_root )
        m_root = wxTreeWndCtrlItemIdInvalid;

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

void wxTreeWndCtrl::DeleteChildren(const wxTreeItemId& item)
{
    wxASSERT_MSG((item != wxTreeWndCtrlItemIdInvalid), wxT("Invalid item!"));

    wxTreeWndCtrlNode *node = (wxTreeWndCtrlNode *)item.m_pItem;
    while ( node->m_children.GetCount() > 0 )
    {
      Delete(node->m_children.Item(0));
    }
}

void wxTreeWndCtrl::DeleteAllItems()
{
    if ( m_root == wxTreeWndCtrlItemIdInvalid )
        return;

    Delete(m_root);
}

int wxTreeWndCtrl::GetItemImage(const wxTreeItemId& item, wxTreeItemIcon which) const
{
    wxASSERT_MSG((item != wxTreeWndCtrlItemIdInvalid), wxT("Invalid item!"));
    wxTreeWndCtrlNode *node = (wxTreeWndCtrlNode *)item.m_pItem;

    wxTreeWndCtrlItem* control = wxDynamicCast(node->GetWindow(), wxTreeWndCtrlItem);
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

wxTreeItemId wxTreeWndCtrl::FindItem(const wxWindow* window) const
{
    if ( m_root == wxTreeWndCtrlItemIdInvalid )
        return wxTreeWndCtrlItemIdInvalid;

    return FindItem(m_root, window);
}

wxTreeItemId wxTreeWndCtrl::FindItem(const wxTreeItemId &item, const wxWindow* window) const
{
    wxASSERT_MSG((item != wxTreeWndCtrlItemIdInvalid), wxT("Invalid item!"));
    wxTreeWndCtrlNode *node = (wxTreeWndCtrlNode *)item.m_pItem;
    
    if ( node->GetWindow() == window )
        return item;

    size_t count = node->m_children.GetCount();
    for ( size_t i = 0; i < count; ++i )
    {
        wxTreeItemId childItem = FindItem(node->m_children.Item(i), window);
        if ( childItem != wxTreeWndCtrlItemIdInvalid )
            return childItem;
    }
    
    return wxTreeWndCtrlItemIdInvalid;
}

wxString wxTreeWndCtrl::GetItemText(const wxTreeItemId& item) const
{
    wxASSERT_MSG((item != wxTreeWndCtrlItemIdInvalid), wxT("Invalid item!"));
    wxTreeWndCtrlNode *node = (wxTreeWndCtrlNode *)item.m_pItem;

    wxTreeWndCtrlItem* control = wxDynamicCast(node->GetWindow(), wxTreeWndCtrlItem);
    if ( control == NULL )
        return wxEmptyString;

    return control->GetText();
}

wxTreeItemData *wxTreeWndCtrl::GetItemData(const wxTreeItemId& item) const
{
    wxASSERT_MSG((item != wxTreeWndCtrlItemIdInvalid), wxT("Invalid item!"));
    wxTreeWndCtrlNode *node = (wxTreeWndCtrlNode *)item.m_pItem;
    
    return (node->GetData());
}

wxColour wxTreeWndCtrl::GetItemTextColour(const wxTreeItemId& item) const
{
    wxASSERT_MSG((item != wxTreeWndCtrlItemIdInvalid), wxT("Invalid item!"));
    wxTreeWndCtrlNode *node = (wxTreeWndCtrlNode *)item.m_pItem;

    wxTreeWndCtrlItem* control = wxDynamicCast(node->GetWindow(), wxTreeWndCtrlItem);
    if ( control == NULL )
        return wxNullColour;

    return control->GetForegroundColour();
}

wxColour wxTreeWndCtrl::GetItemBackgroundColour(const wxTreeItemId& item) const
{
    wxASSERT_MSG((item != wxTreeWndCtrlItemIdInvalid), wxT("Invalid item!"));
    wxTreeWndCtrlNode *node = (wxTreeWndCtrlNode *)item.m_pItem;

    wxTreeWndCtrlItem* control = wxDynamicCast(node->GetWindow(), wxTreeWndCtrlItem);
    if ( control == NULL )
        return wxNullColour;

    return control->GetBackgroundColour();
}

wxFont wxTreeWndCtrl::GetItemFont(const wxTreeItemId& item) const
{
    wxASSERT_MSG((item != wxTreeWndCtrlItemIdInvalid), wxT("Invalid item!"));
    wxTreeWndCtrlNode *node = (wxTreeWndCtrlNode *)item.m_pItem;

    wxTreeWndCtrlItem* control = wxDynamicCast(node->GetWindow(), wxTreeWndCtrlItem);
    if ( control == NULL )
        return wxNullFont;

    return control->GetFont();
}

void wxTreeWndCtrl::SetItemText(const wxTreeItemId& item, const wxString& text)
{
    wxASSERT_MSG((item != wxTreeWndCtrlItemIdInvalid), wxT("Invalid item!"));
    wxTreeWndCtrlNode *node = (wxTreeWndCtrlNode *)item.m_pItem;

    wxTreeWndCtrlItem* control = wxDynamicCast(node->GetWindow(), wxTreeWndCtrlItem);
    if ( control )
        control->SetText(text);
}

void wxTreeWndCtrl::SetItemImage(const wxTreeItemId& item, int image, wxTreeItemIcon which)
{
    wxASSERT_MSG((item != wxTreeWndCtrlItemIdInvalid), wxT("Invalid item!"));
    wxTreeWndCtrlNode *node = (wxTreeWndCtrlNode *)item.m_pItem;

    wxTreeWndCtrlItem* control = wxDynamicCast(node->GetWindow(), wxTreeWndCtrlItem);
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

void wxTreeWndCtrl::SetItemData(const wxTreeItemId& item, wxTreeItemData *data)
{
    wxASSERT_MSG((item != wxTreeWndCtrlItemIdInvalid), wxT("Invalid item!"));
    wxTreeWndCtrlNode *node = (wxTreeWndCtrlNode *)item.m_pItem;

    node->SetData(data);
}

void wxTreeWndCtrl::SetItemTextColour(const wxTreeItemId& item, const wxColour& col)
{
    wxASSERT_MSG((item != wxTreeWndCtrlItemIdInvalid), wxT("Invalid item!"));
    wxTreeWndCtrlNode *node = (wxTreeWndCtrlNode *)item.m_pItem;

    wxTreeWndCtrlItem* control = wxDynamicCast(node->GetWindow(), wxTreeWndCtrlItem);
    if ( control  )
        control->SetForegroundColour(col);
}

void wxTreeWndCtrl::SetItemBackgroundColour(const wxTreeItemId& item, const wxColour& col)
{
    wxASSERT_MSG((item != wxTreeWndCtrlItemIdInvalid), wxT("Invalid item!"));
    wxTreeWndCtrlNode *node = (wxTreeWndCtrlNode *)item.m_pItem;

    wxTreeWndCtrlItem* control = wxDynamicCast(node->GetWindow(), wxTreeWndCtrlItem);
    if ( control  )
        control->SetBackgroundColour(col);
}

void wxTreeWndCtrl::SetItemFont(const wxTreeItemId& item, const wxFont& font)
{
    wxASSERT_MSG((item != wxTreeWndCtrlItemIdInvalid), wxT("Invalid item!"));
    wxTreeWndCtrlNode *node = (wxTreeWndCtrlNode *)item.m_pItem;

    wxTreeWndCtrlItem* control = wxDynamicCast(node->GetWindow(), wxTreeWndCtrlItem);
    if ( control  )
        control->SetFont(font);
}
                         
bool wxTreeWndCtrl::IsExpanded(const wxTreeItemId& item) const
{
    wxCHECK_MSG((item != wxTreeWndCtrlItemIdInvalid), false, wxT("Invalid item!"));

    wxTreeWndCtrlNode *node = (wxTreeWndCtrlNode *)item.m_pItem;
    return node->m_expanded;
}

bool wxTreeWndCtrl::IsVisible(const wxTreeItemId& item) const
{
    if ( item == wxTreeWndCtrlItemIdInvalid )
        return false;

    wxTreeItemId currentItem = GetItemParent(item);
    while ( currentItem != wxTreeWndCtrlItemIdInvalid )
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

void wxTreeWndCtrl::Expand(const wxTreeItemId& item)
{
    wxASSERT_MSG((item != wxTreeWndCtrlItemIdInvalid), wxT("Invalid item!"));
    wxTreeWndCtrlNode *node = (wxTreeWndCtrlNode *)item.m_pItem;

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

void wxTreeWndCtrl::ExpandAllChildren(const wxTreeItemId& item)
{
    wxASSERT_MSG((item != wxTreeWndCtrlItemIdInvalid), wxT("Invalid item!"));

    wxTreeWndCtrlNode *node = (wxTreeWndCtrlNode *)item.m_pItem;
    size_t count = node->m_children.GetCount();
    for ( size_t i = 0; i < count; ++i )
    {
        wxTreeItemId child = node->m_children.Item(i);
        Expand(child);
        ExpandAllChildren(child);
    }
}

void wxTreeWndCtrl::ExpandAll()
{
    if (m_root == wxTreeWndCtrlItemIdInvalid)
        return;
      
    ExpandAllChildren(m_root);
    Expand(m_root);
}

void wxTreeWndCtrl::Collapse(const wxTreeItemId& item)
{
    wxASSERT_MSG((item != wxTreeWndCtrlItemIdInvalid), wxT("Invalid item!"));
    wxTreeWndCtrlNode *node = (wxTreeWndCtrlNode *)item.m_pItem;

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

void wxTreeWndCtrl::CollapseAllChildren(const wxTreeItemId& item)
{
    wxASSERT_MSG((item != wxTreeWndCtrlItemIdInvalid), wxT("Invalid item!"));

    wxTreeWndCtrlNode *node = (wxTreeWndCtrlNode *)item.m_pItem;
    size_t count = node->m_children.GetCount();
    for ( size_t i = 0; i < count; ++i )
    {
        wxTreeItemId child = node->m_children.Item(i);
        Collapse(child);
        CollapseAllChildren(child);
    }
}

void wxTreeWndCtrl::CollapseAll()
{
    if (m_root == wxTreeWndCtrlItemIdInvalid)
        return;
      
    CollapseAllChildren(m_root);
    Collapse(m_root);
}

void wxTreeWndCtrl::Toggle(const wxTreeItemId& item)
{
    wxASSERT_MSG((item != wxTreeWndCtrlItemIdInvalid), wxT("Invalid item!"));

    SetExpanded(item, !IsExpanded(item));
}

void wxTreeWndCtrl::SetExpanded(const wxTreeItemId& item, bool expanded)
{
    wxASSERT_MSG((item != wxTreeWndCtrlItemIdInvalid), wxT("Invalid item!"));

    if ( expanded )
        Expand(item);
    else
        Collapse(item);
}

void wxTreeWndCtrl::SetImageList(wxImageList *imageList)
{
    if (m_ownsImageListNormal)
        delete m_imageListNormal;

    m_imageListNormal = imageList;
    m_ownsImageListNormal = false;
}

void wxTreeWndCtrl::SetStateImageList(wxImageList *imageList)
{
    if (m_ownsImageListState)
        delete m_imageListState;

    m_imageListState = imageList;
    m_ownsImageListState = false;
}
    
void wxTreeWndCtrl::AdjustTreeScrollbars()
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
    wxRect visibleRect = ( m_lastToggledItem == wxTreeWndCtrlItemIdInvalid ) ? wxRect(0, 0, 0, 0) : GetItemBounds(m_lastToggledItem);
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

wxTreeItemId wxTreeWndCtrl::DoInsertItem(const wxTreeItemId& parent,
                                  size_t pos,
                                  const wxString& text,
                                  int image,
                                  int selImage,
                                  wxTreeItemData *data)
{
    wxTreeWndCtrlItem* window = CreateTreeWndCtrlItem(text, image, selImage);
    wxTreeItemId item = DoInsertItem(parent, pos, window, data);
    window->SetItem(item);
    return item;
}

wxTreeItemId wxTreeWndCtrl::DoInsertAfter(const wxTreeItemId& parent,
                                   const wxTreeItemId& previous,
                                   const wxString& text,
                                   int image,
                                   int selImage,
                                   wxTreeItemData *data)
{
    wxTreeWndCtrlItem* window = CreateTreeWndCtrlItem(text, image, selImage);
    wxTreeItemId item = DoInsertAfter(parent, previous, window, data);
    window->SetItem(item);
    return item;
}

wxTreeItemId wxTreeWndCtrl::DoInsertItem(const wxTreeItemId& parent,
                                         size_t pos,
                                         wxWindow *window,
                                         wxTreeItemData *data)
{
    wxCHECK_MSG((parent != wxTreeWndCtrlItemIdInvalid), wxTreeWndCtrlItemIdInvalid, wxT("Node must have a valid parent!"));
    wxCHECK_MSG((window->GetParent() == this), wxTreeWndCtrlItemIdInvalid, wxT("The window must be parented to the wxTreeWndCtrl object!"));

    wxTreeWndCtrlNode *newNode = new wxTreeWndCtrlNode(this, parent, window, 0, data);

    wxTreeWndCtrlNode *parentNode = (wxTreeWndCtrlNode *)parent.m_pItem;
    wxTreeWndCtrlSpacer *parentSpacer = parentNode->GetSpacer();

    wxTreeWndCtrlSpacer *newSpacer = newNode->GetSpacer();
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

wxTreeItemId wxTreeWndCtrl::DoInsertAfter(const wxTreeItemId& parent,
                                          const wxTreeItemId& previous,
                                          wxWindow *window,
                                          wxTreeItemData *data)
{
    wxCHECK_MSG((parent != wxTreeWndCtrlItemIdInvalid), wxTreeWndCtrlItemIdInvalid, wxT("Node must have a valid parent!"));
    wxCHECK_MSG((previous != wxTreeWndCtrlItemIdInvalid), wxTreeWndCtrlItemIdInvalid, wxT("Previous node must be valid!"));
    wxCHECK_MSG((window->GetParent() == this), wxTreeWndCtrlItemIdInvalid, wxT("The window must be parented to the wxTreeWndCtrl object!"));

    wxTreeWndCtrlNode *parentNode = (wxTreeWndCtrlNode *)parent.m_pItem;
    size_t previousIndex = parentNode->m_children.Index(previous.m_pItem);
    if ( previousIndex == wxNOT_FOUND )
        return wxTreeWndCtrlItemIdInvalid;

    return DoInsertItem(parent, previousIndex, window, data);
}

wxTreeWndCtrlItem* wxTreeWndCtrl::CreateTreeWndCtrlItem(const wxString& text, int image, int selImage)
{
    wxTreeWndCtrlItem *window = new wxTreeWndCtrlItem(this, text, image, selImage);
    m_allocatedWindows.Append(window);
    window->Layout();
    return window;
}

void wxTreeWndCtrl::AddNodeToLayout(wxSizer* treeSizer, const wxTreeItemId& item, bool parentExpanded)
{
    wxASSERT_MSG((item != wxTreeWndCtrlItemIdInvalid), wxT("Invalid item!"));

    wxTreeWndCtrlNode *node = (wxTreeWndCtrlNode *)item.m_pItem;

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

void wxTreeWndCtrl::UpdateNumColumns(const wxTreeItemId& item, unsigned int numColumns)
{
    wxASSERT_MSG((item != wxTreeWndCtrlItemIdInvalid), wxT("Invalid item!"));

    wxTreeWndCtrlNode *node = (wxTreeWndCtrlNode *)item.m_pItem;
    node->GetSpacer()->SetNumColumns(numColumns);
    
    ++numColumns;
    size_t count = node->m_children.GetCount();
    for ( size_t i = 0; i < count; ++i )
        UpdateNumColumns(node->m_children.Item(i), numColumns);
}

void wxTreeWndCtrl::GetVisibleItems(wxArrayTreeItemIds& visibleItemList, const wxTreeItemId& item)
{
    wxASSERT_MSG((item != wxTreeWndCtrlItemIdInvalid), wxT("Invalid item!"));

    if ( IsVisible(item) )
        visibleItemList.Add(item);
    
    wxTreeWndCtrlNode *node = (wxTreeWndCtrlNode *)item.m_pItem;
    size_t count = node->m_children.GetCount();
    for ( size_t i = 0; i < count; ++i )
        GetVisibleItems(visibleItemList, node->m_children.Item(i));
}

wxRect wxTreeWndCtrl::GetItemBounds(const wxTreeItemId& item, bool recursive) const
{
    wxASSERT_MSG((item != wxTreeWndCtrlItemIdInvalid), wxT("Invalid item!"));

    wxTreeWndCtrlNode *node = (wxTreeWndCtrlNode *)item.m_pItem;
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