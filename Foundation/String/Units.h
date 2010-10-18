#pragma once

#include "Platform/Types.h"

#include <string>
#include <stdio.h>

#define BYTE_TO_STR_BUF_SIZE 32

namespace Helium
{
    inline tstring BytesToString( u64 bytes )
    {
        tchar buf[ BYTE_TO_STR_BUF_SIZE ];

        if ( bytes < 1024 )
        {
            _sntprintf( buf, BYTE_TO_STR_BUF_SIZE, TXT( "%.0lf B" ), (f64) bytes );
        } 
        else if ( bytes < ( 1024 * 1024 ) )
        {
            _sntprintf( buf, BYTE_TO_STR_BUF_SIZE, TXT( "%.1lf KB" ), bytes / (f64) ( 1024.0f ) );
        }    
        else if ( bytes < ( 1024 * 1024 * 1024 ) )
        {
            _sntprintf( buf, BYTE_TO_STR_BUF_SIZE, TXT( "%.1lf MB" ), bytes / (f64) ( 1024.0f * 1024.0f ) );
        }
        else if ( bytes < ( (u64) 1024 * 1024 * 1024 * 1024 ) )
        {
            _sntprintf( buf, BYTE_TO_STR_BUF_SIZE, TXT( "%.1lf GB" ), bytes / (f64) ( 1024.0f * 1024.0f * 1024.0f ) );
        }
        else
        {
            _sntprintf( buf, BYTE_TO_STR_BUF_SIZE, TXT( "%.1lf TB" ), bytes / (f64) ( 1024.0f * 1024.0f * 1024.0f * 1024.0f ) );
        } 

        return buf;
    }
}