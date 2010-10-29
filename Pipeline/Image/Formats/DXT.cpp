#include "DXT.h"

#include "Platform/Exception.h"

#include "Foundation/Profile.h"
#include "Foundation/Log.h"

#include <squish.h>
//#include "AtiCompress/ATI_Compress.h"

// disabled -- doesn't seem to improve things
#define DO_PERCEPTUAL_WEIGHTING    0

using namespace Helium;

Profile::Accumulator g_MipGenAccum ("MIP Generation");
Profile::Accumulator g_CompressAccum ("DXT Compress");
Profile::Accumulator g_DecompressAccum ("DXT Decompress");

////////////////////////////////////////////////////////////////////////////////////////////////
//
// Used to be: nvDXT lib callback for mip map generation
//
////////////////////////////////////////////////////////////////////////////////////////////////
bool FinalizeMips_HDR( void *data, int mip_level, int width, int height, int depth, u32 mip_size, DXTOptions* dxt_options )
{
    HELIUM_ASSERT(dxt_options->m_mips);
    MipSet* p_mips  = dxt_options->m_mips;
    u32 face        = dxt_options->m_face;
    MipSet::MipInfo* p_curr_mip = &p_mips->m_levels[face][mip_level];

    HELIUM_ASSERT_MSG(depth <= 1, ("Support for HDR volume textures not fully implimented"));

    OutputColorFormat output_format = dxt_options->m_mip_gen_options[Image::R]->m_OutputFormat;

    if (output_format == Helium::OUTPUT_CF_FLOATMAP)
    {
        // input is RGBA in floating point
        p_curr_mip->m_data = new u8[width*height*sizeof(float)*4];
        p_mips->m_datasize[mip_level] = width*height*sizeof(float)*4;
        float* dst = (float*)p_curr_mip->m_data;
        float* src = (float*)data;

        for(i32 i=0;i<width*height;i++)
        {
            // order of components in input data is BGRA
            dst[0] = src[2];
            dst[1] = src[1];
            dst[2] = src[0];
            dst[3] = src[3];
            dst+=4;
            src+=4;
        }
    }
    else if (output_format == Helium::OUTPUT_CF_HALFMAP)
    {
        // input is RGBA in floating point
        p_curr_mip->m_data = new u8[width*height*sizeof(i16)*4];
        p_mips->m_datasize[mip_level] = width*height*sizeof(i16)*4;
        i16* dst = (i16*)p_curr_mip->m_data;
        float* src = (float*)data;
        for(i32 i=0;i<width*height;i++)
        {
            // order of components in input data is BGRA
            dst[0] = FloatToHalf(src[2]);
            dst[1] = FloatToHalf(src[1]);
            dst[2] = FloatToHalf(src[0]);
            dst[3] = FloatToHalf(src[3]);
            dst+=4;
            src+=4;
        }
    }
    else if (output_format == Helium::OUTPUT_CF_F32F32)
    {
        // input is RGBA in floating point
        p_curr_mip->m_data = new u8[width*height*sizeof(float)*2];
        p_mips->m_datasize[mip_level] = width*height*sizeof(float)*2;
        float* dst = (float*)p_curr_mip->m_data;
        float* src = (float*)data;

        for(i32 i=0;i<width*height;i++)
        {
            // order of components in input data is BGRA
            dst[0] = src[2];
            dst[1] = src[1];
            dst+=2;
            src+=4;
        }
    }
    else if (output_format == Helium::OUTPUT_CF_F32)
    {
        // input is RGBA in floating point
        p_curr_mip->m_data = new u8[width*height*sizeof(float)];
        p_mips->m_datasize[mip_level] = width*height*sizeof(float);
        float* dst = (float*)p_curr_mip->m_data;
        float* src = (float*)data;

        for(i32 i=0;i<width*height;i++)
        {
            // order of components in input data is BGRA
            dst[0] = (0.212671f * src[2] + 0.715160f * src[1] + 0.072169f * src[0]);
            dst+=1;
            src+=4;
        }
    }
    else if (output_format == Helium::OUTPUT_CF_RGBE)
    {
        // input is RGBA in floating point
        p_curr_mip->m_data = new u8[width*height*sizeof(u32)];
        p_mips->m_datasize[mip_level] = width*height*sizeof(u32);
        u32* dst = (u32*)p_curr_mip->m_data;
        float* src = (float*)data;

        for(i32 i=0;i<width*height;i++)
        {
            // order of components in input data is BGRA
            *dst = ColorFormatCreateRGBE(src[2],src[1],src[0]);
            src+=4;
            dst+=1;
        }
    }
    else if (output_format == Helium::OUTPUT_CF_F16)
    {
        // input is RGBA in floating point
        p_curr_mip->m_data = new u8[width*height*sizeof(i16)];
        p_mips->m_datasize[mip_level] = width*height*sizeof(i16);
        i16* dst = (i16*)p_curr_mip->m_data;
        float* src = (float*)data;

        for(i32 i=0;i<width*height;i++)
        {
            // order of components in input data is BGRA
            // OUTPUT IS GRAY
            dst[0] = FloatToHalf(0.212671f * src[2] + 0.715160f * src[1] + 0.072169f * src[0]);
            dst+=1;
            src+=4;
        }
    }
    else if (output_format == Helium::OUTPUT_CF_F16F16)
    {
        // input is RGBA in floating point
        p_curr_mip->m_data = new u8[width*height*sizeof(i16)*2];
        p_mips->m_datasize[mip_level] = width*height*sizeof(i16)*2;
        i16* dst = (i16*)p_curr_mip->m_data;
        float* src = (float*)data;

        for(i32 i=0;i<width*height;i++)
        {
            // d3d is in GR order
            dst[0] = FloatToHalf(src[1]);
            dst[1] = FloatToHalf(src[0]);
            dst+=2;
            src+=4;
        }
    }
    else
    {
        HELIUM_ASSERT(0);
        return false;
    }

    p_curr_mip->m_width   = width;
    p_curr_mip->m_height  = height;
    p_curr_mip->m_depth   = depth;

    // on the first face through this code count the number of generated mips
    if(dxt_options->m_count)
    {
        p_mips->m_levels_used++;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// Used to be: nvDXT lib callback for LDR mip map generation
//
////////////////////////////////////////////////////////////////////////////////////////////////
u32 FinalizeMips_LDR( void *data, int mip_level, int width, int height, int depth, u32 mip_size, DXTOptions* dxt_options )
{
    HELIUM_ASSERT(dxt_options->m_mips);
    MipSet* p_mips  = dxt_options->m_mips;
    u32 face        = dxt_options->m_face;
    MipSet::MipInfo* p_curr_mip = &p_mips->m_levels[face][mip_level];
    u32 mip_depth = MAX(1, depth);

    u32 data_size = mip_size * mip_depth;
    p_curr_mip->m_data  = new u8[data_size];
    p_mips->m_datasize[mip_level] = data_size;

    memcpy(p_curr_mip->m_data, data, data_size);

    p_curr_mip->m_width   = width;
    p_curr_mip->m_height  = height;
    p_curr_mip->m_depth   = mip_depth;

    // on the first face through this code count the number of generated mips
    if(dxt_options->m_count)
    {
        p_mips->m_levels_used++;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// Compute the necessary space for a 2D slice of the OutputColorFormat
//
////////////////////////////////////////////////////////////////////////////////////////////////

inline u32 ImageByteSize(OutputColorFormat format, u32 width, u32 height)
{
    f32 total_size  = (f32)width*height*4;
    f32 multiplier  = 1.0f;

    switch(format)
    {
        //Fixed point
    case Helium::OUTPUT_CF_ARGB8888:    multiplier = 1.0f;    break;
    case Helium::OUTPUT_CF_ARGB4444:    multiplier = 0.5f;    break;
    case Helium::OUTPUT_CF_ARGB1555:    multiplier = 0.5f;    break;
    case Helium::OUTPUT_CF_RGB565:      multiplier = 0.5f;    break;
    case Helium::OUTPUT_CF_A8:          multiplier = 0.25f;   break;
    case Helium::OUTPUT_CF_L8:          multiplier = 0.25f;   break;
    case Helium::OUTPUT_CF_AL88:        multiplier = 0.5f;    break;
    case Helium::OUTPUT_CF_DXT1:        multiplier = 0.125f;  break;
    case Helium::OUTPUT_CF_DXT3:        multiplier = 0.25f;   break;
    case Helium::OUTPUT_CF_DXT5:        multiplier = 0.25f;   break;
    case Helium::OUTPUT_CF_DUDV:        multiplier = 0.5f;    break;

        //Floating point
    case Helium::OUTPUT_CF_F32:         multiplier = 1.0f;    break;
    case Helium::OUTPUT_CF_F32F32:      multiplier = 2.0f;    break;
    case Helium::OUTPUT_CF_FLOATMAP:    multiplier = 4.0f;    break;
    case Helium::OUTPUT_CF_F16:         multiplier = 0.5f;    break;
    case Helium::OUTPUT_CF_F16F16:      multiplier = 1.0f;    break;
    case Helium::OUTPUT_CF_HALFMAP:     multiplier = 2.0f;    break;
    case Helium::OUTPUT_CF_RGBE:        multiplier = 1.0f;    break;
    default:
        HELIUM_ASSERT(!"WTF");
    }

    return u32(total_size*multiplier);
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
//
////////////////////////////////////////////////////////////////////////////////////////////////
inline bool ShouldAllocateWorkBuffer(OutputColorFormat format)
{
    switch(format)
    {
    case Helium::OUTPUT_CF_DXT1:
    case Helium::OUTPUT_CF_DXT3:
    case Helium::OUTPUT_CF_DXT5:
        return true;
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
//
////////////////////////////////////////////////////////////////////////////////////////////////
void ProcessMip(const Image* mip, u32 mip_level, u32 face, OutputColorFormat format, DXTOptions* dxt_options, u8* work_buffer, bool convert_to_srgb)
{
    ColorFormat conversion_format = CF_ARGB8888; // default used by dxt compressor
    bool        hdr               = false;
    bool        dxt_compress      = false;

    switch(format)
    {
        //Fixed point
    case Helium::OUTPUT_CF_ARGB4444:  conversion_format = CF_ARGB4444;    break;
    case Helium::OUTPUT_CF_ARGB1555:  conversion_format = CF_ARGB1555;    break;
    case Helium::OUTPUT_CF_RGB565:    conversion_format = CF_RGB565;      break;
    case Helium::OUTPUT_CF_A8:        conversion_format = CF_A8;          break;
    case Helium::OUTPUT_CF_L8:        conversion_format = CF_L8;          break;
    case Helium::OUTPUT_CF_AL88:      conversion_format = CF_AL88;        break;
    case Helium::OUTPUT_CF_DXT1:      dxt_compress      = true;           break;
    case Helium::OUTPUT_CF_DXT3:      dxt_compress      = true;           break;
    case Helium::OUTPUT_CF_DXT5:      dxt_compress      = true;           break;
    case Helium::OUTPUT_CF_DUDV:
        HELIUM_ASSERT("We don't use this format! Converting to A8L8");
        conversion_format = CF_AL88;
        break;

        //Floating point
    case Helium::OUTPUT_CF_F32:
    case Helium::OUTPUT_CF_F32F32:
    case Helium::OUTPUT_CF_FLOATMAP:
    case Helium::OUTPUT_CF_F16:
    case Helium::OUTPUT_CF_F16F16:
    case Helium::OUTPUT_CF_HALFMAP:
    case Helium::OUTPUT_CF_RGBE:
        {
            conversion_format = CF_RGBAFLOATMAP;
            hdr               = true;
        }
        break;
    }

    HELIUM_ASSERT( (face == 0) || (mip->m_Depth == 0) ); // Make sure we're not both a cube and volume texture somehow
    u8* native_data = Image::GenerateFormatData(mip, conversion_format, face, convert_to_srgb);

    u32 depth = MAX(1, mip->m_Depth);

    u32 mip_size = ImageByteSize(format, mip->m_Width, mip->m_Height);

    if (dxt_compress)
    {
        // force alpha to 1 in the source buffer before doing dxt1 compression
        if (format == Helium::OUTPUT_CF_DXT1)
        {
            for (u32 a_idx = 0; a_idx < mip->m_Width*mip->m_Height*depth; ++a_idx)
            {
                native_data[(a_idx << 2) + 3] = 0xFF;
            }
        }

        // set up squish flags
        u32 squish_flags = (squish::kColourClusterFit | squish::kColourIterativeClusterFit);

        switch (format)
        {
        case Helium::OUTPUT_CF_DXT3:      squish_flags |= squish::kDxt3;  break;
        case Helium::OUTPUT_CF_DXT5:      squish_flags |= squish::kDxt5;  break;
        default:                      squish_flags |= squish::kDxt1;  break;
        }

#if DO_PERCEPTUAL_WEIGHTING
        if(convert_to_srgb)
            squish_flags |= squish::kColourMetricPerceptual;    // we are working with a color map, compress based on perceived distances
        else
#endif
            squish_flags |= squish::kColourMetricUniform;       // assume we are working with linear data

        if ((format != Helium::OUTPUT_CF_DXT1) && convert_to_srgb)
        {
            squish_flags |= squish::kWeightColourByAlpha;
        }


        // loop through layers (if it's not a volume texture there will be only one layer)
        u32 native_pixel_size = (ColorFormatBits(conversion_format) >> 3);
        u32 native_layer_size = mip->m_Width * mip->m_Height * native_pixel_size;
        u8* p_curr_native_data = native_data;
        u8* p_curr_work_buffer = work_buffer;
        for(u32 curr_depth = 0; curr_depth < depth; curr_depth++)
        {
            PROFILE_SCOPE_ACCUM(g_CompressAccum);

            u32 size = squish::GetStorageRequirements(mip->m_Width, mip->m_Height, squish_flags);
            void* data = malloc(size);
            squish::CompressImage(p_curr_native_data, mip->m_Width, mip->m_Height, data, squish_flags);
            memcpy(p_curr_work_buffer, data, size);
            free(data);

            p_curr_native_data += native_layer_size;
            p_curr_work_buffer += mip_size;
        }
    }
    else
    {
        work_buffer = native_data;
    }

    if(hdr)
    {
        FinalizeMips_HDR(work_buffer, mip_level, mip->m_Width, mip->m_Height, mip->m_Depth, mip_size, dxt_options);
    }
    else
    {
        FinalizeMips_LDR(work_buffer, mip_level, mip->m_Width, mip->m_Height, mip->m_Depth, mip_size, dxt_options);
    }

    //Clean up
    delete [] native_data;
}


bool Helium::DXTGenerateMipSet(const Image* top_mip, DXTOptions* dxt_options)
{
    //Shorter dereferencing names
    const MipGenOptions*    c_mip_gen_opts[4] = { dxt_options->m_mip_gen_options[Image::R],
        dxt_options->m_mip_gen_options[Image::G],
        dxt_options->m_mip_gen_options[Image::B],
        dxt_options->m_mip_gen_options[Image::A]};

    if ((c_mip_gen_opts[Image::R] == NULL)  || (c_mip_gen_opts[Image::G] == NULL)  ||
        (c_mip_gen_opts[Image::B] == NULL)  || (c_mip_gen_opts[Image::A] == NULL))
    {
        throw Helium::Exception( TXT( "MipGenOptions missing during MipSet generation (DXTGenerateMipSet)" ) );
    }

    PROFILE_SCOPE_ACCUM(g_MipGenAccum);

    //Output format
    HELIUM_ASSERT(dxt_options->m_mips);
    //Output format
    OutputColorFormat       output_format     = dxt_options->m_mips->m_format;
    //Should convert to SRGB
    const bool              c_convert_to_srgb =  c_mip_gen_opts[Image::R]->m_ConvertToSrgb;
    //Current face
    u32                     face              = dxt_options->m_face;
    //Size limit
    u32                     pixel_size_limit  = 1;

    //Since DXT compression works on 4 x 4 pixel blocks, don't go below size 4 for those formats
    if((output_format  == Helium::OUTPUT_CF_DXT1) ||
        (output_format  == Helium::OUTPUT_CF_DXT3) ||
        (output_format  == Helium::OUTPUT_CF_DXT5))
    {
        pixel_size_limit = 4;

        //Don't generate mip-maps for small DXT textures
        if((top_mip->m_Width < 4 ) || (top_mip->m_Height < 4 ))
        {
            //Choose uncompressed ARGB8888

            //We don't support alpha with DXT1 textures
            if(output_format == Helium::OUTPUT_CF_DXT1)
            {
                dxt_options->m_mips->m_runtime.m_alpha_channel = Helium::COLOR_CHANNEL_FORCE_ONE;
            }     

            //Change the format
            output_format                 = Helium::OUTPUT_CF_ARGB8888;
            dxt_options->m_mips->m_format = Helium::OUTPUT_CF_ARGB8888;
        }
    }

    u32                     top_width         = top_mip->m_Width;
    u32                     top_height        = top_mip->m_Height;
    u32                     top_depth         = MAX(1, top_mip->m_Depth);

    //Allocate work space data
    u32 work_space_size = ImageByteSize(output_format, top_width, top_height) * top_depth;
    u8* work_space_data = ShouldAllocateWorkBuffer(output_format) ? new u8[work_space_size] : NULL;

    //Process the top mip
    ProcessMip(top_mip, 0, face, output_format, dxt_options, work_space_data, c_convert_to_srgb);

    //Link to the previous mip in the chain
    const Image*  prev_mip  = top_mip;
    Image*        curr_mip  = NULL;

    //Mips' separate post filters
    const PostMipImageFilter  c_mip_post_filters[]  = { c_mip_gen_opts[Image::R]->m_PostFilter,
        c_mip_gen_opts[Image::G]->m_PostFilter,
        c_mip_gen_opts[Image::B]->m_PostFilter,
        c_mip_gen_opts[Image::A]->m_PostFilter};

    //Mips' separate generation filters
    const FilterType          c_mip_gen_filters[]   = { c_mip_gen_opts[Image::R]->m_Filter,
        c_mip_gen_opts[Image::G]->m_Filter,
        c_mip_gen_opts[Image::B]->m_Filter,
        c_mip_gen_opts[Image::A]->m_Filter};

    //UV wrap address modes
    const UVAddressMode       c_u_wrap_mode         =  c_mip_gen_opts[Image::R]->m_UAddressMode;
    const UVAddressMode       c_v_wrap_mode         =  c_mip_gen_opts[Image::R]->m_VAddressMode;

    //Determine if we need to apply the mips' post filters
    u32 use_mip_post_filters  = c_mip_post_filters[Image::R] +  c_mip_post_filters[Image::G] +
        c_mip_post_filters[Image::B] +  c_mip_post_filters[Image::A];

    //Determine how many mip levels to generate (volume texture currently don't support mip maps)
    u32 mip_count             = top_mip->IsVolumeImage() ? 1 : c_mip_gen_opts[Image::R]->m_Levels;

    if(mip_count == 0)
    {
        u32 mip_count_w = 1;
        u32 mip_count_h = 1;

        //Width
        {
            u32 w = top_mip->m_Width;
            while(w > pixel_size_limit)
            {
                w >>= 1;
                ++mip_count_w;
            }
        }

        //Height
        {
            u32 h = top_mip->m_Height;
            while(h > pixel_size_limit)
            {
                h >>= 1;
                ++mip_count_h;
            }
        }

        //Generate the final mip_count
        mip_count = MAX(mip_count_w, mip_count_h);
    }

    //Generate the rest of the mip-maps
    for(u32 i = 1; i < mip_count; ++i)
    {
        u32 curr_mip_height = MAX(pixel_size_limit, (prev_mip->m_Height  >> 1));
        u32 curr_mip_width  = MAX(pixel_size_limit, (prev_mip->m_Width   >> 1));
        u32 curr_face       = (i == 1) ? face : 0;

        //Generate the source image
        curr_mip      = prev_mip->ScaleImageFace( curr_mip_width, 
            curr_mip_height, 
            curr_face, 
            CF_ARGB8888, 
            c_mip_gen_filters,
            c_u_wrap_mode,
            c_v_wrap_mode);

        //
        // point composite filtering works by blending 50% of a point-sample mip into the generated mip
        //
        bool point_comp_mask[4];
        point_comp_mask[Image::R] = (c_mip_gen_filters[Image::R] == Helium::MIP_FILTER_POINT_COMPOSITE);
        point_comp_mask[Image::G] = (c_mip_gen_filters[Image::G] == Helium::MIP_FILTER_POINT_COMPOSITE);
        point_comp_mask[Image::B] = (c_mip_gen_filters[Image::B] == Helium::MIP_FILTER_POINT_COMPOSITE);
        point_comp_mask[Image::A] = (c_mip_gen_filters[Image::A] == Helium::MIP_FILTER_POINT_COMPOSITE);

        if (point_comp_mask[Image::R] || point_comp_mask[Image::G] || point_comp_mask[Image::B] || point_comp_mask[Image::A])
        {
            // generate a point sampled mip from the higher res image
            const FilterType point_sampled_filters[] = { Helium::MIP_FILTER_POINT, Helium::MIP_FILTER_POINT, Helium::MIP_FILTER_POINT, Helium::MIP_FILTER_POINT };
            Image* point_sampled_mip = prev_mip->ScaleImageFace( curr_mip_width, 
                curr_mip_height, 
                curr_face, 
                CF_ARGB8888, 
                point_sampled_filters,
                c_u_wrap_mode,
                c_v_wrap_mode);

            // combine 50% of the point sampled mip into the curr mip
            curr_mip->BlendImageFace(point_sampled_mip, 0.5f, point_comp_mask);

            delete point_sampled_mip;
        }

        //Check if we need to apply any of the filters to the channels of the mip
        u32 pass_sum  = c_mip_gen_opts[Image::R]->m_ApplyPostFilter[i] +
            c_mip_gen_opts[Image::G]->m_ApplyPostFilter[i] +
            c_mip_gen_opts[Image::B]->m_ApplyPostFilter[i] +
            c_mip_gen_opts[Image::A]->m_ApplyPostFilter[i];

        //Check to see if some filtering is necessary
        if((use_mip_post_filters != 0) && (pass_sum != 0))
        {
            //Determine the final post filters
            PostMipImageFilter  post_filters[]  = { c_mip_gen_opts[Image::R]->m_ApplyPostFilter[i] ? c_mip_post_filters[Image::R] : Helium::IMAGE_FILTER_NONE,
                c_mip_gen_opts[Image::G]->m_ApplyPostFilter[i] ? c_mip_post_filters[Image::G] : Helium::IMAGE_FILTER_NONE,
                c_mip_gen_opts[Image::B]->m_ApplyPostFilter[i] ? c_mip_post_filters[Image::B] : Helium::IMAGE_FILTER_NONE,
                c_mip_gen_opts[Image::A]->m_ApplyPostFilter[i] ? c_mip_post_filters[Image::A] : Helium::IMAGE_FILTER_NONE};

            //Filter the mip
            Image*            filtered_mip    = curr_mip->FilterImageFace(post_filters, 0, i);

            //
            // high pass filtering is handled separately because it is more complex than the other post-mip filters
            //
            bool high_pass_mask[4];
            high_pass_mask[Image::R] = (post_filters[Image::R] == Helium::IMAGE_FILTER_HIGH_PASS);
            high_pass_mask[Image::G] = (post_filters[Image::G] == Helium::IMAGE_FILTER_HIGH_PASS);
            high_pass_mask[Image::B] = (post_filters[Image::B] == Helium::IMAGE_FILTER_HIGH_PASS);
            high_pass_mask[Image::A] = (post_filters[Image::A] == Helium::IMAGE_FILTER_HIGH_PASS);

            if (high_pass_mask[Image::R] || high_pass_mask[Image::G] || high_pass_mask[Image::B] || high_pass_mask[Image::A])
            {
                filtered_mip->HighPassFilterImage(high_pass_mask, curr_face, c_u_wrap_mode, c_v_wrap_mode);
            }


            //Process the filtered mip
            ProcessMip(filtered_mip, i, 0, output_format, dxt_options, work_space_data, c_convert_to_srgb);

            //Clean up the filtered mip
            delete filtered_mip;
        }
        else
        {
            ProcessMip(curr_mip, i, 0, output_format, dxt_options, work_space_data, c_convert_to_srgb);
        }

        //Clean up the previous mips unless it points to the top image
        if(i != 1)
        {
            delete prev_mip;
        }

        //Move onto the next mip
        prev_mip  = curr_mip;
    }

    //Clean up the last mip
    delete    curr_mip;

    //Clean up the work space data
    delete[]  work_space_data;

    return 1;
}