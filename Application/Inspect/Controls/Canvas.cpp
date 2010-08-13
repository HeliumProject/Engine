#include "Canvas.h"
#include "Application/Inspect/Interpreter.h"
#include "Application/Inspect/Controls/Controls.h"

using namespace Helium;
using namespace Helium::Inspect;

BEGIN_EVENT_TABLE( StripCanvasCtrl, wxPanel )
EVT_SHOW( StripCanvasCtrl::OnShow )
EVT_SIZE( StripCanvasCtrl::OnSize )
EVT_LEFT_DOWN( StripCanvasCtrl::OnClick )
END_EVENT_TABLE();

BEGIN_EVENT_TABLE( TreeCanvasCtrl, Helium::TreeWndCtrl )
EVT_SHOW( TreeCanvasCtrl::OnShow )
EVT_SIZE( TreeCanvasCtrl::OnSize )
EVT_LEFT_DOWN( TreeCanvasCtrl::OnClick )
EVT_TREE_ITEM_EXPANDED( wxID_ANY, TreeCanvasCtrl::OnToggle )
EVT_TREE_ITEM_COLLAPSED( wxID_ANY, TreeCanvasCtrl::OnToggle )
END_EVENT_TABLE();

//
// StripCanvasCtrl
//

StripCanvasCtrl::StripCanvasCtrl(wxWindow *parent, wxWindowID winid, const wxPoint& pos, const wxSize& size, long style, const wxString& name )
: wxPanel( parent, winid, pos, size, style, name )
, m_Canvas( NULL )
{
}

void StripCanvasCtrl::OnShow(wxShowEvent& event)
{
    if (m_Canvas)
    {
        m_Canvas->RaiseShow( event.GetShow() );
    }
}

void StripCanvasCtrl::OnSize(wxSizeEvent& event)
{
    Layout();

    event.Skip();
}

void StripCanvasCtrl::OnClick(wxMouseEvent& event)
{
    SetFocus();

    event.Skip();
}


//
// TreeCanvasCtrl
//

TreeCanvasCtrl::TreeCanvasCtrl(wxWindow *parent,
                               wxWindowID winid,
                               const wxPoint& pos,
                               const wxSize& size,
                               long style,
                               const wxString& name,
                               int treeStyle,
                               unsigned int columnSize,
                               wxBitmap expandedBitmap,
                               wxBitmap collapsedBitmap,
                               wxPen pen,
                               unsigned int clickTolerance)
                               : Helium::TreeWndCtrl(parent, winid, pos, size, wxALWAYS_SHOW_SB | style, name, treeStyle, columnSize, expandedBitmap, collapsedBitmap, pen, clickTolerance)
                               , m_Canvas(NULL)
{
    SetScrollRate(SCROLL_INCREMENT, SCROLL_INCREMENT);

    //wxALWAYS_SHOW_SB in the constructor will ensure that there is always scroll bars
    //in the canvas window.  This will prevent the need from doing layout on the window
    //more than once.
    //This function will ensure that the horizontal bar are never seen.  If a horizontal
    //bar might be necessary, then the best way would be to remove this and just let both
    //bars be seen. 
    ::ShowScrollBar( (HWND) GetHandle(), SB_HORZ, FALSE );
}

Canvas* TreeCanvasCtrl::GetCanvas()
{
    return m_Canvas;
}

void TreeCanvasCtrl::SetCanvas(Canvas* canvas)
{
    m_Canvas = canvas;
}

void TreeCanvasCtrl::OnShow(wxShowEvent& event)
{
    if (m_Canvas)
    {
        m_Canvas->RaiseShow( event.GetShow() );
    }
}

void TreeCanvasCtrl::OnSize(wxSizeEvent& event)
{
    // to prevent the scroll bar from appearing during an OnSize event, need to 
    // explicitly hide the scroll bar
    ::ShowScrollBar( (HWND) GetHandle(), SB_HORZ, FALSE );

    // to make sure the tree control fits to the width of the canvas, set the
    // virtual size
    int canvasPad = wxSystemSettings::GetMetric( wxSYS_VSCROLL_X ) + ( m_Canvas ? m_Canvas->GetPad() : 0 );
    int w = -1;

    int eventWidth = event.GetSize().GetWidth();
    if ( eventWidth > canvasPad )
    {
        w = eventWidth - canvasPad;
    }
    else
    {
        int virtualWidth = GetVirtualSize().GetWidth();
        if ( virtualWidth > canvasPad )
        {
            w = virtualWidth - canvasPad;
        }
    }

    SetVirtualSizeHints( w, -1, w, -1 );

    Layout();

    event.Skip();
}

