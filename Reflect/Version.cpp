#include "stdafx.h"
#include "Version.h"
#include "Serializers.h"

#include "FileSystem/FileSystem.h"

using namespace Reflect;

REFLECT_DEFINE_CLASS(Version);

void Version::EnumerateClass( Reflect::Compositor<Version>& comp )
{
  comp.AddField( &Version::m_Source, "m_Source" );
  comp.AddField( &Version::m_SourceVersion, "m_SourceVersion" );
}

Reflect::Version::Version()
{
  DetectVersion();
}

Reflect::Version::Version(const char* source, const char* sourceVersion)
: m_Source (source)
, m_SourceVersion (sourceVersion)
{

}

bool Reflect::Version::IsCurrent()
{
  return true;
}

bool Reflect::Version::ConvertToInts( int* ints )
{
  bool return_val = false;

  if ( !m_SourceVersion.empty() )
  {
    return_val = true;

    std::string digit;
    int tmp;

    size_t cur = 0, mark = -1;
    for ( int i = 0; i < 4; ++i )
    {
      ints[i] = 0;

      mark = i == 3 ? m_SourceVersion.length() : m_SourceVersion.find( '.', cur );
      if ( mark == -1 )
      {
        return_val = false;
        break;
      }

      digit = m_SourceVersion.substr( cur, mark - cur );

      if ( 1 == sscanf( digit.c_str(), "%d", &tmp) )
      {
        ints[i] = tmp;
      }
      else
      {
        return_val = false;
        break;
      }

      cur = mark + 1;
    }
  }

  return return_val;
}

void Reflect::Version::DetectVersion()
{
  HMODULE moduleHandle = GetModuleHandle( NULL );

  char exeFilename[ MAX_PATH + 1 ];
  GetModuleFileName( moduleHandle, exeFilename, MAX_PATH );

  m_Source = exeFilename;

  // if we found an executable, get its version
  if ( !m_Source.empty() )
  {
    FileSystem::GetWinVersionInfo( m_Source, m_SourceVersion );

    FileSystem::GetLeaf( m_Source.c_str(), m_Source );
  }
}
