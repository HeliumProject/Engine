#include "Precompile.h"

#include "VaultSearchEnvironment.h"

#include "Foundation/File/Path.h"
#include "Foundation/String/Utilities.h"
#include "Platform/Windows/Memory.h"
#include "Platform/Process.h"

#include <sstream>

namespace Luna
{

  namespace VaultSearchDatabase
  {

    EnvironmentData::EnvironmentData(  )
    {
      tchar buf[MAX_PATH];

      m_UserName.clear();
      ZeroMemory( buf, MAX_PATH );
      GetEnvironmentVariable( TXT( "USERNAME" ), buf, MAX_PATH );
      m_UserName = buf;

      m_Computer.clear();
      ZeroMemory( buf, MAX_PATH );
      GetEnvironmentVariable( TXT( "COMPUTERNAME" ), buf, MAX_PATH );
      m_Computer = buf;


      m_ApplicationName.clear();
      GetModuleFileName( NULL, buf, MAX_PATH );
      tstring processPath = buf;
      size_t idx = processPath.find_last_of( TXT( "\\" ) );
      if ( idx != processPath.npos )
      {
        m_ApplicationName = processPath.substr( idx+1 );
      }
      else
      {
        m_ApplicationName = processPath;
      }
      toLower( m_ApplicationName );
      size_t firstDot = m_ApplicationName.find_last_of( '.' );
      m_ApplicationName = m_ApplicationName.substr( 0, firstDot );

      m_CodeBranch.clear();
      ZeroMemory( buf, MAX_PATH );
      GetEnvironmentVariable( TXT( "NOC_CODE_BRANCH_NAME" ), buf, MAX_PATH );
      m_CodeBranch = buf;
      Nocturnal::Path::Normalize( m_CodeBranch );

      m_ProjectName.clear();
      ZeroMemory( buf, MAX_PATH );
      GetEnvironmentVariable( TXT( "NOC_PROJECT_NAME" ), buf, MAX_PATH );
      m_ProjectName = buf;

      m_ToolsBuildConfig.clear();
      ZeroMemory( buf, MAX_PATH );
      GetEnvironmentVariable( TXT( "NOC_BUILD_CONFIG" ), buf, MAX_PATH );
      m_ToolsBuildConfig = buf;

    }
  }
}