//----------------------------------------------------------------------------------------------------------------------
// Color.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_RENDERING_COLOR_H
#define LUNAR_RENDERING_COLOR_H

#include "Rendering/Rendering.h"
#include "Platform/Math/MathCommon.h"

namespace Lunar
{
    /// 32-bit ARGB color value.
    class LUNAR_RENDERING_API Color
    {
    public:
        /// @name Construction/Destruction
        //@{
        inline Color();
        inline Color( uint8_t r, uint8_t g, uint8_t b, uint8_t a );
        inline Color( float32_t r, float32_t g, float32_t b, float32_t a );
        inline explicit Color( uint32_t argb );
        //@}

        /// @name Data Access
        //@{
        inline uint8_t GetR() const;
        inline uint8_t GetG() const;
        inline uint8_t GetB() const;
        inline uint8_t GetA() const;
        inline uint32_t GetArgb() const;
        inline void SetR( uint8_t r );
        inline void SetG( uint8_t g );
        inline void SetB( uint8_t b );
        inline void SetA( uint8_t a );
        inline void SetArgb( uint32_t argb );

        inline float32_t GetFloatR() const;
        inline float32_t GetFloatG() const;
        inline float32_t GetFloatB() const;
        inline float32_t GetFloatA() const;
        inline void SetFloatR( float32_t r );
        inline void SetFloatG( float32_t g );
        inline void SetFloatB( float32_t b );
        inline void SetFloatA( float32_t a );
        //@}

        /// @name Friend Functions
        //@{
        friend Serializer& operator<<( Serializer& s, Color& rValue );
        //@}

    private:
        /// Color data.
        union
        {
            /// Packed color value.
            uint32_t packed;

            /// Individual color components.
            struct
            {
#if HELIUM_ENDIAN_LITTLE
                /// Blue component.
                uint8_t b;
                /// Green component.
                uint8_t g;
                /// Red component.
                uint8_t r;
                /// Alpha component.
                uint8_t a;
#else
                /// Alpha component.
                uint8_t a;
                /// Red component.
                uint8_t r;
                /// Green component.
                uint8_t g;
                /// Blue component.
                uint8_t b;
#endif
            } components;
        } m_color;
    };
}

#include "Rendering/Color.inl"

#endif  // LUNAR_RENDERING_COLOR_H
