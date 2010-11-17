//----------------------------------------------------------------------------------------------------------------------
// RConstantBuffer.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "RenderingPch.h"
#include "Rendering/RConstantBuffer.h"

namespace Lunar
{
    /// Destructor.
    RConstantBuffer::~RConstantBuffer()
    {
    }

    /// @fn void* RConstantBuffer::Map( ERendererBufferMapHint hint )
    /// Map the buffer contents to a CPU-accessible memory address.
    ///
    /// This allows for updating the contents of an existing buffer.  When the application is done writing to the
    /// buffer, it must call Unmap() to release the mapped address.
    ///
    /// Depending on the buffer type and the target platform, a mapped buffer is likely not to support reading by the
    /// CPU.  Doing so may cause a performance hit or even crashes.
    ///
    /// @param[in] hint  Hint to pass to the renderer as to how existing data will be treated.
    ///
    /// @return  Mapped address if this buffer was mapped successfully, null if not.
    ///
    /// @see Unmap()

    /// @fn void RVertexBuffer::Unmap()
    /// Unmap a buffer previously mapped to a CPU-accessible memory address using Map().
    ///
    /// @see Map()
}
