#include "Precompile.h"

#include "Drawer.h"

#include "Foundation/Flags.h"

using namespace Helium;
using namespace Helium::Editor;

static int s_PopupOffsetY = 0; // Pixel offset for top of popup window

static int s_ButtonMouseOverDelayMilliseconds = 500;

Drawer::Drawer( wxWindow* parent, wxPanel* panel, const wxString& title, const wxBitmap& icon, const DrawerButtonStyle style ) 
: m_Parent( parent )
, m_Panel( panel )
, m_Title( title )
, m_Icon( icon )
, m_Style( style )
, m_Button( NULL )
, m_FloatingPosition( wxDefaultPosition )
, m_AuiManager( NULL )
, m_PaneInfo( NULL )
, m_DrawerWindow( NULL )
{
    // Set up the button
    m_Button = new wxToggleButton( m_Parent, wxID_ANY, m_Title, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
    m_Button->SetValue( false );
    //m_Button->SetBitmap( m_Icon );
    //m_Button->SetMinSize( m_Button->GetBestSize() ); // this should only need to be called after changing the size (padding, margins, etc);

    // Connect listeners
    if ( HasFlags<DrawerButtonStyle>( m_Style, DrawerButtonStyles::ClickToOpen ) )
    {
        m_Button->Connect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( Drawer::OnButtonClicked ), NULL, this );
    }

    if ( HasFlags<DrawerButtonStyle>( m_Style, DrawerButtonStyles::MouseOverToOpen ) )
    {
        m_Button->Connect( wxEVT_ENTER_WINDOW, wxMouseEventHandler( Drawer::OnMouseEnterButton ), NULL, this );
        m_Button->Connect( wxEVT_LEAVE_WINDOW, wxMouseEventHandler( Drawer::OnMouseLeaveButton ), NULL, this );
        
        m_MouseHoverTimer.SetOwner( this );
        Connect( m_MouseHoverTimer.GetId(), wxEVT_TIMER, wxTimerEventHandler( Drawer::OnMouseHoverTimer ), NULL, this );
    }    
}

Drawer::~Drawer()
{
    // Disconnect listeners
    if ( HasFlags<DrawerButtonStyle>( m_Style, DrawerButtonStyles::ClickToOpen ) )
    {
        m_Button->Disconnect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( Drawer::OnButtonClicked ), NULL, this );
    }

    if ( HasFlags<DrawerButtonStyle>( m_Style, DrawerButtonStyles::MouseOverToOpen ) )
    {
        m_Button->Disconnect( wxEVT_ENTER_WINDOW, wxMouseEventHandler( Drawer::OnMouseEnterButton ), NULL, this );
        m_Button->Disconnect( wxEVT_LEAVE_WINDOW, wxMouseEventHandler( Drawer::OnMouseLeaveButton ), NULL, this );
        Disconnect( m_MouseHoverTimer.GetId(), wxEVT_TIMER, wxTimerEventHandler( Drawer::OnMouseHoverTimer ), NULL, this );
    }

    DestroyWindow();

}

void Drawer::DestroyWindow()
{
    if ( !m_DrawerWindow )
    {
        return;
    }

    m_DrawerWindow->Hide();

    if ( HasFlags<DrawerButtonStyle>( m_Style, DrawerButtonStyles::MouseOverToOpen ) )
    {
        m_Panel->Disconnect( wxEVT_LEAVE_WINDOW, wxMouseEventHandler( Drawer::OnMouseLeaveDrawer ), NULL, this );
    }

    m_Panel->Reparent( m_Parent );

    if ( m_AuiManager )
    {
        //m_DrawerWindow->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( Drawer::OnCloseAuiPane ), NULL, this );
        m_AuiManager->DetachPane( m_DrawerWindow );

        HELIUM_ASSERT( m_PaneInfo );
        
        delete m_PaneInfo;
        m_PaneInfo = NULL;
    }
    else
    {
        delete m_DrawerWindow;
    }

    m_DrawerWindow = NULL;
}

void Drawer::SetAuiManager( wxAuiManager* auiManager )
{
    HELIUM_ASSERT( auiManager != m_AuiManager );

    DestroyWindow();

    m_AuiManager = auiManager;
}

const wxString& Drawer::GetTitle() const
{
    return m_Title;
}

const wxBitmap& Drawer::GetIcon() const
{
    return m_Icon;
}

wxToggleButton* Drawer::GetButton()
{
    return m_Button;
}

int32_t Drawer::GetButtonID() const
{
    if ( m_Button )
    {
        return m_Button->GetId();
    }
        
    return wxID_ANY;
}

