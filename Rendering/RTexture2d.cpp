//----------------------------------------------------------------------------------------------------------------------
// RTexture2d.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "RenderingPch.h"
#include "Rendering/RTexture2d.h"

using namespace Helium;

/// Destructor.
RTexture2d::~RTexture2d()
{
}

/// @copydoc RTexture::GetType()
RTexture::EType RTexture2d::GetType() const
{
    return TYPE_2D;
}

/// @fn void* RTexture2d::Map( uint32_t mipLevel, size_t& rPitch, ERendererBufferMapHint hint )
/// Map a mip level of this texture to a user-writable memory address.
///
/// @param[in]  mipLevel  Mip level to map, or an invalid index value to map the entire texture.  Note that the
///                       texture must support whole-resource mapping (as determined by CanMapWholeResource()) if an
///                       invalid index is specified.
/// @param[out] rPitch    Number of bytes per row of data in the locked texture.  For uncompressed texture formats,
///                       this is the number of bytes per row of pixels.  For block-compressed texture formats (i.e.
///                       DXT formats), this is the number of bytes per row of blocks.
/// @param[in]  hint      Hint for the renderer as to how the mapped data will be used.
///
/// @return  Pointer to the mapped texture data, or null if mapping failed.
///
/// @see Unmap(), CanMapWholeResource()

/// @fn void RTexture2d::Unmap( uint32_t mipLevel )
/// Unmap previously mapped texture data.
///
/// @param[in] mipLevel  Mip level to unmap, or an invalid index value to unmap the entire texture (if the entire
///                      texture was previously mapped by passing an invalid index to Map()).  Note that the same
///                      index must be provided as that provided in the corresponding call to Map(); an invalid
///                      index cannot be used to unmap mip levels mapped one-at-a-time through individual calls to
///                      Map(), and specific mip level indices cannot be specified if the entire texture resource
///                      was mapped by calling Map() with an invalid index.
///
/// @see Map(), CanMapWholeResource()

/// @fn bool RTexture2d::CanMapWholeResource() const
/// Get whether all mip levels can be mapped at once in a single, contiguous region of memory.
///
/// @return  True if this entire texture can be mapped as a single region of memory, false if not.
///
/// @see Map(), Unmap()

/// @fn uint32_t RTexture2d::GetWidth( uint32_t mipLevel ) const
/// Get the width of the specified mip level of this texture.
///
/// @param[in] mipLevel  Mip level index.
///
/// @return  Width of the specified mip level in texels, or an invalid index if the mip level specified is outside
///          the range of mips provided by this texture.
///
/// @see GetHeight(), GetPixelFormat()

/// @fn uint32_t RTexture2d::GetHeight( uint32_t mipLevel ) const
/// Get the height of the specified mip level of this texture.
///
/// @param[in] mipLevel  Mip level index.
///
/// @return  Height of the specified mip level in texels, or an invalid index if the mip level specified is outside
///          the range of mips provided by this texture.
///
/// @see GetWidth(), GetPixelFormat()

/// @fn ERendererPixelFormat RTexture2d::GetPixelFormat() const
/// Get the pixel format of this texture.
///
/// @return  Texture pixel format.
///
/// @see GetWidth(), GetHeight()

/// @fn RSurface* RTexture2d::GetSurface( uint32_t mipLevel )
/// Get the surface for the specified texture mip level.
///
/// @param[in] mipLevel  Mip level index.
///
/// @return  Surface for the specified mip level.
