//----------------------------------------------------------------------------------------------------------------------
// Path.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "CorePch.h"
#include "Core/Path.h"

#include "Core/String.h"

namespace Lunar
{
    /// Free any statically allocated resources.
    ///
    /// This should only be called immediately prior to application exit.
    void Path::Shutdown()
    {
        sm_invalidPathCharacters.Clear();
        sm_invalidFileNameCharacters.Clear();
    }

    /// Get the directory component of the given path string.
    ///
    /// @param[out] rOut   Directory component, with the trailing directory separator character removed unless the
    ///                    result is a root directory (i.e. "C:\" on Windows, "/" on OS X or Unix).  If @c rPath itself
    ///                    is empty or a root directory, this will be set to an empty string.
    /// @param[in]  rPath  Path string to parse.
    void Path::GetDirectoryName( String& rOut, const String& rPath )
    {
        // Check for empty paths or root directories.
        if( rPath.IsEmpty() || IsRootDirectory( rPath ) )
        {
            rOut.Clear();
            return;
        }

        // Locate the last path separator character in the string.
        size_t separatorIndex = rPath.FindAnyReverse( L_PATH_SEPARATOR_LIST );
        if( IsInvalid( separatorIndex ) )
        {
            // Character not found.
            rOut.Clear();

            return;
        }

        // Set the output string to the directory, including a trailing separator character only if the result is a root
        // directory.
        size_t tempStringSize = separatorIndex + 1;

        if( &rOut == &rPath )
        {
            // Input and output strings are the same, so shrink down the output string in-place as needed (memory will
            // not be reallocated when shrinking).
            rOut.Remove( tempStringSize, rOut.GetSize() - tempStringSize );
            if( !IsRootDirectory( rOut ) )
            {
                rOut.Remove( tempStringSize - 1 );
            }
        }
        else
        {
            // Input and output strings differ, so work with a copy of the directory name to avoid unnecessary dynamic
            // heap allocations.
            StackMemoryHeap<>& rStackHeap = ThreadLocalStackAllocator::GetMemoryHeap();
            StackMemoryHeap<>::Marker stackMarker( rStackHeap );

            tchar_t* pTempString =
                static_cast< tchar_t* >( rStackHeap.Allocate( sizeof( tchar_t ) * ( tempStringSize + 1 ) ) );
            HELIUM_ASSERT( pTempString );

            MemoryCopy( pTempString, rPath.GetData(), sizeof( tchar_t ) * tempStringSize );
            pTempString[ tempStringSize ] = TXT( '\0' );

            if( !IsRootDirectory( pTempString ) )
            {
                pTempString[ tempStringSize - 1 ] = TXT( '\0' );
            }

            rOut = pTempString;

            stackMarker.Pop();
        }
    }

    /// Get the base name of the given path.
    ///
    /// @param[out] rOut             Base name (path name with the directory component removed) of @c rPath.
    /// @param[in]  rPath            Path string to parse.
    /// @param[in]  bStripExtension  True to strip the extension from the base name as well, false to leave it intact.
    void Path::GetBaseName( String& rOut, const String& rPath, bool bStripExtension )
    {
        // Check for empty paths or root directories.
        if( rPath.IsEmpty() || IsRootDirectory( rPath ) )
        {
            rOut.Clear();
            return;
        }

        // Locate the last path separator character in the string.
        size_t separatorIndex = rPath.FindAnyReverse( L_PATH_SEPARATOR_LIST );
        if( IsInvalid( separatorIndex ) )
        {
            // Character not found, so use the entire string.
            if( &rOut != &rPath )
            {
                rOut = rPath;
            }

            return;
        }

        // Set the output string to the base name.
        size_t baseNameStartIndex = separatorIndex + 1;

        if( &rOut == &rPath )
        {
            // Input and output strings are the same, so shrink down the output string in-place as needed (memory will
            // not be reallocated when shrinking).
            rOut.Remove( 0, baseNameStartIndex );
            if( bStripExtension )
            {
                size_t extensionIndex = rOut.FindReverse( TXT( '.' ) );
                if( IsValid( extensionIndex ) )
                {
                    rOut.Remove( extensionIndex, rOut.GetSize() - extensionIndex );
                }
            }
        }
        else
        {
            // Input and output strings differ, so work with a copy of the base name to avoid unnecessary dynamic heap
            // allocations.
            size_t baseNameSize = rPath.GetSize() - baseNameStartIndex;
            if( bStripExtension )
            {
                size_t extensionIndex = rPath.FindReverse( TXT( '.' ) );
                if( IsValid( extensionIndex ) && extensionIndex >= baseNameStartIndex )
                {
                    baseNameSize = extensionIndex - baseNameStartIndex;
                }
            }

            StackMemoryHeap<>& rStackHeap = ThreadLocalStackAllocator::GetMemoryHeap();
            StackMemoryHeap<>::Marker stackMarker( rStackHeap );

            tchar_t* pTempString =
                static_cast< tchar_t* >( rStackHeap.Allocate( sizeof( tchar_t ) * ( baseNameSize + 1 ) ) );
            HELIUM_ASSERT( pTempString );

            MemoryCopy( pTempString, rPath.GetData() + baseNameStartIndex, sizeof( tchar_t ) * baseNameSize );
            pTempString[ baseNameSize ] = TXT( '\0' );

            rOut = pTempString;

            stackMarker.Pop();
        }
    }

