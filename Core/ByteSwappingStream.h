//----------------------------------------------------------------------------------------------------------------------
// ByteSwappingStream.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_CORE_BYTE_SWAPPING_STREAM_H
#define LUNAR_CORE_BYTE_SWAPPING_STREAM_H

#include "Core/Stream.h"

namespace Lunar
{
    /// Stream wrapper that swaps the byte order of chunks of data read from or written to the stream.
    class LUNAR_CORE_API ByteSwappingStream : public Stream
    {
    public:
        /// @name Construction/Destruction
        //@{
        explicit ByteSwappingStream( Stream* pStream = NULL );
        virtual ~ByteSwappingStream();
        //@}

        /// @name Stream Assignment
        //@{
        virtual void Open( Stream* pStream );
        //@}

        /// @name Stream Interface
        //@{
        virtual void Close();
        virtual bool IsOpen() const;

        virtual size_t Read( void* pBuffer, size_t size, size_t count );
        virtual size_t Write( const void* pBuffer, size_t size, size_t count );

        virtual void Flush();

        virtual int64_t Seek( int64_t offset, ESeekOrigin origin );
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
        /// Underlying stream on which to perform byte swapping.
        Stream* m_pStream;
    };
}

#endif  // LUNAR_CORE_BYTE_SWAPPING_STREAM_H
