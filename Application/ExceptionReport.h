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
        std::string  m_UserName;
        std::string  m_Computer;
        std::string  m_ApplicationPath;
        std::string  m_ApplicationName;
        std::string  m_CmdLineArgs;
        std::string  m_BuildConfig;
        uint64_t m_MemTotalReserve;
        uint64_t m_MemTotalCommit;
        uint64_t m_MemTotalFree;
        uint64_t m_MemLargestFree;
        std::string  m_Environment;
    };
}