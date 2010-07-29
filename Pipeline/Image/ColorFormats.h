#pragma once

#include <math.h>

#include "Platform/Assert.h"
#include "Platform/Types.h"
#include "Foundation/Math/Half.h"
#include "Foundation/Math/Vector3.h"

namespace Helium
{
  const u32 RGBE_EXPONENT_BIAS=128;

  //-----------------------------------------------------------------------------
  // Color formats supported by the texture loaders
  enum ColorFormat
  {
    CF_UNKNOWN = 0,     //Used for error codes
    CF_ARGB8888,        //32bit packed color (alpha: 24-31, Red: 16-23, Green:8-15, Blue:0-7)
    CF_ARGB4444,        //16bit packed color (alpha: 11-15, Red: 8-11, Green:4-7, Blue:0-3)
    CF_ARGB1555,        //16bit packed color (alpha:15, Red:10-14, Green:5-9, Blue: 0-4)
    CF_RGB565,          //16bit packed color (Red:11-15, Green:5-10, Blue: 0-4)
    CF_A8,              //single 8bit alpha only (range 0.0 to 1.0)
    CF_L8,              //single 8bit unsigned channel (range 0.0 to 1.0)
    CF_AL88,            //16 bit, 8 bits of luminance and 8 bits of alpha
    CF_L16,             //single 16 bit unsigned channel (range 0.0 to 1.0)
    CF_RGBAFLOATMAP,    //4 IEEE floats, stored in RGBA order
    CF_RGBAHALFMAP,     //4 HALF floats, stored in RGBA order
    CF_F32,             //single float
    CF_F32F32,          //pair of floats
    CF_F16,             //single half
    CF_F16F16,          //pair of halfs
    CF_RGBE,            //RGB with exponent (no alpha) (Exponent: 24-31, Blue: 16-23, Green 8-15, Red 0-7)
    _COLOR_FORMAT_FORCE_SIZE = 0xffffffff
  };

  //-----------------------------------------------------------------------------
  // Output color formats supported by the processors. If a format exists in this
  // enum and the above then it can be assumed they are the same format
  enum OutputColorFormat
  {
    OUTPUT_CF_UNKNOWN,
    OUTPUT_CF_ARGB8888,
    OUTPUT_CF_ARGB4444,
    OUTPUT_CF_ARGB1555,
    OUTPUT_CF_RGB565,
    OUTPUT_CF_A8,
    OUTPUT_CF_L8,
    OUTPUT_CF_AL88,
    OUTPUT_CF_DXT1,
    OUTPUT_CF_DXT3,
    OUTPUT_CF_DXT5,
    OUTPUT_CF_DUDV,
    OUTPUT_CF_F32,
    OUTPUT_CF_F32F32,
    OUTPUT_CF_FLOATMAP,
    OUTPUT_CF_F16,
    OUTPUT_CF_F16F16,
    OUTPUT_CF_HALFMAP,
    OUTPUT_CF_RGBE,
    OUTPUT_CF_COUNT
  };

  //-----------------------------------------------------------------------------
  // returns the raw storage bits per pixel of a given pixel format
  inline u32 ColorFormatBits(ColorFormat format)
  {
    switch (format)
    {
    case CF_L8:
    case CF_A8:
      return 8;

    case CF_RGB565:
    case CF_ARGB4444:
    case CF_ARGB1555:
    case CF_AL88:
    case CF_L16:
    case CF_F16:
      return 16;

    case CF_ARGB8888:
    case CF_RGBE:
    case CF_F32:
    case CF_F16F16:
      return 32;

    case CF_RGBAHALFMAP:
    case CF_F32F32:
      return 64;

    case CF_RGBAFLOATMAP:
      return 128;
    }

    return 0;
  }


  //-----------------------------------------------------------------------------
  // returns true if the specified format is an HDR format
  inline bool ColorFormatHDR(OutputColorFormat format)
  {
    switch (format)
    {
    case OUTPUT_CF_ARGB8888:
    case OUTPUT_CF_ARGB4444:
    case OUTPUT_CF_ARGB1555:
    case OUTPUT_CF_RGB565:
    case OUTPUT_CF_A8:
    case OUTPUT_CF_L8:
    case OUTPUT_CF_AL88:
    case OUTPUT_CF_DXT1:
    case OUTPUT_CF_DXT3:
    case OUTPUT_CF_DXT5:
    case OUTPUT_CF_DUDV:
      return false;

    case OUTPUT_CF_F32:
    case OUTPUT_CF_F32F32:
    case OUTPUT_CF_FLOATMAP:
    case OUTPUT_CF_F16:
    case OUTPUT_CF_F16F16:
    case OUTPUT_CF_HALFMAP:
    case OUTPUT_CF_RGBE:
      return true;
    }

    return false;
  }

