#pragma once

#include "Platform/Types.h"

#define IPC_PIPE_ROOT ""

namespace IPC
{
  struct Pipe
  {
    void* m_Handle;
    struct Overlapped
    {
      u32* Internal;
      u32* InternalHigh;
      union {
          struct {
              u32 Offset;
              u32 OffsetHigh;
          };

          void* Pointer;
      };
      void* hEvent;
    } m_Overlapped;

    Pipe(int);
    ~Pipe();
  };
}
