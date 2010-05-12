#include "stdafx.h"

#include "AssetManager.h"
#include "ManagedAsset.h"
#include "GetManagedAssetVisitor.h"

#include "Asset/AssetInit.h"
#include "Asset/EntityManifest.h"
#include "Asset/ArtFileAttribute.h"
#include "Attribute/AttributeHandle.h"
#include "Asset/Exceptions.h"
#include "AppUtils/AppUtils.h"
#include "Common/Container/Insert.h" 
#include "Common/Flags.h"
#include "Common/InitializerStack.h"
#include "Common/String/Utilities.h"
#include "Common/Types.h"
#include "Content/Scene.h"
#include "File/Manager.h"
#include "File/ManagedFileUtils.h"
#include "FileSystem/FileSystem.h"
#include "Finder/Finder.h"
#include "Finder/LunaSpecs.h"
#include "Finder/AssetSpecs.h"
#include "Finder/ContentSpecs.h"
#include "Finder/AnimationSpecs.h"
#include "Finder/FinderSpec.h"
#include "Finder/ProjectSpecs.h"
#include "Console/Console.h"
#include "RCS/RCS.h"
#include "Reflect/Class.h"
#include "Reflect/Field.h"
#include "Reflect/Serializers.h"
#include "Reflect/Version.h"
#include "TUID/TUID.h"
#include "UIToolKit/ImageManager.h"

using namespace AssetManager;
using Nocturnal::Insert; 
using namespace Asset;
using namespace Asset::AssetFlags;

#define CONFIRM_RENAME_STRING_SIZE 1024
#define MAX_DEPTH 30

//*************************************************************************//
//
//  INITIALIZE & CLEANUP FUNCTIONS
//
//*************************************************************************//

namespace RelatedFileFunctions
{
  enum RelatedFileFunction
  {
    Export,
    MetaData,
    Modify,
  };
}
typedef RelatedFileFunctions::RelatedFileFunction RelatedFileFunction;

class RelatedFile
{
public:
  const FinderSpecs::ModifierSpec* m_Spec;
  RelatedFileFunction                m_Function;

  RelatedFile( const FinderSpecs::ModifierSpec& spec, RelatedFileFunction function )
    : m_Spec( &spec )
    , m_Function( function )
  {}
};
typedef std::vector< RelatedFile > M_RelatedFile;
M_RelatedFile g_RelatedFiles;


int g_InitRef = 0;
Nocturnal::InitializerStack g_InitializerStack;


void AssetManager::Initialize()
{
  if ( ++g_InitRef > 1 )
  {
    return;
  }

  ASSETMANAGER_SCOPE_TIMER((""));

  g_InitializerStack.Push( Finder::Initialize, Finder::Cleanup );
  g_InitializerStack.Push( Reflect::Initialize, Reflect::Cleanup );
  g_InitializerStack.Push( Asset::Initialize, Asset::Cleanup );
  g_InitializerStack.Push( File::Initialize, File::Cleanup );

  UIToolKit::ImageManagerInit( FinderSpecs::Luna::DEFAULT_THEME_FOLDER.GetFolder(),
    FinderSpecs::Luna::GAME_THEME_FOLDER.GetFolder() );

  g_RelatedFiles.push_back( RelatedFile( FinderSpecs::Content::MANIFEST_DECORATION, RelatedFileFunctions::Export ) );
  g_RelatedFiles.push_back( RelatedFile( FinderSpecs::Content::ANIMATION_DECORATION, RelatedFileFunctions::Export ) );
  g_RelatedFiles.push_back( RelatedFile( FinderSpecs::Content::RIGGED_DECORATION, RelatedFileFunctions::Export ) );
  g_RelatedFiles.push_back( RelatedFile( FinderSpecs::Content::STATIC_DECORATION, RelatedFileFunctions::Export ) );
  g_RelatedFiles.push_back( RelatedFile( FinderSpecs::Content::COLLISION_DECORATION, RelatedFileFunctions::Export ) );
  g_RelatedFiles.push_back( RelatedFile( FinderSpecs::Content::DESTRUCTION_GLUE_DECORATION, RelatedFileFunctions::Export ) );
  g_RelatedFiles.push_back( RelatedFile( FinderSpecs::Content::PATHFINDING_DECORATION, RelatedFileFunctions::Export ) );
  g_RelatedFiles.push_back( RelatedFile( FinderSpecs::Content::CINESCENE_DECORATION, RelatedFileFunctions::Export ) );

  g_RelatedFiles.push_back( RelatedFile( FinderSpecs::Animation::EVENTS_DECORATION, RelatedFileFunctions::MetaData ) );

  g_RelatedFiles.push_back( RelatedFile( FinderSpecs::Extension::JPG, RelatedFileFunctions::Modify ) );
}