void TreeCanvasCtrl::OnClick(wxMouseEvent& event)
{
    SetFocus();

    event.Skip();
}

void TreeCanvasCtrl::OnToggle(wxTreeEvent& event)
{
    wxTreeItemId item = event.GetItem();

    PanelItemData* panelItemData = (PanelItemData*) GetItemData( item );
    if ( !panelItemData->GetIgnoreToggle() )
    {
        Panel* panel = panelItemData->GetPanel();
        if ( panel )
        {
            bool newExpandState = !panel->IsExpanded();

            Canvas* canvas = panel->GetCanvas();
            if ( canvas )
            {
                canvas->SetPanelExpandState( panel->GetPath(), newExpandState ? ExpandStates::Expanded : ExpandStates::Collapsed );
            }

            panel->SetExpanded( newExpandState );
        }
    }

    event.Skip();
}

Canvas::Canvas ()
: m_IsLocked( false )
, m_PanelsExpanded( false )
{
    m_Canvas = this;
    m_StdSize = Math::Point (100, 20);
    m_Border = 4;
    m_Pad = 2;
}

Canvas::~Canvas()
{
    if (m_Window)
    {
        TreeCanvasCtrl* window = Control::Cast<TreeCanvasCtrl>(this);

        if (window->GetCanvas() == this)
        {
            window->SetCanvas(NULL);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// 
// 
TreeCanvasCtrl* Canvas::GetControl()
{
    return static_cast< TreeCanvasCtrl* >( m_Window );
}

///////////////////////////////////////////////////////////////////////////////
// 
// 
void Canvas::SetControl(TreeCanvasCtrl* control)
{
    m_Window = control;
    control->SetCanvas(this);
}

///////////////////////////////////////////////////////////////////////////////
// 
// 
ControlPtr Canvas::Create(int type, Interpreter* interpreter)
{
    INSPECT_SCOPE_TIMER( ("") );

    // create control
    ControlPtr control = Reflect::ObjectCast<Control>( Reflect::Registry::GetInstance()->CreateInstance( type ) );
    control->SetInterpreter( interpreter );
    control->SetCanvas( this );
    control->Create();

    return control;
}

///////////////////////////////////////////////////////////////////////////////
// Overridden to remove any controls it added to the tree
// 
void Canvas::RemoveControl(Control* control)
{
    TreeCanvasCtrl* treeWndCtrl = GetControl();

    wxTreeItemId item = treeWndCtrl->FindItem( control->GetWindow() );
    if ( item != Helium::TreeWndCtrlItemIdInvalid )
    {
        treeWndCtrl->Delete( item );
    }

    __super::RemoveControl( control );
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
        for ( V_Control::iterator itr = m_Controls.begin(), end = m_Controls.end(); itr != end; ++itr )
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
#if INSPECT_REFACTOR
        treeWndCtrl->SetImageList( Helium::GlobalFileIconsTable().GetSmallImageList() );
        treeWndCtrl->SetStateImageList( Helium::GlobalFileIconsTable().GetSmallImageList() );
#endif
    }

    wxTreeItemId root = treeWndCtrl->GetRootItem();
    if ( root == Helium::TreeWndCtrlItemIdInvalid )
    {
        root = treeWndCtrl->AddRoot( TXT( "Canvas Root" ) );
    }

    V_Control::const_iterator itr = m_Controls.begin();
    V_Control::const_iterator end = m_Controls.end();
    for( ; itr != end; ++itr )
    {
        Control* c = *itr;
        if ( m_PanelsExpanded )
        {
            Inspect::Panel* panel = Reflect::ObjectCast<Inspect::Panel>( c );
            if ( panel )
            {
                panel->SetShowTreeNode( false );
            }
        }

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

void Canvas::SetLocked(bool isLocked)
{
    m_IsLocked = isLocked;
    V_Control::iterator itr = m_Controls.begin();
    V_Control::iterator end = m_Controls.end();
    for( ; itr != end; ++itr )
    {
        // Push the enable state back down to the control (it will query back for the lock status).
        (*itr)->SetEnabled( (*itr)->IsEnabled() );
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
