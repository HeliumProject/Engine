#include "Precompile.h"

#include "Drawer.h"

#include "Foundation/Flags.h"

using namespace Helium;
using namespace Helium::Editor;

static int s_PopupOffsetY = 0; // Pixel offset for top of popup window

static int s_ButtonMouseOverDelayMilliseconds = 500;
static int s_MouseLocationTimerFrequency = 100;

Drawer::Drawer( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
: wxPanel( parent, id, pos, size, style )
, m_Parent( parent )
, m_Icon( wxART_MISSING_IMAGE )
, m_Button( NULL )
, m_Panel( NULL )
, m_ButtonStyle( DrawerButtonStyles::Default )
, m_FloatingPosition( wxDefaultPosition )
, m_AuiManager( NULL )
, m_PaneInfo( NULL )
, m_CurrentFrame( NULL )
{
    wxSizer* sizer = new wxBoxSizer( wxHORIZONTAL );
    SetSizer( sizer );

    // Set up the button
    m_Button = new Button( this, wxID_ANY, GetLabel(), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
    m_Button->SetButtonOptions( ButtonOptions::Toggle );
    m_Button->SetBitmap( wxArtProvider::GetBitmap( ArtIDs::Actions::Down, wxART_OTHER, wxSize( 8, 8 ) ) );
    m_Button->SetBitmapPosition( wxDirection( wxRIGHT ) );
    m_Button->SetBitmapMargins( 1, 1 );
    //m_Button->SetButtonOptions( ButtonOptions::Toggle );
    m_Button->SetValue( false );

    // Add the button to the sizer
    sizer->Add( m_Button, 1, wxEXPAND | wxALL, 0 );

    // Connect listeners
    if ( HasFlags<DrawerButtonStyle>( m_ButtonStyle, DrawerButtonStyles::ClickToOpen ) )
    {
        m_Button->Connect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( Drawer::OnButtonClicked ), NULL, this );
    }

    if ( HasFlags<DrawerButtonStyle>( m_ButtonStyle, DrawerButtonStyles::MouseOverToOpen ) )
    {
        m_Button->Connect( wxEVT_ENTER_WINDOW, wxMouseEventHandler( Drawer::OnMouseEnterButton ), NULL, this );
        m_Button->Connect( wxEVT_LEAVE_WINDOW, wxMouseEventHandler( Drawer::OnMouseLeaveButton ), NULL, this );
        
        m_MouseHoverTimer.SetOwner( this );
        Connect( m_MouseHoverTimer.GetId(), wxEVT_TIMER, wxTimerEventHandler( Drawer::OnMouseHoverTimer ), NULL, this );

        m_MouseLocationTimer.SetOwner( this );
        Connect( m_MouseLocationTimer.GetId(), wxEVT_TIMER, wxTimerEventHandler( Drawer::OnMouseLocationTimer ), NULL, this );
    }

    Layout();
}

Drawer::~Drawer()
{
    // Disconnect listeners
    if ( HasFlags<DrawerButtonStyle>( m_ButtonStyle, DrawerButtonStyles::ClickToOpen ) )
    {
        m_Button->Disconnect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( Drawer::OnButtonClicked ), NULL, this );
    }

    if ( HasFlags<DrawerButtonStyle>( m_ButtonStyle, DrawerButtonStyles::MouseOverToOpen ) )
    {
        m_Button->Disconnect( wxEVT_ENTER_WINDOW, wxMouseEventHandler( Drawer::OnMouseEnterButton ), NULL, this );
        m_Button->Disconnect( wxEVT_LEAVE_WINDOW, wxMouseEventHandler( Drawer::OnMouseLeaveButton ), NULL, this );
        Disconnect( m_MouseHoverTimer.GetId(), wxEVT_TIMER, wxTimerEventHandler( Drawer::OnMouseHoverTimer ), NULL, this );
        Disconnect( m_MouseLocationTimer.GetId(), wxEVT_TIMER, wxTimerEventHandler( Drawer::OnMouseLocationTimer ), NULL, this );
    }

    DestroyWindow();

}

