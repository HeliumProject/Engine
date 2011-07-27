////////////////////////////////////////////////////////////////////////////////////////////////
//
//  swizzle.h
//
//  Written by: Rob Wyatt
//
//  General swizzle code for any bit depth, this can be made much faster but as this it tools code
//  its not important. If we need to swizzle on the console then this code should NOT be used, it
//  is very hard on the cache.
//
////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Platform/Types.h"

#include "Image/API.h"
#include "Image/ColorFormats.h"

namespace Helium
{
  ////////////////////////////////////////////////////////////////////////////////////////////////
  //   Most of the time when messing with a texture, you will be incrementing
  //   by a constant value in each dimension.  Those deltas can be converted
  //   to an intermediate value via the SwizzleXXX(num) methods which can be
  //   used to quickly increment a dimension.
  //
  //  The 2d swizzling/unswizzling code grabs a rectangular block of optimized size from the texture, 
  //  rearranges that, moves it to the destination, then repeats with the next block.
  //
  //  swizzling,                  8-bit or 16-bit
  //  linear:                     swizzled:
  //  00 01 02 03 04 05 06 07     
  //  10 11 12 13 14 15 16 17     
  //  20 21 22 23 24 25 26 27     00 01 10 11 02 03 12 13 20 21 30 31 22 23 32 33 \
  //  30 31 32 33 34 35 36 37 =>  04 05 14 15 06 07 16 17 24 25 34 35 26 27 36 37 \
  //  40 41 42 43 44 45 46 47     40 41 50 51 42 43 52 53 60 61 70 71 62 63 72 73 \
  //  50 51 52 53 54 55 56 57     44 45 54 55 46 47 56 57 64 65 74 75 66 67 76 77   
  //  60 61 62 63 64 65 66 67     
  //  70 71 72 73 74 75 76 77     
  //
  //  swizzling,                  32-bit
  //  linear:                     swizzled:
  //  00 01 02 03 04 05 06 07     
  //  10 11 12 13 14 15 16 17 =>  00 01 10 11 02 03 12 13 20 21 30 31 22 23 32 33 \
  //  20 21 22 23 24 25 26 27     04 05 14 15 06 07 16 17 24 25 34 35 26 27 36 37  
  //  30 31 32 33 34 35 36 37
  //
  //  unswizzling, 8-bit or 16-bit
  //  swizzled:                                             linear:
  //  00 01 10 11 02 03 12 13 20 21 30 31 22 21 32 31 \     00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f
  //  04 05 14 15 06 07 16 17 24 25 34 35 26 27 36 37... => 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f
  //  08 09 18 19 0a 0b 1a 1b 28 29 38 39 2a 2b 3a 3b \     20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f
  //  0c 0d 1c 1d 0e 0f 1e 1f 2c 2d 3c 3d 2e 2f 3e 3f       30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f
  //
  //  unswizzling, 32-bit
  //  swizzled:                                             linear:
  //                                                        00 01 02 03 04 05 06 07
  //  00 01 10 11 02 03 12 13 20 21 30 31 22 23 32 33 \  => 10 11 12 13 14 15 16 17
  //  04 05 14 15 06 07 16 17 24 25 34 35 26 27 36 37       20 21 22 23 24 25 26 27
  //                                                        30 31 32 33 34 35 36 37
  //
  //  The algorithm moves through the linear texture left->right, top->bottom,
  //  which means the swizzled array gets 64 or 128-byte blocks written to it in a
  //  seemingly random order.
  //
  //////////////////////////////////////////////////////////////////////////////////////////////// 
  class HELIUM_IMAGE_API Swizzler 
  {
  public:
    // Dimensions of the texture
    uint32_t m_width;                      //width of the texture this instance of the class has been initialized for
    uint32_t m_height;                     //height of the texture this instance of the class has been initialized for
    uint32_t m_depth;                      //depth of the texture this instance of the class has been initialized for

    // Internal mask for each coordinate
    uint32_t m_masku;                      //internal mask for u coordinate
    uint32_t m_maskv;                      //internal mask for v coordinate
    uint32_t m_maskw;                      //internal mask for w coordinate

    // Swizzled texture coordinates
    uint32_t m_u;                          //texture map (converted) u coordinate
    uint32_t m_v;                          //texture map (converted) v coordinate
    uint32_t m_w;                          //texture map (converted) w coordinate