  //-----------------------------------------------------------------------------
  // returns true if the specified format is an HDR format
  inline bool ColorFormatHDR(ColorFormat format)
  {
    switch (format)
    {
    case CF_L8:
    case CF_A8:
    case CF_RGB565:
    case CF_ARGB4444:
    case CF_ARGB1555:
    case CF_AL88:
    case CF_ARGB8888:
      return false;

    case CF_L16:
    case CF_RGBE:
    case CF_F32:
    case CF_F16:
    case CF_F32F32:
    case CF_F16F16:
    case CF_RGBAFLOATMAP:
    case CF_RGBAHALFMAP:
      return true;
    }
    return false;
  }

  //-----------------------------------------------------------------------------
  // returns a compatible pixel format given an output format
  inline ColorFormat CompatibleColorFormat(OutputColorFormat format)
  {
    switch (format)
    {
    case OUTPUT_CF_ARGB8888:
      return CF_ARGB8888;
    case OUTPUT_CF_ARGB4444:
      return CF_ARGB4444;
    case OUTPUT_CF_ARGB1555:
      return CF_ARGB1555;
    case OUTPUT_CF_RGB565:
      return CF_RGB565;
    case OUTPUT_CF_A8:
      return CF_A8;
    case OUTPUT_CF_L8:
      return CF_L8;
    case OUTPUT_CF_AL88:
      return CF_AL88;
    case OUTPUT_CF_F32:
      return CF_F32;
    case OUTPUT_CF_F32F32:
      return CF_F32F32;
    case OUTPUT_CF_FLOATMAP:
      return CF_RGBAFLOATMAP;
    case OUTPUT_CF_F16:
      return CF_F16;
    case OUTPUT_CF_F16F16:
      return CF_F16F16;
    case OUTPUT_CF_HALFMAP:
      return CF_RGBAHALFMAP;
    case OUTPUT_CF_RGBE:
      return CF_RGBE;
    }

    return CF_UNKNOWN;
  }

  //-----------------------------------------------------------------------------
  // Returns the text name of an output color format
  inline const char* ColorFormatName(OutputColorFormat format)
  {
    switch (format)
    {
    case OUTPUT_CF_ARGB8888:
      return "ARGB8888";
    case OUTPUT_CF_ARGB4444:
      return "ARGB4444";
    case OUTPUT_CF_ARGB1555:
      return "ARGB1555";
    case OUTPUT_CF_RGB565:
      return "RGB565";
    case OUTPUT_CF_A8:
      return "A8";
    case OUTPUT_CF_L8:
      return "L8";
    case OUTPUT_CF_AL88:
      return "AL88";
    case OUTPUT_CF_DXT1:
      return "DXT1";
    case OUTPUT_CF_DXT3:
      return "DXT3";
    case OUTPUT_CF_DXT5:
      return "DXT5";
    case OUTPUT_CF_DUDV:
      return "DUDV";
    case OUTPUT_CF_F32:
      return "F32";
    case OUTPUT_CF_F32F32:
      return "F32F32";
    case OUTPUT_CF_FLOATMAP:
      return "F32F32F32F32";
    case OUTPUT_CF_F16:
      return "F16";
    case OUTPUT_CF_F16F16:
      return "F16F16";
    case OUTPUT_CF_HALFMAP:
      return "F16F16F16F16";
    case OUTPUT_CF_RGBE:
      return "RGBE";
    }

    return "UNKNOWN";
  }

  //-----------------------------------------------------------------------------
  inline void ClampColor(float &r,float &g,float &b,float &a)
  {
    if (r>1.0f)
        r=1.0f;
    if (r<0.0f)
        r=0.0f;

    if (g>1.0f)
        g=1.0f;
    if (g<0.0f)
        g=0.0f;

    if (b>1.0f)
        b=1.0f;
    if (b<0.0f)
        b=0.0f;

    if (a>1.0f)
        a=1.0f;
    if (a<0.0f)
        a=0.0f;
  }