    /// Get the file extension in the given path.
    ///
    /// @param[out] rOut   File extension, including the preceding dot.
    /// @param[in]  rPath  Path string to parse.
    void Path::GetExtension( String& rOut, const String& rPath )
    {
        // Locate the last instance of either a dot for the extension or a path separator, whichever comes first.
        size_t extensionIndex = rPath.FindAnyReverse( L_PATH_SEPARATOR_LIST TXT( "." ) );
        if( IsInvalid( extensionIndex ) || rPath[ extensionIndex ] != TXT( '.' ) )
        {
            rOut.Clear();
            return;
        }

        if( &rOut == &rPath )
        {
            // Input and output strings are the same, so shrink down the output string in-place as needed (memory will
            // not be reallocated when shrinking).
            rOut.Remove( 0, extensionIndex );
        }
        else
        {
            // Input and output strings differ, so work with a copy of the extension to avoid unnecessary dynamic heap
            // allocations.
            size_t extensionSize = rPath.GetSize() - extensionIndex;

            StackMemoryHeap<>& rStackHeap = ThreadLocalStackAllocator::GetMemoryHeap();
            StackMemoryHeap<>::Marker stackMarker( rStackHeap );

            tchar_t* pTempString =
                static_cast< tchar_t* >( rStackHeap.Allocate( sizeof( tchar_t ) * ( extensionSize + 1 ) ) );
            HELIUM_ASSERT( pTempString );

            MemoryCopy( pTempString, rPath.GetData() + extensionIndex, sizeof( tchar_t ) * extensionSize );
            pTempString[ extensionSize ] = TXT( '\0' );

            rOut = pTempString;

            stackMarker.Pop();
        }
    }

    /// Combine the two path strings with a path separator if necessary.
    ///
    /// @param[out] rOut    The combined path string.
    /// @param[in]  rPath0  First path string.
    /// @param[in]  rPath1  Second path string.
    void Path::Combine( String& rOut, const String& rPath0, const String& rPath1 )
    {
        size_t path0Size = rPath0.GetSize();
        size_t path1Size = rPath1.GetSize();

        bool bPath0EndsWithSeparator = false;  // These can both remain false if either path string is empty.
        bool bPath1StartsWithSeparator = false;

        bool bIncludeSeparator;
        if( path0Size == 0 || path1Size == 0 )
        {
            bIncludeSeparator = false;
        }
        else
        {
            tchar_t path0End = rPath0[ path0Size - 1 ];
            tchar_t path1Start = rPath1[ 0 ];

            bPath0EndsWithSeparator = ( path0End == L_PATH_SEPARATOR_CHAR || path0End == L_ALT_PATH_SEPARATOR_CHAR );
            bPath1StartsWithSeparator =
                ( path1Start == L_PATH_SEPARATOR_CHAR || path1Start == L_ALT_PATH_SEPARATOR_CHAR );

            bIncludeSeparator = ( !bPath0EndsWithSeparator && !bPath1StartsWithSeparator );
        }

        const tchar_t* pPath1 = rPath1.GetData();

        StackMemoryHeap<>& rStackHeap = ThreadLocalStackAllocator::GetMemoryHeap();
        StackMemoryHeap<>::Marker stackMarker( rStackHeap );
        if( &rOut == &rPath1 )
        {
            // Output string and the second path are the same, so work with a copy of the second path.
            tchar_t* pNewPath = static_cast< tchar_t* >( rStackHeap.Allocate( sizeof( tchar_t ) * ( path1Size + 1 ) ) );
            HELIUM_ASSERT( pNewPath );

            MemoryCopy( pNewPath, pPath1, sizeof( tchar_t ) * path1Size );
            pNewPath[ path1Size ] = TXT( '\0' );

            pPath1 = pNewPath;
        }

        if( bPath0EndsWithSeparator && bPath1StartsWithSeparator )
        {
            // Don't concatenate with multiple separators.
            ++pPath1;
            --path1Size;
        }

        size_t outSize = path0Size + path1Size;
        if( bIncludeSeparator )
        {
            ++outSize;
        }

        if( &rOut == &rPath0 )
        {
            // First path is the same as our output string, so combine in-place.
            rOut.Reserve( outSize );
        }
        else
        {
            rOut.Remove( 0, rOut.GetSize() );
            rOut.Reserve( outSize );
            rOut += rPath0;
        }

        if( bIncludeSeparator )
        {
            rOut += L_PATH_SEPARATOR_CHAR;
        }

        if( pPath1 )
        {
            rOut += pPath1;
        }

        stackMarker.Pop();
    }

    /// Get whether the given string specifies a path to a root directory for the current platform.
    ///
    /// @param[in] rPath  Path string to test.
    ///
    /// @return  True if the path is a root directory, false if not.
    bool Path::IsRootDirectory( const String& rPath )
    {
        return IsRootDirectory( rPath.GetData() );
    }
}
