#pragma once

#include "Foundation/Stream.h"

namespace Helium
{
    /// Binary stream that buffers read and write operations for an underlying stream.
    ///
    /// @note  A BufferedStream should never wrap a ByteSwappingStream.  Doing so will invalidate proper byte swapping,
    ///        as the information needed to properly perform byte swapping is not retained by the BufferedStream.  A
    ///        ByteSwappingStream can be safely used to wrap access to a BufferedStream, though.
    class HELIUM_FOUNDATION_API BufferedStream : public Stream
    {
    public:
        /// Default buffer size, in bytes.
        static const size_t DEFAULT_BUFFER_SIZE = 4096;

        /// @name Construction/Destruction
        //@{
        explicit BufferedStream( Stream* pStream = NULL, size_t bufferSize = DEFAULT_BUFFER_SIZE );
        virtual ~BufferedStream();
        //@}

        /// @name Stream Assignment
        //@{
        virtual void Open( Stream* pStream, size_t bufferSize = DEFAULT_BUFFER_SIZE );
        //@}

        /// @name Stream Interface
        //@{
        virtual void Close();
        virtual bool IsOpen() const;

        virtual size_t Read( void* pBuffer, size_t size, size_t count );
        virtual size_t Write( const void* pBuffer, size_t size, size_t count );

        virtual void Flush();

        virtual int64_t Seek( int64_t offset, SeekOrigin origin );
        virtual int64_t Tell() const;
        virtual int64_t GetSize() const;
        //@}

        /// @name Stream Capabilities
        //@{
        virtual bool CanRead() const;
        virtual bool CanWrite() const;
        virtual bool CanSeek() const;
        //@}

    private:
        /// Underlying stream.
        Stream* m_pStream;

        /// Stream buffer.
        void* m_pBuffer;
        /// Stream buffer size.
        size_t m_bufferSize;

        /// Stream offset of the beginning of the buffer (-1 if seeking is not supported).
        int64_t m_bufferStart;
        /// Number of bytes currently in the buffer.
        size_t m_bufferedByteCount;
        /// Current offset within the buffer.
        size_t m_bufferOffset;

        /// True if the buffer current contains read data, false if it contains written data.
        bool m_bReadData;
    };
}
