#pragma once

#include <string>
#include <vector>
#include <list>

#include "Platform/Types.h"
#include "Math/Common.h"

#include "Image/API.h"
#include "Image/MipSet.h"

namespace Helium
{
  ////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // Image()
  //
  // Create a 2D texture in the specified format
  //
  ////////////////////////////////////////////////////////////////////////////////////////////////
  class HELIUM_IMAGE_API Image
  {
  public:
    enum ImageType
    {
      REGULAR,
      CUBE,
      VOLUME,
    };

    enum ImageChannels
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

    enum TwoDImageValues
    {
      TWO_D_DEPTH     = 1,
    };

    enum CubeImageValues
    {
      CUBE_DEPTH      = 0,
      CUBE_NUM_FACES  = 6,
    };

    enum VolumeImageValues
    {
      VOLUME_MAX_DEPTH = 512, // Max depth allowed for a volume texture

      VOLUME_NUM_IDENTIFIERS = 2,
    };
    static tchar_t* p_volume_identifier_strings[VOLUME_NUM_IDENTIFIERS];

    struct LoadRAWInfo
    {
      uint32_t  m_RawFormat;
      uint32_t  m_Width;
      uint32_t  m_Height;
      bool m_FlipVertical;
    };

  public:
    ColorFormat  m_NativeFormat;      // Native format of surface
    uint32_t          m_Width;             // Width of surface
    uint32_t          m_Height;            // Height of surface
    uint32_t          m_Depth;             // Depth of surface (1 for 2D and 0 for cube maps, N for volume maps)
    uint32_t          m_DataSize;          // Size in bytes of the textureData (all faces are the same size)

    float32_t*         m_Channels[CUBE_NUM_FACES][NUM_TEXTURE_CHANNELS];    // Image data bits (2D and volume maps only use entry 0, cube maps use all CUBE_NUM_FACES)

