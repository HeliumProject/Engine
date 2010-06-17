#pragma once

#include "Luna/API.h"
#include "Platform/Types.h"
#include "Foundation/Exception.h"

#include <string>

namespace Luna
{
  namespace BrowserSearchDatabase
  {

    /// @class EnvironmentData
    /// This class represents various environment data that we want to gather
    ///
    class EnvironmentData
    {
    public:

      ///
      /// Process state information: This information is used to add support information to the database
      ///   values are derived from multiple places including the environment variablse
      /// 
      std::string           m_UserName;         //!< Name of the user currently executing
      std::string           m_Computer;         //!< Name of the computer currently executing
      std::string           m_ProjectName;      //!< Current project name
      std::string           m_CodeBranch;       //!< Current branch
      std::string           m_ToolsBuildConfig;
      std::string           m_ApplicationName;  //!< Name of the current application


    public:
      
      /// @constructor
      /// Gather environment data and store them in the appropriate fields
      EnvironmentData( );      
    };
  }
}