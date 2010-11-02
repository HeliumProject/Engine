//----------------------------------------------------------------------------------------------------------------------
// Stream.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_CORE_STREAM_H
#define LUNAR_CORE_STREAM_H

#include "Core/Core.h"

namespace Lunar
{
    /// Byte stream interface.
    class LUNAR_CORE_API Stream : NonCopyable
    {
    public:
        /// Stream seek origin.
        enum ESeekOrigin
        {
            SEEK_ORIGIN_FIRST   =  0,
            SEEK_ORIGIN_INVALID = -1,

            SEEK_ORIGIN_CURRENT,  ///< Seek relative to the current location.
            SEEK_ORIGIN_BEGIN,    ///< Seek relative to the beginning of the stream.
            SEEK_ORIGIN_END,      ///< Seek relative to the end of the stream.

            SEEK_ORIGIN_MAX,
            SEEK_ORIGIN_LAST = SEEK_ORIGIN_MAX - 1
        };

        /// @name Construction/Destruction
        //@{
        virtual ~Stream() = 0;
        //@}

        /// @name Stream Interface
        //@{
        virtual void Close() = 0;
        virtual bool IsOpen() const = 0;

        virtual size_t Read( void* pBuffer, size_t size, size_t count ) = 0;
        virtual size_t Write( const void* pBuffer, size_t size, size_t count ) = 0;

        virtual void Flush() = 0;

        virtual int64_t Seek( int64_t offset, ESeekOrigin origin ) = 0;
        virtual int64_t Tell() const = 0;
        virtual int64_t GetSize() const = 0;
        //@}

        /// @name Stream Capabilities
        //@{
        virtual bool CanRead() const = 0;
        virtual bool CanWrite() const = 0;
        virtual bool CanSeek() const = 0;
        //@}
    };
}

#endif  // LUNAR_CORE_STREAM_H
