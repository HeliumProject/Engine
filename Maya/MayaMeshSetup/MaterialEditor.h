#pragma once

#include "API.h"

#include "Common/Memory/SmartPtr.h"
#include "Common/Automation/Event.h"

#include <wx/notebook.h>

// Forwards
namespace Symbol
{
  class Enum;
  typedef Nocturnal::SmartPtr< Enum > EnumPtr;
}

namespace UIToolKit
{
  class AutoCompleteComboBox;
}

namespace Content
{
  class Material;
  typedef Nocturnal::SmartPtr< Material > MaterialPtr;
}

namespace Maya
{
  // Forwards
  class MaterialEditorPanel;

  /////////////////////////////////////////////////////////////////////////////
  // UI for editing the material settings (visual, acoustic, physical) on
  // geometry.  The UI consists of 3 combo boxes, a set of buttons for
  // performing actions, a set of radio buttons for toggling the display settings
  // in Maya, and a color key legend to explain what enum value corresponds to
  // what color in the UI.
  // 
  class MAYAMESHSETUP_API MaterialEditor : public wxDialog, public Nocturnal::RefCountBase< MaterialEditor > 
  {
  public:
    MaterialEditor( wxWindow* parent = NULL, wxPoint pos = wxDefaultPosition, wxSize size = wxDefaultSize );
    virtual ~MaterialEditor();

    void SetStatusText( const wxString& text );
    void GetMaterialSettings( const Content::MaterialPtr& material );

  protected:
    virtual void OnButtonClickApply( wxCommandEvent& event );
    virtual void OnButtonClickClear( wxCommandEvent& event );
    virtual void OnButtonClickSelect( wxCommandEvent& event );
    virtual void OnRadioOff( wxCommandEvent& event );
    virtual void OnRadioVisual( wxCommandEvent& event );
    virtual void OnRadioAcoustic( wxCommandEvent& event );
    virtual void OnRadioPhysics( wxCommandEvent& event );
    virtual void OnNotebookPageChanged( wxNotebookEvent& event );
    virtual void OnMouseEnter( wxMouseEvent& event );
    virtual void OnMouseLeave( wxMouseEvent& event );
    virtual void OnCloseEvent( wxCloseEvent& event );

  private:
    void RefreshComboBox( UIToolKit::AutoCompleteComboBox* comboBox, Symbol::Enum* symEnum, int materialComponent );
    void RefreshColorLegend( Symbol::Enum* symEnum, int materialComponent );
    void ClearRadioButtons( wxRadioButton* except );

  private:
    MaterialEditorPanel* m_Panel;
    wxStatusBar* m_StatusBar;
    Symbol::Enum* m_CurrentLegend;
    Symbol::EnumPtr m_VisualEnum;
    Symbol::EnumPtr m_AcousticEnum;
    Symbol::EnumPtr m_PhysicsEnum;

  };

  typedef Nocturnal::SmartPtr< MaterialEditor > MaterialEditorPtr;
}
