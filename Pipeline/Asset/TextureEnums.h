#pragma once

#include "Foundation/Reflect/Enumeration.h"
#include "Pipeline/Texture/Image/ColorFormats.h"
#include "Pipeline/Texture/Image/MipSet.h"

namespace Asset
{
  namespace TextureWrapModes
  {
    enum TextureWrapMode
    {
      Wrap    = Nocturnal::UV_WRAP,
      Mirror  = Nocturnal::UV_MIRROR,
      Clamp   = Nocturnal::UV_CLAMP,
      Border  = Nocturnal::UV_BORDER,
      Count
    };

    NOC_COMPILE_ASSERT(Count == Nocturnal::UV_COUNT);

    static void TextureWrapModeEnumerateEnumeration( Reflect::Enumeration* info )
    {
      info->AddElement(Wrap, "Wrap");
      info->AddElement(Mirror, "Mirror");
      info->AddElement(Clamp, "Clamp");
      info->AddElement(Border, "Border");
    }
  }

  typedef TextureWrapModes::TextureWrapMode TextureWrapMode;

  namespace TextureColorFormats
  {
    enum TextureColorFormat
    {
      UNKNOWN  = Nocturnal::OUTPUT_CF_UNKNOWN,
      ARGB8888 = Nocturnal::OUTPUT_CF_ARGB8888,
      ARGB4444 = Nocturnal::OUTPUT_CF_ARGB4444,
      ARGB1555 = Nocturnal::OUTPUT_CF_ARGB1555,
      RGB565   = Nocturnal::OUTPUT_CF_RGB565,
      A8       = Nocturnal::OUTPUT_CF_A8,
      L8       = Nocturnal::OUTPUT_CF_L8,
      AL88     = Nocturnal::OUTPUT_CF_AL88,
      DXT1     = Nocturnal::OUTPUT_CF_DXT1,
      DXT3     = Nocturnal::OUTPUT_CF_DXT3,
      DXT5     = Nocturnal::OUTPUT_CF_DXT5,
      DUDV     = Nocturnal::OUTPUT_CF_DUDV,
      F32      = Nocturnal::OUTPUT_CF_F32,
      F32F32   = Nocturnal::OUTPUT_CF_F32F32,
      FLOATMAP = Nocturnal::OUTPUT_CF_FLOATMAP,
      F16      = Nocturnal::OUTPUT_CF_F16,
      F16F16   = Nocturnal::OUTPUT_CF_F16F16,
      HALFMAP  = Nocturnal::OUTPUT_CF_HALFMAP,
      RGBE     = Nocturnal::OUTPUT_CF_RGBE,
      COUNT
    };

    NOC_COMPILE_ASSERT(COUNT == Nocturnal::OUTPUT_CF_COUNT);

    static void TextureColorFormatEnumerateEnumeration( Reflect::Enumeration* info )
    {
      info->AddElement(UNKNOWN, "UNKNOWN");
      info->AddElement(ARGB8888, "ARGB8888");
      info->AddElement(ARGB4444, "ARGB4444");
      info->AddElement(ARGB1555, "ARGB1555");
      info->AddElement(RGB565, "RGB565");
      info->AddElement(A8, "A8");
      info->AddElement(L8, "L8");
      info->AddElement(AL88, "AL88");
      info->AddElement(DXT1, "DXT1");
      info->AddElement(DXT3, "DXT3");
      info->AddElement(DXT5, "DXT5");
      info->AddElement(DUDV, "DUDV");
      info->AddElement(F32, "F32");
      info->AddElement(F32F32, "F32F32");
      info->AddElement(FLOATMAP, "FLOATMAP");
      info->AddElement(F16, "F16");
      info->AddElement(F16F16, "F16F16");
      info->AddElement(HALFMAP, "HALFMAP");
      info->AddElement(RGBE, "RGBE");
    }
  }
  typedef TextureColorFormats::TextureColorFormat TextureColorFormat;

  // Enum of run time tex min filtering
  namespace RunTimeFilters
  {
    enum RunTimeFilter
    {
      RTF_POINT = Nocturnal::FILTER_POINT_SELECT_MIP,
      RTF_BILINEAR = Nocturnal::FILTER_LINEAR_SELECT_MIP,
      RTF_TRILINEAR = Nocturnal::FILTER_LINEAR_LINEAR_MIP,
      RTF_ANISO2_BI = Nocturnal::FILTER_ANISO_2_SELECT_MIP,
      RTF_ANISO2_TRI = Nocturnal::FILTER_ANISO_2_LINEAR_MIP,
      RTF_ANISO4_BI = Nocturnal::FILTER_ANISO_4_SELECT_MIP,
      RTF_ANISO4_TRI = Nocturnal::FILTER_ANISO_4_LINEAR_MIP,
      RTF_COUNT
    };
    NOC_COMPILE_ASSERT( RTF_COUNT == Nocturnal::FILTER_COUNT );
    static void RunTimeFilterEnumerateEnumeration( Reflect::Enumeration* info )
    {
      info->AddElement(RTF_POINT, "RTF_POINT", "POINT");
      info->AddElement(RTF_BILINEAR, "RTF_BILINEAR", "BILINEAR");
      info->AddElement(RTF_TRILINEAR, "RTF_TRILINEAR", "TRILINEAR");
      info->AddElement(RTF_ANISO2_BI, "RTF_ANISO2_BI", "ANISO2_BI");
      info->AddElement(RTF_ANISO2_TRI, "RTF_ANISO2_TRI", "ANISO2_TRI");
      info->AddElement(RTF_ANISO4_BI, "RTF_ANISO4_BI", "ANISO4_BI");
      info->AddElement(RTF_ANISO4_TRI, "RTF_ANISO4_TRI", "ANISO4_TRI");
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
      info->AddElement(ONE_ONE, "ONE_ONE");
      info->AddElement(ONE_HALF, "ONE_HALF");
      info->AddElement(ONE_FOURTH, "ONE_FOURTH");
    }
  }
  typedef ReductionRatios::ReductionRatio ReductionRatio;

