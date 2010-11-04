//----------------------------------------------------------------------------------------------------------------------
// FileStream.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "CorePch.h"
#include "Core/FileStream.h"

namespace Lunar
{
    /// Constructor.
    FileStream::FileStream()
        : m_modeFlags( 0 )
    {
    }

    /// Destructor.
    FileStream::~FileStream()
    {
    }

    /// Open a file.
    ///
    /// @param[in] pPath      Path name of the file to open.
    /// @param[in] modeFlags  Combination of EMode flags specifying the mode in which to open the file.
    /// @param[in] bTruncate  If the MODE_WRITE flag is set, true to truncate any existing file, false to append to any
    ///                       existing file.  This is ignored if MODE_WRITE is not set.
    ///
    /// @return  True if the file was successfully opened, false if not.
    ///
    /// @see Close(), IsOpen()
    bool FileStream::Open( const tchar_t* pPath, uint32_t modeFlags, bool bTruncate )
    {
        L_ASSERT( pPath );

        // Verify that at least one mode flag is given.
        if( !( modeFlags & ( MODE_READ | MODE_WRITE ) ) )
        {
            L_ASSERT_MESSAGE_FALSE( TXT( "At least one FileStream::EMode flag must be set" ) );
            return false;
        }

        // Close any currently open file.
        Close();

        // Pass onto the derived class implementation.
        bool bResult = OpenActual( pPath, modeFlags, bTruncate && ( modeFlags & MODE_WRITE ) );
        if( bResult )
        {
            m_modeFlags = modeFlags;
        }

        return bResult;
    }

    /// @copydoc Stream::CanRead()
    bool FileStream::CanRead() const
    {
        return( IsOpen() && ( m_modeFlags & MODE_READ ) != 0 );
    }

    /// @copydoc Stream::CanWrite()
    bool FileStream::CanWrite() const
    {
        return( IsOpen() && ( m_modeFlags & MODE_WRITE ) != 0 );
    }

    /// @copydoc Stream::CanSeek()
    bool FileStream::CanSeek() const
    {
        return IsOpen();
    }

    /// @fn bool OpenActual( const tchar_t* pFileName, uint32_t modeFlags, bool bTruncate )
    /// Perform the actual platform-specific work of opening a file.
    ///
    /// The base FileStream class automatically handles closing any currently open file and verifying that at least one
    /// of the mode flags are set.
    ///
    /// @param[in] pPath      Path name of the file to open.
    /// @param[in] modeFlags  Combination of EMode flags specifying the mode in which to open the file.
    /// @param[in] bTruncate  If the MODE_WRITE flag is set, true to truncate any existing file, false to append to any
    ///                       existing file.  This is guaranteed not to be set if MODE_WRITE is not set.
    ///
    /// @return  True if the file was successfully opened, false if not.
}
