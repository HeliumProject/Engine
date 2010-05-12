#include "stdafx.h"

#include "ArtFilePage.h"
#include "ManagedAsset.h"
#include "Wizard.h"

#include "Asset/ArtFileAttribute.h"
#include "Asset/AssetClass.h"
#include "Attribute/AttributeHandle.h"

#include "File/Manager.h"
#include "FileSystem/FileSystem.h"

#include "Finder/AssetSpecs.h"
#include "Finder/ExtensionSpecs.h"
#include "Finder/LunaSpecs.h"
#include "Finder/ProjectSpecs.h"

// Using
using namespace AssetManager;

// Statics
static const char* s_NewLabel = "Create a new Maya file in the default location.";
static const char* s_ExistingLabel = "Associate with an existing Maya file.";


///////////////////////////////////////////////////////////////////////////////
// Constructor - protected; Use Create function to make a new instance of this
// class.
// 
ArtFilePage::ArtFilePage( Wizard* wizard, i32 attribTypeID )
: FileBackedPage( wizard, attribTypeID, FinderSpecs::Project::PROJECT_ASSETS_FOLDER, FinderSpecs::Extension::MAYA_BINARY, s_NewLabel, s_ExistingLabel )
{
  NOC_ASSERT( Reflect::GetType<Asset::ArtFileAttribute>() == attribTypeID );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
ArtFilePage::~ArtFilePage()
{
}

///////////////////////////////////////////////////////////////////////////////
// Static creation function.  Returns a new (heap-allocated) instance of this 
// class.
// 
UIToolKit::WizardPage* ArtFilePage::Create( Wizard* wizard, i32 attribTypeID )
{
  return new ArtFilePage( wizard, attribTypeID );
}

///////////////////////////////////////////////////////////////////////////////
// Creates an empty Maya file at the specified location and returns the TUID.
// 
tuid ArtFilePage::CreateNewFile( const std::string& path )
{
  tuid result = TUID::Null;
  
  try
  {
    FileSystem::MakePath( path, true );
    FileSystem::Copy( FinderSpecs::Luna::EMPTY_MAYA_FILE.GetFile( FinderSpecs::Luna::CONFIG_FOLDER ), path, false );
    result = File::GlobalManager().Open( path );
  }
  catch ( const Nocturnal::Exception& e )
  {
    std::stringstream str;
    str << "Failed to copy Maya template file to " << path << "." << std::endl << "Reason: " << e.what();
    wxMessageBox( str.str().c_str(), "Error", wxCENTER | wxICON_ERROR | wxOK, GetWizard() );
  }

  return result;
}


///////////////////////////////////////////////////////////////////////////////
// Assigns the specified TUID to the appropriate member of the attribute.
// 
void ArtFilePage::Finished( tuid fileID )
{
  Attribute::AttributeEditor< Asset::ArtFileAttribute > editor( GetWizard()->GetAssetClass() );
  if ( editor.Valid() )
  {
    editor->m_FileID = fileID;
    editor.Commit();
  }
}
