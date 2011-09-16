//----------------------------------------------------------------------------------------------------------------------
// Color.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_RENDERING_COLOR_H
#define HELIUM_RENDERING_COLOR_H

#include "Rendering/Rendering.h"
#include "Math/Common.h"

#include "Foundation/Reflect/Data/SimpleData.h"
#include "Foundation/Reflect/Data/DataDeduction.h"






namespace Helium
{
    /// 32-bit ARGB color value.
    class HELIUM_RENDERING_API Color
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

        /// @name Overloaded Operators
        //@{
        inline bool operator==( const Color& rOther ) const;
        inline bool operator!=( const Color& rOther ) const;
        //@}

        /// @name Friend Functions
        //@{
        //friend Serializer& operator<<( Serializer& s, Color& rValue );
        //@}
        
//         friend HELIUM_RENDERING_API tostream& operator<<(tostream& outStream, const Color& v);
//         friend HELIUM_RENDERING_API tistream& operator>>(tistream& inStream, Color& v);

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
    
//////////////// #if HELIUM_SHARED && !defined(HELIUM_RENDERING_EXPORTS)
////////////////         
//////////////// #ifdef _MSC_VER
//////////////// #pragma warning( push )
//////////////// #pragma warning( disable : 4231 )  // nonstandard extension used : 'extern' before template explicit instantiation
//////////////// #endif
//////////////// 
////////////////             extern template HELIUM_RENDERING_API SimpleData<HDRColor4>;
////////////////         
//////////////// #ifdef _MSC_VER
//////////////// #pragma warning( pop )
//////////////// #endif
//////////////// 
//////////////// #endif
//////////////// 
////////////////     typedef Helium::Reflect::SimpleData<Color> RendererColorData;



#define API_DEFINE HELIUM_RENDERING_API
#define TEMPLATE_NAME RenderingSimpleData
#include "Foundation/Reflect/Data/SimpleDataTemplate.h.inl"
#undef API_DEFINE
#undef TEMPLATE_NAME

        //typedef Helium::RenderingSimpleData<Color> RenderingColorData;
}

#include "Rendering/Color.inl"

#endif  // HELIUM_RENDERING_COLOR_H
