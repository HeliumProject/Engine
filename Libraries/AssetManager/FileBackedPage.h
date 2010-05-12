#pragma once

#include "WizardPage.h"
#include "Common/Types.h"
#include "TUID/TUID.h"

// Forwards
namespace Finder
{
  class FolderSpec;
  class ModifierSpec;
}

namespace AssetManager
{

  // Forwards
  class Wizard;
  class FileBackedAttrPanel;

  ///////////////////////////////////////////////////////////////////////////
  // Base class for wizard pages that allow the user to choose between an
  // existing file and creating a new one (at a default location).
  // 
  class FileBackedPage NOC_ABSTRACT : public WizardPage< FileBackedAttrPanel >
  {
  protected:
    i32                           m_AttributeTypeID;
    const Finder::FolderSpec*     m_DefaultFolder;
    const Finder::ModifierSpec*   m_Extension;

  protected:
    FileBackedPage( Wizard* wizard, i32 attribTypeID, const Finder::FolderSpec& defaultFolder, const Finder::ModifierSpec& ext, const std::string& newLabel, const std::string& existingLabel );

  public:
    virtual ~FileBackedPage();

    void SetOpenExisting( bool openExisting );
    void SetExistingPath( const std::string& path );

    virtual bool TransferDataToWindow() NOC_OVERRIDE;
    virtual bool TransferDataFromWindow() NOC_OVERRIDE;

    virtual void Finished() NOC_OVERRIDE;

  protected:
    void ConnectListeners();
    void DisconnectListeners();

    // Overridable functions
  protected:
    virtual std::string GetDefaultNewFilePath();

    // Will be called when the wizard is exiting and the user has chosen to 
    // create a new file.  Derived classes should create the file and return
    // the TUID.
    virtual tuid CreateNewFile( const std::string& path ) = 0;

    // Called when the wizard is finished and the file has been resolved to
    // a TUID.  Derived classes should store this information on their 
    // attribute in the appropriate place.
    virtual void Finished( tuid fileID ) = 0;    

  private:
    void UpdateEnableState();

  private:
    void OnRadioButtonSelected( wxCommandEvent& args );
    void OnButtonExistingClicked( wxCommandEvent& args );
    void OnButtonFindExistingClicked( wxCommandEvent& args );
  };
}
