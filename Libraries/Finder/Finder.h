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
}
