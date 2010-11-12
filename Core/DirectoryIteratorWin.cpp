//----------------------------------------------------------------------------------------------------------------------
// DirectoryIteratorWin.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "CorePch.h"

#if HELIUM_OS_WIN

#include "Core/DirectoryIteratorWin.h"

#include "Core/Path.h"

namespace Lunar
{
    /// Constructor.
    ///
    /// @param[in] pDirectoryPath  Path name of the directory in which to search.
    DirectoryIteratorWin::DirectoryIteratorWin( const tchar_t* pDirectoryPath )
        : m_hFindFile( INVALID_HANDLE_VALUE )
    {
        HELIUM_ASSERT( pDirectoryPath );

        size_t directoryLength = StringLength( pDirectoryPath );

        const tchar_t* pWildcardSuffix;
        size_t wildcardSuffixSize;
        if( directoryLength != 0 &&
            pDirectoryPath[ directoryLength - 1 ] != L_PATH_SEPARATOR_CHAR &&
            pDirectoryPath[ directoryLength - 1 ] != L_ALT_PATH_SEPARATOR_CHAR )
        {
            const tchar_t wildcardSuffix[] = TXT( "\\*.*" );

            pWildcardSuffix = wildcardSuffix;
            wildcardSuffixSize = sizeof( wildcardSuffix );
        }
        else
        {
            const tchar_t wildcardSuffix[] = TXT( "*.*" );

            pWildcardSuffix = wildcardSuffix;
            wildcardSuffixSize = sizeof( wildcardSuffix );
        }

        StackMemoryHeap<>& rStackHeap = ThreadLocalStackAllocator::GetMemoryHeap();
        StackMemoryHeap<>::Marker stackMarker( rStackHeap );

        tchar_t* pSearchString = static_cast< tchar_t* >( rStackHeap.Allocate(
            sizeof( tchar_t ) * directoryLength + wildcardSuffixSize ) );
        HELIUM_ASSERT( pSearchString );
        MemoryCopy( pSearchString, pDirectoryPath, sizeof( tchar_t ) * directoryLength );
        MemoryCopy( pSearchString + directoryLength, pWildcardSuffix, wildcardSuffixSize );

        m_hFindFile = FindFirstFile( pSearchString, &m_findFileData );
    }

    /// Destructor.
    DirectoryIteratorWin::~DirectoryIteratorWin()
    {
        if( m_hFindFile != INVALID_HANDLE_VALUE )
        {
            FindClose( m_hFindFile );
        }
    }

    /// @copydoc DirectoryIterator::GetFileName()
    bool DirectoryIteratorWin::GetFileName( String& rFileName ) const
    {
        if( m_hFindFile == INVALID_HANDLE_VALUE )
        {
            return false;
        }

        rFileName = m_findFileData.cFileName;

        return true;
    }

    /// @copydoc DirectoryIterator::IsValid()
    bool DirectoryIteratorWin::IsValid() const
    {
        return ( m_hFindFile != INVALID_HANDLE_VALUE );
    }

    /// @copydoc DirectoryIterator::Advance()
    bool DirectoryIteratorWin::Advance()
    {
        if( m_hFindFile == INVALID_HANDLE_VALUE )
        {
            return false;
        }

        for( ; ; )
        {
            if( !FindNextFile( m_hFindFile, &m_findFileData ) )
            {
                FindClose( m_hFindFile );
                m_hFindFile = INVALID_HANDLE_VALUE;

                return false;
            }

            // Skip over "." and "..".
            if( m_findFileData.cFileName[ 0 ] != TXT( '.' ) ||
                ( m_findFileData.cFileName[ 1 ] != TXT( '\0' ) &&
                  ( m_findFileData.cFileName[ 1 ] != TXT( '.' ) || m_findFileData.cFileName[ 2 ] != TXT( '\0' ) ) ) )
            {
                return true;
            }
        }
    }
}

#endif  // HELIUM_OS_WIN
