#include "Precompile.h"
#include "WindowSettings.h"

#include "Common/CommandLine.h"

#include <wx/aui/aui.h>
#include <wx/display.h>

using namespace Luna;

// Definition
REFLECT_DEFINE_CLASS( WindowSettings )

void WindowSettings::EnumerateClass( Reflect::Compositor<WindowSettings>& comp )
{
  Reflect::Field* fieldVersion = comp.AddField( &WindowSettings::m_Version, "m_Version" );
  Reflect::Field* fieldDockingState = comp.AddField( &WindowSettings::m_DockingState, "m_DockingState" );
  Reflect::Field* fieldIsMaximized = comp.AddField( &WindowSettings::m_IsMaximized, "m_IsMaximized" );
  Reflect::Field* fieldPosX = comp.AddField( &WindowSettings::m_PosX, "m_PosX" );
  Reflect::Field* fieldPosY = comp.AddField( &WindowSettings::m_PosY, "m_PosY" );
  Reflect::Field* fieldWidth = comp.AddField( &WindowSettings::m_Width, "m_Width" );
  Reflect::Field* fieldHeight = comp.AddField( &WindowSettings::m_Height, "m_Height" );
}

// Statics
const char* WindowSettings::s_Reset = "resetWin";
const char* WindowSettings::s_ResetLong = "ResetWindowSettings";


///////////////////////////////////////////////////////////////////////////////
// Static initialization.
// 
void WindowSettings::InitializeType()
{
  Reflect::RegisterClass<WindowSettings>( "WindowSettings" );
}

///////////////////////////////////////////////////////////////////////////////
// Static cleanup.
// 
void WindowSettings::CleanupType()
{
  Reflect::UnregisterClass<WindowSettings>();
}

///////////////////////////////////////////////////////////////////////////////
// Checks the window settings against the provided version number and recreates 
// the window settings if necessary.
// 
void WindowSettings::CheckWindowSettings( WindowSettingsPtr& settings, const std::string& version )
{
  if ( !settings )
  {
    settings = new WindowSettings( version );
  }
  else if ( settings->GetCurrentVersion() != version )
  {
    settings = new WindowSettings( version );
  }
  else if ( Nocturnal::GetCmdLineFlag( s_Reset ) )
  {
    settings = new WindowSettings( version );
}
}

///////////////////////////////////////////////////////////////////////////////
// Determines if the specified settings are valid and returns true if so.
// 
bool WindowSettings::ValidatePositionAndSize( wxPoint pos, wxSize size )
{
  // If you have a default x position, y must match and vice versa
  if ( pos.x == wxDefaultPosition.x && pos.y != wxDefaultPosition.y || pos.x != wxDefaultPosition.x && pos.y == wxDefaultPosition.y )
  {
    return false;
  }

  // If you have a default width, height must match and vice versa
  if ( size.x == wxDefaultSize.x && size.y != wxDefaultSize.y || size.x != wxDefaultSize.x && size.y == wxDefaultSize.y )
  {
    return false;
  }

  // Make sure each of the four corners of the window lie within one of the 
  // currently connected displays.
  if ( pos != wxDefaultPosition )
  {
    if ( wxDisplay::GetFromPoint( pos ) == wxNOT_FOUND )
    {
      return false;
    }

    if ( size != wxDefaultSize )
    {
      if ( wxDisplay::GetFromPoint( pos + wxPoint( size.x, 0 ) ) == wxNOT_FOUND )
      {
        return false;
      }
      else if ( wxDisplay::GetFromPoint( pos + wxPoint( 0, size.y ) ) == wxNOT_FOUND )
      {
        return false;
      }
      else if ( wxDisplay::GetFromPoint( pos + wxPoint( size.x, size.y ) ) == wxNOT_FOUND )
      {
        return false;
      }
    }
  }

  return true;
}


///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
WindowSettings::WindowSettings( const std::string& version, wxPoint pos, wxSize size )
: m_Version( version )
, m_IsMaximized( false )
, m_PosX( pos.x )
, m_PosY( pos.y )
, m_Width( size.x )
, m_Height( size.y )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
WindowSettings::~WindowSettings()
{
}

///////////////////////////////////////////////////////////////////////////////
// Get the version of these settings.
// 
const std::string& WindowSettings::GetCurrentVersion() const
{
  return m_Version;
}

