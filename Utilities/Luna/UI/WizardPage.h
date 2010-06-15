#pragma once

#include "Luna/API.h"
#include <wx/wizard.h>

namespace Luna
{
  // Forwards
  class Wizard;

  ///////////////////////////////////////////////////////////////////////////
  // Base class for creating wizard pages.  Provides functions for setting up
  // the previous and next pages.  Note that calling SetNext will automatically 
  // call SetPrev for the page that was passed in.
  // 
  class LUNA_CORE_API WizardPage : public wxWizardPage
  {
  private:
    Wizard* m_Wizard;
    WizardPage* m_Prev;
    WizardPage* m_Next;

  public:
    WizardPage( Wizard* wizard );
    virtual ~WizardPage();

    Wizard* GetWizard() const;
    virtual void Finished();

    void SetPrev( WizardPage* page );
    void SetNext( WizardPage* page );

    // Overrides
    virtual wxWizardPage* GetPrev() const;
    virtual wxWizardPage* GetNext() const;
  };
}
