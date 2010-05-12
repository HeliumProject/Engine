#pragma once

#include <math.h>
#include <string>
#include <vector>
#include <list>

#include "Common/Types.h"

#include "API.h"
#include "MipSet.h"

namespace IG
{
  ////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // Texture()
  //
  // Create a 2D texture in the specified format
  //
  ////////////////////////////////////////////////////////////////////////////////////////////////
  class TEXTURE_API Texture
  {
  public:
    enum TextureType
    {
      REGULAR,
      CUBE,
      VOLUME,
    };

    enum TextureChannels
    {
      R,
      G,
      B,
      A,
      NUM_TEXTURE_CHANNELS,
    };

    enum RawFormats
    {
      RGBFLOAT,    //loads as CF_RGBAFLOATMAP, alpha is set to 1.0
      RGBAFLOAT,   //loads as CF_RGBAFLOATMAP
    };

    enum TwoDTextureValues
    {
      TWO_D_DEPTH     = 1,
    };

    enum CubeTextureValues
    {
      CUBE_DEPTH      = 0,
      CUBE_NUM_FACES  = 6,
    };

    enum VolumeTextureValues
    {
      VOLUME_MAX_DEPTH = 512, // Max depth allowed for a volume texture

      VOLUME_NUM_IDENTIFIERS = 2,
    };
    static char* p_volume_identifier_strings[VOLUME_NUM_IDENTIFIERS];

    struct LoadRAWInfo
    {
      u32  m_RawFormat;
      u32  m_Width;
      u32  m_Height;
      bool m_FlipVertical;
    };

  public:
    ColorFormat  m_NativeFormat;      // Native format of surface
    u32          m_Width;             // Width of surface
    u32          m_Height;            // Height of surface
    u32          m_Depth;             // Depth of surface (1 for 2D and 0 for cube maps, N for volume maps)
    u32          m_DataSize;          // Size in bytes of the textureData (all faces are the same size)

    f32*         m_Channels[CUBE_NUM_FACES][NUM_TEXTURE_CHANNELS];    // Texture data bits (2D and volume maps only use entry 0, cube maps use all CUBE_NUM_FACES)

