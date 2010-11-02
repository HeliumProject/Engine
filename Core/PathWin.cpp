//----------------------------------------------------------------------------------------------------------------------
// PathWin.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "CorePch.h"

#if L_OS_WIN

#include "Core/Path.h"

#define INVALID_PATH_CHARACTERS \
    L_T( "\01\02\03\04\05\06\07\10\11\12\13\14\15\16\17\20\21\22\23\24\25\26\27\30\31\32\33\34\35\36\37\"<>|\b\t*?" )

namespace Lunar
{
    String Path::sm_invalidPathCharacters( INVALID_PATH_CHARACTERS );
    String Path::sm_invalidFileNameCharacters( INVALID_PATH_CHARACTERS L_T( ":\\/" ) );

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
        if( character == L_T( '\0' ) )
        {
            return false;
        }

        // Check if the path is a drive specification (i.e. "C:\").
        if( ( character >= L_T( 'a' ) && character <= L_T( 'z' ) ) ||
            ( character >= L_T( 'A' ) && character <= L_T( 'Z' ) ) )
        {
            character = pPath[ 1 ];
            if( character != L_T( ':' ) )
            {
                return false;
            }

            character = pPath[ 2 ];
            if( character != L_T( '\\' ) && character != L_T( '/' ) )
            {
                return false;
            }

            return( pPath[ 3 ] == L_T( '\0' ) );
        }

        // Check if the path is the root of a UNC path (i.e. "\\server\").
        if( character == L_T( '\\' ) || character == L_T( '/' ) )
        {
            character = pPath[ 1 ];
            if( character != L_T( '\\' ) && character != L_T( '/' ) )
            {
                return false;
            }

            // Check for a valid network resource name and trailing path separator.
            size_t characterIndex = 2;
            for( ; ; )
            {
                character = pPath[ characterIndex ];
                ++characterIndex;

                if( character == L_T( '\\' ) || character == L_T( '/' ) )
                {
                    // Make sure we have at least something for a resource name.
                    if( characterIndex == 3 )
                    {
                        return false;
                    }

                    break;
                }

                if( character == L_T( '\0' ) || IsValid( sm_invalidFileNameCharacters.Find( character ) ) )
                {
                    // Invalid character encountered.
                    return false;
                }
            }

            return( pPath[ characterIndex ] == L_T( '\0' ) );
        }

        return false;
    }
}

#endif  // L_OS_WIN
