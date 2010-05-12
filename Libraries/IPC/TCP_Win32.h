#pragma once

#ifndef _WINDOWS_
#error Windows.h not included
#endif

typedef int socklen_t;

namespace IPC
{
  struct Socket
  {
    SOCKET m_Handle;
    OVERLAPPED m_Overlapped;

    Socket(int)
      : m_Handle (0)
    {
      memset(&m_Overlapped, 0, sizeof(m_Overlapped));
      m_Overlapped.hEvent = ::CreateEvent(0, true, false, 0);
    }

    ~Socket()
    {
      ::CloseHandle( m_Overlapped.hEvent );
    }

    operator SOCKET()
    {
      return m_Handle;
    }
  };
}
