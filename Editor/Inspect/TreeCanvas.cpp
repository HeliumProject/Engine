#include "Precompile.h"
#include "TreeCanvas.h"

#include "Editor/FileIconsTable.h"
#include "Editor/Inspect/TreeCanvasWidget.h"

using namespace Helium;
using namespace Helium::Editor;

const static int SCROLL_INCREMENT = 8;

class ContainerItemData : public wxTreeItemData
{
public:
    ContainerItemData( Inspect::Container* container )
        : wxTreeItemData()
        , m_Container( container )
        , m_IgnoreToggle( false )
    {
    }

    Inspect::Container* GetContainer()
    {
        return m_Container;
    }

    void StartIgnoreToggle()
    {
        m_IgnoreToggle = true;
    }

    void EndIgnoreToggle()
    {
        m_IgnoreToggle = false;
    }

    bool GetIgnoreToggle()
    {
        return m_IgnoreToggle;
    }

protected:
    Inspect::Container* m_Container;
    bool                m_IgnoreToggle;
};

TreeCanvas::TreeCanvas( TreeWndCtrl* treeWndCtrl )
: Canvas( treeWndCtrl )
, m_TreeWndCtrl( treeWndCtrl )
, m_RootId( Helium::TreeWndCtrlItemIdInvalid )
{
    SetWidgetCreator< TreeCanvasWidget, Container >();

    // for now we must always show scroll bars
    HELIUM_ASSERT( m_TreeWndCtrl->GetWindowStyle() & wxALWAYS_SHOW_SB );

    m_TreeWndCtrl->SetScrollRate(SCROLL_INCREMENT, SCROLL_INCREMENT);
    m_TreeWndCtrl->SetImageList( GlobalFileIconsTable().GetSmallImageList() );
    m_TreeWndCtrl->SetStateImageList( GlobalFileIconsTable().GetSmallImageList() );

    wxTreeItemId root = m_TreeWndCtrl->GetRootItem();
    if ( root == Helium::TreeWndCtrlItemIdInvalid )
    {
        root = m_TreeWndCtrl->AddRoot( TXT( "INVISIBLE_ROOT" ) );
        m_RootId = root;
    }

    // wxALWAYS_SHOW_SB in the constructor will ensure that there is always scroll bars
    //  in the canvas window.  This will prevent the need from doing layout on the window more than once.
    //  This function will ensure that the horizontal bar are never seen.  If a horizontal
    //  bar might be necessary, then the best way would be to remove this and just let both
    //  bars be seen. 
#pragma TODO("Figure out how to properly handle this edge case only calling wx code")
    ::ShowScrollBar( (HWND) m_TreeWndCtrl->GetHandle(), SB_HORZ, FALSE );

    m_TreeWndCtrl->Connect( m_TreeWndCtrl->GetId(), wxEVT_SIZE, wxSizeEventHandler( TreeCanvas::OnSize ), NULL, this );
    m_TreeWndCtrl->Connect( m_TreeWndCtrl->GetId(), wxEVT_COMMAND_TREE_ITEM_EXPANDED, wxTreeEventHandler( TreeCanvas::OnToggle ), NULL, this );
    m_TreeWndCtrl->Connect( m_TreeWndCtrl->GetId(), wxEVT_COMMAND_TREE_ITEM_COLLAPSED, wxTreeEventHandler( TreeCanvas::OnToggle ), NULL, this );
}

void TreeCanvas::OnSize(wxSizeEvent& event)
{
    // to prevent the scroll bar from appearing during an OnSize event, need to 
    // explicitly hide the scroll bar
    ::ShowScrollBar( (HWND) m_TreeWndCtrl->GetHandle(), SB_HORZ, FALSE );

    // to make sure the tree control fits to the width of the canvas, set the
    // virtual size
    int canvasPad = wxSystemSettings::GetMetric( wxSYS_VSCROLL_X ) + GetPad();
    int w = -1;

    int eventWidth = event.GetSize().GetWidth();
    if ( eventWidth > canvasPad )
    {
        w = eventWidth - canvasPad;
    }
    else
    {
        int virtualWidth = m_TreeWndCtrl->GetVirtualSize().GetWidth();
        if ( virtualWidth > canvasPad )
        {
            w = virtualWidth - canvasPad;
        }
    }

    m_TreeWndCtrl->SetVirtualSizeHints( w, -1, w, -1 );
    m_TreeWndCtrl->Layout();

    event.Skip();
}

