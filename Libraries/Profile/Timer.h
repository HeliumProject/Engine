#pragma once

#include "API.h"
#include "common/types.h"

namespace Profile
{
  //
  // Bare Timer
  //

  class PROFILE_API Timer
  {
  private:
    u64 m_StartTime;

  public:
    Timer()
    {
      Reset();
    }

  private:
    Timer(const Timer& rhs)
    {

    }

  public:
    // reset timer (for re-use)
    void Reset();

    // get elapsed time in millis
    float Elapsed();
  };
}
