#pragma once

#include "API.h"

#include <string>

namespace Windows
{
  WINDOWS_API bool ModuleContainsProc(const std::string& dll, const std::string& proc);
}