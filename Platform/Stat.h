#pragma once

#include "API.h"
#include "Types.h"

namespace Platform
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
        u16 m_Mode;
        i64 m_Size;
        u64 m_AccessTime;
        u64 m_ModifiedTime;
        u64 m_CreatedTime;

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