  // Enum of filter types than can be used to generate mip levels
  namespace MipGenFilterTypes
  {
    enum MipGenFilterType
    {
      MIP_NONE            = Nocturnal::MIP_FILTER_NONE,
      MIP_POINT           = Nocturnal::MIP_FILTER_POINT,
      MIP_BOX             = Nocturnal::MIP_FILTER_BOX,
      MIP_TRIANGLE        = Nocturnal::MIP_FILTER_TRIANGLE,
      MIP_QUADRATIC       = Nocturnal::MIP_FILTER_QUADRATIC,
      MIP_CUBIC           = Nocturnal::MIP_FILTER_CUBIC,
      MIP_MITCHELL        = Nocturnal::MIP_FILTER_MITCHELL,
      MIP_GAUSSIAN        = Nocturnal::MIP_FILTER_GAUSSIAN,
      MIP_SINC            = Nocturnal::MIP_FILTER_SINC,
      MIP_KAISER          = Nocturnal::MIP_FILTER_KAISER,
      MIP_POINT_COMPOSITE = Nocturnal::MIP_FILTER_POINT_COMPOSITE,
      MIP_COUNT
    };
    NOC_COMPILE_ASSERT( MIP_COUNT == Nocturnal::MIP_FILTER_COUNT );

    static void MipGenFilterTypeEnumerateEnumeration( Reflect::Enumeration* info )
    {
      info->AddElement(MIP_NONE, "MIP_NONE", "NONE");
      info->AddElement(MIP_POINT, "MIP_POINT", "POINT");
      info->AddElement(MIP_BOX, "MIP_BOX", "BOX");
      info->AddElement(MIP_TRIANGLE, "MIP_TRIANGLE", "TRIANGLE");
      info->AddElement(MIP_QUADRATIC, "MIP_QUADRATIC", "QUADRATIC");
      info->AddElement(MIP_CUBIC, "MIP_CUBIC", "CUBIC");
      info->AddElement(MIP_MITCHELL, "MIP_MITCHELL", "MITCHELL");
      info->AddElement(MIP_GAUSSIAN, "MIP_GAUSSIAN", "GAUSSIAN");
      info->AddElement(MIP_SINC, "MIP_SINC", "SINC");
      info->AddElement(MIP_KAISER, "MIP_KAISER", "KAISER");
      info->AddElement(MIP_POINT_COMPOSITE, "MIP_POINT_COMPOSITE", "POINT_COMPOSITE");
    }
  }
  typedef MipGenFilterTypes::MipGenFilterType MipGenFilterType;

  // Enum of image processing operations that can be used after the mip maps are generated
  namespace PostMipFilterTypes
  {
    enum PostMipFilterType
    {
      POST_NOCHANGE        = Nocturnal::IMAGE_FILTER_NONE,
      POST_LIGHTER         = Nocturnal::IMAGE_FILTER_LIGHTER,
      POST_DARKER          = Nocturnal::IMAGE_FILTER_DARKER,
      POST_MORE_CONTRAST   = Nocturnal::IMAGE_FILTER_MORE_CONTRAST,
      POST_LESS_CONTRAST   = Nocturnal::IMAGE_FILTER_LESS_CONTRAST,
      POST_SMOOTH          = Nocturnal::IMAGE_FILTER_SMOOTH,
      POST_SHARPEN_GRADUAL = Nocturnal::IMAGE_FILTER_SHARPEN_GRADUAL,
      POST_SHARPEN1X       = Nocturnal::IMAGE_FILTER_SHARPEN1X,
      POST_SHARPEN2X       = Nocturnal::IMAGE_FILTER_SHARPEN2X,
      POST_SHARPEN3X       = Nocturnal::IMAGE_FILTER_SHARPEN3X,
      POST_HIGH_PASS       = Nocturnal::IMAGE_FILTER_HIGH_PASS,
      POST_COUNT
    };
    NOC_COMPILE_ASSERT( POST_COUNT == Nocturnal::IMAGE_FILTER_COUNT );

    static void PostMipFilterTypeEnumerateEnumeration( Reflect::Enumeration* info )
    {
      info->AddElement(POST_NOCHANGE, "POST_NOCHANGE", "NO_CHANGE");
      info->AddElement(POST_LIGHTER, "POST_LIGHTER", "LIGHTER");
      info->AddElement(POST_DARKER, "POST_DARKER", "DARKER");
      info->AddElement(POST_MORE_CONTRAST, "POST_MORE_CONTRAST", "MORE_CONTRAST");
      info->AddElement(POST_LESS_CONTRAST, "POST_LESS_CONTRAST", "LESS_CONTRAST");
      info->AddElement(POST_SMOOTH, "POST_SMOOTH", "SMOOTH");
      info->AddElement(POST_SHARPEN_GRADUAL, "POST_SHARPEN_GRADUAL", "SHARPEN_GRADUAL");
      info->AddElement(POST_SHARPEN1X, "POST_SHARPEN1X", "SHARPEN1X");
      info->AddElement(POST_SHARPEN2X, "POST_SHARPEN2X", "SHARPEN2X");
      info->AddElement(POST_SHARPEN3X, "POST_SHARPEN3X", "SHARPEN3X");
      info->AddElement(POST_HIGH_PASS, "POST_HIGH_PASS", "HIGH_PASS");
    }
  }
  typedef PostMipFilterTypes::PostMipFilterType PostMipFilterType;
}
