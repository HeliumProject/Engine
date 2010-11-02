//----------------------------------------------------------------------------------------------------------------------
// ExternalMemoryStream.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Lunar
{
    /// Get a pointer to the memory stream data.
    ///
    /// @return  Memory stream data.
    const void* ExternalMemoryStream::GetData() const
    {
        return m_pStart;
    }
}
