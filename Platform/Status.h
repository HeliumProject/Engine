#pragma once

#include "Platform/API.h"
#include "Platform/Types.h"

namespace Helium
{
    namespace StatusModes
    {
        enum Type
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
    typedef StatusModes::Type StatusMode;

    class HELIUM_PLATFORM_API Status
    {
	public:
        Status();

	    bool Read( const tchar_t* path );

        uint32_t     m_Mode;
        uint64_t     m_Size;
        uint64_t     m_CreatedTime;
        uint64_t     m_ModifiedTime;
        uint64_t     m_AccessTime;
	};
}
