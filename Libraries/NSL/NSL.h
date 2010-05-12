#pragma once

#include "API.h"
#include "Exceptions.h"

#include "FileSystem/FileSystem.h"

#include "Common/Types.h"

#include <string>

// Network Storage Library
namespace NSL
{
  //
  // Exported API
  //

  NSL_API const std::string& StorageRoot();
  NSL_API const std::string& RelativeProcessed();
  
  NSL_API void CopyToNetwork( std::string relativeSource, const std::string& keyFile = "", bool onlyIfNewer = true );
  NSL_API void CopyFromNetwork( const std::string& relativeSource, const std::string& keyFile = "", bool onlyIfNewer = true );

  NSL_API void CopyToNetwork( V_string& relativePaths, const std::string& keyFile = "", bool onlyIfNewer = true );
  NSL_API void CopyFromNetwork( V_string& relativePaths, const std::string& keyFile = "", bool onlyIfNewer = true );

  //
  // Internal API
  //

  void Copy( std::string sourcePath, std::string targetPath, bool useTempFile, std::string keyFile = "", bool onlyIfNewer = true );
  void Copy( V_string& sourcePaths, const std::string& targetPath, bool useTempFile, const std::string& keyFile = "", bool onlyIfNewer = true );
  void CopyFile( std::string sourceFile, std::string targetFile, bool useTempFile, bool onlyIfNewer = true );

  std::string GetDestinationFile( const std::string& source, const std::string& targetDirectory );

  bool SourceIsNewer( const std::string& source, const std::string& target );
}