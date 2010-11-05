//----------------------------------------------------------------------------------------------------------------------
// ExternalMemoryStream.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_CORE_EXTERNAL_MEMORY_STREAM_H
#define LUNAR_CORE_EXTERNAL_MEMORY_STREAM_H

#include "Core/Stream.h"

namespace Lunar
{
    /// Stream for reading from and writing to a pre-allocated memory buffer.
    class LUNAR_CORE_API ExternalMemoryStream : public Stream
    {
    public:
        /// @name Construction/Destruction
        //@{
        ExternalMemoryStream();
        ExternalMemoryStream( void* pData, size_t size );
        virtual ~ExternalMemoryStream();
        //@}

        /// @name Stream Interface
        //@{
        void Open( void* pData, size_t size );

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

        /// @name Data Access
        //@{
        inline const void* GetData() const;
        //@}

    private:
        /// Memory buffer.
        uint8_t* m_pStart;
        /// Memory buffer end.
        uint8_t* m_pEnd;
        /// Current read/write location.
        uint8_t* m_pCurrent;
    };
}

#include "Core/ExternalMemoryStream.inl"

#endif  // LUNAR_CORE_EXTERNAL_MEMORY_STREAM_H
