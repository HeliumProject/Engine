#pragma once

#include "API.h"

typedef unsigned long DWORD;

namespace RCSHooks
{
  RCSHOOKS_API void Initialize();
  RCSHOOKS_API void Cleanup();

  RCSHOOKS_API bool HookProcess(DWORD process_id);
  RCSHOOKS_API bool UnhookProcess(DWORD process_id);

  RCSHOOKS_API void HookAllProcesses();
  RCSHOOKS_API void UnhookAllProcesses();
};