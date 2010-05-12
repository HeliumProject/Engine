#pragma once

#include "API.h"
#include <wx/wizard.h>

namespace UIToolKit
{
  // Forwards
  class Wizard;

  ///////////////////////////////////////////////////////////////////////////
  // Base class for creating wizard pages.  Provides functions for setting up
  // the previous and next pages.  Note that calling SetNext will automatically 
  // call SetPrev for the page that was passed in.
  // 
  class UITOOLKIT_API WizardPage NOC_ABSTRACT : public wxWizardPage
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
    virtual wxWizardPage* GetPrev() const NOC_OVERRIDE;
    virtual wxWizardPage* GetNext() const NOC_OVERRIDE;
  };
}