void TreeCanvas::OnToggle(wxTreeEvent& event)
{
    wxTreeItemId item = event.GetItem();

    ContainerItemData* containerItemData = (ContainerItemData*) m_TreeWndCtrl->GetItemData( item );
    if ( !containerItemData->GetIgnoreToggle() )
    {
        Container* container = containerItemData->GetContainer();
        if ( container )
        {
#ifdef INSPECT_REFACTOR
            bool newExpandState = !container->IsExpanded();

            SetContainerExpandState( container->GetPath(), newExpandState ? ExpandStates::Expanded : ExpandStates::Collapsed );

            container->SetExpanded( newExpandState );
#endif
        }
    }

    event.Skip();
}

void TreeCanvas::Realize( Inspect::Canvas* canvas )
{
    HELIUM_ASSERT( canvas == this || canvas == NULL );

    SmartPtr< TreeCanvasWidget > widget = new TreeCanvasWidget( this );
    widget->SetTreeWndCtrl( m_TreeWndCtrl );
    widget->SetId( m_RootId );
    SetWidget( widget );

    Inspect::V_Control::const_iterator itr = m_Children.begin();
    Inspect::V_Control::const_iterator end = m_Children.end();
    for( ; itr != end; ++itr )
    {
        Inspect::Control* c = *itr;
        c->Realize( this );
    }

    m_TreeWndCtrl->Freeze();
    m_TreeWndCtrl->Scroll( 0, 0 );
    m_TreeWndCtrl->Layout();
    m_TreeWndCtrl->Thaw();
}

void TreeCanvas::Clear()
{
    Base::Clear();

    m_TreeWndCtrl->DeleteChildren( m_TreeWndCtrl->GetRootItem() );
}

#if INSPECT_REFACTOR

void NameChanged(const tstring& text)
{
    m_Name = text;
    m_Path.clear();

    if ( m_Canvas )
    {
        wxTreeItemId item = m_ItemData.GetId();
        if ( item != Helium::TreeWndCtrlItemIdInvalid )
        {
            Helium::TreeWndCtrl* treeWndCtrl = (Helium::TreeWndCtrl*) m_Window;
            treeWndCtrl->SetItemText( item, m_Name );
        }
    }
}

void ChildRemoving(Control* child)
{
    // This is annoying... it looks like there might be a bug in wx having
    // to do with wxButtons being the "TmpDefaultItem".  The destructor of
    // wxButton usually takes care of this, but because we remove the button
    // before destroying it, the top level window is left holding a dead
    // pointer.  Therefore, we have to clean up that pointer manually.
    // Update: Apparently the same problem occurs with wxTopLevelWindowMSW::m_winLastFocused.
    // Therefore, we will also store and clear it ourselves.
    wxTopLevelWindow* topWindow = wxDynamicCast( wxGetTopLevelParent( m_Window ), wxTopLevelWindow );
    wxWindow* defaultItem = NULL;
    wxWindow* lastFocus = NULL;
    if ( topWindow )
    {
        defaultItem = topWindow->GetTmpDefaultItem();
        lastFocus = topWindow->GetLastFocus();
    }

    // our child's internal window
    wxWindow* window = NULL;

    // hold a reference on the stack
    ControlPtr referenceHolder = control;

    // if the child is realized
    if (control->GetWindow())
    {
        // If the child is a container, clear it before we break the window hierarchy
        if ( control->HasType( Reflect::GetType<Container>() ) )
        {
            Container* container = static_cast< Container* >( control );
            container->Clear();
        }

        // save the child window pointer
        window = control->GetWindow();

        // The item we are about to remove is stored on the frame, so 
        // clear it (see comments at top of this function).
        if ( defaultItem && defaultItem == window )
        {
            topWindow->SetTmpDefaultItem( NULL );
        }

        // The item we are about to remove is stored on the frame (as
        // the item last having focus), so clear it (see comments at 
        // top of this function).
        if ( lastFocus && lastFocus == window )
        {
            topWindow->SetLastFocus( NULL );
        }

        // unhook the child from the nested control
        if ( window->GetParent() == m_Window )
        {
            m_Window->RemoveChild(window);
        }
    }
}

