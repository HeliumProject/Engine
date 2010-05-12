#include "Precompile.h"
#include "DrawerPanel.h"

#include "UIToolKit/Button.h"
#include "UIToolKit/ImageManager.h"

using namespace Luna;

DrawerPanel::DrawerPanel( wxWindow* parent )
: wxPanel( parent )
, m_CurrentDrawer( NULL )
, m_IsFixedSizeButtons( false )
, m_ClickToOpen( true )
{
  wxBoxSizer* sizer = new wxBoxSizer( wxHORIZONTAL );
  SetSizer( sizer );
}

DrawerPanel::~DrawerPanel()
{
  DestroyDrawers();
}

void DrawerPanel::AddDrawer( Drawer* drawer )
{
  const u32 style = wxBU_AUTODRAW;
  const u32 buttonStyle = UIToolKit::ButtonStyles::BU_DEFAULT | UIToolKit::ButtonStyles::BU_TOGGLE;
  const u32 margins = 10;

  // Set up the button
  UIToolKit::Button* button = new UIToolKit::Button( this, wxID_ANY, drawer->GetIcon(), wxDefaultPosition, wxDefaultSize, style );
  m_Buttons.insert( std::make_pair( button->GetId(), button ) );
  button->SetLabel( drawer->GetTitle() );
  button->SetBitmapAlignment();
  button->SetButtonStyle( buttonStyle );
  button->SetMargins( margins );
  button->SetPadding( 10 );
  button->SetToggleState( drawer->GetPane()->IsShown() );
  button->SetMinSize( button->GetBestSize() );    // this should be called after anything that changes the size (padding, margins, etc);

  // Keep track of this drawer
  drawer->SetDrawerID( button->GetId() );
  m_Drawers.insert( drawer );

  // Add the button to the sizer
  GetSizer()->Add( button, m_IsFixedSizeButtons ? 0 : 1, wxEXPAND | wxALL, 0 );

  // Add listeners
  button->Connect( button->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DrawerPanel::OnDrawerButtonClicked ), NULL, this );
  drawer->AddOpenListener( DrawerSignature::Delegate( this, &DrawerPanel::OnDrawerOpened ) );
  drawer->AddCloseListener( DrawerSignature::Delegate( this, &DrawerPanel::OnDrawerClosed ) );

  if ( !m_ClickToOpen )
  {
    button->Connect( button->GetId(), wxEVT_ENTER_WINDOW, wxMouseEventHandler( DrawerPanel::OnMouseEnterButton ), NULL, this );
  }
}

Drawer* DrawerPanel::GetCurrentDrawer() const
{
  return m_CurrentDrawer;
}

bool DrawerPanel::IsFixedSizeButtons() const
{
  return m_IsFixedSizeButtons;
}

void DrawerPanel::SetFixedSizeButtons( bool fixedSize )
{
  if ( fixedSize != m_IsFixedSizeButtons )
  {
    // TODO
    //m_IsFixedSizeButtons = fixedSize;
    //Freeze();
    //DestroyDrawers();
    //GetSizer()->Clear( true );
    //CreateDrawers();
    //Layout();
    //Thaw();
  }
}

bool DrawerPanel::GetClickToOpen() const
{
  return m_ClickToOpen;
}

void DrawerPanel::SetClickToOpen( bool value )
{
  m_ClickToOpen = value;

  M_Button::const_iterator buttonItr = m_Buttons.begin();
  M_Button::const_iterator buttonEnd = m_Buttons.end();
  for ( ; buttonItr != buttonEnd; ++buttonItr )
  {
    UIToolKit::Button* button = buttonItr->second;

    if ( m_ClickToOpen )
    {
      button->Disconnect( button->GetId(), wxEVT_ENTER_WINDOW, wxMouseEventHandler( DrawerPanel::OnMouseEnterButton ), NULL, this );
    }
    else
    {
      button->Connect( button->GetId(), wxEVT_ENTER_WINDOW, wxMouseEventHandler( DrawerPanel::OnMouseEnterButton ), NULL, this );
    }
  }
}

