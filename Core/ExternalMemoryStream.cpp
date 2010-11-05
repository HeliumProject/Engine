//----------------------------------------------------------------------------------------------------------------------
// ExternalMemoryStream.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "CorePch.h"
#include "Core/ExternalMemoryStream.h"

namespace Lunar
{
    /// Constructor.
    ExternalMemoryStream::ExternalMemoryStream()
        : m_pStart( NULL )
        , m_pEnd( NULL )
        , m_pCurrent( NULL )
    {
    }

    /// Constructor.
    ///
    /// @param[in] pData  Start address of the memory buffer to use with this stream.
    /// @param[in] size   Size of the memory buffer to use with this stream (ignored if pData is null).
    ExternalMemoryStream::ExternalMemoryStream( void* pData, size_t size )
        : m_pStart( static_cast< uint8_t* >( pData ) )
        , m_pEnd( pData ? static_cast< uint8_t* >( pData ) + size : NULL )
        , m_pCurrent( static_cast< uint8_t* >( pData ) )
    {
    }

    /// Destructor.
    ExternalMemoryStream::~ExternalMemoryStream()
    {
    }

    /// Set the memory buffer to use with this stream.
    ///
    /// @param[in] pData  Start address of the memory buffer to use, or null to clear any currently set buffer settings.
    /// @param[in] size   Size of the memory buffer to use with this stream (ignored if pData is null).
    void ExternalMemoryStream::Open( void* pData, size_t size )
    {
        m_pStart = static_cast< uint8_t* >( pData );
        m_pEnd = ( pData ? static_cast< uint8_t* >( pData ) + size : NULL );
        m_pCurrent = static_cast< uint8_t* >( pData );
    }

    /// @copydoc Stream::Close()
    void ExternalMemoryStream::Close()
    {
        m_pStart = NULL;
        m_pEnd = NULL;
        m_pCurrent = NULL;
    }

    /// @copydoc Stream::IsOpen()
    bool ExternalMemoryStream::IsOpen() const
    {
        return ( m_pStart != NULL );
    }

    /// @copydoc Stream::Read()
    size_t ExternalMemoryStream::Read( void* pBuffer, size_t size, size_t count )
    {
        HELIUM_ASSERT( pBuffer );
        HELIUM_ASSERT( IsOpen() );

        size_t byteCount = size * count;

        size_t bytesRemaining = static_cast< size_t >( m_pEnd - m_pCurrent );
        if( byteCount > bytesRemaining )
        {
            byteCount = bytesRemaining;
        }

        MemoryCopy( pBuffer, m_pCurrent, byteCount );
        m_pCurrent += byteCount;

        return ( byteCount / size );
    }

    /// @copydoc Stream::Write()
    size_t ExternalMemoryStream::Write( const void* pBuffer, size_t size, size_t count )
    {
        HELIUM_ASSERT( pBuffer );
        HELIUM_ASSERT( IsOpen() );

        size_t byteCount = size * count;

        size_t bytesRemaining = static_cast< size_t >( m_pEnd - m_pCurrent );
        if( byteCount > bytesRemaining )
        {
            byteCount = bytesRemaining;
        }

        MemoryCopy( m_pCurrent, pBuffer, byteCount );
        m_pCurrent += byteCount;

        return ( byteCount / size );
    }

    /// @copydoc Stream::Flush()
    void ExternalMemoryStream::Flush()
    {
        HELIUM_ASSERT( IsOpen() );

        // Nothing needs to be done when flushing this type of stream.
    }

    /// @copydoc Stream::Seek()
    int64_t ExternalMemoryStream::Seek( int64_t offset, ESeekOrigin origin )
    {
        HELIUM_ASSERT( static_cast< size_t >( origin ) < static_cast< size_t >( SEEK_ORIGIN_MAX ) );
        HELIUM_ASSERT( IsOpen() );

        size_t referenceOffset;
        switch( origin )
        {
            case SEEK_ORIGIN_CURRENT:
            {
                referenceOffset = static_cast< size_t >( m_pCurrent - m_pStart );

                break;
            }

            case SEEK_ORIGIN_BEGIN:
            {
                referenceOffset = 0;

                break;
            }

            case SEEK_ORIGIN_END:
            {
                referenceOffset = static_cast< size_t >( m_pEnd - m_pStart );

                break;
            }

            default:
            {
                HELIUM_TRACE( TRACE_ERROR, TXT( "DynamicMemoryStream::Seek(): Invalid seek origin specified.\n" ) );

                return static_cast< int64_t >( static_cast< size_t >( m_pCurrent - m_pStart ) );
            }
        }

        if( offset < 0 )
        {
            uint64_t absOffset = static_cast< uint64_t >( -offset );
            HELIUM_ASSERT( absOffset <= static_cast< uint64_t >( referenceOffset ) );
            if( absOffset > static_cast< uint64_t >( referenceOffset ) )
            {
                HELIUM_TRACE(
                    TRACE_ERROR,
                    TXT( "DynamicMemoryStream::Seek(): Attempted to seek before the start of the memory stream.\n" ) );
            }
            else
            {
                m_pCurrent = m_pStart + referenceOffset - static_cast< size_t >( absOffset );
            }
        }
        else
        {
            uint64_t absOffset = static_cast< uint64_t >( offset );
            HELIUM_ASSERT( absOffset <= static_cast< uint64_t >( static_cast< size_t >( -1 ) - referenceOffset ) );
            if( absOffset > static_cast< uint64_t >( static_cast< size_t >( m_pEnd - m_pStart ) - referenceOffset ) )
            {
                HELIUM_TRACE(
                    TRACE_ERROR,
                    TXT( "DynamicMemoryStream::Seek(): Attempted to seek past the end of the memory stream.\n" ) );
            }
            else
            {
                m_pCurrent = m_pStart + referenceOffset + static_cast< size_t >( absOffset );
            }
        }

        return static_cast< int64_t >( static_cast< size_t >( m_pCurrent - m_pStart ) );
    }

    /// @see Stream::Tell()
    int64_t ExternalMemoryStream::Tell() const
    {
        return static_cast< int64_t >( static_cast< size_t >( m_pCurrent - m_pStart ) );
    }

    /// @see Stream::GetSize()
    int64_t ExternalMemoryStream::GetSize() const
    {
        return static_cast< int64_t >( static_cast< size_t >( m_pEnd - m_pStart ) );
    }

    /// @see Stream::CanRead()
    bool ExternalMemoryStream::CanRead() const
    {
        return true;
    }

    /// @see Stream::CanWrite()
    bool ExternalMemoryStream::CanWrite() const
    {
        return true;
    }

    /// @see Stream::CanSeek()
    bool ExternalMemoryStream::CanSeek() const
    {
        return true;
    }
}
