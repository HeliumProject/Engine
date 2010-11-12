//----------------------------------------------------------------------------------------------------------------------
// PathWin.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "CorePch.h"

#if HELIUM_OS_WIN

#include "Core/Path.h"

#define INVALID_PATH_CHARACTERS \
    TXT( "\01\02\03\04\05\06\07\10\11\12\13\14\15\16\17\20\21\22\23\24\25\26\27\30\31\32\33\34\35\36\37\"<>|\b\t*?" )

namespace Lunar
{
    String Path::sm_invalidPathCharacters( INVALID_PATH_CHARACTERS );
    String Path::sm_invalidFileNameCharacters( INVALID_PATH_CHARACTERS TXT( ":\\/" ) );

    /// Get whether the given string specifies a path to a root directory for the current platform.
    ///
    /// @param[in] pPath  Null-terminated path string to test.
    ///
    /// @return  True if the path is a root directory, false if not.
    bool Path::IsRootDirectory( const tchar_t* pPath )
    {
        // Check for empty strings.
        if( !pPath )
        {
            return false;
        }

        tchar_t character = pPath[ 0 ];
        if( character == TXT( '\0' ) )
        {
            return false;
        }

        // Check if the path is a drive specification (i.e. "C:\").
        if( ( character >= TXT( 'a' ) && character <= TXT( 'z' ) ) ||
            ( character >= TXT( 'A' ) && character <= TXT( 'Z' ) ) )
        {
            character = pPath[ 1 ];
            if( character != TXT( ':' ) )
            {
                return false;
            }

            character = pPath[ 2 ];
            if( character != TXT( '\\' ) && character != TXT( '/' ) )
            {
                return false;
            }

            return( pPath[ 3 ] == TXT( '\0' ) );
        }

        // Check if the path is the root of a UNC path (i.e. "\\server\").
        if( character == TXT( '\\' ) || character == TXT( '/' ) )
        {
            character = pPath[ 1 ];
            if( character != TXT( '\\' ) && character != TXT( '/' ) )
            {
                return false;
            }

            // Check for a valid network resource name and trailing path separator.
            size_t characterIndex = 2;
            for( ; ; )
            {
                character = pPath[ characterIndex ];
                ++characterIndex;

                if( character == TXT( '\\' ) || character == TXT( '/' ) )
                {
                    // Make sure we have at least something for a resource name.
                    if( characterIndex == 3 )
                    {
                        return false;
                    }

                    break;
                }

                if( character == TXT( '\0' ) || IsValid( sm_invalidFileNameCharacters.Find( character ) ) )
                {
                    // Invalid character encountered.
                    return false;
                }
            }

            return( pPath[ characterIndex ] == TXT( '\0' ) );
        }

        return false;
    }
}

#endif  // HELIUM_OS_WIN
