//----------------------------------------------------------------------------------------------------------------------
// D3D9RenderCommandList.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "RenderingD3D9Pch.h"
#include "RenderingD3D9/D3D9RenderCommandList.h"

using namespace Helium;

/// Destructor.
D3D9RenderCommand::~D3D9RenderCommand()
{
}

/// @fn void D3D9RenderCommand::Execute( RRenderCommandProxy* pCommandProxy )
/// Execute this render command through the given command proxy.
///
/// @param[in] pCommandProxy  Command proxy through which to execute the command.

/// Constructor.
///
/// Creates a render command list with the given size.  Note that the size of a command list is fixed after
/// creation.
///
/// @param[in] size  Command list buffer size, in bytes.
D3D9RenderCommandList::D3D9RenderCommandList( size_t size )
{
    if( size == 0 )
    {
        size = 1;
    }

    m_pBuffer = new uint8_t [ size ];
    HELIUM_ASSERT( m_pBuffer );

    m_size = size;
    m_writeOffset = 0;
}

/// Destructor.
D3D9RenderCommandList::~D3D9RenderCommandList()
{
    Iterator listEnd = End();
    for( Iterator listIter = Begin(); listIter != listEnd; ++listIter )
    {
        listIter->~D3D9RenderCommand();
    }

    delete [] m_pBuffer;
}
