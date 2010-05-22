#pragma once

#include "API.h"

#include "Resolver.h"

namespace File
{
  FILE_API void Initialize();
  FILE_API void Cleanup();

  FILE_API Resolver& GlobalResolver();
}