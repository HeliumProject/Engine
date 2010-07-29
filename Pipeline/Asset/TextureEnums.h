#pragma once

#include "Foundation/Reflect/Enumeration.h"
#include "Pipeline/Image/ColorFormats.h"
#include "Pipeline/Image/MipSet.h"

namespace Asset
{
  namespace TextureWrapModes
  {
    enum TextureWrapMode
    {
      Wrap    = Helium::UV_WRAP,
      Mirror  = Helium::UV_MIRROR,
      Clamp   = Helium::UV_CLAMP,
      Border  = Helium::UV_BORDER,
      Count
    };

    HELIUM_COMPILE_ASSERT(Count == Helium::UV_COUNT);

    static void TextureWrapModeEnumerateEnumeration( Reflect::Enumeration* info )
    {
      info->AddElement(Wrap, TXT( "Wrap" ) );
      info->AddElement(Mirror, TXT( "Mirror" ) );
      info->AddElement(Clamp, TXT( "Clamp" ) );
      info->AddElement(Border, TXT( "Border" ) );
    }
  }

  typedef TextureWrapModes::TextureWrapMode TextureWrapMode;

  namespace TextureColorFormats
  {
    enum TextureColorFormat
    {
      UNKNOWN  = Helium::OUTPUT_CF_UNKNOWN,
      ARGB8888 = Helium::OUTPUT_CF_ARGB8888,
      ARGB4444 = Helium::OUTPUT_CF_ARGB4444,
      ARGB1555 = Helium::OUTPUT_CF_ARGB1555,
      RGB565   = Helium::OUTPUT_CF_RGB565,
      A8       = Helium::OUTPUT_CF_A8,
      L8       = Helium::OUTPUT_CF_L8,
      AL88     = Helium::OUTPUT_CF_AL88,
      DXT1     = Helium::OUTPUT_CF_DXT1,
      DXT3     = Helium::OUTPUT_CF_DXT3,
      DXT5     = Helium::OUTPUT_CF_DXT5,
      DUDV     = Helium::OUTPUT_CF_DUDV,
      F32      = Helium::OUTPUT_CF_F32,
      F32F32   = Helium::OUTPUT_CF_F32F32,
      FLOATMAP = Helium::OUTPUT_CF_FLOATMAP,
      F16      = Helium::OUTPUT_CF_F16,
      F16F16   = Helium::OUTPUT_CF_F16F16,
      HALFMAP  = Helium::OUTPUT_CF_HALFMAP,
      RGBE     = Helium::OUTPUT_CF_RGBE,
      COUNT
    };

    HELIUM_COMPILE_ASSERT(COUNT == Helium::OUTPUT_CF_COUNT);

    static void TextureColorFormatEnumerateEnumeration( Reflect::Enumeration* info )
    {
      info->AddElement(UNKNOWN, TXT( "UNKNOWN" ) );
      info->AddElement(ARGB8888, TXT( "ARGB8888" ) );
      info->AddElement(ARGB4444, TXT( "ARGB4444" ) );
      info->AddElement(ARGB1555, TXT( "ARGB1555" ) );
      info->AddElement(RGB565, TXT( "RGB565" ) );
      info->AddElement(A8, TXT( "A8" ) );
      info->AddElement(L8, TXT( "L8" ) );
      info->AddElement(AL88, TXT( "AL88" ) );
      info->AddElement(DXT1, TXT( "DXT1" ) );
      info->AddElement(DXT3, TXT( "DXT3" ) );
      info->AddElement(DXT5, TXT( "DXT5" ) );
      info->AddElement(DUDV, TXT( "DUDV" ) );
      info->AddElement(F32, TXT( "F32" ) );
      info->AddElement(F32F32, TXT( "F32F32" ) );
      info->AddElement(FLOATMAP, TXT( "FLOATMAP" ) );
      info->AddElement(F16, TXT( "F16" ) );
      info->AddElement(F16F16, TXT( "F16F16" ) );
      info->AddElement(HALFMAP, TXT( "HALFMAP" ) );
      info->AddElement(RGBE, TXT( "RGBE" ) );
    }
  }
  typedef TextureColorFormats::TextureColorFormat TextureColorFormat;