void Drawer::DestroyWindow()
{
    if ( !m_CurrentFrame )
    {
        return;
    }

    m_CurrentFrame->Hide();

    if ( HasFlags<DrawerButtonStyle>( m_ButtonStyle, DrawerButtonStyles::MouseOverToOpen ) )
    {
        m_MouseLocationTimer.Stop();
    }

    m_Panel->Reparent( m_Parent );

    if ( m_AuiManager )
    {
        //m_CurrentFrame->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( Drawer::OnCloseAuiPane ), NULL, this );
        m_AuiManager->DetachPane( m_CurrentFrame );

        HELIUM_ASSERT( m_PaneInfo );
        
        delete m_PaneInfo;
        m_PaneInfo = NULL;
    }
    else
    {
        delete m_CurrentFrame;
    }

    m_CurrentFrame = NULL;
}

void Drawer::SetAuiManager( wxAuiManager* auiManager )
{
    HELIUM_ASSERT( auiManager != m_AuiManager );

    DestroyWindow();

    m_AuiManager = auiManager;
}

void Drawer::SetLabel( const wxString& label )
{
    __super::SetLabel( label );

    m_Button->SetLabel( GetLabel() );
    m_Button->SetSize( m_Button->GetBestSize() );
    m_Button->SetMinSize( m_Button->GetBestSize() );

    if ( m_AuiManager && m_PaneInfo )
    {
        m_PaneInfo->Name( GetLabel() );
    }

    if ( m_CurrentFrame )
    {
        m_CurrentFrame->SetLabel( GetLabel() );
    }

    Layout();
}

void Drawer::SetIcon( const tstring& icon )
{
    m_Button->SetBitmap( wxArtProvider::GetIcon( (wxArtID)m_Icon, wxART_OTHER, wxSize( 16, 16 ) ) );
    m_Button->SetSize( m_Button->GetBestSize() );
    m_Button->SetMinSize( m_Button->GetBestSize() );

    Layout();
}

Button* Drawer::GetButton()
{
    return (Button*) m_Button;
}

int32_t Drawer::GetButtonID() const
{
    if ( m_Button )
    {
        return m_Button->GetId();
    }
        
    return wxID_ANY;
}

wxPanel* Drawer::GetPanel()
{
    if ( !m_Panel )
    {
        m_Panel = new wxPanel( this, wxID_ANY );
        
        wxBoxSizer* sizer = new wxBoxSizer( wxHORIZONTAL );
        m_Panel->SetSizer( sizer );

        m_Panel->Hide();
    }
    return m_Panel;
}

void Drawer::SetPanel( wxPanel* panel )
{
    HELIUM_ASSERT( !m_CurrentFrame );

    m_Panel = panel;
}

