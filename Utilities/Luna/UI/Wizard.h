#pragma once

#include "Luna/API.h"
#include <wx/wizard.h>

namespace Luna
{
  // Forwards
  class WizardPage;

  // Indicates which direction the wizard is currently being traversed.
  namespace PageDirections
  {
    enum PageDirection
    {
      Backward,
      Forward
    };
  }
  typedef PageDirections::PageDirection PageDirection;

  /////////////////////////////////////////////////////////////////////////////
  // Abstract base class for creating a Wizard GUI.
  // 
  class LUNA_CORE_API Wizard : public wxWizard
  {
  private:
    PageDirection m_Direction;
    wxWizardPage* m_FirstPage;

  public:
    Wizard( wxWindow* parent, int id = -1, const wxString& title = wxEmptyString, const wxBitmap& bitmap = wxNullBitmap, const wxPoint& pos = wxDefaultPosition, long style = wxDEFAULT_DIALOG_STYLE );
    virtual ~Wizard();

    virtual wxWizardPage* GetFirstPage() const;
    virtual void SetFirstPage( wxWizardPage* page );
    bool IsPageInWizard( wxWizardPage* page );
    virtual bool Run();
    PageDirection GetDirection() const;

  protected:
      // UI callbacks
      virtual void OnWizardFinished( wxWizardEvent& args );
      virtual void OnBackButton( wxCommandEvent& args );
      virtual void OnNextButton( wxCommandEvent& args );
  };
}