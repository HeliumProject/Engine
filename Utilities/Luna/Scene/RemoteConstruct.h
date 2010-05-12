#pragma once

#include "API.h"

#include "rpc/interfaces/rpc_construct_host.h"
#include "rpc/interfaces/rpc_construct_tool.h"

namespace Luna
{
  namespace RemoteConstruct
  {
    // the interface to our local tool
    extern RPC::IConstructionHost* g_ConstructionHost;

    // the interface to our remote view
    extern RPC::IConstructionTool* g_ConstructionTool;

    void Initialize();
    void Cleanup();

    bool EstablishConnection();
    bool Connected();
    void Enable( bool enable );
  }
}