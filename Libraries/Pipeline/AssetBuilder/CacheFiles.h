#pragma once

#include "Pipeline/API.h"
#include "Pipeline/Dependencies/Dependencies.h"
#include "Pipeline/Dependencies/Info/FileInfo.h"

#include "Reflect/Serializers.h"
#include "Reflect/Element.h"
#include "Reflect/Archive.h"
#include "Reflect/Registry.h"

namespace CacheFiles
{
  PIPELINE_API void Initialize();
  PIPELINE_API void Cleanup();

  PIPELINE_API const std::string& GetCacheFilesPath();
  PIPELINE_API std::string GetOutputFilename( const Dependencies::DependencyInfoPtr& file );

  PIPELINE_API bool Get( Dependencies::DependencyGraph& depGraph, const Dependencies::V_DependencyInfo& files );
  PIPELINE_API bool Get( Dependencies::DependencyGraph& depGraph, const Dependencies::FileInfoPtr& file );

  PIPELINE_API void Put( Dependencies::DependencyGraph& depGraph, const Dependencies::V_DependencyInfo& files );
  PIPELINE_API void Put( Dependencies::DependencyGraph& depGraph, const Dependencies::FileInfoPtr& file );
}

