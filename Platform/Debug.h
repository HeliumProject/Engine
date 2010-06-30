#pragma once

#include "API.h"

namespace Platform
{
  // Detects if a debugger is attached to the process
  PLATFORM_API bool IsDebuggerPresent();
}