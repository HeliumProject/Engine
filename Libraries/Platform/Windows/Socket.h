#pragma once

#include <winsock2.h>

#include "Platform/API.h"
#include "Platform/Types.h"

typedef int socklen_t;

namespace Platform
{
  struct PLATFORM_API Socket
  {
    SOCKET m_Handle;
    struct Overlapped
    {
      u32* Internal;
      u32* InternalHigh;
      union
      {
          struct
          {
              u32 Offset;
              u32 OffsetHigh;
          };

          void* Pointer;
      };
      void* hEvent;
    } m_Overlapped;

    Socket(int);
    ~Socket();

    operator SOCKET()
    {
      return m_Handle;
    }
  };
}
