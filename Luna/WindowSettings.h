#pragma once

#include "Luna/API.h"
#include "Settings.h"

class wxAuiManager;

namespace Luna
{
  class WindowSettings;
  typedef Nocturnal::SmartPtr< WindowSettings > WindowSettingsPtr;

  /////////////////////////////////////////////////////////////////////////////
  // Data class for storing window locations.
  // 
  class LUNA_EDITOR_API WindowSettings : public Settings
  {
  public:
    static const tchar* s_Reset;
    static const tchar* s_ResetLong;

  private:
    // Used to invalidate older values for a particular instance of this class
    tstring m_Version;

    // String (wxWidgets AUI) representing docking state of all child panels 
    tstring m_DockingState;

    bool m_IsMaximized;

    // Position of the window (upper left corner)
    i32 m_PosX;
    i32 m_PosY;

    // Size of the window
    i32 m_Width;
    i32 m_Height;

    // RTTI
  public:
    REFLECT_DECLARE_CLASS( WindowSettings, Settings )

    static void EnumerateClass( Reflect::Compositor<WindowSettings>& comp );

  public:
    static void InitializeType();
    static void CleanupType();
    static void CheckWindowSettings( WindowSettingsPtr& settings, const tstring& version );
    static bool ValidatePositionAndSize( wxPoint pos, wxSize size );

    WindowSettings( const tstring& version = TXT( "" ), wxPoint pos = wxDefaultPosition, wxSize size = wxDefaultSize );
    virtual ~WindowSettings();

    virtual const tstring& GetCurrentVersion() const NOC_OVERRIDE;

    void SetFromWindow( const wxWindow* window, wxAuiManager* manager = NULL );
    void ApplyToWindow( wxWindow* window, wxAuiManager* manager = NULL, bool updateAui = false );

    const Reflect::Field* PositionX() const;
    const Reflect::Field* PositionY() const;
    wxPoint GetPosition() const;
    void SetPosition( i32 x, i32 y );
    void SetPosition( wxPoint pos ) { SetPosition( pos.x, pos.y ); }

    const Reflect::Field* Width() const;
    const Reflect::Field* Height() const;
    wxSize GetSize() const;
    void SetSize( i32 width, i32 height );
    void SetSize( wxSize size ) { SetSize( size.x, size.y ); }

    const Reflect::Field* Maximized() const;
    bool IsMaximized() const;
    void SetMaximized( bool maximized );

    const Reflect::Field* DockingState() const;
    const tstring& GetDockingState() const;
    void SetDockingState( const tstring& state );
  };
}