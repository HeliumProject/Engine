//----------------------------------------------------------------------------------------------------------------------
// DynamicMemoryStream.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Lunar
{
    /// Get a pointer to the dynamic array currently in use as this stream's memory buffer.
    ///
    /// @return  Dynamic array in use by this stream.
    DynArray< uint8_t >* DynamicMemoryStream::GetBuffer() const
    {
        return m_pBuffer;
    }
}
