#pragma once

#include "Path.h"

namespace Nocturnal
{
  class COMMON_API File
  {
  public:
    File( const std::string& path = "" )
      : m_ModTime( 0 )
    {
      m_Path.Set( path );
      Update();
    }

    virtual ~File()
    {
    }

    void Update(); // update the status of the file

  public:
    void SetPath( const std::string& path )
    {
      m_Path.Set( path );
      Update();
    }

    Path& GetPath()
    {
      return m_Path;
    }

    bool Exists();
    void MakePath();
    void Create();
    bool Copy( const std::string& target, bool overwrite = true );
    bool Move( const std::string& target );
    bool Delete();

    bool GetAttributes( u32& attributes );
    bool GetStats( struct _stat& stats );
    bool GetStats64( struct _stati64& stats );

    u64 Size();

    bool IsWritable();
    bool IsFile();
    bool IsFolder();
    bool HasChanged();

    std::string CRC();
    bool VerifyCRC( const std::string &hashString );

    std::string MD5();
    bool VerifyMD5( const std::string &hashString );

  private:
    Path  m_Path;
    i64   m_ModTime;
  };
}