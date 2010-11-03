#include "Handle.h"

using namespace Helium;

Handle::Handle( const tstring& path, const tchar_t* mode )
: m_Path( path )
, m_Mode( mode )
, m_FileHandle( NULL )
, m_OpenCount( 0 )
{
}

bool Handle::IsValid()
{
    return m_FileHandle != NULL;
}

bool Handle::Open()
{
    ++m_OpenCount;

    if ( m_FileHandle == NULL )
    {
        m_FileHandle = _tfopen( m_Path.c_str(), m_Mode );
    }

    return IsValid();
}

void Handle::Close()
{
    if ( --m_OpenCount == 0 && m_FileHandle )
    {
        fclose( m_FileHandle );
        m_FileHandle = NULL;
    }
}