  //-----------------------------------------------------------------------------
  inline void ClampColor(i32 &r,i32 &g,i32 &b,i32 &a)
  {
    if (r>255)
        r=255;
    if (r<0)
        r=0;

    if (g>255)
        g=255;
    if (g<0)
        g=0;

    if (b>255)
        b=255;
    if (b<0)
        b=0;

    if (a>255)
        a=255;
    if (a<0)
        a=0;
  }

  //-----------------------------------------------------------------------------
  inline void ColorFormatExpandRGBE(u32 hdr, float& r, float& g,float& b)
  {
    r = ((hdr & 0x000000ff))*0.0039215686f;
    g = ((hdr & 0x0000ff00)>>8)*0.0039215686f;
    b = ((hdr & 0x00ff0000)>>16)*0.0039215686f;
    i32 e = ((hdr & 0xff000000)>>24);
    if (e==0)
    {
      r=0.0f;
      g=0.0f;
      b=0.0f;
      return;
    }

    e-=RGBE_EXPONENT_BIAS; //128
    float factor = ldexp(1.0f,e);

    r*=factor;
    g*=factor;
    b*=factor;
  }

  //-----------------------------------------------------------------------------
  inline u32 ColorFormatCreateRGBE(float red, float grn,float blu)
  {
    // First clamp the values to 0
    if (red < 0.0f) red = 0.0f;
    if (grn < 0.0f) grn = 0.0f;
    if (blu < 0.0f) blu = 0.0f;

    // Compute the exponent from the largest of the three components
    float max = grn > blu ? grn : blu;
    max = red > max ? red : max;

    // If too small, just return 0s.
    if (max < 1.0e-32f)
    {
      return 0;
    }
    // Compute the exponent factor
    int exp;
    float expFactor = frexp(max, &exp) / max;
    i32 r = (i32)(((red * expFactor)*255.0f)+0.5f);
    i32 g = (i32)(((grn * expFactor)*255.0f)+0.5f);
    i32 b = (i32)(((blu * expFactor)*255.0f)+0.5f);
    i32 a = (exp + 128);

    ClampColor(r,g,b,a);

    return ( (a << 24) | (b << 16) | (g << 8) | (r));
  }

  //-----------------------------------------------------------------------------

  inline float SrgbToLinear(float srgb)
  {
    if(srgb <= 0.04045f)
      return srgb / 12.92f;
    else
      return powf((srgb + 0.055f) / 1.055f, 2.4f);
  }

  //-----------------------------------------------------------------------------

  inline float LinearToSrgb(float linear)
  {
    if(linear <= 0.0031308f)
      return linear * 12.92f;
    else
      return 1.055f * powf(linear, 1.f / 2.4f) - 0.055f;
  }

  //-----------------------------------------------------------------------------

