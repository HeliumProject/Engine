#pragma once

#include "Platform/Types.h"

#include "Application/API.h"
#include "Foundation/Exception.h"

namespace Helium
{
    HELIUM_APPLICATION_API void InitializeExceptionListener();
    HELIUM_APPLICATION_API void CleanupExceptionListener();

    class ExceptionReport
    {
    public:
        ExceptionReport( const ExceptionArgs& args );

        // Exception Information
        ExceptionArgs  m_Args;

        // Process State Information
        tstring  m_UserName;
        tstring  m_Computer;
        tstring  m_ApplicationPath;
        tstring  m_ApplicationName;
        tstring  m_CmdLineArgs;
        tstring  m_BuildConfig;
        uint64_t m_MemTotalReserve;
        uint64_t m_MemTotalCommit;
        uint64_t m_MemTotalFree;
        uint64_t m_MemLargestFree;
        tstring  m_Environment;
    };
}