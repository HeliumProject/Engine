#pragma once

#include "WizardPage.h"

// Forwards
namespace Perforce { class PerforcePanel; }

namespace AssetManager
{
  ///////////////////////////////////////////////////////////////////////////
  // First page of the wizard.  Contains information about the type of asset
  // being duplicated.  Allows the user to name the new asset and choose where
  // it will be saved.
  // 
  class PerforcePage : public WizardPage< PerforceUI::Panel >
  {
  private:
    PerforcePage( Wizard* wizard );

  public:
    static UIToolKit::WizardPage* Create( Wizard* wizard );
    virtual ~PerforcePage();

    virtual bool TransferDataToWindow() NOC_OVERRIDE;
    virtual bool TransferDataFromWindow() NOC_OVERRIDE;

    void SetReopenFiles( bool reopenFiles = true );
    void CommitChangelist();
  };
}