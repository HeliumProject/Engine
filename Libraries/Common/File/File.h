#pragma once

#include "Path.h"

namespace Nocturnal
{
  class COMMON_API File
  {
  public:
    File( const std::string& path = "" )
      : m_LastModTime( 0 )
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

    const Path& GetPath() const
    {
      return m_Path;
    }

    bool operator<( const File& rhs ) const
    {
        return m_Path < rhs.m_Path;
    }

    bool Exists() const;
    bool MakePath();
    bool Create();
    bool Copy( const std::string& target, bool overwrite = true );
    bool Move( const std::string& target );
    bool Delete();

    bool GetAttributes( u32& attributes ) const;
    bool GetStats( struct _stat& stats ) const;
    bool GetStats64( struct _stati64& stats ) const;

    u64 GetCreatedTime();
    u64 GetModifiedTime();

    u64 Size();
    std::string SizeAsString();

    bool IsWritable() const;
    bool IsFile() const;
    bool IsFolder() const;
    bool HasChanged() const;
    bool HasChangedSince( u64 lastTime ) const;

    std::string CRC();
    bool VerifyCRC( const std::string &hashString );

    std::string MD5();
    bool VerifyMD5( const std::string &hashString );

  private:
    Path  m_Path;
    i64   m_LastModTime;
  };
}