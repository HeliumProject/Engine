//#include "CorePch.h"
#include "Foundation/Stream/Stream.h"

using namespace Helium;

/// Destructor.
Stream::~Stream()
{
}

/// @fn virtual void Stream::Close()
/// Close the current stream, flushing any buffered contents and releasing any system resources allocated.

/// @fn bool Stream::IsOpen() const
/// Get whether this stream is currently open.
///
/// @return  True if this stream is open, false if not.
///
/// @see Close()

/// @fn size_t Stream::Read( void* pBuffer, size_t size, size_t count )
/// Read data from this stream and advance the stream position by the number of bytes read.
///
/// @param[out] pBuffer  Buffer into which data should be read.
/// @param[in]  size     Size of each block of data to read.
/// @param[in]  count    Number of blocks to read.
///
/// @return  Number of blocks successfully read.
///
/// @see IsOpen(), Write(), Tell(), CanRead()

/// @fn size_t Stream::Write( const void* pBuffer, size_t size, size_t count )
/// Write data to this stream and advance the stream position by the number of bytes written.
///
/// @param[in] pBuffer  Buffer from which data should be written.
/// @param[in] size     Size of each block of data to write.
/// @param[in] count    Number of blocks to write.
///
/// @return  Number of blocks successfully written.
///
/// @see Read(), Tell(), CanWrite()

/// @fn void Stream::Flush()
/// Flush any buffered data, such as data pending to be written to disk or a network socket.

/// @fn int64_t Stream::Seek( int64_t offset, ESeekOrigin origin )
/// Adjust the current stream offset.
///
/// @param[in] offset  Byte offset by which to adjust.
/// @param[in] origin  Origin from which to adjust the offset.
///
/// @return  New stream location, or -1 if this stream does not support seeking.
///
/// @see Tell(), CanSeek()

/// @fn int64_t Stream::Tell() const
/// Get the current stream location.
///
/// @return  Current stream location, or -1 if this stream does not support seeking.
///
/// @see Seek(), CanSeek()

/// @fn int64_t Stream::GetSize() const
/// Get the current size of this stream.
///
/// @return  Total stream size in bytes, or -1 if this stream does not support seeking or have a concept of size.
///
/// @see CanSeek()

/// @fn bool Stream::CanRead() const
/// Get whether this stream supports reading.
///
/// @return  True if this stream supports reading, false if not.
///
/// @see Read(), CanWrite()

/// @fn bool Stream::CanWrite() const
/// Get whether this stream supports writing.
///
/// @return  True if this stream supports writing, false if not.
///
/// @see Write(), CanRead()

/// @fn bool Stream::CanSeek() const
/// Get whether this stream supports seeking.
///
/// @return  True if this stream supports seeking, false if not.
///
/// @see Seek(), Tell(), GetSize()