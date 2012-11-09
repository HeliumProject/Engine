#include "FoundationPch.h"
#include "Foundation/FileStream.h"

using namespace Helium;


/// Attempt to open a file with a new file stream object.
///
/// @param[in] pPath      FilePath name of the file to open.
/// @param[in] modeFlags  Combination of FileStream::EMode flags specifying the mode in which to open the file.
/// @param[in] bTruncate  If the FileStream::MODE_WRITE flag is set, true to truncate any existing file, false to
///                       append to any existing file.  This is ignored if FileStream::MODE_WRITE is not set.
///
/// @return  Pointer to a FileStream instance opened for the specified file if it was successfully opened, null if
///          opening failed.  Note that the caller is responsible for deleting the FileStream instance when it is no
///          longer needed.
FileStream* FileStream::OpenFileStream( const tchar_t* pPath, uint32_t modeFlags, bool bTruncate )
{
    FileStream* pStream = new FileStream();
    HELIUM_ASSERT( pStream );
    if( !pStream->Open( pPath, modeFlags, bTruncate ) )
    {
        delete pStream;
        return NULL;
    }

    return pStream;
}

/// Attempt to open a file with a new file stream object.
///
/// @param[in] rPath      FilePath name of the file to open.
/// @param[in] modeFlags  Combination of FileStream::EMode flags specifying the mode in which to open the file.
/// @param[in] bTruncate  If the FileStream::MODE_WRITE flag is set, true to truncate any existing file, false to
///                       append to any existing file.  This is ignored if FileStream::MODE_WRITE is not set.
///
/// @return  Pointer to a FileStream instance opened for the specified file if it was successfully opened, null if
///          opening failed.  Note that the caller is responsible for deleting the FileStream instance when it is no
///          longer needed.
FileStream* FileStream::OpenFileStream( const String& rPath, uint32_t modeFlags, bool bTruncate )
{
    return OpenFileStream( *rPath, modeFlags, bTruncate );
}

/// Constructor.
FileStream::FileStream()
: m_modeFlags( 0 )
{
}

/// Destructor.
FileStream::~FileStream()
{
    Close();
}

/// @copydoc Stream::Close()
void FileStream::Close()
{
	m_File.Close();
}

/// @copydoc Stream::IsOpen()
bool FileStream::IsOpen() const
{
    return m_File.IsOpen();
}

/// @copydoc Stream::Read()
size_t FileStream::Read( void* pBuffer, size_t size, size_t count )
{
    HELIUM_ASSERT_MSG( m_File.IsOpen(), TXT( "File not open" ) );
    HELIUM_ASSERT_MSG( m_modeFlags & MODE_READ, TXT( "File not open for reading" ) );
    if( !m_File.IsOpen() || !( m_modeFlags & MODE_READ ) )
    {
        return 0;
    }

    size_t byteCount = size * count;
    size_t bytesRead = 0;
    HELIUM_VERIFY( m_File.Read( pBuffer, byteCount, &bytesRead ) );

    return ( bytesRead / size );
}

/// @copydoc Stream::Write()
size_t FileStream::Write( const void* pBuffer, size_t size, size_t count )
{
    HELIUM_ASSERT_MSG( m_File.IsOpen(), TXT( "File not open" ) );
    HELIUM_ASSERT_MSG( m_modeFlags & MODE_WRITE, TXT( "File not open for writing" ) );
	if( !m_File.IsOpen() || !( m_modeFlags & MODE_WRITE ) )
    {
        return 0;
    }

    size_t byteCount = size * count;
    size_t bytesWritten = 0;
    HELIUM_VERIFY( m_File.Write( pBuffer, byteCount, &bytesWritten ) );

    return ( bytesWritten / size );
}

/// @copydoc Stream::Flush()
void FileStream::Flush()
{
    HELIUM_ASSERT_MSG( m_File.IsOpen(), TXT( "File not open" ) );

    // Only files open for writing need to be flushed.
    if( m_File.IsOpen() && ( m_modeFlags & MODE_WRITE ) )
    {
        HELIUM_VERIFY( m_File.Flush() );
    }
}

/// @copydoc Stream::Seek()
int64_t FileStream::Seek( int64_t offset, SeekOrigin origin )
{
	if( !m_File.IsOpen() )
    {
        HELIUM_ASSERT_MSG_FALSE( TXT( "File not open" ) );
        return -1;
    }

    return m_File.Seek( offset, origin );
}

/// @copydoc Stream::Tell()
int64_t FileStream::Tell() const
{
    if( !m_File.IsOpen() )
    {
        HELIUM_ASSERT_MSG_FALSE( TXT( "File not open" ) );
        return -1;
    }

    return m_File.Tell();
}

/// @copydoc Stream::GetSize()
int64_t FileStream::GetSize() const
{
	if( !m_File.IsOpen() )
    {
        HELIUM_ASSERT_MSG_FALSE( TXT( "File not open" ) );
        return -1;
    }

    return m_File.GetSize();
}

/// @copydoc FileStream::OpenActual()
bool FileStream::Open( const tchar_t* pPath, uint32_t modeFlags, bool bTruncate )
{
    HELIUM_ASSERT( pPath );

    // Verify that at least one mode flag is given.
    if( !( modeFlags & ( MODE_READ | MODE_WRITE ) ) )
    {
        HELIUM_ASSERT_MSG_FALSE( TXT( "At least one FileStream::EMode flag must be set" ) );
        return false;
    }

    // Close any currently open file.
    Close();

	HELIUM_ASSERT( !m_File.IsOpen() );
	if ( !m_File.Open( pPath, (Helium::FileMode)modeFlags, bTruncate ) )
    {
        return false;
    }

    m_modeFlags = modeFlags;
    return true;
}