void DrawerPanel::DestroyDrawers()
{
  if ( m_CurrentDrawer )
  {
    m_CurrentDrawer->CloseDrawer();
  }

  S_Drawer::const_iterator drawerItr = m_Drawers.begin();
  S_Drawer::const_iterator drawerEnd = m_Drawers.end();
  for ( ; drawerItr != drawerEnd; ++drawerItr )
  {
    Drawer* drawer = *drawerItr;
    drawer->RemoveOpenListener( DrawerSignature::Delegate( this, &DrawerPanel::OnDrawerOpened ) );
    drawer->RemoveCloseListener( DrawerSignature::Delegate( this, &DrawerPanel::OnDrawerClosed ) );
    delete drawer;
  }
  m_Drawers.clear();

  // Not really necessary since the panel outlives the contained controls
  // button->Disconnect( button->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DrawerPanel::OnDrawerButtonClicked ), NULL, this );
  m_Buttons.clear();
}

Drawer* DrawerPanel::FindDrawer( i32 drawerID )
{
  S_Drawer::const_iterator drawerItr = m_Drawers.begin();
  S_Drawer::const_iterator drawerEnd = m_Drawers.end();
  for ( ; drawerItr != drawerEnd; ++drawerItr )
  {
    if ( ( *drawerItr )->GetDrawerID() == drawerID )
    {
      return *drawerItr;
    }
  }
  return NULL;
}

UIToolKit::Button* DrawerPanel::GetButtonForDrawer( Drawer* drawer )
{
  UIToolKit::Button* button = NULL;
  M_Button::const_iterator found = m_Buttons.find( drawer->GetDrawerID() );
  if ( found != m_Buttons.end() )
  {
    button = found->second;
  }
  return button;
}

void DrawerPanel::ToggleDrawerButton( Drawer* drawer, bool pressed )
{
  GetButtonForDrawer( drawer )->SetToggleState( pressed );
}

void DrawerPanel::OnDrawerButtonClicked( wxCommandEvent& args )
{
  Drawer* drawer = FindDrawer( args.GetId() );
  if ( drawer )
  {
    if ( drawer->IsOpen() )
    {
      drawer->CloseDrawer();
    }
    else
    {
      // Close all other drawers
      S_Drawer::const_iterator drawerItr = m_Drawers.begin();
      S_Drawer::const_iterator drawerEnd = m_Drawers.end();
      for ( ; drawerItr != drawerEnd; ++drawerItr )
      {
        if ( *drawerItr != drawer )
        {
          ( *drawerItr )->CloseDrawer();
        }
      }

      // Open this drawer
      if ( !drawer->IsOpen() )
      {
        UIToolKit::Button* button = GetButtonForDrawer( drawer );
        wxRect buttonRect = button->GetScreenRect();
        drawer->OpenDrawer( buttonRect.GetBottomLeft() );
        if ( !drawer->IsOpen() )
        {
          NOC_BREAK();
        }
      }
    }
  }
}

void DrawerPanel::OnDrawerOpened( const DrawerArgs& args )
{
  if ( m_CurrentDrawer != args.m_Drawer )
  {
    if ( m_CurrentDrawer )
    {
      ToggleDrawerButton( m_CurrentDrawer, false );
    }
    
    m_CurrentDrawer = args.m_Drawer;
  }

  if ( m_CurrentDrawer )
  {
    ToggleDrawerButton( m_CurrentDrawer, true );
  }
}

void DrawerPanel::OnDrawerClosed( const DrawerArgs& args )
{
  ToggleDrawerButton( args.m_Drawer, false );
  if ( args.m_Drawer == m_CurrentDrawer )
  {
    m_CurrentDrawer = NULL;
  }
}

void DrawerPanel::OnMouseEnterButton( wxMouseEvent& args )
{
  args.Skip();
  UIToolKit::Button* button = wxDynamicCast( args.GetEventObject(), UIToolKit::Button );
  if ( button )
  {
    Drawer* drawer = FindDrawer( button->GetId() );
    if ( drawer != m_CurrentDrawer )
    {
      button->ProcessCommand( wxCommandEvent( wxEVT_COMMAND_BUTTON_CLICKED, button->GetId() ) );
    }
  }
}
