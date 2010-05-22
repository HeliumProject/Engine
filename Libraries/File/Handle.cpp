#include "StdAfx.h"

#include "Handle.h"

File::Handle::Handle( const std::string& path, const char* mode )
: m_Path( path )
, m_Mode( mode )
, m_FileHandle( NULL )
, m_OpenCount( 0 )
{
}

bool File::Handle::IsValid()
{
  return m_FileHandle != NULL;
}

bool File::Handle::Open()
{
  ++m_OpenCount;

  if ( m_FileHandle == NULL )
  {
    m_FileHandle = fopen( m_Path.c_str(), m_Mode );
  }

  return IsValid();
}

void File::Handle::Close()
{
  if ( --m_OpenCount == 0 && m_FileHandle )
  {
    fclose( m_FileHandle );
    m_FileHandle = NULL;
  }
}
