#pragma once

#include "FileLocationPage.h"
#include "CreateAssetWizard.h"

namespace AssetManager
{
  ///////////////////////////////////////////////////////////////////////////
  // First page of the wizard.  Contains information about the type of asset
  // being duplicated.  Allows the user to name the new asset and choose where
  // it will be saved.
  // 
  class CreateAssetFileLocationPage : public FileLocationPage
  {
  protected:
    CreateAssetFileLocationPage( CreateAssetWizard* wizard );
    void OnTemplateChanged( wxCommandEvent& args );

  public:
    static UIToolKit::WizardPage* Create( CreateAssetWizard* wizard );
    virtual ~CreateAssetFileLocationPage();

    // Returns properly typed wizard object.
    CreateAssetWizard* GetCreateAssetWizard() const
    {
      return wxStaticCast( __super::GetWizard(), CreateAssetWizard );
    }

    virtual bool TransferDataFromWindow() NOC_OVERRIDE;

  protected:
    virtual std::string GetDefaultDirectory() const NOC_OVERRIDE;
    
  private:
    typedef std::map< i32, const Asset::AssetTemplate* > M_RadioAssetTemplate;

    wxRadioBox* m_RadioBox;
    M_RadioAssetTemplate  m_RadioAssetTemplates; // Indexed by position within the radio box

  };

}