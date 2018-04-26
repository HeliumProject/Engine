#pragma once

#include "EditorSupport/EditorSupport.h"

#if HELIUM_TOOLS

#include <nvtt/nvtt.h>

namespace Helium
{
    /// NVIDIA Texture Tools OutputHandler implementation for outputting texture data to a memory buffer.
    class MemoryTextureOutputHandler : public nvtt::OutputHandler
    {
    public:
        /// Buffer type for mip level data.
        typedef DynamicArray< uint8_t > MipDataArray;
        /// Buffer type for an entire set of mip levels.
        typedef DynamicArray< MipDataArray > MipLevelArray;

        /// @name Construction/Destruction
        //@{
        MemoryTextureOutputHandler( uint32_t width, uint32_t height, bool bCubemap, bool bMipmaps );
        virtual ~MemoryTextureOutputHandler();
        //@}

        /// @name Output Handler Interface
        //@{
        virtual void beginImage( int size, int width, int height, int depth, int face, int miplevel );
		virtual void endImage();
        virtual bool writeData( const void* pData, int size );
        //@}

        /// @name Data Access
        //@{
        inline const MipLevelArray& GetFace( size_t faceIndex ) const;
        //@}

    private:
        /// Texture data for each face.
        MipLevelArray m_faces[ 6 ];
        /// Mip data buffer to which data is currently being written.
        MipDataArray* m_pCurrentMipLevel;
    };
}

#include "EditorSupport/MemoryTextureOutputHandler.inl"

#endif  // HELIUM_TOOLS
