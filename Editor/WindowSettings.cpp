#include "Precompile.h"
#include "WindowSettings.h"

#include <wx/aui/aui.h>
#include <wx/display.h>

using namespace Helium;
using namespace Helium::Editor;

// Statics
const tchar_t* WindowSettings::s_Reset = TXT( "resetWin" );
const tchar_t* WindowSettings::s_ResetLong = TXT( "ResetWindowSettings" );

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
WindowSettings::WindowSettings( wxPoint pos, wxSize size )
: m_IsMaximized( false )
, m_PosX( pos.x )
, m_PosY( pos.y )
, m_Width( size.x )
, m_Height( size.y )
{
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

    if ( !IsMaximized() && Validate( window->GetPosition(), window->GetSize()) )
    {
        // Only store position/size if not maximized, and only if valid
        SetPosition( window->GetPosition() );
        SetSize( window->GetSize() );
    }

    if ( manager )
    {
        SetDockingState( (const wxChar*)manager->SavePerspective().c_str() );
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
        if ( Validate( GetPosition(), GetSize() ) )
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
// Returns the position setting.
// 
wxPoint WindowSettings::GetPosition() const
{
    return wxPoint( m_PosX, m_PosY );
}

///////////////////////////////////////////////////////////////////////////////
// Sets the window position.
// 
void WindowSettings::SetPosition( int32_t x, int32_t y )
{
    m_PosX = x;
    m_PosY = y;
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
void WindowSettings::SetSize( int32_t width, int32_t height )
{
    m_Width = width;
    m_Height = height;
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
    m_IsMaximized = maximized;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the docking state of child windows (see wxAuiManager).
// 
const tstring& WindowSettings::GetDockingState() const
{
    return m_DockingState;
}

///////////////////////////////////////////////////////////////////////////////
// Sets the docking state values from an wxAuiManager.
// 
void WindowSettings::SetDockingState( const tstring& state )
{
    m_DockingState = state;
}

///////////////////////////////////////////////////////////////////////////////
// Checks the window settings against the provided version number and recreates 
// the window settings if necessary.
// 
void WindowSettings::Check( WindowSettingsPtr& settings )
{
    if ( !settings )
    {
        settings = new WindowSettings();
    }
}

///////////////////////////////////////////////////////////////////////////////
// Determines if the specified settings are valid and returns true if so.
// 
bool WindowSettings::Validate( wxPoint pos, wxSize size )
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
