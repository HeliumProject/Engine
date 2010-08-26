#include "Precompile.h"
#include "TreeCanvas.h"

using namespace Helium;
using namespace Helium::Editor;

#if 0

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

void ContainerRealized(Container* parent)
{
    if ( IsRealized() )
    {
        for ( V_Control::iterator itr = m_Children.begin(), end = m_Children.end(); itr != end; ++itr )
        {
            (*itr)->Realize( this );
        }
        return;
    }

    if ( m_Window == NULL )
    {
        INSPECT_SCOPE_TIMER( ( "" ) );
        m_Window = new wxPanel( parent->GetWindow(), wxID_ANY );
    }

    if ( m_Window->GetSizer() == NULL )
    {
        m_Window->SetSizer( new wxBoxSizer( wxHORIZONTAL ) );
        wxSizer* sizer = m_Window->GetSizer();

        V_Sizer sizerList;
        std::vector< i32 > proportionList;

        int proportionMultiplier = 1000;
        int remainingProportion = proportionMultiplier;
        int numRemainingProportions = 0;

        wxSizer* unboundedProportionSizer = NULL;

        V_Control::const_iterator itr = m_Children.begin();
        V_Control::const_iterator end = m_Children.end();
        for( ; itr != end; ++itr )
        {
            Control* c = *itr;
            c->Realize( this );

            if ( c->GetProportionalWidth() > 0.0f )
            {
                int proportion = (int) ( c->GetProportionalWidth() * (f32) proportionMultiplier );
                remainingProportion -= proportion;

                sizerList.push_back( new wxBoxSizer( wxHORIZONTAL ) );
                proportionList.push_back( proportion );
                unboundedProportionSizer = NULL;
            }
            else
            {
                if ( unboundedProportionSizer == NULL )
                {
                    unboundedProportionSizer = new wxBoxSizer( wxHORIZONTAL );
                    ++numRemainingProportions;
                }

                sizerList.push_back( unboundedProportionSizer );
                proportionList.push_back( -1 );
            }
        }

        if ( numRemainingProportions > 1 )
        {
            remainingProportion = (int) ( (f32) remainingProportion / (f32) numRemainingProportions + 0.5f );
        }

        int index = 0;
        int spacing = GetCanvas()->GetPad();

        itr = m_Children.begin();
        end = m_Children.end();
        for( ; itr != end; ++index, ++itr )
        {
            Control* c = *itr;

            int proportion = proportionList[ index ];
            wxSizer* currentSizer = sizerList[ index ];

            if ( sizer->GetItem( currentSizer ) == NULL )
            {
                sizer->Add( currentSizer, proportion > 0 ? proportion : remainingProportion, wxEXPAND | wxTOP | wxBOTTOM, spacing );
            }

            int flags = wxALIGN_CENTER_VERTICAL;
            proportion = 0;
            if ( !c->IsFixedWidth() )
            {
                proportion = 1;
                flags |= wxEXPAND;
            }

            currentSizer->Add( spacing, 0, 0 );
            currentSizer->Add( c->GetWindow(), proportion, flags );
        }
        sizer->Add(spacing, 0, 0);

        m_Window->Layout();
    }

    __super::Realize(parent);
}

void PanelRealized(Container* parent)
{
    if ( IsRealized() )
    {
        for ( V_Control::iterator itr = m_Controls.begin(), end = m_Controls.end(); itr != end; ++itr )
        {
            (*itr)->Realize( this );
        }
        return;
    }

    INSPECT_SCOPE_TIMER( ("") );

    ExpandState cachedState = m_Canvas->GetPanelExpandState( GetPath() );
    switch ( cachedState )
    {
        // The state was cached to be expanded
    case ExpandStates::Expanded:
        m_Expanded = true;
        break;

        // The state was cached to be collapsed
    case ExpandStates::Collapsed:
        m_Expanded = false;
        break;
    }

    m_Parent = parent;

    Helium::TreeWndCtrl* treeWndCtrl = (Helium::TreeWndCtrl*) m_Parent->GetWindow();

    m_Window = treeWndCtrl;
    treeWndCtrl->Freeze();

#ifdef INSPECT_REFACTOR
    int collapsedIndex = Helium::GlobalFileIconsTable().GetIconID( TXT( "ms_folder_closed" ) );
    int expandedIndex = Helium::GlobalFileIconsTable().GetIconID( TXT( "ms_folder_open" ) );
#else
    int collapsedIndex = -1;
    int expandedIndex = -1;
#endif

    wxTreeItemId item = m_ItemData.GetId();
    if ( item == Helium::TreeWndCtrlItemIdInvalid )
    {
        treeWndCtrl->AppendItem( GetParentTreeNode( m_Parent ), m_Name, collapsedIndex, expandedIndex, &m_ItemData );
        item = m_ItemData.GetId();
    }

    V_Control::const_iterator beg = m_Controls.begin();
    V_Control::const_iterator end = m_Controls.end();
    for( V_Control::const_iterator itr = beg; itr != end; ++itr )
    {
        Control* c = *itr;

        c->Realize( this );

        wxWindow* window = c->GetWindow();

        if ( window != treeWndCtrl )
        {
            treeWndCtrl->AppendItem( item, window );
        }
    }

    m_Window->Layout();

    treeWndCtrl->Thaw();

    __super::Realize( parent );
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
// Overridden to remove any controls and delete the root of the tree
// 
void Canvas::Clear()
{
    TreeCanvasCtrl* treeWndCtrl = GetControl();
    treeWndCtrl->DeleteAllItems();

    __super::Clear();
}



///////////////////////////////////////////////////////////////////////////////
// 
// 
void Canvas::Realize(Container* parent)
{
    if ( IsRealized() )
    {
        for ( V_Control::iterator itr = m_Children.begin(), end = m_Children.end(); itr != end; ++itr )
        {
            (*itr)->Realize( this );
        }
        return;
    }

    // the canvas must already be realized - this just realizes its children
    HELIUM_ASSERT( m_Window != NULL );

    TreeCanvasCtrl* treeWndCtrl = GetControl();
    treeWndCtrl->SetColumnSize( 15 );
    if ( ( treeWndCtrl->GetStateImageList() == NULL ) && ( treeWndCtrl->GetImageList() == NULL ) )
    {
#ifdef INSPECT_REFACTOR
        treeWndCtrl->SetImageList( Helium::GlobalFileIconsTable().GetSmallImageList() );
        treeWndCtrl->SetStateImageList( Helium::GlobalFileIconsTable().GetSmallImageList() );
#endif
    }

    wxTreeItemId root = treeWndCtrl->GetRootItem();
    if ( root == Helium::TreeWndCtrlItemIdInvalid )
    {
        root = treeWndCtrl->AddRoot( TXT( "Canvas Root" ) );
    }

    V_Control::const_iterator itr = m_Children.begin();
    V_Control::const_iterator end = m_Children.end();
    for( ; itr != end; ++itr )
    {
        Control* c = *itr;

        c->Realize( this );

        wxWindow* window = c->GetWindow();
        if ( window != m_Window )
        {
            treeWndCtrl->AppendItem( root, window );
        }
    }

    treeWndCtrl->Freeze();
    treeWndCtrl->Layout();
    treeWndCtrl->Scroll( 0, 0 );
    treeWndCtrl->Thaw();
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