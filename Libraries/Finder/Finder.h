#pragma once

#include "API.h"
#include "FinderSpec.h"
#include "Exceptions.h"

namespace Finder
{
  //
  // Finder API
  //
  
  FINDER_API void Initialize();
  FINDER_API void Cleanup();

  FINDER_API void AddFileSpec( const FileSpec &spec );
  FINDER_API const FileSpec* GetFileSpec( const std::string& specName );

  FINDER_API void AddFileExtensionSpec( const ModifierSpec &spec );
  FINDER_API const ModifierSpec* GetFileExtensionSpec( const std::string& fileName );

  FINDER_API const FolderSpec* GetFolderSpec( const std::string& specName );

  FINDER_API void AddFinderSpec( const FinderSpec &spec );
  FINDER_API const FinderSpec* GetFinderSpec( const std::string& specName );

  FINDER_API void HackSpec( FileSpec* spec );
  FINDER_API bool HackSpec( const std::string& specName );
  FINDER_API void HackAllSpecs();

  //
  // Fixed locations around the project
  //
  namespace ToolsFolders
  {
    enum ToolsFolder
    {
      Root, 
      Bat, 
      Bin, 
      Extern, 
      Maya, 
      Scripts, 
      Count
    }; 
  }; 

  typedef ToolsFolders::ToolsFolder ToolsFolder; 

  FINDER_API const std::string& ProjectName();
  FINDER_API const std::string& ProjectGame();
  FINDER_API const std::string& ProjectRoot();
  FINDER_API const std::string& ProjectTemp();
  FINDER_API const std::string& ProjectLog();
  FINDER_API const std::string& ProjectTools(ToolsFolder which = ToolsFolders::Root); 
  FINDER_API const std::string& ProjectCode();
  FINDER_API const std::string& ProjectCodeBranch();
  FINDER_API const std::string& ProjectAssets();
  FINDER_API const std::string& ProjectAssetsBranch();
  FINDER_API const std::string& ProjectMaya();
  FINDER_API const std::string& ProjectTextures();
  FINDER_API const std::string& ProjectExport();
  FINDER_API const std::string& ProjectMetaData();
  FINDER_API const std::string& ProjectScripts();
  FINDER_API const std::string& ProjectBuilt();
  FINDER_API const std::string& ProjectCache();
  FINDER_API const std::string& ProjectProcessed();

  FINDER_API bool IsInProject( const std::string& path );

  FINDER_API const std::string& ProjectUserNetworkRoot();

  FINDER_API std::string  GetExportFolder( const std::string& path, bool useFilename = true );
  FINDER_API std::string  GetBuiltFolder( const std::string& path, bool useFilename = false );
  FINDER_API std::string  GetBuiltFolder( tuid assetId );
  FINDER_API std::string  GetThumbnailFolder( tuid assetId );
  
  FINDER_API void         StripAnyProjectRoot( std::string& path );
  FINDER_API std::string  StripAnyProjectRoot( const char *path );

  FINDER_API void         StripAnyProjectAssets( std::string& path );
  FINDER_API std::string  StripAnyProjectAssets( const char *path );
}