void Drawer::Open()
{
    e_Opening.Raise( DrawerEventArgs( this ) );

    if ( !m_DrawerWindow )
    {
        if ( m_AuiManager )
        {
            HELIUM_ASSERT( !m_PaneInfo );
            m_PaneInfo = new wxAuiPaneInfo();
            m_PaneInfo->Name( m_Title ).Floatable( true ).Float().Dockable( false ).Show( false );

            m_AuiManager->AddPane( m_Panel, *m_PaneInfo );

            m_DrawerWindow = m_PaneInfo->frame;
            //m_DrawerWindow->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( Drawer::OnCloseAuiPane ), NULL, this );
        }
        else
        {            
            m_DrawerWindow = new wxFrame( m_Parent, wxID_ANY, m_Title, wxDefaultPosition, wxDefaultSize, wxFRAME_FLOAT_ON_PARENT|wxFRAME_NO_TASKBAR|wxTAB_TRAVERSAL );
            m_DrawerWindow->SetSizeHints( wxDefaultSize, wxDefaultSize );
	        wxBoxSizer* frameSizer;
	        frameSizer = new wxBoxSizer( wxVERTICAL );
        	
	        frameSizer->Add( m_Panel, 1, wxEXPAND, 0 );
        	
	        m_DrawerWindow->SetSizer( frameSizer );
        }
    }

    m_Panel->Reparent( m_DrawerWindow );
    m_Panel->Show();
    m_DrawerWindow->Layout();

    wxRect buttonRect = m_Button->GetScreenRect();
    m_FloatingPosition = buttonRect.GetBottomLeft();
    m_DrawerWindow->SetPosition( m_FloatingPosition );
    m_DrawerWindow->ShowWithEffect( wxSHOW_EFFECT_SLIDE_TO_BOTTOM, 100 );
    
    if ( HasFlags<DrawerButtonStyle>( m_Style, DrawerButtonStyles::MouseOverToOpen ) )
    {
        m_Panel->Connect( wxEVT_LEAVE_WINDOW, wxMouseEventHandler( Drawer::OnMouseLeaveDrawer ), NULL, this );
    }

    if ( m_AuiManager )
    {
        m_AuiManager->Update();
    }

    m_Button->SetValue( true );

    e_Opened.Raise( DrawerEventArgs( this ) );
}

void Drawer::Close()
{
    e_Closing.Raise( DrawerEventArgs( this ) );
    
    if ( m_DrawerWindow )
    {
        m_DrawerWindow->HideWithEffect( wxSHOW_EFFECT_SLIDE_TO_TOP, 100 );

        if ( HasFlags<DrawerButtonStyle>( m_Style, DrawerButtonStyles::MouseOverToOpen ) )
        {
            m_Panel->Disconnect( wxEVT_LEAVE_WINDOW, wxMouseEventHandler( Drawer::OnMouseLeaveDrawer ), NULL, this );
        }

        if ( m_AuiManager )
        {
            //m_DrawerWindow->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( Drawer::OnCloseAuiPane ), NULL, this );
        }
    }

    m_Button->SetValue( false );

    e_Closed.Raise( DrawerEventArgs( this ) );
}

bool Drawer::IsOpen() const
{    
    return ( m_DrawerWindow && m_DrawerWindow->IsShown() );
}

void Drawer::OnMouseLeaveDrawer( wxMouseEvent& args )
{
    if ( IsOpen() && !HasMouseFocus() )
    {
        Close();
    } 
}

void Drawer::OnButtonClicked( wxCommandEvent& args )
{
    if ( IsOpen() )
    {
        Close();
    }
    else
    {
        // Open this drawer
        Open();
    }
}

void Drawer::OnMouseEnterButton( wxMouseEvent& args )
{
    args.Skip();

    m_MouseHoverTimer.Start( s_ButtonMouseOverDelayMilliseconds, true );
}

void Drawer::OnMouseLeaveButton( wxMouseEvent& args )
{
    args.Skip();

    if ( IsOpen() && !HasMouseFocus() )
    {
        Close();
    }
    m_MouseHoverTimer.Stop();
}

void Drawer::OnMouseHoverTimer( wxTimerEvent& args )
{
    args.Skip();

    if ( !IsOpen() && HasMouseFocus() )
    {
        Open();
    }
}

bool Drawer::HasMouseFocus()
{
    wxRect buttonRect = m_Button->GetScreenRect();

    wxRect frameRect;
    if ( m_DrawerWindow )
    {
        frameRect = m_DrawerWindow->GetScreenRect();   
    }

    wxPoint mousePos = ::wxGetMousePosition();

    return ( buttonRect.Contains( mousePos ) || frameRect.Contains( mousePos ) );
}