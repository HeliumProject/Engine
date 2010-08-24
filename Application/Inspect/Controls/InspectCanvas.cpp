#include "Application/Inspect/Controls/InspectCanvas.h"
#include "Application/Inspect/InspectInterpreter.h"
#include "Application/Inspect/InspectControls.h"

using namespace Helium;
using namespace Helium::Inspect;

Canvas::Canvas ()
: m_PanelsExpanded( false )
{
    m_Canvas = this;
    m_StdSize = Math::Point (100, 20);
    m_Border = 4;
    m_Pad = 2;
}

Canvas::~Canvas()
{

}

void Canvas::RealizeControl(Control* control, Control* parent)
{

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
