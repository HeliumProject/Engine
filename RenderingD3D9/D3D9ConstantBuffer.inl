//----------------------------------------------------------------------------------------------------------------------
// D3D9ConstantBuffer.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Helium
{
    /// Get the constant buffer data.
    ///
    /// @return  Pointer to the constant buffer data.
    ///
    /// @see GetRegisterCount()
    const void* D3D9ConstantBuffer::GetData() const
    {
        return m_pData;
    }

    /// Get the map tag for this buffer.
    ///
    /// The map tag is incremented after each Unmap() call.  The value is used by the immediate command proxy to
    /// determine whether a constant buffer assigned to a constant buffer slot has been modified between consecutive
    /// draw calls.
    ///
    /// @return  Current map tag.
    uint32_t D3D9ConstantBuffer::GetTag() const
    {
        return m_tag;
    }

    /// Get the number of registers covered by this buffer.
    ///
    /// @return  Number of floating-point vector registers covered by this buffer.
    ///
    /// @see GetData()
    uint16_t D3D9ConstantBuffer::GetRegisterCount() const
    {
        return m_registerCount;
    }
}