void PanelUnrealized()
{
    wxTreeItemId item = m_ItemData.GetId();
    if ( item != Helium::TreeWndCtrlItemIdInvalid )
    {
        Helium::TreeWndCtrl* treeWndCtrl = (Helium::TreeWndCtrl*) m_Window;
        treeWndCtrl->Delete( item );
        m_ItemData.SetId( Helium::TreeWndCtrlItemIdInvalid );
    }

    __super::Unrealize();
}

wxTreeItemId Panel::GetParentTreeNode(Container* parent)
{
    for ( ; parent != NULL; parent = parent->GetParent() )
    {
        Panel* parentPanel = Reflect::ObjectCast<Panel>( m_Parent );
        if ( parentPanel )
        {
            wxTreeItemId parentItem = parentPanel->m_ItemData.GetId();
            if ( parentItem != Helium::TreeWndCtrlItemIdInvalid )
            {
                return parentItem;
            }
        }
    }

    Helium::TreeWndCtrl* treeWndCtrl = (Helium::TreeWndCtrl*) m_Window;
    return treeWndCtrl->GetRootItem();
}

void Panel::Read()
{
    if ( IsBound() )
    {
        ReadStringData( m_Name );

        if ( m_Canvas )
        {
            wxTreeItemId item = m_ItemData.GetId();
            if ( item != Helium::TreeWndCtrlItemIdInvalid )
            {
                Helium::TreeWndCtrl* treeWndCtrl = (Helium::TreeWndCtrl*) m_Window;
                treeWndCtrl->SetItemText( item, m_Name );
            }
        }
    }

    __super::Read();
}


///////////////////////////////////////////////////////////////////////////////
// Overridden to remove any controls it added to the tree
// 
void Canvas::RemoveChild(Control* control)
{
    TreeCanvasCtrl* treeWndCtrl = GetControl();

    wxTreeItemId item = treeWndCtrl->FindItem( control->GetWindow() );
    if ( item != Helium::TreeWndCtrlItemIdInvalid )
    {
        treeWndCtrl->Delete( item );
    }

    __super::RemoveChild( control );
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the specified panel should be expanded, and false if it
// should be collapsed.
// 
ExpandState Canvas::GetPanelExpandState( const tstring& panelName ) const
{
    ExpandState state = ExpandStates::Default;
    M_ExpandState::const_iterator found = m_PanelExpandState.find( panelName );
    if ( found != m_PanelExpandState.end() )
    {
        state = found->second ? ExpandStates::Expanded : ExpandStates::Collapsed;
    }

    return state;
}

///////////////////////////////////////////////////////////////////////////////
// The canvas maintains a list of all the expanded panels so that the expansion
// state can be stored between calls to layout the UI.  Call this function
// when the user expands or collapses a panel to update the list.
// 
void Canvas::SetPanelExpandState( const tstring& panelName, ExpandState state )
{
    M_ExpandState::iterator found = m_PanelExpandState.find( panelName );
    bool isExpanded = false;

    switch ( state )
    {
    case ExpandStates::Expanded:
        if ( found != m_PanelExpandState.end() )
        {
            found->second = true;
        }
        else
        {
            m_PanelExpandState.insert( M_ExpandState::value_type( panelName, true ) );
        }
        break;

    case ExpandStates::Collapsed:
        if ( found != m_PanelExpandState.end() )
        {
            found->second = false;
        }
        else
        {
            m_PanelExpandState.insert( M_ExpandState::value_type( panelName, false ) );
        }
        break;

    case ExpandStates::Default:
        if ( found != m_PanelExpandState.end() )
        {
            m_PanelExpandState.erase( found );
        }
        break;
    }
}

void Canvas::SetPanelsExpanded(bool panelsExpanded)
{
    m_PanelsExpanded = panelsExpanded;
}

Math::Point Canvas::GetScroll()
{
    return Math::Point (Control::Cast<wxScrolledWindow>(this)->GetScrollPos(wxHORIZONTAL), Control::Cast<wxScrolledWindow>(this)->GetScrollPos(wxVERTICAL));
}

void Canvas::SetScroll(const Math::Point& scroll)
{
    Control::Cast<wxScrolledWindow>(this)->Scroll(scroll.x, scroll.y);
}


#endif