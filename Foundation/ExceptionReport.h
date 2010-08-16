#pragma once

#include "Platform/Types.h"

#include "Foundation/API.h"
#include "Foundation/Exception.h"

namespace Helium
{
    namespace Debug
    {
        FOUNDATION_API void InitializeExceptionListener();
        FOUNDATION_API void CleanupExceptionListener();

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
            u64      m_MemTotalReserve;
            u64      m_MemTotalCommit;
            u64      m_MemTotalFree;
            u64      m_MemLargestFree;
            tstring  m_Environment;
        };
    }
}