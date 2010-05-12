#pragma once

#include "FileBackedPage.h"
#include "Common/Types.h"
#include "TUID/TUID.h"

namespace AssetManager
{

  ///////////////////////////////////////////////////////////////////////////
  // WizardPage to wrap WorldFileAttributes.  Allows the user to choose an
  // existing world file or create a new one (at a default location).  Stores
  // the file TUID on the attribute when the wizard exits.
  // 
  class WorldFilePage : public FileBackedPage
  {
  protected:
    WorldFilePage( Wizard* wizard, i32 attribTypeID );

  public:
    static UIToolKit::WizardPage* Create( Wizard* wizard, i32 attribTypeID );
    virtual ~WorldFilePage();

  protected:
    virtual std::string GetDefaultNewFilePath() NOC_OVERRIDE;
    virtual tuid CreateNewFile( const std::string& path ) NOC_OVERRIDE;
    virtual void Finished( tuid fileID ) NOC_OVERRIDE;
  };

}