void Drawer::Open()
{
    e_Opening.Raise( DrawerEventArgs( this ) );

    if ( !m_CurrentFrame )
    {
        if ( m_AuiManager )
        {
            HELIUM_ASSERT( !m_PaneInfo );
            m_PaneInfo = new wxAuiPaneInfo();
            m_PaneInfo->Name( GetLabel() ).Floatable( true ).Float().Dockable( false ).Show( false );

            m_AuiManager->AddPane( m_Panel, *m_PaneInfo );

            m_CurrentFrame = m_PaneInfo->frame;
            //m_CurrentFrame->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( Drawer::OnCloseAuiPane ), NULL, this );
        }
        else
        {
            m_CurrentFrame = new wxFrame( m_Parent, wxID_ANY, GetLabel(), wxDefaultPosition, wxDefaultSize, wxFRAME_FLOAT_ON_PARENT|wxFRAME_NO_TASKBAR|wxTAB_TRAVERSAL );
            m_CurrentFrame->SetSizeHints( wxDefaultSize, wxDefaultSize );
        	
            wxBoxSizer* frameSizer = new wxBoxSizer( wxVERTICAL );
	        frameSizer->Add( m_Panel, 1, wxEXPAND | wxALL, 0 );
        	
	        m_CurrentFrame->SetSizer( frameSizer );
        }
    }

    if ( HasFlags<DrawerButtonStyle>( m_ButtonStyle, DrawerButtonStyles::MouseOverToOpen ) )
    {
        m_MouseLocationTimer.Start( s_MouseLocationTimerFrequency );
    }

    m_Panel->Reparent( m_CurrentFrame );
    m_Panel->Show();

    wxSize frameSize( m_CurrentFrame->GetBestSize() );
    m_CurrentFrame->SetSize( frameSize );
    m_CurrentFrame->SetMinSize( frameSize );
    m_CurrentFrame->Layout();

    wxRect buttonRect = m_Button->GetScreenRect();
    m_FloatingPosition = buttonRect.GetBottomLeft();
    m_CurrentFrame->SetPosition( m_FloatingPosition );
    m_CurrentFrame->ShowWithEffect( wxSHOW_EFFECT_SLIDE_TO_BOTTOM, 100 );
    
    if ( m_AuiManager )
    {
        m_AuiManager->Update();
    }

    e_Opened.Raise( DrawerEventArgs( this ) );
}

void Drawer::Close()
{
    e_Closing.Raise( DrawerEventArgs( this ) );
    
    if ( HasFlags<DrawerButtonStyle>( m_ButtonStyle, DrawerButtonStyles::MouseOverToOpen ) )
    {
        m_MouseLocationTimer.Stop();
    }
    
    if ( m_CurrentFrame )
    {
        m_Panel->Hide();
        m_CurrentFrame->HideWithEffect( wxSHOW_EFFECT_SLIDE_TO_TOP, 100 );

        if ( m_AuiManager )
        {
            //m_CurrentFrame->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( Drawer::OnCloseAuiPane ), NULL, this );
        }
    }

    m_Button->SetValue( false );

    e_Closed.Raise( DrawerEventArgs( this ) );
}

bool Drawer::IsOpen() const
{    
    return ( m_CurrentFrame && m_CurrentFrame->IsShown() );
}

void Drawer::OnMouseLocationTimer( wxTimerEvent& args )
{
    args.Skip();

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

    // early out of mouse events unless MouseOverToOpen option 
    if ( !HasFlags<DrawerButtonStyle>( m_ButtonStyle, DrawerButtonStyles::MouseOverToOpen ) )
    {
        return;
    }

    m_MouseHoverTimer.Start( s_ButtonMouseOverDelayMilliseconds, true );
}

void Drawer::OnMouseLeaveButton( wxMouseEvent& args )
{
    args.Skip();

    // early out of mouse events unless MouseOverToOpen option 
    if ( !HasFlags<DrawerButtonStyle>( m_ButtonStyle, DrawerButtonStyles::MouseOverToOpen ) )
    {
        return;
    }
    
    m_MouseHoverTimer.Stop();
}

void Drawer::OnMouseHoverTimer( wxTimerEvent& args )
{
    args.Skip();

    // early out of mouse events unless MouseOverToOpen option 
    if ( !HasFlags<DrawerButtonStyle>( m_ButtonStyle, DrawerButtonStyles::MouseOverToOpen ) )
    {
        return;
    }

    if ( !IsOpen() && HasMouseFocus() )
    {
        Open();
    }
}

bool Drawer::HasMouseFocus()
{
    wxRect buttonRect = m_Button->GetScreenRect();

    wxRect frameRect;
    if ( m_CurrentFrame )
    {
        frameRect = m_CurrentFrame->GetScreenRect();   
    }

    wxPoint mousePos = ::wxGetMousePosition();

    return ( buttonRect.Contains( mousePos ) || frameRect.Contains( mousePos ) );
}