void AssetManager::Cleanup()
{
  if ( --g_InitRef )
  {
    return;
  }

  ASSETMANAGER_SCOPE_TIMER((""));

  UIToolKit::ImageManagerCleanup();

  g_RelatedFiles.clear();

  g_InitializerStack.Cleanup();
}


//*************************************************************************//
//
//  GET RELATED FILES
//
//*************************************************************************//

std::string GetRelatedFilePath( const std::string& path, const Finder::ModifierSpec* spec, RelatedFileFunction function )
{
  switch ( function )
  {
  default:
    return std::string( "" );

  case RelatedFileFunctions::Export:
    return ((const Finder::DecorationSpec*)spec)->GetExportFile( path );

  case RelatedFileFunctions::MetaData:
    return ((const Finder::DecorationSpec*)spec)->GetMetaDataFile( path );
    break;

  case RelatedFileFunctions::Modify:
    std::string newPath = path;
    spec->Modify( newPath );
    return newPath;
  }    
}

/////////////////////////////////////////////////////////////////////////////
void GetRelatedFiles( ManagedAssetFile* managedAssetFile )
{
  for each ( const RelatedFile& info in g_RelatedFiles )
  {
    std::string file = GetRelatedFilePath( managedAssetFile->m_File->m_Path, info.m_Spec, info.m_Function );

    if ( FileSystem::Exists( file ) )
    {
      std::string newFile = "";
      if ( !managedAssetFile->m_NewFile->m_Path.empty() )
      {
        newFile = GetRelatedFilePath( managedAssetFile->m_NewFile->m_Path, info.m_Spec, info.m_Function );
      }
      managedAssetFile->m_RelatedFiles[file] = newFile;
    }
    else
    {
      // we may have previously added it, remove the file if it no longer matters
      managedAssetFile->m_RelatedFiles.erase( file );
    }
  }
}