    ////////////////////////////////////////////////////////////////////////////////////////////////
    Swizzler(): m_width(0), m_height(0), m_depth(0),
      m_masku(0), m_maskv(0), m_maskw(0),
      m_u(0), m_v(0), m_w(0)
    {
    
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Initializes the swizzler
    Swizzler(uint32_t width, uint32_t height, uint32_t depth)
    { 
      Init(width, height, depth);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    void Init(uint32_t width, uint32_t height, uint32_t depth)
    {
      m_width = width; 
      m_height = height; 
      m_depth = depth;
      m_masku = 0;
      m_maskv = 0;
      m_maskw = 0;
      m_u = 0;
      m_v = 0;
      m_w = 0;

      uint32_t i = 1;
      uint32_t j = 1;
      uint32_t k;

      do 
      {
        k = 0;
        if (i < width)   
        { 
          m_masku |= j;   
          k = (j<<=1);  
        }

        if (i < height)  
        { 
          m_maskv |= j;   
          k = (j<<=1);  
        }

        if (i < depth)   
        {
          m_maskw |= j;   
          k = (j<<=1);  
        }

        i <<= 1;
      } 
      while (k);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // converts num to an intermediate value that can be used to modify the u coordinate
    uint32_t SwizzleU(uint32_t num)
    {
      uint32_t r = 0;

      for (uint32_t i = 1; i <= m_masku; i <<= 1) 
      {
        if (m_masku & i) 
        {
          r |= (num & i);
        }
        else
        {
          num <<= 1;
        }
      }

      return r;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // converts num to an intermediate value that can be used to modify the v coordinate
    uint32_t SwizzleV(uint32_t num)
    {
      uint32_t r = 0;

      for (uint32_t i = 1; i <= m_maskv; i <<= 1) 
      {
        if (m_maskv & i)
        {
          r |= (num & i);
        }
        else
        {
          num <<= 1;
        }
      }

      return r;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // converts num to an intermediate value that can be used to modify the w coordinate
    uint32_t SwizzleW(uint32_t num)
    {
      uint32_t r = 0;

      for (uint32_t i = 1; i <= m_maskw; i <<= 1) 
      {
        if (m_maskw & i)
        {
          r |= (num & i);
        }
        else
        {
          num <<= 1;
        }
      }

      return r;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // converts u,v,w to an intermediate value that can be used to modify all the coordinates
    uint32_t Swizzle(uint32_t u, uint32_t v, uint32_t w)
    {
      return SwizzleU(u) | SwizzleV(v) | SwizzleW(w);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //takes an index to the swizzled texture,  and extracts & returns the u coordinate
    uint32_t UnswizzleU(uint32_t num)
    {
      uint32_t r = 0;
      for (uint32_t i = 1, j = 1; i; i <<= 1) 
      {
        if (m_masku & i)  
        {   
          r |= (num & j);   
          j <<= 1; 
        } 
        else         
        {   
          num >>= 1; 
        }
      }
      return r;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //takes an index to the swizzled texture,  and extracts & returns the v coordinate
    uint32_t UnswizzleV(uint32_t num)
    {
      uint32_t r = 0;

      for (uint32_t i = 1, j = 1; i; i <<= 1) 
      {
        if (m_maskv & i)  
        {   
          r |= (num & j);   
          j <<= 1; 
        } 
        else
        {   
          num >>= 1; 
        }
      }

      return r;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //takes an index to the swizzled texture,  and extracts & returns the w coordinate
    uint32_t UnswizzleW(uint32_t num)
    {
      uint32_t r = 0;

      for (uint32_t i = 1, j = 1; i; i <<= 1) 
      {
        if (m_maskw & i)  
        {   
          r |= (num & j);   
          j <<= 1; 
        } 
        else         
        {   
          num >>= 1; 
        }
      }

      return r;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Sets a texture coordinate
    __forceinline uint32_t SetU(uint32_t num) { return m_u = num  & m_masku ; }
    __forceinline uint32_t SetV(uint32_t num) { return m_v = num  & m_maskv ; }
    __forceinline uint32_t SetW(uint32_t num) { return m_w = num  & m_maskw ; }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Adds a value to a texture coordinate
    __forceinline uint32_t AddU(uint32_t num) { return m_u = ( m_u - ( (0-num) & m_masku ) ) & m_masku; }
    __forceinline uint32_t AddV(uint32_t num) { return m_v = ( m_v - ( (0-num) & m_maskv ) ) & m_maskv; }
    __forceinline uint32_t AddW(uint32_t num) { return m_w = ( m_w - ( (0-num) & m_maskw ) ) & m_maskw; }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Subtracts a value from a texture coordinate
    __forceinline uint32_t SubU(uint32_t num) { return m_u = ( m_u - num & m_masku ) & m_masku; }
    __forceinline uint32_t SubV(uint32_t num) { return m_v = ( m_v - num & m_maskv ) & m_maskv; }
    __forceinline uint32_t SubW(uint32_t num) { return m_w = ( m_w - num & m_maskw ) & m_maskw; }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Increments a texture coordinate
    __forceinline uint32_t IncU()        { return m_u = ( m_u - m_masku ) & m_masku; }
    __forceinline uint32_t IncV()        { return m_v = ( m_v - m_maskv ) & m_maskv; }
    __forceinline uint32_t IncW()        { return m_w = ( m_w - m_maskw ) & m_maskw; }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Decrements a texture coordinate
    __forceinline uint32_t DecU()        { return m_u = ( m_u - 1 ) & m_masku; }
    __forceinline uint32_t DecV()        { return m_v = ( m_v - 1 ) & m_maskv; }
    __forceinline uint32_t DecW()        { return m_w = ( m_w - 1 ) & m_maskw; }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Gets the current swizzled address for a 2D or 3D texture
    __forceinline uint32_t Get2D()      { return m_u | m_v; }
    __forceinline uint32_t Get3D()      { return m_u | m_v | m_w; }
  };

  ////////////////////////////////////////////////////////////////////////////////////////////////
  HELIUM_IMAGE_API void UnswizzleBox
    (
    uint8_t*       src,          // The source buffer (swizzled data)
    uint8_t*       dst,          // The destination buffer
    uint32_t       Width,        // The width of the entire texture
    uint32_t       Height,       // The height of the entire texture (1 for 1D)
    uint32_t       Depth,        // The depth of the entire texture (1 for 2D)
    uint32_t       BytesPerPixel // Bytes per pixel of the texture format
    );

  ////////////////////////////////////////////////////////////////////////////////////////////////
  HELIUM_IMAGE_API void SwizzleBox
    (
    uint8_t*         src,          // The source buffer (linear data)
    uint8_t*         dst,          // The destination buffer (swizzled data)
    uint32_t         Width,        // The width of the entire texture
    uint32_t         Height,       // The height of the entire texture (1 for 1D)
    uint32_t         Depth,        // The depth of the entire texture (1 for 2D)
    uint32_t         BytesPerPixel // Bytes per pixel of the texture format
    );
}

