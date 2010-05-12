#pragma once

#include "API.h"
#include "Common/Types.h"
#include "Debug/Exception.h"

namespace AppUtils
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
    std::string           m_UserName;
    std::string           m_Computer;
    std::string           m_ApplicationPath;
    std::string           m_ApplicationName;
    std::string           m_CmdLineArgs;
    std::string           m_InheritedArgs;
    std::string           m_AssetBranch;
    std::string           m_CodeBranch;
    std::string           m_ProjectName;
    bool                  m_IsToolsBuilder;
    bool                  m_IsSymbolBuilder;
    std::string           m_ToolsVersion;
    std::string           m_ToolsBuildConfig;
    std::string           m_ToolsReleaseName;
    u64                   m_MemTotalReserve;
    u64                   m_MemTotalCommit;
    u64                   m_MemTotalFree;
    u64                   m_MemLargestFree;
    std::string           m_Environment;
  };
}