  public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // Texture()
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    Texture();

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // Texture()
    //
    // Create a 2D texture in the specified format
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    Texture(u32 width, u32 height, ColorFormat native_fmt);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // Texture()
    //
    // Create a 2D, Volume or Cube texture in the specified format.
    // For a 2D texture specify depth as 1
    // For a cube texture specify depth as 0
    // For a volume texture specify depth as requried
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    Texture(u32 width, u32 height, u32 depth, ColorFormat native_fmt);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // ~Texture()
    //
    // Destroy the texure and release any resources
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    ~Texture();


    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // FillFaceData()
    //
    // converts the data in in_data from fmt to float and places it in face
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void  FillFaceData(u32 face, ColorFormat fmt, const void* in_data);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // Clone()
    //
    // Clone an exact copy of the texture
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    Texture* Clone() const;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // GenerateFormatData()
    //
    // Allocates data for the new format and does the conversion.
    // The user is responsible for cleaning up the data he acquired
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    static  u8* GenerateFormatData(const Texture* src_tex, ColorFormat data_format, u32 face, bool convert_to_srgb);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // CloneFace()
    //
    // For a 2D texture this is identical to the above call, however for volume and cube textures
    // a single face/slice is extracted and returned as a 2D texture
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    Texture* CloneFace(u32 face=0) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // InsertFace()
    //
    // Inserts the specified texture in the volume/cube map at the specified face position. The
    // face being inserted has to be 2D texture and has to be the same width and height as the
    // destination.
    // This function can convert a 2D texture into a cube map
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool InsertFace(Texture* tex,u32 face);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // WriteTGA()
    //
    // Write a 32bit TGA, if the map is a volume or cube map then the face parameter is used to determine
    // which face (cubemap) or slice (volume map) is written.
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool WriteTGA(const char* fname, u32 face, bool convert_to_srgb) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // WriteRAW()
    //
    // Writes raw data for a given face with a simple header(u32 width,u32 height, u32 format)
    // which face (cubemap) or slice (volume map) is written.
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool WriteRAW(const char* fname, void* data, u32 size, u32 face, bool convert_to_srgb) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // WriteDDS()
    //
    // Write the texture data in the DDS format specified in output_fmt.
    // If output_fmt is not specified, the function will pick the best OUTPUT_CF match
    // for m_NativeFormat
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool WriteDDS(const char* fname, bool convert_to_srgb, OutputColorFormat output_fmt = OUTPUT_CF_UNKNOWN) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // ConvertGrayScale()
    //
    // Convert a Texture to gray scale, the format of the surface is not changed and
    // the alpha values if present are not touched.
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void ConvertGrayScale();

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // FlipVertical()
    //
    // turns the image upside down in place, works directly on the texture memory
    // and does not care about format.
    // For a volume or cube map the face parameter specifies the slice or cube face
    // to flip
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void FlipVertical(u32 face=0);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // FlipHorizontal()
    //
    // Flips horizontally in place
    // For a volume or cube map the face parameter specifies the slice or cube face
    // to flip
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void FlipHorizontal(u32 face=0);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // CleanFloatData()
    //
    // Some packages write out NANs for unused pixels in HDR images, this function
    // will go through and replace any non valid floating point numbers with 0.0f
    // This is automatically called when a float format is loaded.
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void CleanFloatData();


    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void InvertColors(u32 face = 0);
    void DoubleContrast(u32 face = 0);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool BlendImageFace(const Texture* blend_source, float blend_strength, bool* channel_masks = NULL, u32 src_face = 0, u32 dst_face = 0);
    bool OverlayBlendImageFace(const Texture* base_source, const Texture* overlay_source, const bool* channel_masks = NULL, u32 face = 0, u32 base_face = 0, u32 overlay_face = 0);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void  HighPassFilterImage(const bool* channel_mask, u32 face, UVAddressMode u_wrap_mode, UVAddressMode v_wrap_mode);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    Texture*  FilterImageFace(const PostMipImageFilter *filters, u32 face, u32 mip_index = 0) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // ScaleImage()
    //
    // NOTE: This alway clones the source texture. The source texture is left intact.
    //       The filter is applied to all the color channels.
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    Texture* ScaleImage(u32           width,
                        u32           height, 
                        ColorFormat   new_format, 
                        FilterType    filter, 
                        UVAddressMode u_wrap_mode = UV_CLAMP,
                        UVAddressMode v_wrap_mode = UV_CLAMP) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // ScaleImage()
    //
    // NOTE: This alway clones the source texture. The source texture is left intact.
    //       The first filter is applied to the red channel.
    //       The second filter is applied to the green channel.
    //       The third filter is applied to the blue channel.
    //       The fourth filter is applied to the alpha channel.
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    Texture* ScaleImage(u32               width, 
                        u32               height, 
                        ColorFormat       new_format, 
                        const FilterType* filters,
                        UVAddressMode     u_wrap_mode = UV_CLAMP,
                        UVAddressMode     v_wrap_mode = UV_CLAMP) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // ScaleImageFace()
    //
    // NOTE: This alway clones the surface at index <face> from source texture. The source texture is left intact.
    //       The filter is applied to all the color channels.
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    Texture* ScaleImageFace(u32           width, 
                            u32           height, 
                            u32           face, 
                            ColorFormat   new_format,
                            FilterType    filter,
                            UVAddressMode u_wrap_mode = UV_CLAMP,
                            UVAddressMode v_wrap_mode = UV_CLAMP) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // ScaleImageFace()
    //
    // NOTE: This alway clones the surface at index <face> from source texture. The source texture is left intact.
    //       The first filter is applied to the red channel.
    //       The second filter is applied to the green channel.
    //       The third filter is applied to the blue channel.
    //       The fourth filter is applied to the alpha channel.
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    Texture* ScaleImageFace(u32               width, 
                            u32               height, 
                            u32               face, 
                            ColorFormat       new_format, 
                            const FilterType* filters,                      
                            UVAddressMode     u_wrap_mode = UV_CLAMP,
                            UVAddressMode     v_wrap_mode = UV_CLAMP) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // RelativeScaleImage()
    //
    // NOTE: This alway clones the source texture. If the image is not ARGB8888 or RGBAFLOATMAP,
    //       the clone is converted. The result is then scaled. The source texture is left intact.
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    Texture* RelativeScaleImage(float scale_x, float scale_y, ColorFormat new_format, FilterType filter) const
    {
      // scale by the amount specified and round to the nearest pixel
      u32 new_x = (u32)(((float)m_Width*scale_x)+0.5f);
      u32 new_y = (u32)(((float)m_Height*scale_y)+0.5f);
      return ScaleImage(new_x, new_y, new_format, filter);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // ScaleUpNextPowerOfTwo()
    //
    // Scale up to the next power of 2, the scale is calculated independently for X and Y.
    // If either are already a power of 2 they are not modified.
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    Texture* ScaleUpNextPowerOfTwo(ColorFormat new_format, FilterType filter)
    {
      u32 new_x = ::Math::NextPowerOfTwo(m_Width);
      u32 new_y = ::Math::NextPowerOfTwo(m_Height);
      return ScaleImage(new_x, new_y, new_format, filter);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // ScaleDownNextPowerOfTwo()
    //
    // Scale down to the previous power of 2, the scale is calculated independently for X and Y.
    // If either are already a power of 2 they are not modified.
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    Texture* ScaleDownNextPowerOfTwo(ColorFormat new_format, FilterType filter)
    {
      u32 new_x = ::Math::PreviousPowerOfTwo(m_Width);
      u32 new_y = ::Math::PreviousPowerOfTwo(m_Height);
      return ScaleImage(new_x, new_y, new_format, filter);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // AdjustToNextPowerOf2()
    //
    // This will pad the texture to a power of two, the padding is full of black
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    Texture* AdjustToNextPowerOf2() const;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // AdjustExposure()
    //
    // Adjusts the exposure of the image by the specified number of camera f-stops.
    // NOTE: ONLY WORKS FOR FLOATING POINT TEXTURES
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool AdjustExposure(float fstop);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // AdjustGamma()
    //
    // Adjust the gamma of the specified image
    // NOTE: ONLY WORKS FOR FLOATING POINT TEXTURES
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool AdjustGamma(float gamma);

    ////////////////////////////////////////////////////////////////////////////////////////////////
  	//
  	// 	Function which converts from perceptually linear space to physically linear space:
  	//
  	//  f(x)                    f(x)
  	//	    |      /                |     /
  	//      |    /                  |    /
  	//      |  /                    |   /
  	//      | /            -->      |  /
  	//      |/                      | /
  	//      |/_______               |/_______
  	//				 x                       x
  	//
  	//	or
  	//
  	//  f(x)                    f(x)
  	//	    |     /                 |    /
  	//      |    /                  |    /
  	//      |   /                   |    /
  	//      |  /            -->     |   /
  	//      | /                     |  /
  	//      |/_______               |/_______
  	//				 x                       x
  	//
  	//	-	takes the input and makes it darker
  	//	-	also called gamma expansion or gamma decoding
  	//
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void ConvertSrgbToLinear();

    ////////////////////////////////////////////////////////////////////////////////////////////////
  	//
  	// Functions which convert from physically linear space to perceptually linear space:
  	//
  	//
  	//  f(x)                    f(x)
  	//	    |     /                 |      /
  	//      |    /                  |    /
  	//      |   /                   |  /
  	//      |  /            -->     | /
  	//      | /                     |/
  	//      |/_______               |/_______
  	//				 x                       x
  	//
  	//	or
  	//
  	//  f(x)                    f(x)
  	//	    |    /                  |     /
  	//      |    /                  |    /
  	//      |    /                  |   /
  	//      |   /            -->    |  /
  	//      |  /                    | /
  	//      |/_______               |/_______
  	//				 x                       x
  	//
  	//	-	takes the input and makes it brighter, this is the function which converts a 50%
  	//		intensity to 186 in grey scale
  	//	- allocates more bits to darker values which is exactly what we want on low precision devices
  	//	-	also called gamma compression, gamma encoding, or gamma correction
  	//
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void ConvertLinearToSrgb();

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // Read()
    //
    // Read the 2D position as an HDR pixel, for a cube or volume map the face parameter specifies
    // which face or slice to read from.
    //
    // Returns a HDR floating point color (regardless of format) of the pixel at
    // the specified location
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void Read(u32 x,u32 y, float& r, float& g, float& b, float& a, u32 face=0) const
    {
      NOC_ASSERT(m_Channels[face][R]);
      u32 pixel_idx = y*m_Width + x;
      r = m_Channels[face][R][pixel_idx];
      g = m_Channels[face][G][pixel_idx];
      b = m_Channels[face][B][pixel_idx];
      a = m_Channels[face][A][pixel_idx];
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // Write()
    //
    // Writes a HDR floating point color to the pixel (unless palettized) at
    // the specified 2D location. For cube and volume maps the face parameter specifies which face
    // to write.
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool Write(u32 x,u32 y, float r, float g, float b, float a, u32 face=0)
    {
      NOC_ASSERT(m_Channels[face][R]);
      u32 pixel_idx = y*m_Width + x;
      m_Channels[face][R][pixel_idx] = r;
      m_Channels[face][G][pixel_idx] = g;
      m_Channels[face][B][pixel_idx] = b;
      m_Channels[face][A][pixel_idx] = a;
      return true;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // PrepareFor2ChannelNormalMap()
    //
    // Used to convert ARGB data into a format suitable for creating switched green/alpha 2-channel
    // type normal maps.
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void PrepareFor2ChannelNormalMap(bool is_detail_map, bool is_detail_map_only = false);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Returns true if any pixel satisfies the < or > check against the threshold value
    // (based off of ShaderGenerator::IsTexChannelDataValid)
    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsChannelDataSet( u32 face, u32 channel, f32 threshold_value, bool valid_if_greater ) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // GenerateMipSet()
    //
    // Generate the mipset for this texture.
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    MipSet* GenerateMipSet(const MipGenOptions& options, const MipSet::RuntimeSettings& runtime) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // GenerateMipSet()
    //
    // Generate the mipset for this texture.
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    MipSet* GenerateMipSet(const MipGenOptions** options_rgba, const MipSet::RuntimeSettings& runtime) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // GetFacePtr()
    //
    // Given a face/sclice number return a pointer to the 2D surface.
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////

    f32* GetFacePtr(u32 face, u32 channel) const
    {
      f32* texture_data;

      if (m_Depth==0)
      {
        // cube map, face is the face to use
        if (face>=CUBE_NUM_FACES)
        {
          return 0;
        }
        texture_data = m_Channels[face][channel];
      }
      else
      {
        // 2D or volume map, either cause use texture 0 and calculate the slice address
        // from the face
        if (face>=m_Depth)
        {
          return 0;
        }
        texture_data = m_Channels[0][channel] + (m_Width*m_Height*face);
      }

      return texture_data;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // Type()
    //
    // Get the type of the texture, either REGULAR, CUBE, VOLUME
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    TextureType Type() const
    {
      if (m_Depth==CUBE_DEPTH)
      {
        return CUBE;
      }
      else if (m_Depth==TWO_D_DEPTH)
      {
        return REGULAR;
      }

      return VOLUME;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Returns true if this is a 2D texture
    inline bool Is2DTexture( void ) const 
    {
      return (m_Depth == TWO_D_DEPTH);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Returns true if this is a cube map texture
    inline bool IsCubeMapTexture( void ) const 
    {
      return (m_Depth == CUBE_DEPTH);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Returns true if this is a volume texture
    inline bool IsVolumeTexture( void ) const 
    {
      return (m_Depth > TWO_D_DEPTH);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // Sample2D()
    //
    // Bilinearly sample a texture and return a floar color.
    // The texture is assumed to be wrapped in both directions
    // The pixel centers are at 0.5
    // Depending on the SAMPLE_NORMALIZED flag the UVs are either normalized or not
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    static const u32 SAMPLE_NORMALIZED = 0x00000001;
    void Sample2D(float u, float v, float& r, float& g, float& b, float& a, u32 face, u32 flags=0) const;

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // GenerateMultiChannelSettingMipSet()
    //
    // - Where all different channel mips are generated differently and combined in the end for compression
    //   (output texture:normal map should never call this)
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    MipSet* GenerateMultiChannelSettingMipSet(const TextureGenerationSettings&  settings, 
                                              const MipSet::RuntimeSettings&    runtime_settings);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // GenerateFinalizedMipSet()
    //
    // - applies settings defined by TextureGenerationSettings
    // - scales texture to a power of 2
    // - compresses normal maps
    // - generates mips set
    // - it is the clients responsibility to delete resultant mip set
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    MipSet* GenerateFinalizedMipSet(const TextureGenerationSettings&  settings, 
                                    const MipSet::RuntimeSettings&    runtime_settings, 
                                    const bool                        is_normal_map,
                                    const bool                        is_detail_map );

  private:
    inline  void  FixUpChannels(u32 face)
    {
      if(m_Depth == 0)
      {
        NOC_ASSERT(face < CUBE_NUM_FACES);
        if(m_Channels[face][R])
        {
          u32 channel_f32_size  = m_Width*m_Height;
          m_Channels[face][G]   = m_Channels[face][R] + channel_f32_size;
          m_Channels[face][B]   = m_Channels[face][G] + channel_f32_size;
          m_Channels[face][A]   = m_Channels[face][B] + channel_f32_size;
        }
        else
        {
          m_Channels[face][G]   = NULL;
          m_Channels[face][B]   = NULL;
          m_Channels[face][A]   = NULL;
         }
      }
      else
      {
        NOC_ASSERT(m_Channels[0][R]);

        u32 channel_f32_size    = m_Width*m_Height*m_Depth;
        m_Channels[0][G]        = m_Channels[0][R] + channel_f32_size;
        m_Channels[0][B]        = m_Channels[0][G] + channel_f32_size;
        m_Channels[0][A]        = m_Channels[0][B] + channel_f32_size;
      }
    }

    void  FilterImage(const PostMipImageFilter*  filters,
                      const Texture*             src,
                      u32                        dst_face,
                      u32                        src_face,
                      u32                        mip_index = 0);

  public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // File loading code, most of the loaders load from a memory address and it is expected that
    // the caller or LoadFile() untility function has already loaded the file into memory. However,
    // for the file formats that external libraries this is not always possible and in these cases
    // file can only be loaded from filename
    //
    // Data contained in this class is always assumed to be linear. If you know that the data on
    // file has already been gamma corrected then set convert_to_linear to true when loading.
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Load BMP as a 2D image
    ////////////////////////////////////////////////////////////////////////////////////////////////
    static Texture* LoadBMP(const void* bmpadr, bool convert_to_linear);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Load TGA as a 2D image
    ////////////////////////////////////////////////////////////////////////////////////////////////
    static Texture* LoadTGA(const void* tgaadr, bool convert_to_linear);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Load JPEG as a 2D image
    // NOTE: JPEG files that are not standard color files cannot be loaded.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    static Texture* LoadJPG(const void* jpgadr, bool convert_to_linear);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Load PFM as a 2D image, PFM data is assumed to always be linear
    ////////////////////////////////////////////////////////////////////////////////////////////////
    static Texture* LoadPFM(const void* pfmadr);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Load HDR as a 2D image, HDR data is assumed to always be linear
    ////////////////////////////////////////////////////////////////////////////////////////////////
    static Texture* LoadHDR(const void* hdradr);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Loads RAW data as a texture, only certain raw formats are supported and a
    // specific raw format gets converted to a supported color format. See the
    // RawFormats enum above for how formats get converted.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    static Texture* LoadRAW(const void* rawadr, bool convert_to_linear, LoadRAWInfo* inf);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Load a DDS file, can be a cube or volume map however this texture class only holds the top
    // level therefore mips are ignored.
    //
    // Only formats support by the texture class are load (ie not compressed), if you want to support any format
    // then load the DDS directly to a MipSet, however doing this prevents any further processing
    // of the image.
    //
    // To direclty load a DDS as output data see the LoadMipSet() and LoadMipSetDDS() functions
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    static Texture* LoadDDS(const void* ddsadr, bool convert_to_linear);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Load a TIFF file as a 2D image
    // NOTE: TIFF files cannot currently be loaded from memory
    ////////////////////////////////////////////////////////////////////////////////////////////////
    static Texture* LoadTIFF(const char* filename, bool convert_to_linear);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Load a file based on file extension, the returned texture could be 2D, volume or cube depending
    // on what was loaded.
    // If the file name begins with "ANIM_", a volume texture will be constructed from the image
    // files in the sibling subfolder with the same name as the file without the extension.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    static Texture* LoadFile(const char* p_path, bool convert_to_linear, LoadRAWInfo* raw_info);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // LoadFile without the checking for "ANIM_" volume textures.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    static Texture* LoadSingleFile(const char* filename, bool convert_to_linear, LoadRAWInfo* raw_info);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // LoadDDSToMipSet()
    //
    // Loads Direct3D .dds files to directly to a mip set, all the mips are loaded as is along with
    // additional faces and layers. This function can load DDS files with formats not supported by
    // the image code, for example DXTC formats or YUV formats.
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    static MipSet* LoadDDSToMipSet(const void* ddsadr);
  };
}