  public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // Image()
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    Image();

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // Image()
    //
    // Create a 2D texture in the specified format
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    Image(uint32_t width, uint32_t height, ColorFormat native_fmt);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // Image()
    //
    // Create a 2D, Volume or Cube texture in the specified format.
    // For a 2D texture specify depth as 1
    // For a cube texture specify depth as 0
    // For a volume texture specify depth as requried
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    Image(uint32_t width, uint32_t height, uint32_t depth, ColorFormat native_fmt);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // ~Image()
    //
    // Destroy the texure and release any resources
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    ~Image();


    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // FillFaceData()
    //
    // converts the data in in_data from fmt to float and places it in face
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void  FillFaceData(uint32_t face, ColorFormat fmt, const void* in_data);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // Clone()
    //
    // Clone an exact copy of the texture
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    Image* Clone() const;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // GenerateFormatData()
    //
    // Allocates data for the new format and does the conversion.
    // The user is responsible for cleaning up the data he acquired
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    static  uint8_t* GenerateFormatData(const Image* src_tex, ColorFormat data_format, uint32_t face, bool convert_to_srgb);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // CloneFace()
    //
    // For a 2D texture this is identical to the above call, however for volume and cube textures
    // a single face/slice is extracted and returned as a 2D texture
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    Image* CloneFace(uint32_t face=0) const;

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
    bool InsertFace(Image* tex,uint32_t face);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // WriteTGA()
    //
    // Write a 32bit TGA, if the map is a volume or cube map then the face parameter is used to determine
    // which face (cubemap) or slice (volume map) is written.
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool WriteTGA(const tchar_t* fname, uint32_t face, bool convert_to_srgb) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // WriteRAW()
    //
    // Writes raw data for a given face with a simple header(uint32_t width,uint32_t height, uint32_t format)
    // which face (cubemap) or slice (volume map) is written.
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool WriteRAW(const tchar_t* fname, void* data, uint32_t size, uint32_t face, bool convert_to_srgb) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // WriteDDS()
    //
    // Write the texture data in the DDS format specified in output_fmt.
    // If output_fmt is not specified, the function will pick the best OUTPUT_CF match
    // for m_NativeFormat
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool WriteDDS(const tchar_t* fname, bool convert_to_srgb, OutputColorFormat output_fmt = OUTPUT_CF_UNKNOWN) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // ConvertGrayScale()
    //
    // Convert a Image to gray scale, the format of the surface is not changed and
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
    void FlipVertical(uint32_t face=0);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // FlipHorizontal()
    //
    // Flips horizontally in place
    // For a volume or cube map the face parameter specifies the slice or cube face
    // to flip
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void FlipHorizontal(uint32_t face=0);

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
    void InvertColors(uint32_t face = 0);
    void DoubleContrast(uint32_t face = 0);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool BlendImageFace(const Image* blend_source, float blend_strength, bool* channel_masks = NULL, uint32_t src_face = 0, uint32_t dst_face = 0);
    bool OverlayBlendImageFace(const Image* base_source, const Image* overlay_source, const bool* channel_masks = NULL, uint32_t face = 0, uint32_t base_face = 0, uint32_t overlay_face = 0);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void  HighPassFilterImage(const bool* channel_mask, uint32_t face, UVAddressMode u_wrap_mode, UVAddressMode v_wrap_mode);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    Image*  FilterImageFace(const PostMipImageFilter *filters, uint32_t face, uint32_t mip_index = 0) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // ScaleImage()
    //
    // NOTE: This alway clones the source texture. The source texture is left intact.
    //       The filter is applied to all the color channels.
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    Image* ScaleImage(uint32_t           width,
                        uint32_t           height, 
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
    Image* ScaleImage(uint32_t               width, 
                        uint32_t               height, 
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
    Image* ScaleImageFace(uint32_t           width, 
                            uint32_t           height, 
                            uint32_t           face, 
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
    Image* ScaleImageFace(uint32_t               width, 
                            uint32_t               height, 
                            uint32_t               face, 
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
    Image* RelativeScaleImage(float scale_x, float scale_y, ColorFormat new_format, FilterType filter) const
    {
      // scale by the amount specified and round to the nearest pixel
      uint32_t new_x = (uint32_t)(((float)m_Width*scale_x)+0.5f);
      uint32_t new_y = (uint32_t)(((float)m_Height*scale_y)+0.5f);
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
    Image* ScaleUpNextPowerOfTwo(ColorFormat new_format, FilterType filter)
    {
      uint32_t new_x = NextPowerOfTwo(m_Width);
      uint32_t new_y = NextPowerOfTwo(m_Height);
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
    Image* ScaleDownNextPowerOfTwo(ColorFormat new_format, FilterType filter)
    {
      uint32_t new_x = PreviousPowerOfTwo(m_Width);
      uint32_t new_y = PreviousPowerOfTwo(m_Height);
      return ScaleImage(new_x, new_y, new_format, filter);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // AdjustToNextPowerOf2()
    //
    // This will pad the texture to a power of two, the padding is full of black
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    Image* AdjustToNextPowerOf2() const;

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
    void Read(uint32_t x,uint32_t y, float& r, float& g, float& b, float& a, uint32_t face=0) const
    {
      HELIUM_ASSERT(m_Channels[face][R]);
      uint32_t pixel_idx = y*m_Width + x;
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
    bool Write(uint32_t x,uint32_t y, float r, float g, float b, float a, uint32_t face=0)
    {
      HELIUM_ASSERT(m_Channels[face][R]);
      uint32_t pixel_idx = y*m_Width + x;
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
    bool IsChannelDataSet( uint32_t face, uint32_t channel, float32_t threshold_value, bool valid_if_greater ) const;

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

    float32_t* GetFacePtr(uint32_t face, uint32_t channel) const
    {
      float32_t* texture_data;

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
    ImageType Type() const
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
    inline bool Is2DImage( void ) const 
    {
      return (m_Depth == TWO_D_DEPTH);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Returns true if this is a cube map texture
    inline bool IsCubeMapImage( void ) const 
    {
      return (m_Depth == CUBE_DEPTH);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Returns true if this is a volume texture
    inline bool IsVolumeImage( void ) const 
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
    static const uint32_t SAMPLE_NORMALIZED = 0x00000001;
    void Sample2D(float u, float v, float& r, float& g, float& b, float& a, uint32_t face, uint32_t flags=0) const;

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // GenerateMultiChannelSettingMipSet()
    //
    // - Where all different channel mips are generated differently and combined in the end for compression
    //   (output texture:normal map should never call this)
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    MipSet* GenerateMultiChannelSettingMipSet(const ImageGenerationSettings&  settings, 
                                              const MipSet::RuntimeSettings&    runtime_settings);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // GenerateFinalizedMipSet()
    //
    // - applies settings defined by ImageGenerationSettings
    // - scales texture to a power of 2
    // - compresses normal maps
    // - generates mips set
    // - it is the clients responsibility to delete resultant mip set
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    MipSet* GenerateFinalizedMipSet(const ImageGenerationSettings&  settings, 
                                    const MipSet::RuntimeSettings&    runtime_settings, 
                                    const bool                        is_normal_map,
                                    const bool                        is_detail_map );

  private:
    inline  void  FixUpChannels(uint32_t face)
    {
      if(m_Depth == 0)
      {
        HELIUM_ASSERT(face < CUBE_NUM_FACES);
        if(m_Channels[face][R])
        {
          uint32_t channel_f32_size  = m_Width*m_Height;
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
        HELIUM_ASSERT(m_Channels[0][R]);

        uint32_t channel_f32_size    = m_Width*m_Height*m_Depth;
        m_Channels[0][G]        = m_Channels[0][R] + channel_f32_size;
        m_Channels[0][B]        = m_Channels[0][G] + channel_f32_size;
        m_Channels[0][A]        = m_Channels[0][B] + channel_f32_size;
      }
    }

    void  FilterImage(const PostMipImageFilter*  filters,
                      const Image*             src,
                      uint32_t                        dst_face,
                      uint32_t                        src_face,
                      uint32_t                        mip_index = 0);

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
    static Image* LoadBMP(const void* bmpadr, bool convert_to_linear);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Load TGA as a 2D image
    ////////////////////////////////////////////////////////////////////////////////////////////////
    static Image* LoadTGA(const void* tgaadr, bool convert_to_linear);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Load JPEG as a 2D image
    // NOTE: JPEG files that are not standard color files cannot be loaded.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    static Image* LoadJPG(const void* jpgadr, bool convert_to_linear);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Load PFM as a 2D image, PFM data is assumed to always be linear
    ////////////////////////////////////////////////////////////////////////////////////////////////
    static Image* LoadPFM(const void* pfmadr);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Load HDR as a 2D image, HDR data is assumed to always be linear
    ////////////////////////////////////////////////////////////////////////////////////////////////
    static Image* LoadHDR(const void* hdradr);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Loads RAW data as a texture, only certain raw formats are supported and a
    // specific raw format gets converted to a supported color format. See the
    // RawFormats enum above for how formats get converted.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    static Image* LoadRAW(const void* rawadr, bool convert_to_linear, LoadRAWInfo* inf);

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
    static Image* LoadDDS(const void* ddsadr, bool convert_to_linear);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Load a file based on file extension, the returned texture could be 2D, volume or cube depending
    // on what was loaded.
    // If the file name begins with "ANIM_", a volume texture will be constructed from the image
    // files in the sibling subfolder with the same name as the file without the extension.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    static Image* LoadFile(const tchar_t* p_path, bool convert_to_linear, LoadRAWInfo* raw_info);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // LoadFile without the checking for "ANIM_" volume textures.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    static Image* LoadSingleFile(const tchar_t* filename, bool convert_to_linear, LoadRAWInfo* raw_info);

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

