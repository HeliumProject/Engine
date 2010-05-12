#pragma once

#include "WizardPage.h"

namespace AssetManager
{
  // Forwards
  class SummaryPanel;

  ///////////////////////////////////////////////////////////////////////////
  // First page of the wizard.  Contains information about the type of asset
  // being duplicated.  Allows the user to name the new asset and choose where
  // it will be saved.
  // 
  class SummaryPage : public WizardPage< SummaryPanel >
  {
  private:
    SummaryPage( Wizard* wizard );

  public:
    static UIToolKit::WizardPage* Create( Wizard* wizard );
    virtual ~SummaryPage();

    virtual bool TransferDataToWindow() NOC_OVERRIDE;
    virtual bool TransferDataFromWindow() NOC_OVERRIDE;
  };

}