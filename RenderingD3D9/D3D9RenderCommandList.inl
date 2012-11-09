//----------------------------------------------------------------------------------------------------------------------
// D3D9RenderCommandList.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Helium
{
    /// Allocate a new command with no parameters.
    ///
    /// @return  New command.
    template< typename T >
    T* D3D9RenderCommandList::NewCommand()
    {
        void* pAddress = AllocateCommandSpace< T >();
        HELIUM_ASSERT( pAddress );

        return new( pAddress ) T;
    }

    /// Allocate a new command with one parameter.
    ///
    /// @param[in] rParam0  Command parameter.
    ///
    /// @return  New command.
    template< typename T, typename P0 >
    T* D3D9RenderCommandList::NewCommand( const P0& rParam0 )
    {
        void* pAddress = AllocateCommandSpace< T >();
        HELIUM_ASSERT( pAddress );

        return new( pAddress ) T( rParam0 );
    }

    /// Allocate a new command with two parameters.
    ///
    /// @param[in] rParam0  Command parameter.
    /// @param[in] rParam1  Command parameter.
    ///
    /// @return  New command.
    template< typename T, typename P0, typename P1 >
    T* D3D9RenderCommandList::NewCommand( const P0& rParam0, const P1& rParam1 )
    {
        void* pAddress = AllocateCommandSpace< T >();
        HELIUM_ASSERT( pAddress );

        return new( pAddress ) T( rParam0, rParam1 );
    }

    /// Allocate a new command with three parameters.
    ///
    /// @param[in] rParam0  Command parameter.
    /// @param[in] rParam1  Command parameter.
    /// @param[in] rParam2  Command parameter.
    ///
    /// @return  New command.
    template< typename T, typename P0, typename P1, typename P2 >
    T* D3D9RenderCommandList::NewCommand( const P0& rParam0, const P1& rParam1, const P2& rParam2 )
    {
        void* pAddress = AllocateCommandSpace< T >();
        HELIUM_ASSERT( pAddress );

        return new( pAddress ) T( rParam0, rParam1, rParam2 );
    }

    /// Allocate a new command with four parameters.
    ///
    /// @param[in] rParam0  Command parameter.
    /// @param[in] rParam1  Command parameter.
    /// @param[in] rParam2  Command parameter.
    /// @param[in] rParam3  Command parameter.
    ///
    /// @return  New command.
    template< typename T, typename P0, typename P1, typename P2, typename P3 >
    T* D3D9RenderCommandList::NewCommand( const P0& rParam0, const P1& rParam1, const P2& rParam2, const P3& rParam3 )
    {
        void* pAddress = AllocateCommandSpace< T >();
        HELIUM_ASSERT( pAddress );

        return new( pAddress ) T( rParam0, rParam1, rParam2, rParam3 );
    }

    /// Allocate a new command with five parameters.
    ///
    /// @param[in] rParam0  Command parameter.
    /// @param[in] rParam1  Command parameter.
    /// @param[in] rParam2  Command parameter.
    /// @param[in] rParam3  Command parameter.
    /// @param[in] rParam4  Command parameter.
    ///
    /// @return  New command.
    template< typename T, typename P0, typename P1, typename P2, typename P3, typename P4 >
    T* D3D9RenderCommandList::NewCommand(
        const P0& rParam0,
        const P1& rParam1,
        const P2& rParam2,
        const P3& rParam3,
        const P4& rParam4 )
    {
        void* pAddress = AllocateCommandSpace< T >();
        HELIUM_ASSERT( pAddress );

        return new( pAddress ) T( rParam0, rParam1, rParam2, rParam3, rParam4 );
    }

    /// Allocate a new command with six parameters.
    ///
    /// @param[in] rParam0  Command parameter.
    /// @param[in] rParam1  Command parameter.
    /// @param[in] rParam2  Command parameter.
    /// @param[in] rParam3  Command parameter.
    /// @param[in] rParam4  Command parameter.
    /// @param[in] rParam5  Command parameter.
    ///
    /// @return  New command.
    template< typename T, typename P0, typename P1, typename P2, typename P3, typename P4, typename P5 >
    T* D3D9RenderCommandList::NewCommand(
        const P0& rParam0,
        const P1& rParam1,
        const P2& rParam2,
        const P3& rParam3,
        const P4& rParam4,
        const P5& rParam5 )
    {
        void* pAddress = AllocateCommandSpace< T >();
        HELIUM_ASSERT( pAddress );

        return new( pAddress ) T( rParam0, rParam1, rParam2, rParam3, rParam4, rParam5 );
    }

    /// Allocate space in this command buffer for a command of the template type.
    ///
    /// @return  Allocated address if allocated successfully, null if there is not enough space in this command buffer.
    template< typename T >
    void* D3D9RenderCommandList::AllocateCommandSpace()
    {
        // Make sure we have enough space remaining in the command buffer and the command size.  Note that we are
        // explicitly using 64-bit sizes for commands to ensure platform consistency and 8-byte padding.
        size_t alignedSize = Align( sizeof( T ), sizeof( uint64_t ) );
        size_t bytesRemaining = m_size - m_writeOffset;
        if( bytesRemaining < alignedSize + sizeof( size_t ) )
        {
            HELIUM_TRACE(
                TraceLevels::Error,
                ( TXT( "D3D9RenderCommandList: Failed to allocate a command of size %" ) TPRIuSZ TXT( " (%" ) TPRIuSZ
                  TXT( " bytes needed; %" ) TPRIuSZ TXT( " bytes remaining in buffer).\n" ) ),
                alignedSize,
                alignedSize + sizeof( size_t ),
                bytesRemaining );

            // Move the write offset to the end of the buffer to prevent out-of-order writing of commands.
            m_writeOffset = m_size;

            return NULL;
        }

        // Write the command size to the buffer first.
        *reinterpret_cast< uint64_t* >( m_pBuffer + m_writeOffset ) = static_cast< uint64_t >( alignedSize );
        m_writeOffset += sizeof( uint64_t );

        // Get the command address and update the write offset to the end of the command.
        void* pAllocation = m_pBuffer + m_writeOffset;
        m_writeOffset += alignedSize;

        return pAllocation;
    }

    /// Get an iterator referencing the beginning of this command list.
    ///
    /// @return  Iterator at the beginning of this command list.
    ///
    /// @see End()
    D3D9RenderCommandList::Iterator D3D9RenderCommandList::Begin()
    {
        return Iterator( m_pBuffer );
    }

    /// Get an iterator referencing the end of this command list.
    ///
    /// @return  Iterator at the end of this command list.
    ///
    /// @see Begin()
    D3D9RenderCommandList::Iterator D3D9RenderCommandList::End()
    {
        return Iterator( m_pBuffer + m_writeOffset );
    }

    /// Constructor.
    ///
    /// This creates an iterator in an uninitialized state.  It must be initialized separately or through one of the
    /// other constructor overloads before use.
    D3D9RenderCommandList::Iterator::Iterator()
    {
    }

    /// Constructor.
    ///
    /// @param[in] pCurrent  Current iterator position.
    D3D9RenderCommandList::Iterator::Iterator( uint8_t* pCurrent )
        : m_pCurrent( pCurrent )
    {
    }

    /// Increment this iterator to the next render command.
    ///
    /// @return  Reference to this iterator.
    D3D9RenderCommandList::Iterator& D3D9RenderCommandList::Iterator::operator++()
    {
        size_t size = *reinterpret_cast< uint32_t* >( m_pCurrent );
        m_pCurrent += sizeof( uint32_t ) + size;

        return *this;
    }

    /// Get the render command referenced by this iterator.
    ///
    /// @return  Reference to the current render command.
    D3D9RenderCommand& D3D9RenderCommandList::Iterator::operator*()
    {
        return *reinterpret_cast< D3D9RenderCommand* >( m_pCurrent + sizeof( uint32_t ) );
    }

    /// Get the render command referenced by this iterator.
    ///
    /// @return  Pointer to the current render command.
    D3D9RenderCommand* D3D9RenderCommandList::Iterator::operator->()
    {
        return reinterpret_cast< D3D9RenderCommand* >( m_pCurrent + sizeof( uint32_t ) );
    }

    /// Check whether this iterator references the same render command as the given iterator.
    ///
    /// @param[in] rIterator  Iterator with which to compare.
    ///
    /// @return  True if this iterator matches the given iterator, false if not.
    bool D3D9RenderCommandList::Iterator::operator==( const Iterator& rIterator )
    {
        return ( m_pCurrent == rIterator.m_pCurrent );
    }

    /// Check whether this iterator does not reference the same render command as the given iterator.
    ///
    /// @param[in] rIterator  Iterator with which to compare.
    ///
    /// @return  True if this iterator does not match the given iterator, false if they do match.
    bool D3D9RenderCommandList::Iterator::operator!=( const Iterator& rIterator )
    {
        return ( m_pCurrent != rIterator.m_pCurrent );
    }
}