  // Enum of run time tex min filtering
  namespace RunTimeFilters
  {
    enum RunTimeFilter
    {
      RTF_POINT = Helium::FILTER_POINT_SELECT_MIP,
      RTF_BILINEAR = Helium::FILTER_LINEAR_SELECT_MIP,
      RTF_TRILINEAR = Helium::FILTER_LINEAR_LINEAR_MIP,
      RTF_ANISO2_BI = Helium::FILTER_ANISO_2_SELECT_MIP,
      RTF_ANISO2_TRI = Helium::FILTER_ANISO_2_LINEAR_MIP,
      RTF_ANISO4_BI = Helium::FILTER_ANISO_4_SELECT_MIP,
      RTF_ANISO4_TRI = Helium::FILTER_ANISO_4_LINEAR_MIP,
      RTF_COUNT
    };
    HELIUM_COMPILE_ASSERT( RTF_COUNT == Helium::FILTER_COUNT );
    static void RunTimeFilterEnumerateEnumeration( Reflect::Enumeration* info )
    {
      info->AddElement(RTF_POINT, TXT( "RTF_POINT" ), TXT( "POINT" ) );
      info->AddElement(RTF_BILINEAR, TXT( "RTF_BILINEAR" ), TXT( "BILINEAR" ) );
      info->AddElement(RTF_TRILINEAR, TXT( "RTF_TRILINEAR" ), TXT( "TRILINEAR" ) );
      info->AddElement(RTF_ANISO2_BI, TXT( "RTF_ANISO2_BI" ), TXT( "ANISO2_BI" ) );
      info->AddElement(RTF_ANISO2_TRI, TXT( "RTF_ANISO2_TRI" ), TXT( "ANISO2_TRI" ) );
      info->AddElement(RTF_ANISO4_BI, TXT( "RTF_ANISO4_BI" ), TXT( "ANISO4_BI" ) );
      info->AddElement(RTF_ANISO4_TRI, TXT( "RTF_ANISO4_TRI" ), TXT( "ANISO4_TRI" ) );
    }
  }
  typedef RunTimeFilters::RunTimeFilter RunTimeFilter;

  // Reduction ratio for texture format and compression
  namespace ReductionRatios
  {
    enum ReductionRatio
    {
      ONE_ONE,
      ONE_HALF,
      ONE_FOURTH,
    };
    static void ReductionRatioEnumerateEnumeration( Reflect::Enumeration* info )
    {
      info->AddElement(ONE_ONE, TXT( "ONE_ONE" ) );
      info->AddElement(ONE_HALF, TXT( "ONE_HALF" ) );
      info->AddElement(ONE_FOURTH, TXT( "ONE_FOURTH" ) );
    }
  }
  typedef ReductionRatios::ReductionRatio ReductionRatio;

  // Enum of filter types than can be used to generate mip levels
  namespace MipGenFilterTypes
  {
    enum MipGenFilterType
    {
      MIP_NONE            = Helium::MIP_FILTER_NONE,
      MIP_POINT           = Helium::MIP_FILTER_POINT,
      MIP_BOX             = Helium::MIP_FILTER_BOX,
      MIP_TRIANGLE        = Helium::MIP_FILTER_TRIANGLE,
      MIP_QUADRATIC       = Helium::MIP_FILTER_QUADRATIC,
      MIP_CUBIC           = Helium::MIP_FILTER_CUBIC,
      MIP_MITCHELL        = Helium::MIP_FILTER_MITCHELL,
      MIP_GAUSSIAN        = Helium::MIP_FILTER_GAUSSIAN,
      MIP_SINC            = Helium::MIP_FILTER_SINC,
      MIP_KAISER          = Helium::MIP_FILTER_KAISER,
      MIP_POINT_COMPOSITE = Helium::MIP_FILTER_POINT_COMPOSITE,
      MIP_COUNT
    };
    HELIUM_COMPILE_ASSERT( MIP_COUNT == Helium::MIP_FILTER_COUNT );

