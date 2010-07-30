#pragma once

#include "API.h"

namespace Helium
{
  // Detects if a debugger is attached to the process
  PLATFORM_API bool IsDebuggerPresent();
}