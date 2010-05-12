#pragma once

#include "API.h"

namespace Windows
{
  //
  // Helper object for taking handles
  //  Allocate one of these on the stack to have it hold a handle while in a function
  //

  template<class H>
  class HoldHandle
  {
  private:
    H m_Handle;

  public:
    HoldHandle(H handle)
      : m_Handle (handle)
    {

    }

    ~HoldHandle()
    {
      ::CloseHandle(m_Handle);  
    }

    operator H()
    {
      return m_Handle;
    }
  };
}