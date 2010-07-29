#pragma once

#include "Platform/Types.h"

#include <string>
#include <stdio.h>

namespace Helium
{
    inline tstring BytesToString( u64 bytes )
    {
        tchar buf[ 32 ];
        if ( bytes >= 1024 * 1024 * 1024 )
        {
            _snprintf( buf, 32, "%0.2f GB", bytes / (f32) ( 1024 * 1024 * 1024 ) );
        }      
        else if ( bytes >= 1024 * 1024 )
        {
            _snprintf( buf, 32, "%0.2f MB", bytes / (f32) ( 1024 * 1024 ) );
        }      
        else if ( bytes >= 1024 )
        {
            _snprintf( buf, 32, "%0.2f KB", bytes / (f32) ( 1024 ) );
        }      
        else
        {
            _snprintf( buf, 32, "%0.2f B", bytes );
        }

        return buf;
    }
}