///////////////////////////////////////////////////////////////////////////////
// Sets all member variables based upon the values in the provided window and
// optional wxAuiManager.
// 
void WindowSettings::SetFromWindow( const wxWindow* window, wxAuiManager* manager )
{
  wxTopLevelWindow* tlw = wxDynamicCast( window, wxTopLevelWindow );
  if ( tlw )
  {
    SetMaximized( tlw->IsMaximized() );
  }
  else
  {
    SetMaximized( false );
  }

  if ( !IsMaximized() && ValidatePositionAndSize( window->GetPosition(), window->GetSize()) )
  {
    // Only store position/size if not maximized, and only if valid
    SetPosition( window->GetPosition() );
    SetSize( window->GetSize() );
  }

  if ( manager )
  {
    SetDockingState( manager->SavePerspective().c_str() );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Applies the settings from this class to the specified window and optional
// aui manager.
// 
void WindowSettings::ApplyToWindow( wxWindow* window, wxAuiManager* manager, bool updateAui )
{
  wxTopLevelWindow* tlw = wxDynamicCast( window, wxTopLevelWindow );
  
  if ( tlw )
  {
    tlw->Maximize( IsMaximized() );
  }

  if ( !tlw || !IsMaximized() )
  {
    if ( ValidatePositionAndSize( GetPosition(), GetSize() ) )
    {
    window->SetPosition( GetPosition() );
    window->SetSize( GetSize() );
  }
  }

  if ( manager )
  {
    manager->LoadPerspective( GetDockingState().c_str() );
    if ( updateAui )
    {
      manager->Update();
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Returns the field representing the x position.
// 
const Reflect::Field* WindowSettings::PositionX() const
{
  return GetClass()->FindField( &WindowSettings::m_PosX );
}

///////////////////////////////////////////////////////////////////////////////
// Returns the field representing the y position.
// 
const Reflect::Field* WindowSettings::PositionY() const
{
  return GetClass()->FindField( &WindowSettings::m_PosY );
}

///////////////////////////////////////////////////////////////////////////////
// Returns the position setting.
// 
wxPoint WindowSettings::GetPosition() const
{
  return wxPoint( m_PosX, m_PosY );
}

///////////////////////////////////////////////////////////////////////////////
// Sets the window position.
// 
void WindowSettings::SetPosition( i32 x, i32 y )
{
  Set( PositionX(), x );
  Set( PositionY(), y );
}

///////////////////////////////////////////////////////////////////////////////
// Returns the field for the width.
// 
const Reflect::Field* WindowSettings::Width() const
{
  return GetClass()->FindField( &WindowSettings::m_Width );
}

///////////////////////////////////////////////////////////////////////////////
// Returns the field for the height.
// 
const Reflect::Field* WindowSettings::Height() const
{
  return GetClass()->FindField( &WindowSettings::m_Height );
}

///////////////////////////////////////////////////////////////////////////////
// Returns the size setting.
// 
wxSize WindowSettings::GetSize() const
{
  return wxSize( m_Width, m_Height );
}

///////////////////////////////////////////////////////////////////////////////
// Set the window size.  
// 
void WindowSettings::SetSize( i32 width, i32 height )
{
  Set( Width(), width );
  Set( Height(), height );
}

///////////////////////////////////////////////////////////////////////////////
// Returns the field for the maximized state.
// 
const Reflect::Field* WindowSettings::Maximized() const
{
  return GetClass()->FindField( &WindowSettings::m_IsMaximized );
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the window is maximized.
// 
bool WindowSettings::IsMaximized() const
{
  return m_IsMaximized;
}

///////////////////////////////////////////////////////////////////////////////
// Sets whether or not the window is maximized.
// 
void WindowSettings::SetMaximized( bool maximized )
{
  Set( Maximized(), maximized );
}

///////////////////////////////////////////////////////////////////////////////
// Returns the field for docking state.
// 
const Reflect::Field* WindowSettings::DockingState() const
{
  return GetClass()->FindField( &WindowSettings::m_DockingState );
}

///////////////////////////////////////////////////////////////////////////////
// Returns the docking state of child windows (see wxAuiManager).
// 
const std::string& WindowSettings::GetDockingState() const
{
  return m_DockingState;
}

///////////////////////////////////////////////////////////////////////////////
// Sets the docking state values from an wxAuiManager.
// 
void WindowSettings::SetDockingState( const std::string& state )
{
  Set( DockingState(), state );
}
