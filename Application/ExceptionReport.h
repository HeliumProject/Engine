#pragma once

#include "Platform/Types.h"

#include "Application/API.h"
#include "Application/Exception.h"

namespace Application
{
  /////////////////////////////////////////////////////////////////////////////
  // Basic Exception report
  // 
  class ExceptionReport
  {
  public:
    ExceptionReport( const Debug::ExceptionArgs& args );

    // Exception Information
    Debug::ExceptionArgs  m_Args;

    // Process State Information
    tstring  m_UserName;
    tstring  m_Computer;
    tstring  m_ApplicationPath;
    tstring  m_ApplicationName;
    tstring  m_CmdLineArgs;
    tstring  m_BuildConfig;
    u64      m_MemTotalReserve;
    u64      m_MemTotalCommit;
    u64      m_MemTotalFree;
    u64      m_MemLargestFree;
    tstring  m_Environment;
  };
}
