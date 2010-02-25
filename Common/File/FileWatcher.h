#pragma once

#include "../API.h"
#include "../Automation/Event.h"
#include "File.h"

namespace Nocturnal
{
  struct COMMON_API FileChangedArgs
  {
    std::string m_Path;

    FileChangedArgs( const std::string& path )
      : m_Path( path )
    {
    }
  };
  typedef Nocturnal::Signature< void, const FileChangedArgs& > FileChangedSignature;

  typedef void* HANDLE;
  struct COMMON_API FileWatch
  {
    HANDLE                      m_ChangeHandle;
    FileChangedSignature::Event m_Event;
    File                        m_File;

    FileWatch()
      : m_ChangeHandle( NULL )
    {
    }
  };

  typedef std::map< std::string, FileWatch > M_PathToFileWatch;

  typedef void* HANDLE;

  class COMMON_API FileWatcher
  {
  private:
    M_PathToFileWatch m_Watches;

  public:
    FileWatcher();
    ~FileWatcher();

    bool Add( const std::string& path, FileChangedSignature::Delegate& listener );
    bool Remove( const std::string& path, FileChangedSignature::Delegate& listener );
    bool Watch( int timeout = 0xFFFFFFFF );
  };
}