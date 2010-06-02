#include "Version.h"
#include "Exceptions.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include <iostream>
#include <sstream>

#include "Common/Config.h"
#include "Common/Version.h"
#include "Console/Console.h"

using namespace AppUtils;

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

  Version(const std::string& str)
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

  void FromString(const std::string& str)
  {
    if (!_stricmp(str.c_str(), "ignore"))
    {
      m_Ignore = true;
    }
    else
    {
      char temp;
      std::istringstream stream (str);

      stream >> m_Project;
      stream >> temp;
      stream >> m_Compatible;
      stream >> temp;
      stream >> m_Feature;
      stream >> temp;
      stream >> m_Patch;

      if (stream.fail())
      {
        throw AppUtils::Exception("Invalid version format '%s'", str.c_str());
      }
    }
  }

  std::string IntoString()
  {
    std::ostringstream str;

    str << m_Project;
    str << ".";
    str << m_Compatible;
    str << ".";
    str << m_Feature;
    str << ".";
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

char* CleanStr(char* buff)
{
  int i,j;
  j = (int)strlen(buff);
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

bool GetVersion(const std::string& filePath, Version& version)
{
  FILE *f = NULL;

  bool result = false;
  f = fopen(filePath.c_str(), "r");

  if (f)
  {
    char buff[128];
    buff[0] = 0;

    if ( fgets(buff,sizeof(buff)-1,f) )
    {
      version = Version ( CleanStr(buff) );
      result = true;
    }

    fclose(f);
  }

  return result;
}

bool GetVersion(std::string filePath, std::string& versionString)
{
  FILE *f = NULL;

  bool result = false;
  f = fopen(filePath.c_str(), "r");

  if(f)
  {
    char buff[128];
    buff[0] = 0;

    if( fgets(buff,sizeof(buff)-1,f) )
    {
      versionString = CleanStr(buff);
      result = true;
    }

    fclose(f);
  }

  return result;
}

void HandleResult(VersionResult result, std::string details)
{
  char buf[256];
  std::string message;

  switch (result)
  {
  case VersionResults::Project:
    {
      sprintf(buf, "Your tools for project '%s' are for a different project. You cannot proceed using these tools.", NOCTURNAL_PROJECT_NAME);
      message = buf;
      break;
    }

  case VersionResults::Compatible:
    {
      sprintf(buf, "Your tools for project '%s' are not compatible anymore. Sorry, but you must update immediately.", NOCTURNAL_PROJECT_NAME);
      message = buf;
      break;
    }

  case VersionResults::Feature:
    {
      sprintf(buf, "The tools for project '%s' have been improved. You should update as soon as possible.", NOCTURNAL_PROJECT_NAME);
      message = buf;
      break;
    }

  case VersionResults::Patch:
    {
      sprintf(buf, "The tools for project '%s' have been updated. You should update when you get a chance.", NOCTURNAL_PROJECT_NAME);
      message = buf;
      break;
    }
  }

  if (!details.empty())
  {
    message += " (";
    message += details;
    message += ")";
  }

  if (result > VersionResults::Feature)
  {
    throw AppUtils::CheckVersionException ( message.c_str() );
  }
  else
  {
    Console::Warning("%s\n", message.c_str());
  }
}

void AppUtils::CheckVersion()
{
  //const char* projectName = getenv( NOCTURNAL_STUDIO_PREFIX "PROJECT_NAME" );
  //if ( projectName == NULL )
  //{
  //  throw AppUtils::Exception( "The %sPROJECT_NAME environment variable not set!\n", NOCTURNAL_STUDIO_PREFIX );
  //}

  //if ( _stricmp( projectName, NOCTURNAL_PROJECT_NAME ) != 0 )
  //{
  //  throw AppUtils::Exception( "You are running in the wrong project: '%s' != '%s'\n", projectName, NOCTURNAL_PROJECT_NAME );
  //}

  //const char* networkVersionPath = getenv( NOCTURNAL_STUDIO_PREFIX "NETWORK_VERSION_FILE" );
  //if ( networkVersionPath == NULL )
  //{
  //  throw AppUtils::Exception( "The %sNETWORK_VERSION_FILE environment variable not set!\n", NOCTURNAL_STUDIO_PREFIX );
  //}

  //const char* ignoreVersion = getenv( NOCTURNAL_STUDIO_PREFIX "IGNORE_TOOLS_VERSION" );
  //if ( ignoreVersion != NULL )
  //{
  //  if ( atoi( ignoreVersion ) )
  //  {
  //    Console::Warning( "Ignoring tools version.  This is dangerous and you could cause data loss.\n" );
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
  //  Console::Warning("Unable to read network version from '%s'\n", networkVersionPath);
  //  return;
  //}

  //// do the compare
  //VersionResult result = localVersion.Compare( networkVersion );

  //// mainline version check, local to network
  //if (result != VersionResults::Match)
  //{
  //  HandleResult(result, std::string("local: ") + localVersion.IntoString() + " != " + "network: " + networkVersion.IntoString());
  //}
}
