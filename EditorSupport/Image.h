//----------------------------------------------------------------------------------------------------------------------
// Image.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_EDITOR_SUPPORT_IMAGE_H
#define HELIUM_EDITOR_SUPPORT_IMAGE_H

#include "EditorSupport/EditorSupport.h"

#if L_EDITOR

namespace Helium
{
    class Color;

    /// Base interface for loading and parsing image data loaded from disk (primarily for texture processing).
    class HELIUM_EDITOR_SUPPORT_API Image
    {
    public:
        /// Color channel identifiers.
        enum EChannel
        {
            CHANNEL_FIRST   =  0,
            CHANNEL_INVALID = -1,

            /// Red channel.
            CHANNEL_RED,
            /// Green channel.
            CHANNEL_GREEN,
            /// Blue channel.
            CHANNEL_BLUE,
            /// Alpha channel.
            CHANNEL_ALPHA,

            CHANNEL_MAX,
            CHANNEL_LAST = CHANNEL_MAX - 1
        };

        /// Image color format.
        class HELIUM_EDITOR_SUPPORT_API Format
        {
        public:
            /// @name Construction/Destruction
            //@{
            inline Format();
            //@}

            /// @name Data Access
            //@{
            inline void SetBytesPerPixel( uint8_t bytesPerPixel );
            inline uint8_t GetBytesPerPixel() const;

            inline void SetChannelBitCount( EChannel channel, uint8_t bitCount );
            inline uint8_t GetChannelBitCount( EChannel channel ) const;
            inline const uint8_t* GetChannelBitCounts() const;

            inline void SetChannelBitOffset( EChannel channel, uint8_t bitOffset );
            inline uint8_t GetChannelBitOffset( EChannel channel ) const;
            inline const uint8_t* GetChannelBitOffsets() const;

            inline void SetPalette( const Color* pPalette, uint32_t paletteSize );
            inline const Color* GetPalette() const;
            inline uint32_t GetPaletteSize() const;
            //@}

            /// @name Format Validation
            //@{
            bool Validate() const;
            //@}

            /// @name Static Utility Functions
            //@{
            inline static uint32_t ComputeChannelMask( uint8_t bitCount, uint8_t bitOffset );
            //@}

        private:
            /// Color palette.
            const Color* m_pPalette;
            /// Number of colors in the color palette.
            uint32_t m_paletteSize;

            /// Number of bits for each channel within a given pixel.
            uint8_t m_channelBitCounts[ CHANNEL_MAX ];
            /// Bit offset of each channel within a given pixel.
            uint8_t m_channelBitOffsets[ CHANNEL_MAX ];

            /// Bytes per pixel.
            uint8_t m_bytesPerPixel;
        };

        /// Image initialization data.
        struct HELIUM_EDITOR_SUPPORT_API InitParameters
        {
            /// Color format.
            Format format;

            /// Initial pixel data.  If this is null, the image will be allocated, but the pixel values will be left in
            /// an uninitialized state.
            const void* pPixelData;

            /// Image width.
            uint32_t width;
            /// Image height.
            uint32_t height;
            /// Byte pitch per row of pixels.  If this is zero, the pitch will be computed automatically based on the
            /// width and bytes per pixel so as to pack each row without any padding.
            uint32_t pitch;

            /// @name Construction/Destruction
            //@{
            inline InitParameters();
            //@}
        };

        /// @name Construction/Destruction
        //@{
        Image();
        Image( const Image& rSource );
        ~Image();
        //@}

        /// @name Image Setup
        //@{
        bool Initialize( const InitParameters& rParameters );
        void Unload();

        void Swap( Image& rImage );
        //@}

        /// @name Image Information
        //@{
        inline void* GetPixelData();
        inline const void* GetPixelData() const;

        inline uint32_t GetWidth() const;
        inline uint32_t GetHeight() const;
        inline uint32_t GetPitch() const;

        inline const Format& GetFormat() const;
        //@}

        /// @name Image Conversion
        //@{
        bool Convert( Image& rDestination, const Format& rFormat ) const;
        //@}

        /// @name Overloaded Operators
        //@{
        Image& operator=( const Image& rSource );
        //@}

    protected:
        /// Pixel data.
        void* m_pPixelData;

        /// Image width, in pixels.
        uint32_t m_width;
        /// Image height, in pixels.
        uint32_t m_height;
        /// Bytes per row of pixels.
        uint32_t m_pitch;

        /// Image format.
        Format m_format;

        /// @name Private Utility Functions
        //@{
        void PrivateCopy( const Image& rSource );
        void PrivateFree();
        //@}
    };
}

#include "EditorSupport/Image.inl"

#endif  // L_EDITOR

#endif  // HELIUM_EDITOR_SUPPORT_IMAGE_H
