#include "Version.h"
#include "Exceptions.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include <iostream>
#include <sstream>

#include "Foundation/Version.h"
#include "Foundation/Log.h"

using namespace Application;

class Version
{
private:
  bool m_Ignore;
  unsigned m_Project;
  unsigned m_Compatible;
  unsigned m_Feature;
  unsigned m_Patch;

public:
  Version()
    : m_Ignore (false)
    , m_Project (0)
    , m_Compatible (0)
    , m_Feature (0)
    , m_Patch (0)
  {

  }

  Version(const Version& rhs)
    : m_Ignore (false)
    , m_Project (rhs.m_Project)
    , m_Compatible (rhs.m_Compatible)
    , m_Feature (rhs.m_Feature)
    , m_Patch (rhs.m_Patch)
  {

  }

  Version(unsigned project, unsigned compatible, unsigned feature, unsigned patch)
    : m_Ignore (false)
    , m_Project (project)
    , m_Compatible (compatible)
    , m_Feature (feature)
    , m_Patch (patch)
  {

  }

  Version(const tstring& str)
    : m_Ignore (false)
    , m_Project (0)
    , m_Compatible (0)
    , m_Feature (0)
    , m_Patch (0)
  {
    FromString( str );
  }

  bool Ignore()
  {
    return m_Ignore;
  }

  void FromString(const tstring& str)
  {
    if (!_tcsicmp(str.c_str(), TXT( "ignore" ) ) )
    {
      m_Ignore = true;
    }
    else
    {
      tchar temp;
      tistringstream stream (str);

      stream >> m_Project;
      stream >> temp;
      stream >> m_Compatible;
      stream >> temp;
      stream >> m_Feature;
      stream >> temp;
      stream >> m_Patch;

      if (stream.fail())
      {
        throw Application::Exception( TXT( "Invalid version format '%s'" ), str.c_str());
      }
    }
  }

  tstring IntoString()
  {
    tostringstream str;

    str << m_Project;
    str << TXT( "." );
    str << m_Compatible;
    str << TXT( "." );
    str << m_Feature;
    str << TXT( "." );
    str << m_Patch;

    return str.str();
  }

  VersionResult Compare(const Version& rhs)
  {
    if (m_Ignore || rhs.m_Ignore)
    {
      return VersionResults::Match;
    }

    if (m_Project != rhs.m_Project)
    {
      return VersionResults::Project;
    }

    if (m_Compatible != rhs.m_Compatible)
    {
      return VersionResults::Compatible;
    }

    if (m_Feature != rhs.m_Feature)
    {
      return VersionResults::Feature;
    }

    if (m_Patch != rhs.m_Patch)
    {
      return VersionResults::Patch;
    }

    return VersionResults::Match;
  }
};

tchar* CleanStr(tchar* buff)
{
  int i,j;
  j = (int)_tcslen(buff);
  for( ;j > 0; j--)
  {
    if ((buff[j-1]!=' ')&&(buff[j-1]!='\t')&&(buff[j-1]!=0xa)&&(buff[j-1]!=0xd))
      break;
    buff[j-1] = 0;
  }
  for(i = 0; i < j; i++)
  {
    if ((buff[i]!=' ')&&(buff[i]!='\t'))
      break;
  }
  return &buff[i];
}

bool GetVersion(const tstring& filePath, Version& version)
{
  FILE *f = NULL;

  bool result = false;
  f = _tfopen(filePath.c_str(), TXT( "r" ) );

  if (f)
  {
    tchar buff[128];
    buff[0] = 0;

    if ( _fgetts(buff,sizeof(buff)-1,f) )
    {
      version = Version ( CleanStr(buff) );
      result = true;
    }

    fclose(f);
  }

  return result;
}

bool GetVersion(tstring filePath, tstring& versionString)
{
  FILE *f = NULL;

  bool result = false;
  f = _tfopen(filePath.c_str(), TXT( "r" ) );

  if(f)
  {
    tchar buff[128];
    buff[0] = 0;

    if( _fgetts(buff,sizeof(buff)-1,f) )
    {
      versionString = CleanStr(buff);
      result = true;
    }

    fclose(f);
  }

  return result;
}

void HandleResult(VersionResult result, tstring details)
{
  tstring message;

  switch (result)
  {
  case VersionResults::Project:
    {
      message = TXT( "Your tools are for a different project. You cannot proceed using these tools." );
      break;
    }

  case VersionResults::Compatible:
    {
      message = TXT( "Your tools are not compatible anymore. Sorry, but you must update immediately." );
      break;
    }

  case VersionResults::Feature:
    {
      message = TXT( "The tools have been improved. You should update as soon as possible." );
      break;
    }

  case VersionResults::Patch:
    {
      message = TXT( "The tools have been updated. You should update when you get a chance.");
      break;
    }
  }

  if (!details.empty())
  {
    message += TXT( " (" );
    message += details;
    message += TXT( ")" );
  }

  if (result > VersionResults::Feature)
  {
    throw Application::CheckVersionException( message.c_str() );
  }
  else
  {
    Log::Warning( TXT( "%s\n" ), message.c_str());
  }
}

void Application::CheckVersion()
{
  //const char* projectName = getenv( "NOC_PROJECT_NAME" );
  //if ( projectName == NULL )
  //{
  //  throw Application::Exception( "The %sPROJECT_NAME environment variable not set!\n", NOCTURNAL_STUDIO_PREFIX );
  //}

  //if ( _stricmp( projectName, NOCTURNAL_PROJECT_NAME ) != 0 )
  //{
  //  throw Application::Exception( "You are running in the wrong project: '%s' != '%s'\n", projectName, NOCTURNAL_PROJECT_NAME );
  //}

  //const char* networkVersionPath = getenv( "NOC_NETWORK_VERSION_FILE" );
  //if ( networkVersionPath == NULL )
  //{
  //  throw Application::Exception( "The %sNETWORK_VERSION_FILE environment variable not set!\n", NOCTURNAL_STUDIO_PREFIX );
  //}

  //const char* ignoreVersion = getenv( "NOC_IGNORE_TOOLS_VERSION" );
  //if ( ignoreVersion != NULL )
  //{
  //  if ( atoi( ignoreVersion ) )
  //  {
  //    Log::Warning( "Ignoring tools version.  This is dangerous and you could cause data loss.\n" );
  //    return;
  //  }
  //}

  //Version localVersion (NOCTURNAL_VERSION_PROJECT, NOCTURNAL_VERSION_COMPATIBLE, NOCTURNAL_VERSION_FEATURE, NOCTURNAL_VERSION_PATCH);

  //// retrieve from file
  //Version networkVersion;
  //bool gotNetworkVersion = GetVersion( networkVersionPath, networkVersion);

  //// continue if we can't read the network version
  //if (!gotNetworkVersion)
  //{
  //  Log::Warning("Unable to read network version from '%s'\n", networkVersionPath);
  //  return;
  //}

  //// do the compare
  //VersionResult result = localVersion.Compare( networkVersion );

  //// mainline version check, local to network
  //if (result != VersionResults::Match)
  //{
  //  HandleResult(result, tstring("local: ") + localVersion.IntoString() + " != " + "network: " + networkVersion.IntoString());
  //}
}
