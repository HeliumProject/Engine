#pragma once

#include "API.h"
#include "CollisionModeDisplayNode.h"

#include "Common/Automation/Event.h"

namespace Maya
{
  // Forwards
  class CollisionModePanel;

  /////////////////////////////////////////////////////////////////////////////
  // UI for editing the collision mode settings (visual, acoustic, physical) on
  // geometry objects.  The UI consists of 4 checkboxes, a set of buttons for
  // performing actions, a set of radio buttons for toggling the display settings
  // in Maya, and a color key legend to explain what enum value corresponds to
  // what color in the UI.
  // 
  class MAYAMESHSETUP_API CollisionModeEditor : public wxDialog, public Nocturnal::RefCountBase< CollisionModeEditor > 
  {
  private:
    CollisionModePanel* m_Panel;
    wxStatusBar*        m_StatusBar;

  public:
    CollisionModeEditor( wxWindow* parent = NULL, wxPoint pos = wxDefaultPosition, wxSize size = wxDefaultSize );
    virtual ~CollisionModeEditor();

    void SetStatusText( const wxString& text );
    CollisionMode GetCollisionModeSettings();

  private:
    void ClearRadioButtons( wxRadioButton* except );

    void OnButtonClickApply( wxCommandEvent& event );
    void OnButtonClickClear( wxCommandEvent& event );

    void OnRadioOff( wxCommandEvent& event );
    void OnRadioCameraIgnore( wxCommandEvent& event );
    void OnRadioImpassable( wxCommandEvent& event );
    void OnRadioNonTraversable( wxCommandEvent& event );
    void OnRadioTransparent( wxCommandEvent& event );
    void OnMouseEnter( wxMouseEvent& event );
    void OnMouseLeave( wxMouseEvent& event );
    void OnCloseEvent( wxCloseEvent& event );
  };

  typedef Nocturnal::SmartPtr< CollisionModeEditor > CollisionModeEditorPtr;
}
