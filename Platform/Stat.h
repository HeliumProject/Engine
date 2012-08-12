#pragma once

#include "API.h"
#include "Types.h"

namespace Helium
{
    namespace FileModeFlags
    {
        enum FileModeFlag
        {
            None             = 0,
            Directory        = 1 << 0,
            Link             = 1 << 1,
            Pipe             = 1 << 2,
            Special          = 1 << 3,

            Read             = 1 << 8,
            Write            = 1 << 9,
            Execute          = 1 << 10
        };
    }
    typedef FileModeFlags::FileModeFlag FileModeFlag;

    struct HELIUM_PLATFORM_API Stat
    {
        uint32_t     m_Mode;
        uint64_t     m_Size;
        uint64_t     m_CreatedTime;
        uint64_t     m_ModifiedTime;
        uint64_t     m_AccessTime;

        Stat()
            : m_Mode( 0 )
            , m_Size( 0 )
            , m_CreatedTime( 0 )
            , m_ModifiedTime( 0 )
            , m_AccessTime( 0 )
        {
        }
    };

    HELIUM_PLATFORM_API bool StatPath( const tchar_t* path, Stat& stat );
}
