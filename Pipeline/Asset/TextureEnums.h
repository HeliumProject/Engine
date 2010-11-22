#pragma once

#include "Foundation/Reflect/Enumeration.h"
#include "Pipeline/Image/ColorFormats.h"
#include "Pipeline/Image/MipSet.h"

namespace Helium
{
    namespace Asset
    {
        class TextureWrapMode
        {
        public:
            enum Enum
            {
                Wrap    = UV_WRAP,
                Mirror  = UV_MIRROR,
                Clamp   = UV_CLAMP,
                Border  = UV_BORDER,
                Count
            };

            REFLECT_DECLARE_ENUMERATION( TextureWrapMode );

            static void EnumerateEnum( Reflect::Enumeration& info )
            {
                info.AddElement(Wrap,   TXT( "Wrap" ) );
                info.AddElement(Mirror, TXT( "Mirror" ) );
                info.AddElement(Clamp,  TXT( "Clamp" ) );
                info.AddElement(Border, TXT( "Border" ) );
            }
        };

        class TextureColorFormat
        {
        public:
            enum Enum
            {
                UNKNOWN  = OUTPUT_CF_UNKNOWN,
                ARGB8888 = OUTPUT_CF_ARGB8888,
                ARGB4444 = OUTPUT_CF_ARGB4444,
                ARGB1555 = OUTPUT_CF_ARGB1555,
                RGB565   = OUTPUT_CF_RGB565,
                A8       = OUTPUT_CF_A8,
                L8       = OUTPUT_CF_L8,
                AL88     = OUTPUT_CF_AL88,
                DXT1     = OUTPUT_CF_DXT1,
                DXT3     = OUTPUT_CF_DXT3,
                DXT5     = OUTPUT_CF_DXT5,
                DUDV     = OUTPUT_CF_DUDV,
                F32      = OUTPUT_CF_F32,
                F32F32   = OUTPUT_CF_F32F32,
                FLOATMAP = OUTPUT_CF_FLOATMAP,
                F16      = OUTPUT_CF_F16,
                F16F16   = OUTPUT_CF_F16F16,
                HALFMAP  = OUTPUT_CF_HALFMAP,
                RGBE     = OUTPUT_CF_RGBE,
                COUNT
            };

            REFLECT_DECLARE_ENUMERATION( TextureColorFormat );

            static void EnumerateEnum( Reflect::Enumeration& info )
            {
                info.AddElement(UNKNOWN,    TXT( "UNKNOWN" ) );
                info.AddElement(ARGB8888,   TXT( "ARGB8888" ) );
                info.AddElement(ARGB4444,   TXT( "ARGB4444" ) );
                info.AddElement(ARGB1555,   TXT( "ARGB1555" ) );
                info.AddElement(RGB565,     TXT( "RGB565" ) );
                info.AddElement(A8,         TXT( "A8" ) );
                info.AddElement(L8,         TXT( "L8" ) );
                info.AddElement(AL88,       TXT( "AL88" ) );
                info.AddElement(DXT1,       TXT( "DXT1" ) );
                info.AddElement(DXT3,       TXT( "DXT3" ) );
                info.AddElement(DXT5,       TXT( "DXT5" ) );
                info.AddElement(DUDV,       TXT( "DUDV" ) );
                info.AddElement(F32,        TXT( "F32" ) );
                info.AddElement(F32F32,     TXT( "F32F32" ) );
                info.AddElement(FLOATMAP,   TXT( "FLOATMAP" ) );
                info.AddElement(F16,        TXT( "F16" ) );
                info.AddElement(F16F16,     TXT( "F16F16" ) );
                info.AddElement(HALFMAP,    TXT( "HALFMAP" ) );
                info.AddElement(RGBE,       TXT( "RGBE" ) );
            }
        };

        // Enum of run time tex min filtering
        class RunTimeFilter
        {
        public:
            enum Enum
            {
                POINT       = FILTER_POINT_SELECT_MIP,
                BILINEAR    = FILTER_LINEAR_SELECT_MIP,
                TRILINEAR   = FILTER_LINEAR_LINEAR_MIP,
                ANISO2_BI   = FILTER_ANISO_2_SELECT_MIP,
                ANISO2_TRI  = FILTER_ANISO_2_LINEAR_MIP,
                ANISO4_BI   = FILTER_ANISO_4_SELECT_MIP,
                ANISO4_TRI  = FILTER_ANISO_4_LINEAR_MIP,
                COUNT
            };

            REFLECT_DECLARE_ENUMERATION( RunTimeFilter );

            static void EnumerateEnum( Reflect::Enumeration& info )
            {
                info.AddElement(POINT,      TXT( "POINT" ) );
                info.AddElement(BILINEAR,   TXT( "BILINEAR" ) );
                info.AddElement(TRILINEAR,  TXT( "TRILINEAR" ) );
                info.AddElement(ANISO2_BI,  TXT( "ANISO2_BI" ) );
                info.AddElement(ANISO2_TRI, TXT( "ANISO2_TRI" ) );
                info.AddElement(ANISO4_BI,  TXT( "ANISO4_BI" ) );
                info.AddElement(ANISO4_TRI, TXT( "ANISO4_TRI" ) );
            }
        };

