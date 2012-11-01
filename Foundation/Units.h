#pragma once

#include "Platform/Types.h"

#include <string>
#include <stdio.h>

#define BYTE_TO_STR_BUF_SIZE 32

namespace Helium
{
    inline tstring BytesToString( uint64_t bytes )
    {
        tchar_t buf[ BYTE_TO_STR_BUF_SIZE ];

        if ( bytes < 1024 )
        {
            _sntprintf( buf, BYTE_TO_STR_BUF_SIZE, TXT( "%.0lf B" ), (float64_t) bytes );
        } 
        else if ( bytes < ( 1024 * 1024 ) )
        {
            _sntprintf( buf, BYTE_TO_STR_BUF_SIZE, TXT( "%.1lf KB" ), bytes / (float64_t) ( 1024.0f ) );
        }    
        else if ( bytes < ( 1024 * 1024 * 1024 ) )
        {
            _sntprintf( buf, BYTE_TO_STR_BUF_SIZE, TXT( "%.1lf MB" ), bytes / (float64_t) ( 1024.0f * 1024.0f ) );
        }
        else if ( bytes < ( (uint64_t) 1024 * 1024 * 1024 * 1024 ) )
        {
            _sntprintf( buf, BYTE_TO_STR_BUF_SIZE, TXT( "%.1lf GB" ), bytes / (float64_t) ( 1024.0f * 1024.0f * 1024.0f ) );
        }
        else
        {
            _sntprintf( buf, BYTE_TO_STR_BUF_SIZE, TXT( "%.1lf TB" ), bytes / (float64_t) ( 1024.0f * 1024.0f * 1024.0f * 1024.0f ) );
        } 

        return buf;
    }
}