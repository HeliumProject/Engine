#pragma once

#ifndef _WINDOWS_
#error Windows.h not included
#endif

#define IPC_PIPE_ROOT ""

namespace IPC
{
  struct Pipe
  {
    HANDLE m_Handle;
    OVERLAPPED m_Overlapped;

    Pipe(int)
      : m_Handle (0)
    {
      memset(&m_Overlapped, 0, sizeof(m_Overlapped));
      m_Overlapped.hEvent = ::CreateEvent(0, true, false, 0);
    }

    ~Pipe()
    {
      ::CloseHandle( m_Overlapped.hEvent );
    }
  };
}
