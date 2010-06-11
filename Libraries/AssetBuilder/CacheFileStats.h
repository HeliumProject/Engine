#pragma once

#include "Platform/Types.h"

namespace CacheFileStats
{
  // initialize and cleanup db connection
  void Initialize();
  void Cleanup();

  // test connection status (did we connect in Initialize())
  bool IsConnected();

  // transaction helpers
  void BeginTransaction();
  void CommitTransaction();
  void RollbackTransaction();

  // update built file statistics
  bool UpdateStats( bool download, u64 size, u32 numFiles, f32 speed, f32 duration );
}