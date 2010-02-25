#pragma once

#include <string>

#ifndef REQUIRES_WINDOWS_H
#define REQUIRES_WINDOWS_H
#endif
#include "API.h"

#include "Common/Exception.h"

//
// Helper objects for taking critical sections
//

namespace Windows
{
  //
  // CriticalSection - Thin wrapper for win32 that supports manual and TakeSection helper usage
  //

  class CriticalSection
  {
  private:
    friend class TakeSection;
    CRITICAL_SECTION m_Section;

  public:
    inline CriticalSection()
    {
      InitializeCriticalSection(&m_Section);
    }

    inline ~CriticalSection()
    {
      DeleteCriticalSection(&m_Section);
    }

    inline void Enter()
    {
      EnterCriticalSection(&m_Section);
    }

    inline void Leave()
    {
      LeaveCriticalSection(&m_Section);
    }
  };

  //
  // TakeSection - Allocate one of these on the stack to have it hold a critical section while in a function
  //

  class TakeSection
  {
  private:
    CRITICAL_SECTION* m_Section;

  public:
    TakeSection(CRITICAL_SECTION& section)
      : m_Section (&section)
    {
      EnterCriticalSection(m_Section);
    }

    TakeSection(CriticalSection& section)
      : m_Section (&section.m_Section)
    {
      EnterCriticalSection(m_Section);
    }

  private:
    TakeSection(const TakeSection& rhs)
      : m_Section (NULL)
    {
      throw Nocturnal::Exception ("Cannot copy a critical section helper");
    }

  public:
    ~TakeSection()
    {
      LeaveCriticalSection(m_Section);  
    }
  };
}