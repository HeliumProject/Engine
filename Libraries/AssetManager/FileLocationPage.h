#pragma once

#include "WizardPage.h"


namespace AssetManager
{
  // Forwards
  class LocationPanel;

  ///////////////////////////////////////////////////////////////////////////
  // First page of the wizard.  Contains information about the type of asset
  // being duplicated.  Allows the user to name the new asset and choose where
  // it will be saved.
  // 
  class FileLocationPage : public WizardPage< LocationPanel >
  {
  public:
    FileLocationPage( Wizard* wizard );

    static UIToolKit::WizardPage* Create( Wizard* wizard );
    virtual ~FileLocationPage();

    virtual void SetDefaultDirectory( const std::string& defaultDir );

    virtual bool TransferDataToWindow() NOC_OVERRIDE;
    virtual bool TransferDataFromWindow() NOC_OVERRIDE;

  protected:
    // UI callbacks
    void OnDirectoryTextChanged( wxCommandEvent& args );
    void OnNameTextChanged( wxCommandEvent& args );
    
    
    void OnButtonChooseDirectory( wxCommandEvent& args );
    void OnCheckCreateAssetSubfolder( wxCommandEvent& args );

  protected:
    void ConnectListeners();
    void DisconnectListeners();

    virtual std::string GetDefaultName() const;
    virtual std::string GetDefaultDirectory() const;

    std::string ConstructFilePath() const;

  protected:
    std::string       m_Name;
    std::string       m_Directory;

    bool              m_AddDirectory;
    bool              m_GotDefaultDirectory;
  };
}
