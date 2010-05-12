#pragma once

#include "API.h"
#include "Dependencies/FileInfo.h"

#include "Reflect/Serializers.h"
#include "Reflect/Element.h"
#include "Reflect/Archive.h"
#include "Reflect/Registry.h"

namespace CacheFiles
{
  ASSETBUILDER_API void Initialize();
  ASSETBUILDER_API void Cleanup();

  ASSETBUILDER_API const std::string& GetCacheFilesPath();
  ASSETBUILDER_API std::string GetOutputFilename( const Dependencies::DependencyInfoPtr& file );

  ASSETBUILDER_API bool Get( const Dependencies::V_DependencyInfo& files );
  ASSETBUILDER_API bool Get( const Dependencies::FileInfoPtr& file );

  ASSETBUILDER_API void Put( const Dependencies::V_DependencyInfo& files );
  ASSETBUILDER_API void Put( const Dependencies::FileInfoPtr& file );
}

