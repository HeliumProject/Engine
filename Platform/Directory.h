#pragma once

#include "API.h"

#include "Platform/Types.h"
#include "Platform/Stat.h"
#include "Platform/File.h"

namespace Helium
{
    struct DirectoryHandle
    {
        DirectoryHandle( const tstring& path = TXT( "" ) )
            : m_Path( path )
            , m_Handle( InvalidHandleValue )
        {
        }

        tstring m_Path;
        void*   m_Handle;
    };

    struct FileFindData
    {
        FileFindData( const tstring& filename = TXT( "" ) )
            : m_Filename( filename )
        {
        }

        tstring	m_Filename;
        Stat    m_Stat;
    };

    HELIUM_PLATFORM_API bool FindFirst( DirectoryHandle& handle, FileFindData& data );
    HELIUM_PLATFORM_API bool FindNext( DirectoryHandle& handle, FileFindData& data );
    HELIUM_PLATFORM_API bool GetExtendedData( DirectoryHandle& handle, FileFindData& );
    HELIUM_PLATFORM_API bool CloseFind( DirectoryHandle& handle );
}