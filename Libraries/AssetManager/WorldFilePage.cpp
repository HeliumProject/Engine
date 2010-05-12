#include "stdafx.h"

#include "WorldFilePage.h"

#include "Wizard.h"

#include "Attribute/AttributeHandle.h"
#include "Asset/WorldFileAttribute.h"
#include "Content/Scene.h"
#include "File/Manager.h"
#include "FileSystem/FileSystem.h"
#include "Finder/AssetSpecs.h"

// Using
using namespace AssetManager;


// Statics
static const char* s_LabelCreate = "Create a new world file.";
static const char* s_LabelExisting = "Associate with an existing world file.";

///////////////////////////////////////////////////////////////////////////////
// Constructor - protected; Use Create function to make a new instance of this
// class.
// 
WorldFilePage::WorldFilePage( Wizard* wizard, i32 attribTypeID )
: FileBackedPage( wizard, attribTypeID, FinderSpecs::Asset::LEVEL_FOLDER, FinderSpecs::Asset::WORLD_DECORATION, s_LabelCreate, s_LabelExisting )
{
  NOC_ASSERT( Reflect::GetType<Asset::WorldFileAttribute>() == attribTypeID );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
WorldFilePage::~WorldFilePage()
{
}


///////////////////////////////////////////////////////////////////////////////
// Static function to create a new (heap allocated) instance of this class.
// 
UIToolKit::WizardPage* WorldFilePage::Create( Wizard* wizard, i32 attribTypeID )
{
  return new WorldFilePage( wizard, attribTypeID );
}


///////////////////////////////////////////////////////////////////////////////
// Return the file path to use for the "create new" field.
// 
std::string WorldFilePage::GetDefaultNewFilePath()
{
  std::string path( GetWizard()->GetNewFileLocation() );
  FileSystem::StripExtension( path );
  m_Extension->Modify( path );
  return path;
}

///////////////////////////////////////////////////////////////////////////////
// Creates an empty Reflect Archive file at the specified location.
// 
tuid WorldFilePage::CreateNewFile( const std::string& path )
{
  bool success;
  try
  {
    Reflect::Archive::ToFile( Reflect::V_Element(), path );
    success = true;
  }
  catch ( Nocturnal::Exception& ex )
  {
    Console::Error( "%s\n", ex.what() );
    success = false;
  }

  if ( success )
  {
    tuid addedFile;
    try
    {
      addedFile = File::GlobalManager().Add( path );
    }
    catch ( const File::Exception& e )
    {
      std::stringstream str;
      str << "Failed to add file " << path << "." << std::endl << "Reason: " << e.what();
      wxMessageBox( str.str().c_str(), "Error", wxCENTER | wxICON_ERROR | wxOK, GetWizard() );
    }
    try
    {
      Content::Scene scene( addedFile );

      scene.Update();

      scene.Serialize();
    }   
    catch ( const Nocturnal::Exception& e)
    {
      std::stringstream str;
      str << "Failed to add default lighting volume to file " << path << "." << std::endl << "Reason: " << e.what();
      wxMessageBox( str.str().c_str(), "Error", wxCENTER | wxICON_ERROR | wxOK, GetWizard() );
    }
    return addedFile;
  }

  return TUID::Null;
}

///////////////////////////////////////////////////////////////////////////////
// Assigns the specified TUID to the WorldFileAttribute.
// 
void WorldFilePage::Finished( tuid fileID )
{
  Attribute::AttributeEditor< Asset::WorldFileAttribute > editor( GetWizard()->GetAssetClass() );
  if ( editor.Valid() )
  {
    editor->m_FileID = fileID;
    editor.Commit();
  }
}
