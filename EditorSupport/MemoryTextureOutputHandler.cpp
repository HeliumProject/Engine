//----------------------------------------------------------------------------------------------------------------------
// MemoryTextureOutputHandler.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "EditorSupportPch.h"

#if HELIUM_TOOLS

#include "EditorSupport/MemoryTextureOutputHandler.h"

using namespace Helium;

/// Constructor.
///
/// @param[in] width     Width of the largest mip level, in texels.
/// @param[in] height    Height of the largest mip level, in texels.
/// @param[in] bCubemap  True if the texture is a cube map, false if it is a 2D texture.
/// @param[in] bMipmaps  True if a full mip chain will be compressed for the texture, false if only the top-most mip
///                      level will be compressed.
MemoryTextureOutputHandler::MemoryTextureOutputHandler(
    uint32_t width,
    uint32_t height,
    bool bCubemap,
    bool bMipmaps )
    : m_pCurrentMipLevel( NULL )
{
    HELIUM_ASSERT( width != 0 );
    HELIUM_ASSERT( height != 0 );

    // Compute the number of mip levels that will be compressed.
    size_t mipCount = 1;
    if( bMipmaps )
    {
        // Note that we don't need to clamp the width or height to be at least 1 since we don't need the actual mip
        // level sizes (just the number of mip levels.
        while( width > 1 || height > 1 )
        {
            width /= 2;
            height /= 2;

            ++mipCount;
        }
    }

    // Reserve space for the mip level buffers for each face (one face for 2D textures, six for cube maps).
    m_faces[ 0 ].Reserve( mipCount );
    m_faces[ 0 ].Resize( mipCount );

    if( bCubemap )
    {
        for( size_t faceIndex = 1; faceIndex < HELIUM_ARRAY_COUNT( m_faces ); ++faceIndex )
        {
            m_faces[ faceIndex ].Reserve( mipCount );
            m_faces[ faceIndex ].Resize( mipCount );
        }
    }
}

/// Destructor.
MemoryTextureOutputHandler::~MemoryTextureOutputHandler()
{
}

/// Indicate the start of a new compressed image that's part of the final texture.
///
/// @param[in] size      Image size, in bytes.
/// @param[in] width     Image width, in texels.
/// @param[in] height    Image height, in texels.
/// @param[in] depth     Image depth (for 3D textures), in texels.
/// @param[in] face      Face index (for cube maps).
/// @param[in] miplevel  Mip level index.
///
/// @see writeData()
void MemoryTextureOutputHandler::beginImage(
    int size,
    int /*width*/,
    int /*height*/,
    int /*depth*/,
    int face,
    int miplevel )
{
    // Make sure the previous level was fully written.
    HELIUM_ASSERT( !m_pCurrentMipLevel || m_pCurrentMipLevel->GetSize() == m_pCurrentMipLevel->GetCapacity() );

    // Make sure the face and mip level indices are valid.
    HELIUM_ASSERT( static_cast< size_t >( face ) < HELIUM_ARRAY_COUNT( m_faces ) );
    MipLevelArray& rFace = m_faces[ face ];

    HELIUM_ASSERT( static_cast< size_t >( miplevel ) < rFace.GetSize() );
    MipDataArray& rMipData = rFace[ miplevel ];

    HELIUM_ASSERT( rMipData.IsEmpty() );

    // Allocate space for the compressed data and set the mip data buffer as our active buffer.
    rMipData.Clear();
    rMipData.Reserve( size );
    rMipData.Resize( 0 );

    m_pCurrentMipLevel = &rMipData;
}

/// Output data.  Compressed data is output as soon as it's generated to minimize memory allocations.
///
/// @param[in] pData  Data to write.
/// @param[in] size   Number of bytes to write.
///
/// @return  True if the data was written successfully, false if not.
///
/// @see beginImage()
bool MemoryTextureOutputHandler::writeData( const void* pData, int size )
{
    // Make sure a mip level is active.
    HELIUM_ASSERT( m_pCurrentMipLevel );
    if( !m_pCurrentMipLevel )
    {
        return false;
    }

    // Make sure the mip data buffer has allocated space remaining for the data to be written.
    size_t levelCapacity = m_pCurrentMipLevel->GetCapacity();
    size_t levelSize = m_pCurrentMipLevel->GetSize();
    size_t bytesRemaining = levelCapacity - levelSize;
    HELIUM_ASSERT( bytesRemaining >= static_cast< size_t >( size ) );
    if( bytesRemaining < static_cast< size_t >( size ) )
    {
        return false;
    }

    // Write the texture data.
    m_pCurrentMipLevel->AddArray( static_cast< const uint8_t* >( pData ), size );

    return true;
}

#endif  // HELIUM_TOOLS
