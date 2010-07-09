#pragma once

#include "Luna/API.h"
#include "Platform/Types.h"
#include "Platform/Exception.h"

#include <string>

namespace Luna
{
  namespace VaultSearchDatabase
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
      tstring           m_UserName;         //!< Name of the user currently executing
      tstring           m_Computer;         //!< Name of the computer currently executing
      tstring           m_ProjectName;      //!< Current project name
      tstring           m_CodeBranch;       //!< Current branch
      tstring           m_ToolsBuildConfig;
      tstring           m_ApplicationName;  //!< Name of the current application


    public:
      
      /// @constructor
      /// Gather environment data and store them in the appropriate fields
      EnvironmentData( );      
    };
  }
}