  // only integer destination formats are converted to srgb if flag is set
  inline bool MakeColorFormatBatch(void* dst, u32 pixel_count, ColorFormat fmt, f32 *r, f32 *g, f32 *b, f32 *a, bool convert_to_srgb)
  {
  #ifndef START_BATCH_LOOP
    #define START_BATCH_LOOP  for(u32 p = 0; p < pixel_count; ++p)\
                              {
    #define END_BATCH_LOOP    }
  #endif
    #define CONV_TO_SRGB(r, g, b) { if(convert_to_srgb) { r = LinearToSrgb(r); g = LinearToSrgb(g); b = LinearToSrgb(b); } }
    switch(fmt)
    {
      case CF_RGBAFLOATMAP:
      {
        float* ptr = (float*)dst;
        START_BATCH_LOOP
          *ptr++ = r[p];
          *ptr++ = g[p];
          *ptr++ = b[p];
          *ptr++ = a[p];
        END_BATCH_LOOP
        return true;
      }
      case CF_RGBAHALFMAP:
      {
        i16* ptr = (i16*)dst;
        START_BATCH_LOOP
          *ptr++ = ::Math::FloatToHalf(r[p]);
          *ptr++ = ::Math::FloatToHalf(g[p]);
          *ptr++ = ::Math::FloatToHalf(b[p]);
          *ptr++ = ::Math::FloatToHalf(a[p]);
        END_BATCH_LOOP
        return true;
      }
      case CF_RGBE:
      {
        u32* ptr = (u32*)dst;
        START_BATCH_LOOP
          ptr[p] = ColorFormatCreateRGBE(r[p],g[p],b[p]);
        END_BATCH_LOOP
        return true;
      }
      case CF_F32:
      {
        float* ptr = (float*)dst;
        START_BATCH_LOOP
          ptr[p] = 0.212671f * r[p] + 0.715160f * g[p] + 0.072169f * b[p];
        END_BATCH_LOOP
        return true;
      }
      case CF_F32F32:
      {
        float* ptr = (float*)dst;
        START_BATCH_LOOP
          *ptr++ = r[p];
          *ptr++ = g[p];
        END_BATCH_LOOP
        return true;
      }
      case CF_F16:
      {
        i16* ptr = (i16*)dst;
        START_BATCH_LOOP
          ptr[p] = ::Math::FloatToHalf(0.212671f * r[p] + 0.715160f * g[p] + 0.072169f * b[p]);
        END_BATCH_LOOP
        return true;
      }
      case CF_F16F16:
      {
        i16* ptr = (i16*)dst;
        START_BATCH_LOOP
          *ptr++ = ::Math::FloatToHalf(r[p]);
          *ptr++ = ::Math::FloatToHalf(g[p]);
        END_BATCH_LOOP
        return true;
      }

      case CF_ARGB4444:
      {
        u16* ptr = (u16*)dst;
        START_BATCH_LOOP
          float fl_r = r[p];
          float fl_g = g[p];
          float fl_b = b[p];
          float fl_a = a[p];
          ClampColor(fl_r,fl_g,fl_b,fl_a);
          CONV_TO_SRGB(fl_r,fl_g,fl_b);
          u32  red    = (i32)(fl_r*15.0f + 0.5f);
          u32  green  = (i32)(fl_g*15.0f + 0.5f);
          u32  blue   = (i32)(fl_b*15.0f + 0.5f);
          u32  alpha  = (i32)(fl_a*15.0f + 0.5f);
          ptr[p] = ((alpha<<12) | (red<<8) | (green<<4) | (blue<<0));
        END_BATCH_LOOP
        return true;
      }

      case CF_RGB565:
      {
        u16* ptr = (u16*)dst;
        START_BATCH_LOOP
          float fl_r = r[p];
          float fl_g = g[p];
          float fl_b = b[p];
          float fl_a = a[p];
          ClampColor(fl_r,fl_g,fl_b,fl_a);
          CONV_TO_SRGB(fl_r,fl_g,fl_b);
          u32  red = (u32)(fl_r*31.0f + 0.5f);
          u32  green = (u32)(g[p]*63.0f + 0.5f);
          u32  blue = (u32)(b[p]*31.0f + 0.5f);
          ptr[p] = ( (red<<11) | (green<<5) | (blue<<0) );
        END_BATCH_LOOP
        return true;
      }

      case CF_ARGB8888:
      {
        u32* ptr = (u32*)dst;
        START_BATCH_LOOP
          float fl_r = r[p];
          float fl_g = g[p];
          float fl_b = b[p];
          float fl_a = a[p];
          ClampColor(fl_r,fl_g,fl_b,fl_a);
          CONV_TO_SRGB(fl_r,fl_g,fl_b);
          i32  red    = (i32)(fl_r*255.0f + 0.5f);
          i32  green  = (i32)(fl_g*255.0f + 0.5f);
          i32  blue   = (i32)(fl_b*255.0f + 0.5f);
          i32  alpha  = (i32)(fl_a*255.0f + 0.5f);
          ptr[p] = ((alpha<<24) | (red<<16) | (green<<8) | (blue<<0));
        END_BATCH_LOOP
        return true;
      }
      case CF_ARGB1555:
      {
        u16* ptr = (u16*)dst;
        START_BATCH_LOOP
          float fl_r = r[p];
          float fl_g = g[p];
          float fl_b = b[p];
          float fl_a = a[p];
          ClampColor(fl_r,fl_g,fl_b,fl_a);
          CONV_TO_SRGB(fl_r,fl_g,fl_b);
          i32  red    = (i32)(fl_r*31.0f + 0.5f);
          i32  green  = (i32)(fl_g*31.0f + 0.5f);
          i32  blue   = (i32)(fl_b*31.0f + 0.5f);
          i32  alpha  = (i32)(fl_a+0.5);
          ptr[p] = ((alpha<<15) | (red<<10) | (green<<5) | (blue<<0));
        END_BATCH_LOOP
        return true;
      }
      case CF_A8:
      {
        START_BATCH_LOOP
          float fl_a = a[p];
          ClampColor(fl_a,fl_a,fl_a,fl_a);
          u32  alpha = (u32)(fl_a*255.0f + 0.5f);
          *((u8*)dst + p) = (u8)alpha;
        END_BATCH_LOOP
        return true;
      }

      case CF_AL88:
      {
        START_BATCH_LOOP
          float fl_r = r[p];
          float fl_g = g[p];
          float fl_b = b[p];
          float fl_a = a[p];
          ClampColor(fl_r,fl_g,fl_b,fl_a);
          CONV_TO_SRGB(fl_r,fl_g,fl_b);
          u32  alpha = (u32)(fl_a*255.0f + 0.5f);
          u32 gray = (u32)((0.212671f * fl_r + 0.715160f * fl_g + 0.072169f * fl_b)*255.0f + 0.5f);
          HELIUM_ASSERT(gray <= 255);
          *((u16*)dst + p) = ((u16)(alpha<<8))|(u8)gray;
        END_BATCH_LOOP
        return true;
      }
      case CF_L8:
      {
        START_BATCH_LOOP
          float fl_r = r[p];
          float fl_g = g[p];
          float fl_b = b[p];
          float fl_a = a[p];
          ClampColor(fl_r,fl_g,fl_b,fl_a);
          CONV_TO_SRGB(fl_r,fl_g,fl_b);
          i32 gray = (i32)((0.212671f * fl_r + 0.715160f * fl_g + 0.072169f * fl_b)*255.0f + 0.5f);
          HELIUM_ASSERT(gray <= 255);
          *((u8*)dst + p) = (u8)gray;
        END_BATCH_LOOP
        return true;
      }
      case CF_L16:
      {
        START_BATCH_LOOP
          float fl_r = r[p];
          float fl_g = g[p];
          float fl_b = b[p];
          float fl_a = a[p];
          ClampColor(fl_r,fl_g,fl_b,fl_a);
          i32 gray = (i32)((0.212671f * fl_r + 0.715160f * fl_g + 0.072169f * fl_b)*65535.0f + 0.5f);
          HELIUM_ASSERT(gray <= 65535);
          *((u16*)dst + p) = (u16)gray;
        END_BATCH_LOOP
        return true;
      }
    }
    return false;
  }

  //-----------------------------------------------------------------------------
  inline bool MakeColorFormatPixel(void* color,ColorFormat fmt, float r, float g, float b, float a, bool convert_to_srgb)
  {
    if (color==0)
    {
      return false;
    }

    return MakeColorFormatBatch(color, 1, fmt, &r, &g, &b, &a, convert_to_srgb);
  }

  //-----------------------------------------------------------------------------
  inline bool MakeHDRPixel(const void* color,ColorFormat fmt, float &r, float &g, float &b, float &a)
  {
    if (color==0)
      return false;

    switch(fmt)
    {
      case CF_RGBAFLOATMAP:
      {
        float* ptr = (float*)color;
        r = ptr[0];
        g = ptr[1];
        b = ptr[2];
        a = ptr[3];
        return true;
      }
      case CF_RGBAHALFMAP:
      {
        i16* ptr = (i16*)color;
        r = ::Math::HalfToFloat(ptr[0]);
        g = ::Math::HalfToFloat(ptr[1]);
        b = ::Math::HalfToFloat(ptr[2]);
        a = ::Math::HalfToFloat(ptr[3]);
        return true;
      }
      case CF_RGBE:
      {
        u32* ptr = (u32*)color;
        ColorFormatExpandRGBE(*ptr,r,g,b);
        a=1.0f;
        return true;
      }
      case CF_F32:
      {
        float* ptr = (float*)color;
        r = ptr[0];
        g = ptr[0];
        b = ptr[0];
        a = 1.0f;
        return true;
      }
      case CF_F32F32:
      {
        float* ptr = (float*)color;
        r = ptr[0];
        g = ptr[1];
        b = 0.0f;
        a = 1.0f;
        return true;
      }
      case CF_F16:
      {
        i16* ptr = (i16*)color;
        r = g = b = ::Math::HalfToFloat(ptr[0]);
        a = 1.0f;
        return true;
      }
      case CF_F16F16:
      {
        i16* ptr = (i16*)color;
        r = ::Math::HalfToFloat(ptr[0]);
        g = ::Math::HalfToFloat(ptr[1]);
        b = 0.0f;
        a = 1.0f;
        return true;
      }

      case CF_ARGB8888:
      {
        u32  val = *(u32*)color;
        a = ((float)((val & 0xff000000)>>24))/255.0f;
        r = ((float)((val & 0x00ff0000)>>16))/255.0f;
        g = ((float)((val & 0x0000ff00)>>8))/255.0f;
        b = ((float)((val & 0x000000ff)))/255.0f;
        return true;
      }
      case CF_ARGB1555:
      {
        u16  val = *(u16*)color;
        a = ((float)((val & 0x8000)>>15));
        r = ((float)((val & 0x7c00)>>10))/31.0f;
        g = ((float)((val & 0x03e0)>>5))/31.0f;
        b = ((float)((val & 0x001f)))/31.0f;
        return true;
      }
      case CF_ARGB4444:
      {
        u16  val = *(u16*)color;
        a = ((float)((val & 0xf000)>>12))/15.0f;
        r = ((float)((val & 0x0f00)>>8))/15.0f;
        g = ((float)((val & 0x00f0)>>4))/15.0f;
        b = ((float)((val & 0x000f)))/15.0f;
        return true;
      }
      case CF_RGB565:
      {
        u16  val = *(u16*)color;
        a = 1.0f;
        r = ((float)((val & 0xf800)>>11))/31.0f;
        g = ((float)((val & 0x07E0)>>5))/63.0f;
        b = ((float)((val & 0x001f)))/31.0f;
        return true;
      }
      case CF_A8:
      {
        u8 val = *(u8*)color;
        r=g=b = 0.0f;
        a = ((float)(val))/255.0f;
        return true;
      }
      case CF_AL88:
      {
        u16 val = *(u16*)color;
        r=g=b = ((float)(val&0xff))/255.0f;
        a = ((float)((val>>8)&0xff))/255.0f;
        return true;
      }
      case CF_L8:
      {
        u8  val = *(u8*)color;
        r=g=b = ((float)(val))/255.0f;
        a=1.0f;
        return true;
      }
      case CF_L16:
      {
        u16  val = *(u16*)color;
        r=g=b = ((float)(val))/65535.0f;
        a = 1.0f;
        return true;
      }
    }
    return false;
  }

  //-----------------------------------------------------------------------------
  // Given CF_ARGB8888 color, return a normalized version, 255 is put in the alpha channel.
  inline u32 NormalizeARGBNormal(u32 const argb8888)
  {
    float red   = float((argb8888 & 0x00ff0000)>>16);
    float green = float((argb8888 & 0x0000ff00)>>8);
    float blue  = float((argb8888 & 0x000000ff)>>0);

    // this computation can never return zero vector so it is always safe to normalize
    red   = (red   / 255.f) * 2.f - 1.f;
    green = (green / 255.f) * 2.f - 1.f;
    blue  = (blue  / 255.f) * 2.f - 1.f;

    ::Math::Vector3 normal(red, green, blue);
    normal.Normalize();

    // convert range 0 -> 1
    normal *= ::Math::Vector3(0.5f);
    normal += ::Math::Vector3(0.5f);

    u32 result;

    MakeColorFormatPixel(&result, CF_ARGB8888, normal.x, normal.y, normal.z, 1.f, false);

    return result;
  }

  //-----------------------------------------------------------------------------
  // Given a ColorFormat, return whether or not it contains alpha
  inline bool FormatHasAlpha( ColorFormat format )
  {
    switch(format)
    {
    case CF_RGBAFLOATMAP:
    case CF_RGBAHALFMAP:
    case CF_ARGB8888:
    case CF_ARGB1555:
    case CF_ARGB4444:
    case CF_A8:
    case CF_AL88:
      {
        return true;
      }

    case CF_RGBE:
    case CF_F32:
    case CF_F32F32:
    case CF_F16:
    case CF_F16F16:
    case CF_RGB565:
    case CF_L8:
    case CF_L16:
      {
        return false;
      }
    }

    HELIUM_BREAK();

    return true;
  }
}