    static void MipGenFilterTypeEnumerateEnumeration( Reflect::Enumeration* info )
    {
      info->AddElement(MIP_NONE, TXT( "MIP_NONE" ), TXT( "NONE" ) );
      info->AddElement(MIP_POINT, TXT( "MIP_POINT" ), TXT( "POINT" ) );
      info->AddElement(MIP_BOX, TXT( "MIP_BOX" ), TXT( "BOX" ) );
      info->AddElement(MIP_TRIANGLE, TXT( "MIP_TRIANGLE" ), TXT( "TRIANGLE" ) );
      info->AddElement(MIP_QUADRATIC, TXT( "MIP_QUADRATIC" ), TXT( "QUADRATIC" ) );
      info->AddElement(MIP_CUBIC, TXT( "MIP_CUBIC" ), TXT( "CUBIC" ) );
      info->AddElement(MIP_MITCHELL, TXT( "MIP_MITCHELL" ), TXT( "MITCHELL" ) );
      info->AddElement(MIP_GAUSSIAN, TXT( "MIP_GAUSSIAN" ), TXT( "GAUSSIAN" ) );
      info->AddElement(MIP_SINC, TXT( "MIP_SINC" ), TXT( "SINC" ) );
      info->AddElement(MIP_KAISER, TXT( "MIP_KAISER" ), TXT( "KAISER" ) );
      info->AddElement(MIP_POINT_COMPOSITE, TXT( "MIP_POINT_COMPOSITE" ), TXT( "POINT_COMPOSITE" ) );
    }
  }
  typedef MipGenFilterTypes::MipGenFilterType MipGenFilterType;

  // Enum of image processing operations that can be used after the mip maps are generated
  namespace PostMipFilterTypes
  {
    enum PostMipFilterType
    {
      POST_NOCHANGE        = Helium::IMAGE_FILTER_NONE,
      POST_LIGHTER         = Helium::IMAGE_FILTER_LIGHTER,
      POST_DARKER          = Helium::IMAGE_FILTER_DARKER,
      POST_MORE_CONTRAST   = Helium::IMAGE_FILTER_MORE_CONTRAST,
      POST_LESS_CONTRAST   = Helium::IMAGE_FILTER_LESS_CONTRAST,
      POST_SMOOTH          = Helium::IMAGE_FILTER_SMOOTH,
      POST_SHARPEN_GRADUAL = Helium::IMAGE_FILTER_SHARPEN_GRADUAL,
      POST_SHARPEN1X       = Helium::IMAGE_FILTER_SHARPEN1X,
      POST_SHARPEN2X       = Helium::IMAGE_FILTER_SHARPEN2X,
      POST_SHARPEN3X       = Helium::IMAGE_FILTER_SHARPEN3X,
      POST_HIGH_PASS       = Helium::IMAGE_FILTER_HIGH_PASS,
      POST_COUNT
    };
    HELIUM_COMPILE_ASSERT( POST_COUNT == Helium::IMAGE_FILTER_COUNT );

    static void PostMipFilterTypeEnumerateEnumeration( Reflect::Enumeration* info )
    {
      info->AddElement(POST_NOCHANGE, TXT( "POST_NOCHANGE" ), TXT( "NO_CHANGE" ) );
      info->AddElement(POST_LIGHTER, TXT( "POST_LIGHTER" ), TXT( "LIGHTER" ) );
      info->AddElement(POST_DARKER, TXT( "POST_DARKER" ), TXT( "DARKER" ) );
      info->AddElement(POST_MORE_CONTRAST, TXT( "POST_MORE_CONTRAST" ), TXT( "MORE_CONTRAST" ) );
      info->AddElement(POST_LESS_CONTRAST, TXT( "POST_LESS_CONTRAST" ), TXT( "LESS_CONTRAST" ) );
      info->AddElement(POST_SMOOTH, TXT( "POST_SMOOTH" ), TXT( "SMOOTH" ) );
      info->AddElement(POST_SHARPEN_GRADUAL, TXT( "POST_SHARPEN_GRADUAL" ), TXT( "SHARPEN_GRADUAL" ) );
      info->AddElement(POST_SHARPEN1X, TXT( "POST_SHARPEN1X" ), TXT( "SHARPEN1X" ) );
      info->AddElement(POST_SHARPEN2X, TXT( "POST_SHARPEN2X" ), TXT( "SHARPEN2X" ) );
      info->AddElement(POST_SHARPEN3X, TXT( "POST_SHARPEN3X" ), TXT( "SHARPEN3X" ) );
      info->AddElement(POST_HIGH_PASS, TXT( "POST_HIGH_PASS" ), TXT( "HIGH_PASS" ) );
    }
  }
  typedef PostMipFilterTypes::PostMipFilterType PostMipFilterType;
}