        // Enum of filter types than can be used to generate mip levels
        class MipGenFilterType
        {
        public:
            enum Enum
            {
                NONE            = MIP_FILTER_NONE,
                POINT           = MIP_FILTER_POINT,
                BOX             = MIP_FILTER_BOX,
                TRIANGLE        = MIP_FILTER_TRIANGLE,
                QUADRATIC       = MIP_FILTER_QUADRATIC,
                CUBIC           = MIP_FILTER_CUBIC,
                MITCHELL        = MIP_FILTER_MITCHELL,
                GAUSSIAN        = MIP_FILTER_GAUSSIAN,
                SINC            = MIP_FILTER_SINC,
                KAISER          = MIP_FILTER_KAISER,
                POINT_COMPOSITE = MIP_FILTER_POINT_COMPOSITE,
                COUNT
            };

            REFLECT_DECLARE_ENUMERATION( MipGenFilterType );

            static void EnumerateEnum( Reflect::Enumeration& info )
            {
                info.AddElement(NONE,               TXT( "NONE" ));
                info.AddElement(POINT,              TXT( "POINT" ));
                info.AddElement(BOX,                TXT( "BOX" ));
                info.AddElement(TRIANGLE,           TXT( "TRIANGLE" ));
                info.AddElement(QUADRATIC,          TXT( "QUADRATIC" ));
                info.AddElement(CUBIC,              TXT( "CUBIC" ));
                info.AddElement(MITCHELL,           TXT( "MITCHELL" ));
                info.AddElement(GAUSSIAN,           TXT( "GAUSSIAN" ));
                info.AddElement(SINC,               TXT( "SINC" ));
                info.AddElement(KAISER,             TXT( "KAISER" ));
                info.AddElement(POINT_COMPOSITE,    TXT( "POINT_COMPOSITE" ));
            }
        };

        // Enum of image processing operations that can be used after the mip maps are generated
        class PostMipFilterType
        {
        public:
            enum Enum
            {
                NOCHANGE        = IMAGE_FILTER_NONE,
                LIGHTER         = IMAGE_FILTER_LIGHTER,
                DARKER          = IMAGE_FILTER_DARKER,
                MORE_CONTRAST   = IMAGE_FILTER_MORE_CONTRAST,
                LESS_CONTRAST   = IMAGE_FILTER_LESS_CONTRAST,
                SMOOTH          = IMAGE_FILTER_SMOOTH,
                SHARPEN_GRADUAL = IMAGE_FILTER_SHARPEN_GRADUAL,
                SHARPEN1X       = IMAGE_FILTER_SHARPEN1X,
                SHARPEN2X       = IMAGE_FILTER_SHARPEN2X,
                SHARPEN3X       = IMAGE_FILTER_SHARPEN3X,
                HIGH_PASS       = IMAGE_FILTER_HIGH_PASS,
                COUNT
            };

            REFLECT_DECLARE_ENUMERATION( PostMipFilterType );

            static void EnumerateEnum( Reflect::Enumeration& info )
            {
                info.AddElement(NOCHANGE,           TXT( "NOCHANGE" ));
                info.AddElement(LIGHTER,            TXT( "LIGHTER" ));
                info.AddElement(DARKER,             TXT( "DARKER" ));
                info.AddElement(MORE_CONTRAST,      TXT( "MORE_CONTRAST" ));
                info.AddElement(LESS_CONTRAST,      TXT( "LESS_CONTRAST" ));
                info.AddElement(SMOOTH,             TXT( "SMOOTH" ));
                info.AddElement(SHARPEN_GRADUAL,    TXT( "SHARPEN_GRADUAL" ));
                info.AddElement(SHARPEN1X,          TXT( "SHARPEN1X" ));
                info.AddElement(SHARPEN2X,          TXT( "SHARPEN2X" ));
                info.AddElement(SHARPEN3X,          TXT( "SHARPEN3X" ));
                info.AddElement(HIGH_PASS,          TXT( "HIGH_PASS" ));
            }
        };

        // Reduction ratio for texture format and compression
        class ReductionRatio
        {
        public:
            enum Enum
            {
                ONE_TO_ONE,
                ONE_HALF,
                ONE_FOURTH,
            };

            REFLECT_DECLARE_ENUMERATION( ReductionRatio );

            static void EnumerateEnum( Reflect::Enumeration& info )
            {
                info.AddElement(ONE_TO_ONE, TXT( "ONE_TO_ONE" ) );
                info.AddElement(ONE_HALF,   TXT( "ONE_HALF" ) );
                info.AddElement(ONE_FOURTH, TXT( "ONE_FOURTH" ) );
            }
        };
    }
}