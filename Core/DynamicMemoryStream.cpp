//----------------------------------------------------------------------------------------------------------------------
// DynamicMemoryStream.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "CorePch.h"
#include "Core/DynamicMemoryStream.h"

namespace Lunar
{
    /// Constructor.
    ///
    /// @param[in] pBuffer  Dynamic array to use as this stream's memory buffer.
    DynamicMemoryStream::DynamicMemoryStream( DynArray< uint8_t >* pBuffer )
        : m_pBuffer( pBuffer )
        , m_offset( 0 )
    {
    }

    /// Destructor.
    DynamicMemoryStream::~DynamicMemoryStream()
    {
    }

    /// Set the buffer to use with this stream.
    ///
    /// Any currently assigned buffer will be unset, and the current stream offset will be reset to zero.
    ///
    /// @param[in] pBuffer  Dynamic array of bytes to use as this stream's memory buffer.
    void DynamicMemoryStream::Open( DynArray< uint8_t >* pBuffer )
    {
        m_pBuffer = pBuffer;
        m_offset = 0;
    }

    /// @copydoc Stream::Close()
    void DynamicMemoryStream::Close()
    {
        m_pBuffer = NULL;
        m_offset = 0;
    }

    /// @copydoc Stream::IsOpen()
    bool DynamicMemoryStream::IsOpen() const
    {
        return ( m_pBuffer != NULL );
    }

    /// @copydoc Stream::Read()
    size_t DynamicMemoryStream::Read( void* pBuffer, size_t size, size_t count )
    {
        L_ASSERT( m_pBuffer );
        if( !m_pBuffer )
        {
            return 0;
        }

        size_t bytesRemaining = m_pBuffer->GetSize() - m_offset;
        size_t byteCount = Min( size * count, bytesRemaining );

        MemoryCopy( pBuffer, m_pBuffer->GetData() + m_offset, byteCount );
        m_offset += byteCount;

        return ( byteCount / size );
    }

    /// @copydoc Stream::Write()
    size_t DynamicMemoryStream::Write( const void* pBuffer, size_t size, size_t count )
    {
        L_ASSERT( m_pBuffer );
        if( !m_pBuffer )
        {
            return 0;
        }

        size_t byteCount = size * count;

        size_t bytesRemaining = m_pBuffer->GetSize() - m_offset;
        size_t writeCount = Min( byteCount, bytesRemaining );

        MemoryCopy( m_pBuffer->GetData() + m_offset, pBuffer, writeCount );

        size_t remainingCount = byteCount - writeCount;
        if( remainingCount != 0 )
        {
            m_pBuffer->AddArray( static_cast< const uint8_t* >( pBuffer ) + writeCount, remainingCount );
        }

        m_offset += byteCount;

        return ( byteCount / size );
    }

    /// @copydoc Stream::Flush()
    void DynamicMemoryStream::Flush()
    {
        // Nothing needs to be done for this class.
    }

    /// @copydoc Stream::Seek()
    int64_t DynamicMemoryStream::Seek( int64_t offset, ESeekOrigin origin )
    {
        L_ASSERT( m_pBuffer );
        if( !m_pBuffer )
        {
            return -1;
        }

        L_ASSERT( static_cast< size_t >( origin ) < static_cast< size_t >( SEEK_ORIGIN_MAX ) );

        size_t referenceOffset;
        switch( origin )
        {
            case SEEK_ORIGIN_CURRENT:
            {
                referenceOffset = m_offset;

                break;
            }

            case SEEK_ORIGIN_BEGIN:
            {
                referenceOffset = 0;

                break;
            }

            case SEEK_ORIGIN_END:
            {
                referenceOffset = m_pBuffer->GetSize();

                break;
            }

            default:
            {
                L_LOG( LOG_ERROR, L_T( "DynamicMemoryStream::Seek(): Invalid seek origin specified.\n" ) );

                return static_cast< int64_t >( m_offset );
            }
        }

        if( offset < 0 )
        {
            uint64_t absOffset = static_cast< uint64_t >( -offset );
            L_ASSERT( absOffset <= static_cast< uint64_t >( referenceOffset ) );
            if( absOffset > static_cast< uint64_t >( referenceOffset ) )
            {
                L_LOG(
                    LOG_ERROR,
                    L_T( "DynamicMemoryStream::Seek(): Attempted to seek before the start of the memory stream.\n" ) );
            }
            else
            {
                m_offset = referenceOffset - static_cast< size_t >( absOffset );
            }
        }
        else
        {
            uint64_t absOffset = static_cast< uint64_t >( offset );
            L_ASSERT( absOffset <= static_cast< uint64_t >( static_cast< size_t >( -1 ) - referenceOffset ) );
            if( absOffset >= static_cast< uint64_t >( static_cast< size_t >( -1 ) - referenceOffset ) )
            {
                L_LOG(
                    LOG_ERROR,
                    ( L_T( "DynamicMemoryStream::Seek(): Attempted to seek outside the maximum buffer size supported " )
                      L_T( "by the current platform.\n" ) ) );
            }
            else
            {
                m_offset = referenceOffset + static_cast< size_t >( absOffset );

                if( m_offset > m_pBuffer->GetSize() )
                {
                    size_t additionalBytes = m_offset - m_pBuffer->GetSize();
                    m_pBuffer->Add( 0, additionalBytes );
                }
            }
        }

        return static_cast< int64_t >( m_offset );
    }

    /// @copydoc Stream::Tell()
    int64_t DynamicMemoryStream::Tell() const
    {
        return ( m_pBuffer ? static_cast< int64_t >( m_offset ) : -1 );
    }

    /// @copydoc Stream::GetSize()
    int64_t DynamicMemoryStream::GetSize() const
    {
        return ( m_pBuffer ? static_cast< int64_t >( m_pBuffer->GetSize() ) : 0 );
    }

    /// @copydoc Stream::CanRead()
    bool DynamicMemoryStream::CanRead() const
    {
        return ( m_pBuffer != NULL );
    }

    /// @copydoc Stream::CanWrite()
    bool DynamicMemoryStream::CanWrite() const
    {
        return ( m_pBuffer != NULL );
    }

    /// @copydoc Stream::CanSeek()
    bool DynamicMemoryStream::CanSeek() const
    {
        return ( m_pBuffer != NULL );
    }
}
