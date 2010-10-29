#pragma once

#include "API.h"
#include "Types.h"

namespace Helium
{
    namespace ModeFlags
    {
        enum ModeFlag
        {
            File             = 1 << 0,  // File and Directory should be mutually exclusive
            Directory        = 1 << 1,
            Pipe             = 1 << 2,
            Special          = 1 << 3,

            Read             = 1 << 8,
            Write            = 1 << 9,
            Execute          = 1 << 10
        };
    }
    typedef ModeFlags::ModeFlag ModeFlag;

    struct PLATFORM_API Stat
    {
        uint16_t m_Mode;
        int64_t m_Size;
        uint64_t m_AccessTime;
        uint64_t m_ModifiedTime;
        uint64_t m_CreatedTime;

        Stat()
            : m_Mode( 0 )
            , m_Size( 0 )
            , m_AccessTime( 0 )
            , m_ModifiedTime( 0 )
            , m_CreatedTime( 0 )
        {
        }
    };

    PLATFORM_API bool StatPath( const tchar* path, Stat& stat );
}
