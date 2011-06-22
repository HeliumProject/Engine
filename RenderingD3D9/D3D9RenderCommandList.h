//----------------------------------------------------------------------------------------------------------------------
// D3D9RenderCommandList.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_RENDERING_D3D9_D3D9_RENDER_COMMAND_LIST_H
#define LUNAR_RENDERING_D3D9_D3D9_RENDER_COMMAND_LIST_H

#include "RenderingD3D9/RenderingD3D9.h"
#include "Rendering/RRenderCommandList.h"

namespace Helium
{
    /// Direct3D 9 render command.
    class D3D9RenderCommand
    {
    public:
        /// @name Construction/Destruction
        //@{
        virtual ~D3D9RenderCommand() = 0;
        //@}

        /// @name Command Execution
        //@{
        virtual void Execute( D3D9ImmediateCommandProxy* pCommandProxy ) = 0;
        //@}
    };

    /// Direct3D 9 render command list.
    class D3D9RenderCommandList : public RRenderCommandList
    {
    public:
        /// Command iterator.
        class Iterator
        {
        public:
            /// @name Construction/Destruction
            //@{
            inline Iterator();
            inline Iterator( uint8_t* pCurrent );
            //@}

            /// @name Overloaded Operators
            //@{
            inline Iterator& operator++();
            inline D3D9RenderCommand& operator*();
            inline D3D9RenderCommand* operator->();
            inline bool operator==( const Iterator& rIterator );
            inline bool operator!=( const Iterator& rIterator );
            //@}

        private:
            /// Current command pointer.
            uint8_t* m_pCurrent;
        };

        /// Default command list size, in bytes.
        static const size_t DEFAULT_SIZE = 32 * 1024;

        /// @name Construction/Destruction
        //@{
        D3D9RenderCommandList( size_t size = DEFAULT_SIZE );
        //@}

        /// @name Command Allocation
        //@{
        template< typename T > T* NewCommand();
        template< typename T, typename P0 > T* NewCommand( const P0& rParam0 );
        template< typename T, typename P0, typename P1 > T* NewCommand( const P0& rParam0, const P1& rParam1 );
        template< typename T, typename P0, typename P1, typename P2 > T* NewCommand(
            const P0& rParam0, const P1& rParam1, const P2& rParam2 );
        template< typename T, typename P0, typename P1, typename P2, typename P3 > T* NewCommand(
            const P0& rParam0, const P1& rParam1, const P2& rParam2, const P3& rParam3 );
        template< typename T, typename P0, typename P1, typename P2, typename P3, typename P4 > T* NewCommand(
            const P0& rParam0, const P1& rParam1, const P2& rParam2, const P3& rParam3, const P4& rParam4 );
        template< typename T, typename P0, typename P1, typename P2, typename P3, typename P4, typename P5 >
            T* NewCommand(
                const P0& rParam0, const P1& rParam1, const P2& rParam2, const P3& rParam3, const P4& rParam4,
                const P5& rParam5 );
        //@}

        /// @name Command Iteration
        //@{
        inline Iterator Begin();
        inline Iterator End();
        //@}

    private:
        /// Command buffer.
        uint8_t* m_pBuffer;
        /// Total command buffer size.
        size_t m_size;
        /// Current command buffer write offset.
        size_t m_writeOffset;

        /// @name Construction/Destruction
        //@{
        ~D3D9RenderCommandList();
        //@}

        /// @name Private Utility Functions
        //@{
        template< typename T > void* AllocateCommandSpace();
        //@}
    };
}

#include "RenderingD3D9/D3D9RenderCommandList.inl"

#endif  // LUNAR_RENDERING_D3D9_D3D9_RENDER_COMMAND_LIST_H
