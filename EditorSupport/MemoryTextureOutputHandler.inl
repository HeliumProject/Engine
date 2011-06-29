//----------------------------------------------------------------------------------------------------------------------
// MemoryTextureOutputHandler.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Helium
{
    /// Get the array of mip levels for the specified face.
    ///
    /// @param[in] faceIndex  Texture face index.  For 2D textures, this should always be zero.  For cube maps, this
    ///                       should less than 6.
    const MemoryTextureOutputHandler::MipLevelArray& MemoryTextureOutputHandler::GetFace( size_t faceIndex ) const
    {
        HELIUM_ASSERT( faceIndex < HELIUM_ARRAY_COUNT( m_faces ) );

        return m_faces[ faceIndex ];
    }
}
