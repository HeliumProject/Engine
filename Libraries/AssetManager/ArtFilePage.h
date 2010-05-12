#pragma once

#include "FileBackedPage.h"
#include "Common/Types.h"

namespace AssetManager
{

  ///////////////////////////////////////////////////////////////////////////
  // Custom wizard page for ArtFileAttributes.  Allows the user to select an
  // art file to go with their asset.  Assigns the file to the appropriate
  // attribute on the wizard's asset class when the wizard finishes.
  // 
  class ArtFilePage : public FileBackedPage
  {
  private:
    i32 m_AttributeTypeID;

  protected:
    ArtFilePage( Wizard* wizard, i32 attribTypeID );
  public:
    virtual ~ArtFilePage();

    static UIToolKit::WizardPage* Create( Wizard* wizard, i32 attribTypeID );

  protected:
    virtual tuid CreateNewFile( const std::string& path ) NOC_OVERRIDE;
    virtual void Finished( tuid fileID ) NOC_OVERRIDE;
  };

}
