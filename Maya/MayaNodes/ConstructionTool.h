#pragma once

#include "TUID/TUID.h"

#include "rpc/rpc.h"
#include "IPC/Connection.h"
#include "rpc/interfaces/rpc_construct_tool.h"
#include "rpc/interfaces/rpc_construct_host.h"

namespace Construction
{
  // the interface to our local tool
  extern RPC::IConstructionTool* g_ConstructionTool;

  // the interface to our remote view
  extern RPC::IConstructionHost* g_ConstructionHost;

  // class of the entity that uses the current scene as it's art file
  extern tuid g_ClassID;

  bool Initialize();
  void Cleanup();
  void Dispatch();

  bool EstablishConnection();
  bool Connected();
}