/////////////////////////////////////////////////////////////////////////////
void AssetManager::GetManagedAssetRelatedFiles( ManagedAsset* managedAsset )
{
  for each ( const M_ManagedAssetFiles::value_type& fileIndex in managedAsset->m_ManagedAssetFiles )
  {
    const ManagedAssetFilePtr& managedAssetFile = fileIndex.second;

    if ( Nocturnal::HasFlags<i32>( managedAssetFile->m_ConfigOptions, AssetFlags::PerformOperation ) )
    {
      managedAssetFile->m_RelatedFiles.clear();
      GetRelatedFiles( managedAssetFile );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Determine which shaders this asset is currently using
//
void AssetManager::GetManagedAssetShaders( const tuid id, S_tuid& shaderIDs )
{
  static const char* mayaFileProcessorDLL = "MayaFileProcessor";

  try
  {    
    FileProcessor::MayaIProcessor* mayaFileProcessor = (FileProcessor::MayaIProcessor*) FileProcessorInterface::AllocateProcessor( mayaFileProcessorDLL );
    if ( !mayaFileProcessor->GetShaderIDs( id, shaderIDs ) )
    {
      Console::Error( "Unable to retireive associated shaders from asset.\n" );
    }

    FileProcessorInterface::FreeProcessor( mayaFileProcessorDLL );
  }
  catch ( ... )
  {
    Console::Error( "Problems occurred while loading/unloading %s.dll.\n", mayaFileProcessorDLL );
  }
}


///////////////////////////////////////////////////////////////////////////////
// Determine which maya files this asset is currently using
//
void AssetManager::ListMayaFiles( const tuid id )
{
  File::S_ManagedFilePtr mayaFiles;
  V_string mayaFilePaths;

  S_tuid fileIDs;
  fileIDs.insert( id );

  FindMayaFiles( fileIDs, false, mayaFiles );

  File::S_ManagedFilePtr::const_iterator itrID = mayaFiles.begin();
  File::S_ManagedFilePtr::const_iterator endID  = mayaFiles.end();
  for( ; itrID != endID; ++itrID )
  {
    File::ManagedFilePtr file = (*itrID);
    mayaFilePaths.push_back( file->m_Path );
  }

  Console::Print( "\n" );
  Console::Print( "Maya File Count: %d\n", mayaFilePaths.size() );
  V_string::iterator mayaItr = mayaFilePaths.begin();
  V_string::iterator mayaEnd = mayaFilePaths.end();
  for( ; mayaItr != mayaEnd; ++mayaItr )
  {
    Console::Print( "Maya File: %s\n", (*mayaItr).c_str() );
  }

}

void AssetManager::ListStaticContentFiles( const tuid id )
{
  V_string contentFilePaths;
  GetStaticContentFiles( id, contentFilePaths );
  Console::Print( "\n" );
  Console::Print( "Static File Count: %d\n", contentFilePaths.size() );

  V_string::iterator contentItr = contentFilePaths.begin();
  V_string::iterator contentEnd = contentFilePaths.end();
  for( ; contentItr != contentEnd; ++contentItr )
  {
    Console::Print( "Static Export File: %s\n", (*contentItr).c_str() );
  }
}

void AssetManager::GetStaticContentFiles( const tuid id, V_string& staticContentFiles )
{
  File::S_ManagedFilePtr mayaFiles;
  V_string mayaPaths;

  S_tuid fileIDs;
  fileIDs.insert( id );

  FindMayaFiles( fileIDs, false, mayaFiles );

  File::S_ManagedFilePtr::const_iterator itrID = mayaFiles.begin();
  File::S_ManagedFilePtr::const_iterator endID  = mayaFiles.end();
  for( ; itrID != endID; ++itrID )
  {
    File::ManagedFilePtr file = (*itrID);
    mayaPaths.push_back( file->m_Path );
  }

  V_string::iterator mayaItr = mayaPaths.begin();
  V_string::iterator mayaEnd = mayaPaths.end();
  for( ; mayaItr != mayaEnd; ++mayaItr )
  {
    std::string staticFile = FinderSpecs::Content::STATIC_DECORATION.GetExportFile( (*mayaItr).c_str() );
    staticContentFiles.push_back( staticFile );
  }
}

///////////////////////////////////////////////////////////////////////////////
void AssetManager::FindMayaFiles( const S_tuid& fileIdSet, bool recurse, File::S_ManagedFilePtr& mayaFiles, bool needsToExist )
{
  static std::string mayaExtension    = FinderSpecs::Extension::MAYA_BINARY.GetExtension();
  static std::string reflectExtension = FinderSpecs::Extension::REFLECT_BINARY.GetExtension();

  S_tuid fileIds = fileIdSet;
  while( !fileIds.empty() )
  {
    S_tuid fileIdsForThisPass = fileIds;
    fileIds.clear();

    File::ManagedFilePtr file;
    for each( const tuid fileID in fileIdsForThisPass )
    {
      file = File::GlobalManager().GetManagedFile( fileID );
      if ( !file.ReferencesObject() )
      {
        continue;
      }

      if ( needsToExist && !FileSystem::Exists( file->m_Path ) )
      {
        continue;
      }

      if ( FileSystem::HasExtension( file->m_Path, mayaExtension ) )
      {
        mayaFiles.insert( file );
        continue;
      }

      if ( FileSystem::HasExtension( file->m_Path, reflectExtension ) )
      {
        Asset::AssetClassPtr assetClass;
        try
        {
          assetClass = Asset::AssetClass::FindAssetClass( fileID );
          if ( !assetClass.ReferencesObject() )
          {
            continue;
          }
        }
        catch( Nocturnal::Exception& )
        {
          continue;
        }

        Asset::AnimationSetAssetPtr animationSet = Reflect::ObjectCast< Asset::AnimationSetAsset >( assetClass );
        if( animationSet.ReferencesObject() )
        {
          Asset::V_AnimationClipData clips;
          animationSet->GetAnimationClips( clips );

          for each ( const Asset::AnimationClipDataPtr& clip in clips )
          {
            fileIds.insert( clip->m_ArtFile );
          }
        }

        Attribute::AttributeViewer< Asset::ArtFileAttribute > model ( assetClass );
        if ( model.Valid() )
        {
          fileIds.insert( model->m_FileID );
        }

        // if we want to recurse into nested assets
        if ( recurse )
        {
          Attribute::AttributeViewer< Asset::AnimationAttribute > animation( assetClass );
          if ( animation.Valid() && animation->m_AnimationSetId != TUID::Null )
            fileIds.insert( animation->m_AnimationSetId );

#pragma TODO("This is pretty harsh and special case, if there were a nicer way to do this, it would be cool..." )
          if ( assetClass->GetEngineType() == Asset::EngineTypes::Moby )
          {
            File::V_ManagedFilePtr cinematicFiles;
            File::GlobalManager().Find( "*.cinematic.irb", cinematicFiles );

            for each ( const File::ManagedFilePtr& cinematicFile in cinematicFiles )
            {
              Asset::AssetClassPtr cinematicAsset = Asset::AssetClass::FindAssetClass( cinematicFile->m_Id );

              if ( !cinematicAsset.ReferencesObject() )
              {
                continue;
              }

              Attribute::AttributeViewer< Asset::ArtFileAttribute > cinematicArtFile( cinematicAsset );
              if ( !cinematicArtFile.Valid() )
              {
                continue;
              }

              std::string cinematicArtFilePath = File::GlobalManager().GetPath( cinematicArtFile->m_FileID );
              std::string cinematicAnimationFile = FinderSpecs::Content::ANIMATION_DECORATION.GetExportFile( cinematicArtFilePath );

              if ( !FileSystem::Exists( cinematicAnimationFile ) )
              {
                continue;
              }

              Content::Scene cinematicScene( cinematicAnimationFile );
              for each ( const Content::AnimationClipPtr& animationClip in cinematicScene.m_AnimationClips )
              {
                if ( animationClip->m_OptionalEntityID == assetClass->m_AssetClassID )
                {
                  fileIds.insert( cinematicArtFile->m_FileID );
                }
              }
            }
          }
        }
      }
    }
  }
}

/////////////////////////////////////////////////////////////////////////////
void AssetManager::GetManagedFolderRelatedFiles( ManagedFolder* managedFolder )
{
  for each ( const ManagedAssetPtr& managedAsset in managedFolder->m_ManagedAssets )
  {
    GetManagedAssetRelatedFiles( managedAsset );
  }

  //for each ( const M_String::value_type& fileIndex in managedFolder->m_AdditionalFiles )
  //{
  //  const std::string& managedAssetFilePath = fileIndex.second;
  //  ...
  //}
}

/////////////////////////////////////////////////////////////////////////////
void AssetManager::GetManagedAsset( ManagedAsset* managedAsset, bool getRelatedFiles )
{
  GetManagedAssetVisitor getManagedAssetVisitor( managedAsset );
  managedAsset->m_Asset->Host( getManagedAssetVisitor );

  if ( getRelatedFiles )
  {
    GetManagedAssetRelatedFiles( managedAsset );
  }
}



//*************************************************************************//
//
//  GET RENAME FOLDER
//
//*************************************************************************//

void GetNewFilePath( ManagedFolder* managedFolder, const std::string& filePath, std::string& newFilePath, bool isAssetClass = false )
{
  // if they are moving an asset filePath, take better care in making the correct target name
  if ( isAssetClass )
  {
    std::string fileName = FileSystem::GetLeaf( filePath );
    FileSystem::StripExtension( fileName, 2 );

    std::string folderName = FileSystem::GetLeaf( managedFolder->m_Path );
    std::string newFolderName = FileSystem::GetLeaf( managedFolder->m_NewPath );

    if ( ( newFolderName.compare( folderName ) != 0 )
      && ( fileName.compare( folderName ) == 0 ) )
    {
      newFilePath = managedFolder->m_NewPath;
      FileSystem::AppendPath( newFilePath, newFolderName );
      FileSystem::SetExtension( newFilePath, FileSystem::GetExtension( filePath ) );

      return;
    }
  }

  std::string relativeFilePath = filePath;
  FileSystem::StripPrefix( managedFolder->m_Path, relativeFilePath );
  newFilePath = managedFolder->m_NewPath;
  FileSystem::AppendPath( newFilePath, relativeFilePath );
}


/////////////////////////////////////////////////////////////////////////////
// Rename every asset and file under the given path and best guess any and 
// all related files.
//
void AssetManager::GetManagedFolder( ManagedFolder* managedFolder, bool getRelatedFiles )
{
  V_string listOfFilePaths;
  FileSystem::GetFiles( managedFolder->m_Path, listOfFilePaths, "*.*", (FileSystem::FileFindFlags)FileSystem::FindFlags::Recursive );

  for each ( const std::string& filePath in listOfFilePaths )
  {
    // see if it's an asset file and if so, get the rename asset for it
    if ( FileSystem::HasExtension( filePath, FinderSpecs::Extension::REFLECT_BINARY.GetExtension() ) )
    {
        tuid fileId = File::GlobalManager().GetID( filePath );
        if ( fileId != TUID::Null )
        {
          Asset::AssetClassPtr assetClass = Asset::AssetClass::FindAssetClass( fileId );
          if ( assetClass.ReferencesObject() )
          {
            std::string newFilePath = filePath;

            if ( managedFolder->m_UseNewPath )
            {
              GetNewFilePath( managedFolder, filePath, newFilePath, true );
            }

            managedFolder->m_ManagedAssets.push_back( new ManagedAsset( assetClass, managedFolder->m_OperationFlag, filePath, newFilePath ) );
            const ManagedAssetPtr& managedAsset = managedFolder->m_ManagedAssets.back();
            GetManagedAsset( managedAsset, getRelatedFiles );

            // no need to add to the m_ManagedAssetFiles
            continue;
          }
        } 
      }

    // otherwise try to find the file in the system
    File::ManagedFilePtr file = File::GlobalManager().GetManagedFile( filePath );

    // if we couldn't find the ManagedFile...
    if ( !file )
    {
      std::string newFilePath = filePath;

      if ( managedFolder->m_UseNewPath )
      {
        GetNewFilePath( managedFolder, filePath, newFilePath );
      }

      managedFolder->m_AdditionalFiles.insert( M_string::value_type( filePath, newFilePath ) );
      continue;
    }
    else
    {
      // this is because there's a bug in the Find function that places a wild card
      // char in from of the search string, even if you don't want it to
      // this returns things like maya/entities... when you only wanted to search in entities/...
      if ( !FileSystem::HasPrefix( managedFolder->m_Path, file->m_Path ) )
      {
        continue;
      }

      ManagedAssetFilePtr managedAssetFile = new ManagedAssetFile( (ConfigOptions) AssetFlags::PerformOperation );
      managedAssetFile->m_File = file;
      managedAssetFile->m_NewFile = new File::ManagedFile();

      if ( managedFolder->m_UseNewPath )
      {
        GetNewFilePath( managedFolder, managedAssetFile->m_File->m_Path, managedAssetFile->m_NewFile->m_Path );
      }

      managedFolder->m_ManagedAssetFiles.insert( M_ManagedAssetFiles::value_type( managedAssetFile->m_File->m_Id, managedAssetFile ) );
    }
  }
}




//*************************************************************************//
//
// VALIDATION
// 
//*************************************************************************//

/////////////////////////////////////////////////////////////////////////////
inline bool IsManagedPath( const std::string& filePath, std::string& errMsg )
{
  if ( !FileSystem::HasPrefix( File::GlobalManager().GetManagedAssetsRoot(), filePath ) )
  {
    errMsg = std::string( "Cannot manage file, " ) + filePath + std::string(", it is outside of the project assets directory: " )+ File::GlobalManager().GetManagedAssetsRoot();
    return false;
  }

  return true;
}

/////////////////////////////////////////////////////////////////////////////
bool ValidateFilePathOperation( const std::string& filePath, const std::string& newFilePath, std::string& errMsg )
{
#pragma TODO( "Write the ValidateFilePathOperation function body :p" )
  return true;
}

/////////////////////////////////////////////////////////////////////////////
bool ValidateFileOperation( ManagedAssetFile* managedAssetFile, bool useNewPath, std::string& errMsg )
{
  NOC_ASSERT( managedAssetFile->m_File->m_Id != TUID::Null );
  if ( managedAssetFile->m_File->m_Id == TUID::Null )
  {
    NOC_BREAK();
    return true;
  }
#pragma TODO( "File librabry should provide validation functions so that they only exist in one place" )
  if ( useNewPath )
  {
    if ( Nocturnal::HasFlags<i32>( managedAssetFile->m_ConfigOptions, AssetFlags::PerformOperation ) )
    {
      if ( managedAssetFile->m_NewFile->m_Path.empty() )
      {
        errMsg = "No new path was specified for file: " + managedAssetFile->m_File->m_Path;
        return false;
      }

      if ( managedAssetFile->m_NewFile->m_Path == managedAssetFile->m_File->m_Path )
      {
        errMsg = "Please specify a new file path that is not the same as the old path: " + managedAssetFile->m_File->m_Path;
        return false;
      }

      if ( !IsManagedPath( managedAssetFile->m_NewFile->m_Path, errMsg ) )
      {
        return false;
      }

      File::V_ManagedFilePtr listOfFiles;
      File::GlobalManager().Find( managedAssetFile->m_NewFile->m_Path, listOfFiles );

      if ( !listOfFiles.empty() )
      { 
        errMsg = "A file already exists with the path: " + managedAssetFile->m_NewFile->m_Path;
        return false;
      }

    }
    else
    {
      if ( managedAssetFile->m_NewFile->m_Path.empty() )
      {
        errMsg = "Please specify a file to reference for file: " + managedAssetFile->m_File->m_Path;
        return false;
      }

      if ( !FileSystem::Exists( managedAssetFile->m_NewFile->m_Path ) )
      {
        errMsg = "The file referenced does not exist in the project assets directory: " + managedAssetFile->m_NewFile->m_Path;
        return false;
      }

      File::V_ManagedFilePtr listOfFiles;
      File::GlobalManager().Find( managedAssetFile->m_NewFile->m_Path, listOfFiles );

      if ( listOfFiles.empty() )
      { 
        errMsg = "The file referenced does not exist in the project: " + managedAssetFile->m_NewFile->m_Path;
        return false;
      }
    }
  }

#pragma TODO( "validate managedAssetFile->m_RelatedFiles" )
  //for each ( const M_string::value_type& relatedFilePair in managedAssetFile->m_RelatedFiles )
  //{      
  //}

  return true;
}

/////////////////////////////////////////////////////////////////////////////
// validates that the new paths are not already in the Resolver
bool AssetManager::ValidateAssetOperation( ManagedAsset* managedAsset, std::string& errMsg )
{
  if ( managedAsset->m_UseNewPath
    && !IsManagedPath( managedAsset->m_NewPath, errMsg ) )
  {
    return false;
  }

  for each ( const M_ManagedAssetFiles::value_type& fileIndex in managedAsset->m_ManagedAssetFiles )
  {
    const ManagedAssetFilePtr& managedAssetFile = fileIndex.second;

    if ( !ValidateFileOperation( managedAssetFile, managedAsset->m_UseNewPath, errMsg ) )
      return false;
  }

  return true;
}



//*************************************************************************//
//
// PROCESS RENAME
// 
//*************************************************************************//

/////////////////////////////////////////////////////////////////////////////
void ProcessRenameFile( ManagedAssetFile* managedAssetFile, const File::ManagerConfig managerConfig, M_string& moveFiles, M_string& relatedFiles )
{
  if ( ( Nocturnal::HasFlags<i32>( managedAssetFile->m_ConfigOptions, AssetFlags::PerformOperation ) )
    && ( moveFiles.find( managedAssetFile->m_File->m_Path ) ==  moveFiles.end() ) )
  {
    moveFiles[managedAssetFile->m_File->m_Path] = managedAssetFile->m_NewFile->m_Path;
    relatedFiles.insert( managedAssetFile->m_RelatedFiles.begin(), managedAssetFile->m_RelatedFiles.end() );
  }
  else if ( ( Nocturnal::HasFlags<i32>( managedAssetFile->m_ConfigOptions, AssetFlags::PerformOperation ) )
    && ( moveFiles.find( managedAssetFile->m_File->m_Path ) !=  moveFiles.end() ) 
    && managedAssetFile->m_NewFile->m_Path != moveFiles[managedAssetFile->m_File->m_Path] )
  {
    throw Nocturnal::Exception( "The file %s has already been moved to a different location.", managedAssetFile->m_File->m_Path.c_str() );
  }
}

/////////////////////////////////////////////////////////////////////////////
// takes the asset, M_RenameElement and new path; moves the asset, all of the files in the renameElements   
void AssetManager::ProcessRenameAsset( ManagedAsset* managedAsset, const File::ManagerConfig managerConfig, bool rename )
{
  M_string moveFiles;
  M_string relatedFiles;

  for each ( const M_ManagedAssetFiles::value_type& fileIndex in managedAsset->m_ManagedAssetFiles )
  {
    const ManagedAssetFilePtr& managedAssetFile = fileIndex.second;

    ProcessRenameFile( managedAssetFile, managerConfig, moveFiles, relatedFiles );
  }
  
  File::GlobalManager().Move( moveFiles, managerConfig, rename );

  File::ManagerConfig moveManagerConfig = ( managerConfig & ~File::ManagerConfigs::Resolver );
  File::GlobalManager().Move( relatedFiles, moveManagerConfig, rename );

  Asset::AssetClass::InvalidateCache( managedAsset->m_Asset->m_AssetClassID );

  File::GlobalManager().Move( managedAsset->m_Asset->GetFilePath(), managedAsset->m_NewPath, managerConfig, rename );    
}


/////////////////////////////////////////////////////////////////////////////
// takes the asset, M_RenameElement and new path; moves the asset, all of the files in the renameElements   
void AssetManager::UpdateRenameAsset( ManagedAsset* managedAsset, bool rename )
{
  UpdateRenameAssetVisitor updateManagedAssetVisitor( managedAsset, rename );
  managedAsset->m_Asset->Host( updateManagedAssetVisitor );
}


//*************************************************************************//
//
// PROCESS COPY
// 
//*************************************************************************//

/////////////////////////////////////////////////////////////////////////////
void AssetManager::ProcessDuplicateAsset( ManagedAsset* managedAsset, const File::ManagerConfig managerConfig )
{
  ProcessRenameAsset( managedAsset, managerConfig, false );
}


/////////////////////////////////////////////////////////////////////////////
Asset::AssetClassPtr AssetManager::UpdateDuplicateAsset( ManagedAsset* managedAsset )
{
  if ( managedAsset->m_NewPath.empty() )
  {
    return NULL;
  }

  tuid newAssetId = File::GlobalManager().GetID( managedAsset->m_NewPath );

  Asset::AssetClassPtr newAssetClass = Reflect::ObjectCast< Asset::AssetClass >( managedAsset->m_Asset->Clone() );
  newAssetClass->m_AssetClassID = newAssetId;

  Asset::AssetClass::InvalidateCache( newAssetClass->m_AssetClassID );

  managedAsset->m_Asset = newAssetClass;

  UpdateRenameAssetVisitor updateManagedAssetVisitor( managedAsset, false );
  managedAsset->m_Asset->Host( updateManagedAssetVisitor );

  return newAssetClass;
}



//*************************************************************************//
//
// PROCESS DELETE
// 
//*************************************************************************//

/////////////////////////////////////////////////////////////////////////////
void ProcessDeleteFile( ManagedAssetFile* managedAssetFile, S_string& deleteFiles, S_string& relatedFiles )
{
  if ( !Nocturnal::HasFlags<i32>( managedAssetFile->m_ConfigOptions, AssetFlags::PerformOperation ) )
  {
    return;
  }

  if ( !managedAssetFile->m_File->m_Path.empty() )
  {
    deleteFiles.insert( managedAssetFile->m_File->m_Path );
  }

  for each ( const M_string::value_type& relatedFile in managedAssetFile->m_RelatedFiles )
  {
    relatedFiles.insert( relatedFile.first );
  }
}

/////////////////////////////////////////////////////////////////////////////
// validates that the new paths are not already in the Resolver
void AssetManager::ProcessDeleteAsset( ManagedAsset* managedAsset, const File::ManagerConfig managerConfig )
{
  S_string deleteFiles;
  S_string relatedFiles;

  for each ( const M_ManagedAssetFiles::value_type& fileIndex in managedAsset->m_ManagedAssetFiles )
  {
    const ManagedAssetFilePtr& managedAssetFile = fileIndex.second;

    ProcessDeleteFile( managedAssetFile, deleteFiles, relatedFiles );
  }

  File::GlobalManager().Delete( deleteFiles, managerConfig );

  File::ManagerConfig relatedManagerConfig = ( managerConfig & ~File::ManagerConfigs::Resolver );
  File::GlobalManager().Delete( relatedFiles, relatedManagerConfig );

  std::string assetPath = managedAsset->m_Asset->GetFilePath();
  Asset::AssetClass::InvalidateCache( managedAsset->m_Asset->m_AssetClassID );

  File::GlobalManager().Delete( assetPath, managerConfig );
}




//*************************************************************************//
//
// Asset Migration to a new project
//
//*************************************************************************//



/////////////////////////////////////////////////////////////////////////////
// 
void GetMigrateFiles( const File::ManagedFilePtr& sourceFile, File::S_ManagedFilePtr& sourceFiles, M_string& relatedFiles, bool getShaderUsages )
{ 
  RCS::File rcsFile( sourceFile->m_Path );
  rcsFile.Sync();

  if ( !FileSystem::HasExtension( sourceFile->m_Path, FinderSpecs::Extension::REFLECT_BINARY.GetExtension() ) )
  {
    return;
  }

  Asset::AssetClassPtr assetClass = NULL;
  try
  {
    if ( ( assetClass = Asset::AssetClass::FindAssetClass( sourceFile->m_Id ) ) == NULL )
    {
      return;
    }
  }
  catch( const Nocturnal::Exception& )
  {
    return;
  }

  OperationFlag opFlags = AssetManager::OperationFlags::Duplicate;
  ManagedAsset managedAsset(assetClass, opFlags) ;

  GetManagedAssetVisitor getManagedAssetVisitor( &managedAsset );
  managedAsset.m_Asset->Host( getManagedAssetVisitor );
  GetManagedAssetRelatedFiles( &managedAsset );

  if ( getShaderUsages )
  {
    std::string unused;
    Attribute::AttributePtr shaderUsageAttribute = new Asset::ShaderUsagesAttribute();

    if ( assetClass->GetAttribute( shaderUsageAttribute->GetType() ).ReferencesObject()
      || assetClass->ValidateAttribute( shaderUsageAttribute, unused ) )
    {
      S_tuid shaderIDs;
      GetManagedAssetShaders( assetClass->GetFileID(), shaderIDs );

      for each ( const tuid& shaderID in shaderIDs )
      {
        File::ManagedFilePtr shaderFile = File::GlobalManager().GetManagedFile( shaderID );

        RCS::File rcsShaderFile( shaderFile->m_Path );
        rcsShaderFile.Sync();

        Insert<File::S_ManagedFilePtr>::Result inserted = sourceFiles.insert( shaderFile );
        if ( inserted.second )
        {
          GetMigrateFiles( *inserted.first, sourceFiles, relatedFiles, false );
        }
      }
    }
  }

  // determine if the tuids returned are asset classes that can be loaded
  // like animation class
  M_ManagedAssetFiles::iterator itElem = managedAsset.m_ManagedAssetFiles.begin();
  M_ManagedAssetFiles::iterator itEndElem = managedAsset.m_ManagedAssetFiles.end();
  for ( ; itElem != itEndElem ; ++itElem )
  {
    ManagedAssetFilePtr& subManagedAssetPtr = itElem->second;

    if( subManagedAssetPtr->m_File->m_Id == TUID::Null )
    {
      continue;
    }

    Insert<File::S_ManagedFilePtr>::Result inserted;

    if ( subManagedAssetPtr->m_File->m_Path.empty() )
    {
      inserted = sourceFiles.insert( File::GlobalManager().GetManagedFile( subManagedAssetPtr->m_File->m_Id ));
    } 
    else
    {
      inserted = sourceFiles.insert( subManagedAssetPtr->m_File );
    }       

    if ( inserted.second )
    {  
      // append the renameFile->m_RelatedFiles
      relatedFiles.insert( subManagedAssetPtr->m_RelatedFiles.begin(), subManagedAssetPtr->m_RelatedFiles.end() );

      // recurse
      GetMigrateFiles( *inserted.first, sourceFiles, relatedFiles, getShaderUsages );
    }
  }
}



//////////////////////////////////////////////////////////////////////////////////
//  Migrate assets from one project into the current project
//
void AssetManager::MigrateFile( const File::ManagedFilePtr& sourceAssetFile, File::Manager* sourceManager, const File::ManagerConfig managerConfig )
{
  // Build the list of assets to copy
  M_string relatedFiles;
  File::S_ManagedFilePtr sourceFiles;
  sourceFiles.insert( sourceAssetFile );

  File::SwapGlobalManager( sourceManager );
  GetMigrateFiles( sourceAssetFile, sourceFiles, relatedFiles, true );
  File::SwapGlobalManager();

  // Migrate the list of files returned
  for each ( const File::ManagedFilePtr& sourceFile in sourceFiles )
  {
    File::GlobalManager().Migrate( sourceFile, sourceManager, managerConfig );
  }

  // Migrate the remaining related files
  File::ManagerConfig relatedManagerConfig = ( managerConfig & ~File::ManagerConfigs::Resolver );
  M_string::iterator itRelated = relatedFiles.begin();
  M_string::iterator itEndRelated = relatedFiles.end();
  for ( ; itRelated != itEndRelated ; ++itRelated )
  {
    itRelated->second = itRelated->first;
    FileSystem::StripPrefix( sourceManager->GetManagedAssetsRoot(), itRelated->second );
    itRelated->second = Finder::ProjectAssets() + itRelated->second;

    File::GlobalManager().Copy( itRelated->first, itRelated->second, relatedManagerConfig );
  }
}

