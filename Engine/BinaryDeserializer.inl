//----------------------------------------------------------------------------------------------------------------------
// BinaryDeserializer.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Lunar
{
    /// Get whether byte swapping is set to be performed during deserialization.
    ///
    /// @return  True if byte swapping is enabled, false if not.
    ///
    /// @see SetByteSwapping()
    bool BinaryDeserializer::GetByteSwapping() const
    {
        return ( m_pStream == &m_byteSwappingStream );
    }

    /// Get the current offset within the deserialization stream.
    ///
    /// @return  Deserialization stream offset.
    size_t BinaryDeserializer::GetCurrentOffset() const
    {
        return static_cast< size_t >( m_directStream.Tell() );
    }
}
