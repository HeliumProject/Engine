#pragma once

#include "API.h"

#include "Platform/Types.h"

namespace IPC
{
  class Message;
}

namespace Worker
{
  struct WORKER_API Client
  {
    // initialize connection
    static bool Initialize();
    static void Cleanup();

    // you must delete the object this returns, if non-null
    static IPC::Message* Receive( bool wait = true );

    // a copy is made into the IPC connection system
    static bool Send(u32 id, u32 size = -1, const u8* data = NULL);
  };
}