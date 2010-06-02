#pragma once

#include "API.h"

#include "Resolver.h"

namespace File
{
  FILE_API void Initialize();
  FILE_API void Cleanup();

  FILE_API void InitGlobalResolver( const std::string& resolverDatabaseFile, const std::string& resolverConfigPath );
  FILE_API Resolver* GlobalResolver();
}