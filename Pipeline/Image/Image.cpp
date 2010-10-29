#include "Image.h"

#include "Platform/Exception.h"
#include "Platform/Windows/Windows.h"

#include "Foundation/Profile.h"
#include "Foundation/Log.h"

#include "Pipeline/Image/Utilities/Swizzle.h"
#include "Pipeline/Image/Formats/DXT.h"

#include "nvcore/Debug.h"
#include "nvcore/Ptr.h"
#include "nvmath/nvmath.h"
#include "nvimage/nvimage.h"
#include "nvimage/Image.h"
#include "nvimage/FloatImage.h"
#include "nvimage/Filter.h"

#include "tiffio.h"

using namespace Helium;

//-----------------------------------------------------------------------------
tchar* Image::p_volume_identifier_strings[VOLUME_NUM_IDENTIFIERS] =
{
    TXT( "_anim_" ),
    TXT( "_volume_" ),
};

//-----------------------------------------------------------------------------
Image::Image(uint32_t w, uint32_t h, ColorFormat native_fmt)
{
    // allocate memory for 2D texture
    uint32_t channel_f32_size  = w*h;
    m_Channels[0][R]      = new float32_t[channel_f32_size*4];
    m_Channels[0][G]      = m_Channels[0][R] + channel_f32_size;
    m_Channels[0][B]      = m_Channels[0][G] + channel_f32_size;
    m_Channels[0][A]      = m_Channels[0][B] + channel_f32_size;

    for(uint32_t i = 1; i < CUBE_NUM_FACES; ++i)
    {
        m_Channels[i][R] = NULL;
        m_Channels[i][G] = NULL;
        m_Channels[i][B] = NULL;
        m_Channels[i][A] = NULL;
    }

    m_NativeFormat  = native_fmt;
    m_Width         = w;
    m_Height        = h;
    m_Depth         = TWO_D_DEPTH;
    m_DataSize      = (channel_f32_size*4*sizeof(float32_t));
}


//-----------------------------------------------------------------------------
Image::Image(uint32_t w, uint32_t h, uint32_t d, ColorFormat native_fmt)
{
    // Calculate the size of a surface with all the mips
    uint32_t channel_f32_size  = w*h;
    uint32_t cube = false;
    if (d==0)
    {
        cube  = true;
    }
    else
    {
        channel_f32_size *= d;
    }

    m_Channels[0][R] = new float32_t[channel_f32_size*4];
    m_Channels[0][G] = m_Channels[0][R] + channel_f32_size;
    m_Channels[0][B] = m_Channels[0][G] + channel_f32_size;
    m_Channels[0][A] = m_Channels[0][B] + channel_f32_size;

    if (cube)
    {
        for(uint32_t i = 1; i < CUBE_NUM_FACES; ++i)
        {
            m_Channels[i][R] = new float32_t[channel_f32_size*4];
            m_Channels[i][G] = m_Channels[i][R] + channel_f32_size;
            m_Channels[i][B] = m_Channels[i][G] + channel_f32_size;
            m_Channels[i][A] = m_Channels[i][B] + channel_f32_size;
        }
    }
    else
    {
        for(uint32_t i = 1; i < CUBE_NUM_FACES; ++i)
        {
            m_Channels[i][R] = NULL;
            m_Channels[i][G] = NULL;
            m_Channels[i][B] = NULL;
            m_Channels[i][A] = NULL;
        }
    }

    m_NativeFormat  = native_fmt;
    m_Width         = w;
    m_Height        = h;
    m_Depth         = d;
    m_DataSize      = (channel_f32_size*4*sizeof(float32_t));
}

//-----------------------------------------------------------------------------
Image::Image()
{
}

//-----------------------------------------------------------------------------
Image::~Image()
{
    delete[] m_Channels[0][R];
    delete[] m_Channels[1][R];
    delete[] m_Channels[2][R];
    delete[] m_Channels[3][R];
    delete[] m_Channels[4][R];
    delete[] m_Channels[5][R];
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// FillFaceData()
//
// converts the data in in_data from fmt to float and places it in face
//
////////////////////////////////////////////////////////////////////////////////////////////////

void  Image::FillFaceData(uint32_t face, ColorFormat fmt, const void* in_data)
{
    HELIUM_ASSERT(face < CUBE_NUM_FACES);
    HELIUM_ASSERT(m_Channels[face][R] != NULL);

    uint32_t       color_fmt_bits  = ColorFormatBits(fmt);
    const uint8_t* native_data     = (const uint8_t*)in_data;

    for(uint32_t y = 0; y < m_Height; ++y)
    {
        for(uint32_t x = 0; x < m_Width; ++x)
        {
            uint32_t offset  = ((x*color_fmt_bits) + y*color_fmt_bits*m_Width)>>3;
            uint32_t idx     = (x + y*m_Width);
            MakeHDRPixel( native_data + offset, fmt,
                m_Channels[face][R][idx],
                m_Channels[face][G][idx],
                m_Channels[face][B][idx],
                m_Channels[face][A][idx]);
        }
    }
}

//-----------------------------------------------------------------------------
void Image::ConvertGrayScale()
{
    uint32_t d=m_Depth;
    if (d==0)
    {
        d=1;
    }
    float gray;

    for (uint32_t f=0;f<CUBE_NUM_FACES;f++)
    {
        float32_t* r  = m_Channels[f][R];
        float32_t* g  = m_Channels[f][G];
        float32_t* b  = m_Channels[f][B];

        if (r)
        {
            for (uint32_t i=0;i <m_Width*m_Height*d;i++)
            {
                gray  = (0.212671f * r[i]) + (0.715160f * g[i]) + (0.072169f * b[i]);
                r[i]  = gray;
                g[i]  = gray;
                b[i]  = gray;
            }
        }
    }
}

//-----------------------------------------------------------------------------
bool Image::WriteRAW(const tchar* fname, void* data, uint32_t size, uint32_t face, bool convert_to_srgb) const
{
    // size must be a multiple of 4
    HELIUM_ASSERT( (size&3)==0 );

    float32_t* red_data = GetFacePtr(face, R);

    if (red_data == 0)
    {
        return false;
    }

    FILE * textureFile = _tfopen(fname, TXT( "wb" ) );
    if (!textureFile)
    {
        return false;
    }

    // write 3 words for the header
    fwrite(&m_Width,sizeof(m_Width),1,textureFile);
    fwrite(&m_Height,sizeof(m_Height),1,textureFile);
    fwrite(&m_Depth,sizeof(m_Depth),1,textureFile);

    // if there is no data size is zero
    if (data==0)
        size=0;

    fwrite(&size,sizeof(size),1,textureFile);

    if (data)
    {
        // write the app specific data
        fwrite(data,size,1,textureFile);
    }

    //Generate the native data
    uint8_t* native_data = Image::GenerateFormatData(this, m_NativeFormat, face, convert_to_srgb);
    if(native_data)
    {
        // write image bits
        fwrite(native_data, m_Width*m_Height*(ColorFormatBits(m_NativeFormat)>>3),1,textureFile);
        // clean up native data
        delete[] native_data;
    }

    fclose(textureFile);

    return true;
}

//-----------------------------------------------------------------------------
Image* Image::LoadTIFF(const tchar* fname, bool convert_to_linear)
{
    TIFF* tiff = NULL;
#ifdef UNICODE
    tiff = TIFFOpenW( fname, "r" );
#else
    tiff = TIFFOpen(fname, "r");
#endif
    TIFFRGBAImage img;

    if (tiff == 0)
        return 0;

    Image* result = 0;

    char error[1024];
    if (TIFFRGBAImageOK(tiff, error))
    {
        // any code that gets to here always create a 32bit RGBA texture
        if (TIFFRGBAImageBegin(&img, tiff, 0, error) == 0)
        {
            Log::Warning( TXT( "TIFFRGBAImageBegin: %s\n" ), error);
            TIFFClose(tiff);
            return 0;
        }

        //Allocate space for the native data
        uint8_t* native_data = new uint8_t[img.width*img.height*sizeof(uint32)];

        if (TIFFRGBAImageGet(&img, (uint32*)native_data, img.width, img.height) == 0)
        {
            TIFFClose(tiff);
            delete[] native_data;
            return 0;
        }
        else
        {
            result = new Image(img.width,img.height,CF_ARGB8888);
            result->FillFaceData(0, CF_ARGB8888, native_data);
            delete[] native_data;
        }

        // NOTE: Tiffs are stored upside down so we need to reverse them
        result->FlipVertical();

        // only convert non-floating point images
        if(convert_to_linear)
        {
            result->ConvertSrgbToLinear();
        }
    }

    TIFFClose(tiff);

    return result;
}


//-----------------------------------------------------------------------------
Image* Image::LoadRAW(const void* rawadr, bool convert_to_linear, LoadRAWInfo* inf)
{
    if (inf==0)
        return 0;

    ColorFormat fmt;

    // map the raw formats to the supported color formats
    switch (inf->m_RawFormat)
    {
    case RGBFLOAT:
    case RGBAFLOAT:
        fmt = CF_RGBAFLOATMAP;
        break;
    }
    Image* result = new Image(inf->m_Width,inf->m_Height,fmt);

    switch (inf->m_RawFormat)
    {
    case RGBFLOAT:
    case RGBAFLOAT:
        {
            float* src = (float*)rawadr;
            float* r_dest = (float*)result->m_Channels[0][R];
            float* g_dest = (float*)result->m_Channels[0][G];
            float* b_dest = (float*)result->m_Channels[0][B];
            float* a_dest = (float*)result->m_Channels[0][A];

            for (uint32_t i=0;i<result->m_Width*result->m_Height;i++)
            {
                *r_dest++ = src[0];
                *g_dest++ = src[1];
                *b_dest++ = src[2];
                if (inf->m_RawFormat == RGBAFLOAT)
                {
                    *a_dest++ = src[3];
                    src+=4;
                }
                else
                {
                    *a_dest++ = 1.0f;
                    src+=3;
                }
            }
            break;
        }
    }

    // if we just made a float map then clean the pixels
    if (fmt == CF_RGBAFLOATMAP)
        result->CleanFloatData();

    // only convert non-floating point images
    else if(convert_to_linear)
        result->ConvertSrgbToLinear();

    if (inf->m_FlipVertical)
        result->FlipVertical();

    return result;
}

//-----------------------------------------------------------------------------
static uint8_t* PFMSkipWhiteSpace(uint8_t* data)
{
    while(iswspace(*data))
    {
        data++;
    }
    return data;
}

//-----------------------------------------------------------------------------
static uint8_t* PFMReadLine(uint8_t* data,char* text)
{
    int count = 0;
    while(!iswspace(*data))
    {
        text[count] = (char)*data;
        count++;
        data++;
    }
    text[count]=0;
    return data;
}

//-----------------------------------------------------------------------------
static uint8_t* ReadString(uint8_t* data,char* text,int32_t n)
{
    int count = 0;
    while(*data!=0x0A && *data!=0x0D && count<n)
    {
        text[count] = (char)*data;
        count++;
        data++;
    }
    if (*data==0x0A || *data==0x0D)
    {
        text[count]=*data;
        count++;
        data++;
    }
    text[count]=0;
    return data;
}

//-----------------------------------------------------------------------------
Image* Image::LoadPFM(const void* pfmadr)
{
    uint8_t* data = (uint8_t*)pfmadr;
    data = PFMSkipWhiteSpace(data);
    if ((data[0]!='P' && data[0]!='p') && (data[1]!='F' && data[1]!='f'))
    {
        // header must start with PF
        return 0;
    }
    data+=2;

    char buffer[1024];
    // read the width
    data = PFMSkipWhiteSpace(data);
    data = PFMReadLine(data,buffer);
    uint32_t width = atoi(buffer);

    // height
    data = PFMSkipWhiteSpace(data);
    data = PFMReadLine(data,buffer);
    uint32_t height = atoi(buffer);

    // number of colors (-1.0 for pfm)
    data = PFMSkipWhiteSpace(data);
    data = PFMReadLine(data,buffer);
    data = PFMSkipWhiteSpace(data);

    Image* result = new Image(width,height,CF_RGBAFLOATMAP);
    float* src    = (float*)data;
    float* r_dest = (float*)result->m_Channels[0][R];
    float* g_dest = (float*)result->m_Channels[0][G];
    float* b_dest = (float*)result->m_Channels[0][B];
    float* a_dest = (float*)result->m_Channels[0][A];

    for (uint32_t i=0;i<width*height;i++)
    {
        *r_dest++  = src[0];    //R
        *g_dest++  = src[1];    //G
        *b_dest++  = src[2];    //B
        *a_dest++  = 1.0f;
        src+=3;
    }
    result->CleanFloatData();
    return result;
}

//-----------------------------------------------------------------------------
static int SortAnimFiles( const void *p_a, const void *p_b )
{
    const WIN32_FIND_DATA *p_file_a = (WIN32_FIND_DATA*)p_a;
    const WIN32_FIND_DATA *p_file_b = (WIN32_FIND_DATA*)p_b;

    return(_tcsicmp(p_file_a->cFileName, p_file_b->cFileName));
}

//-----------------------------------------------------------------------------
Image* Image::LoadFile( const tchar* p_path, bool convert_to_linear, LoadRAWInfo* info )
{
    tchar p_filename[256];
    tchar p_ext[256];
    _tsplitpath(p_path, 0, 0, p_filename, p_ext);

    // Check for the file being a proxy for an animated texture
    bool is_volume_texture_set = false;
    for(uint32_t id_index = 0; id_index < VOLUME_NUM_IDENTIFIERS; id_index++)
    {
        if(_tcsstr(p_filename, p_volume_identifier_strings[id_index]) == p_filename)
        {
            is_volume_texture_set = true;
            break;
        }
    }
    if(!is_volume_texture_set) // Not a anim/volume proxy, so do a straight texture load
    {
        return LoadSingleFile(p_path, convert_to_linear, info);
    }

    //
    // The selected texture is a proxy for an animated sequence to make into a volume texture

    tchar old_dir[MAX_PATH];
    if(GetCurrentDirectory(MAX_PATH, old_dir) == 0)
    {
        return NULL;
    }

    tchar anim_folder[MAX_PATH];
    Image* p_anim_texture = NULL;
    uint32_t curr_depth = 0;

    // Truncate the file extension to get the folder name
    _tcscpy(anim_folder, p_path);
    tchar *p_ext_start = _tcsstr(anim_folder, p_ext);
    if(!p_ext_start)
    {
        return NULL;
    }
    p_ext_start[0] = 0;

    SetCurrentDirectory(anim_folder);

    HANDLE h_folder_search;
    WIN32_FIND_DATA folder_search_info;
    WIN32_FIND_DATA folder_files[VOLUME_MAX_DEPTH];

    // Only search for images of the same type as the proxy file
    const uint32_t max_ext_chars = 5;
    for(uint32_t ext_char = max_ext_chars; ext_char; ext_char--)
    {
        p_ext[ext_char] = p_ext[ext_char-1];
    }
    p_ext[0] = '*';

    uint32_t num_files_found = 0;
    h_folder_search = FindFirstFile(p_ext, &folder_search_info);
    if(h_folder_search != INVALID_HANDLE_VALUE)
    {
        do
        {
            if(!(folder_search_info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) // Don't add subfolders
            {
                folder_files[num_files_found] = folder_search_info;
                num_files_found++;
            }
        } while( (num_files_found < VOLUME_MAX_DEPTH) && FindNextFile(h_folder_search, &folder_search_info) );
        FindClose(h_folder_search);
    }

    // Sort the files
    qsort(folder_files, num_files_found, sizeof(folder_files[0]), SortAnimFiles);

    uint32_t frame_width = 0;
    uint32_t frame_height = 0;
    ColorFormat frame_format = CF_ARGB8888;
    FilterType frame_filter = MIP_FILTER_CUBIC;
    uint32_t file_index = 0;
    for(; file_index < num_files_found; file_index++)
    {
        WIN32_FIND_DATA* p_curr_file = &folder_files[file_index];

        Image* p_frame_texture = LoadSingleFile(p_curr_file->cFileName, convert_to_linear, info);
        if(p_frame_texture)
        {
            if(p_frame_texture->m_Depth != TWO_D_DEPTH) // Frames need to be 2D textures
            {
                delete p_frame_texture;
                continue;
            }

            if(!curr_depth) // Init the volume texture based on the first frame
            {
                frame_width = ::Math::NextPowerOfTwo(p_frame_texture->m_Width);
                frame_height = ::Math::NextPowerOfTwo(p_frame_texture->m_Height);
                frame_format = p_frame_texture->m_NativeFormat;
            }

            if( (p_frame_texture->m_Width != frame_width) || (p_frame_texture->m_Height != frame_height) )
            {
                Image* p_scaled_frame = p_frame_texture->ScaleImage(frame_width, frame_height, frame_format, frame_filter);
                delete p_frame_texture;
                p_frame_texture = p_scaled_frame;
            }

            if(!curr_depth) // Init the volume texture based on the first frame
            {
                p_anim_texture = new Image(frame_width, frame_height, num_files_found, frame_format);
            }

            p_anim_texture->InsertFace(p_frame_texture, curr_depth);
            curr_depth++;
        }
        else
        {
            // TODO: Spew some error about not being able to load one of the frames
        }
    }

    SetCurrentDirectory(old_dir);

    // We only support volume texture with a power of two depth due to swizzling and DXT compression limitations
    // TODO: Impement depth scaling?
    if( !curr_depth || (curr_depth != ::Math::NextPowerOfTwo(curr_depth)) )
    {
        delete p_anim_texture;
        p_anim_texture = NULL;

        // Fall back to loading as a regular 2D texture
        return LoadSingleFile(p_path, convert_to_linear, info);
    }

    return p_anim_texture;
}

//-----------------------------------------------------------------------------
Image* Image::LoadSingleFile(const tchar* filename, bool convert_to_linear, LoadRAWInfo* info)
{
    FILE* f;
    f = _tfopen(filename,TXT( "rb" ) );

    if (f==0)
        return 0;

    fseek(f,0,SEEK_END);
    int size = ftell(f);
    fseek(f,0,SEEK_SET);

    uint8_t* data;
    if (size>0)
    {
        data = new uint8_t[size];
        fread(data,size,1,f);
    }
    fclose(f);

    if (size<=0)
        return 0;

    tchar ext[256];
    _tsplitpath(filename, 0, 0, 0, ext);

    Image* result = 0;

    if (_tcsicmp(ext,TXT(".bmp"))==0)
    {
        result = LoadBMP(data, convert_to_linear);
    }
    else if (_tcsicmp(ext,TXT(".tga"))==0)
    {
        result = LoadTGA(data, convert_to_linear);
    }
    else if ((_tcsicmp(ext,TXT(".jpg"))==0) || (_tcsicmp(ext,TXT(".jpeg"))==0))
    {
        result =  LoadJPG(data, convert_to_linear);
    }
    else if ((_tcsicmp(ext,TXT(".tif"))==0) || (_tcsicmp(ext,TXT(".tiff"))==0))
    {
        // TIFFS Cannot be loaded from memory, delete the copy we
        // just loaded and pass the filename to the loader function
        delete data;
        data = 0;
        result =  LoadTIFF(filename, convert_to_linear);
    }
    else if (_tcsicmp(ext,TXT( ".raw" ) )==0)
    {
        if (info)
        {
            result = LoadRAW(data, convert_to_linear, info);
        }
        else
        {
            // No info struct specified so we cannot load raw data
            // Do nothing as the default return code is zero
        }
    }
    else if (_tcsicmp(ext, TXT( ".pfm" ) )==0)
    {
        result = LoadPFM(data);
    }
    else if (_tcsicmp(ext,TXT( ".hdr" ) )==0)
    {
        result = LoadHDR(data);
    }
    else if (_tcsicmp(ext,TXT( ".dds" ) )==0)
    {
        result = LoadDDS(data, convert_to_linear);
    }

    if (data)
        delete data;

    return result;
}

//-----------------------------------------------------------------------------
Image* Image::Clone() const
{
    Image* result = new Image(m_Width,m_Height,m_Depth, m_NativeFormat);
    for (uint32_t f=0;f<CUBE_NUM_FACES;f++)
    {
        if (m_Channels[f][R])
        {
            memcpy(result->m_Channels[f][R], m_Channels[f][R],m_DataSize);
        }
        else
        {
            // face is not in the source so get rid of it from the source
            delete [] result->m_Channels[f][R];
            result->m_Channels[f][R] = 0;
            result->m_Channels[f][G] = 0;
            result->m_Channels[f][B] = 0;
            result->m_Channels[f][A] = 0;
        }
    }
    return result;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
// CloneFace()
//
// For a 2D texture this is identical to the above call, however for volume and cube textures
// a single face/slice is extracted and returned as a 2D texture
//
////////////////////////////////////////////////////////////////////////////////////////////////
Image* Image::CloneFace(uint32_t face) const
{
    float32_t* red_channel = GetFacePtr(face, R);
    if (red_channel==0)
    {
        return 0;
    }
    Image* result = new Image(m_Width,m_Height,1,m_NativeFormat);
    memcpy(result->m_Channels[0][R], red_channel, result->m_DataSize);
    result->FixUpChannels(0);
    return result;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
// InsertFace()
//
// Inserts the specified texture in the volume/cube map at the specified face position. The
// face being inserted has to be 2D texture and has to be the same width and height as the
// destination.
//
////////////////////////////////////////////////////////////////////////////////////////////////
bool Image::InsertFace(Image* tex,uint32_t face)
{
    HELIUM_ASSERT(tex);

    if ((tex->m_Depth!=TWO_D_DEPTH) ||    // input texture has to be a 2D texture
        (tex->m_Width != m_Width) || (tex->m_Height != m_Height) || (tex->m_NativeFormat != m_NativeFormat)) // and has to have the same dimensions
    {
        return false;
    }

    if(IsVolumeImage())
    {
        HELIUM_ASSERT(face < m_Depth);

        float32_t* channel_data[NUM_TEXTURE_CHANNELS];
        for(uint32_t channel_index = 0; channel_index < NUM_TEXTURE_CHANNELS; channel_index++)
        {
            channel_data[channel_index] = GetFacePtr(face, channel_index);
            if(!channel_data[channel_index])
            {
                return false;
            }
        }

        uint32_t channel_size = tex->m_DataSize / NUM_TEXTURE_CHANNELS;
        for(uint32_t channel_index = 0; channel_index < NUM_TEXTURE_CHANNELS; channel_index++)
        {
            memcpy(channel_data[channel_index], tex->m_Channels[0][channel_index], channel_size);
        }

        return true;
    }

    float32_t* red_data = GetFacePtr(face, R);
    if (red_data ==0)
    {
        // this is either a bad face for the texture or it could be a missing face in a cube texture

        // this is a cube map with a missing face
        m_Channels[face][R] = (float32_t*)(new uint8_t[m_DataSize]);
        red_data            = m_Channels[face][R];

        // we may of just converted a 2D map into a cube map
        if (face>0 && m_Depth==TWO_D_DEPTH)
        {
            m_Depth = CUBE_DEPTH;
        }
    }

    memcpy(red_data, tex->m_Channels[0][R],tex->m_DataSize);
    FixUpChannels(face);

    return true;
}


//-----------------------------------------------------------------------------
void Image::CleanFloatData()
{
    uint32_t d = m_Depth;
    if (d==0)
    {
        d=1;
    }


    for (uint32_t f=0;f<CUBE_NUM_FACES;f++)
    {
        uint32_t* data_r = (uint32_t*)m_Channels[f][R];
        if (data_r)
        {
            uint32_t* data_g = (uint32_t*)m_Channels[f][G];
            uint32_t* data_b = (uint32_t*)m_Channels[f][B];
            uint32_t* data_a = (uint32_t*)m_Channels[f][A];

            for (uint32_t i=0;i<m_Width*m_Height*d;i++)
            {
                // this is an infinity or a NAN so replace it with zero
                if ((*data_r & 0x7f000000) == 0x7f000000) { *data_r = 0; }
                if ((*data_g & 0x7f000000) == 0x7f000000) { *data_g = 0; }
                if ((*data_b & 0x7f000000) == 0x7f000000) { *data_b = 0; }
                if ((*data_a & 0x7f000000) == 0x7f000000) { *data_a = 0; }
                ++data_r;
                ++data_g;
                ++data_b;
                ++data_a;
            }
        }
    }
}

//-----------------------------------------------------------------------------
void Image::FlipVertical(uint32_t face)
{
    for(uint32_t c = 0; c < 4; ++c)
    {
        uint8_t* texture_data = (uint8_t*)GetFacePtr(face, c);
        HELIUM_ASSERT(texture_data);

        uint32_t line_bytes = (m_Width*4);
        uint8_t* top = texture_data;
        uint8_t* bottom = texture_data+(line_bytes*m_Height)-line_bytes;
        uint8_t* temp_line = new uint8_t[line_bytes];

        for (uint32_t i=0;i<m_Height/2;i++)
        {
            memcpy(temp_line,top,line_bytes);
            memcpy(top,bottom,line_bytes);
            memcpy(bottom,temp_line,line_bytes);
            top+=line_bytes;
            bottom-=line_bytes;
        }

        delete temp_line;
    }
}

//-----------------------------------------------------------------------------
void Image::FlipHorizontal(uint32_t face)
{
    for(uint32_t c = 0; c < 4; ++c)
    {
        uint8_t* texture_data = (uint8_t*)GetFacePtr(face, c);

        HELIUM_ASSERT(texture_data);

        uint32_t pixel_bytes = 4;
        uint32_t line_bytes  = (m_Width*4);
        uint8_t* temp_pixel  = new uint8_t[pixel_bytes];

        for (uint32_t y=0;y<m_Height;y++)
        {
            uint8_t* left = texture_data+(y*line_bytes);
            uint8_t* right = texture_data+((y+1)*line_bytes)-pixel_bytes;

            for (uint32_t x=0;x<m_Width/2;x++)
            {
                memcpy(temp_pixel,left,pixel_bytes);
                memcpy(left,right,pixel_bytes);
                memcpy(right,temp_pixel,pixel_bytes);
                left+=pixel_bytes;
                right-=pixel_bytes;
            }
        }

        delete temp_pixel;
    }
}

//-----------------------------------------------------------------------------
void Image::Sample2D(float u, float v, float& r, float& g, float& b, float& a,uint32_t face, uint32_t flags) const
{
    float w = (flags & SAMPLE_NORMALIZED)?m_Width:1.0f;

    int u1 = ((int)floor((u*w)-0.5f))%m_Width;
    int u2 = (int)floor((u*w)+0.5f)%m_Width;
    int v1 = (int)floor((v*w)-0.5f)%m_Height;
    int v2 = (int)floor((v*w)+0.5f)%m_Height;

    float fu2 = fmod((u*w)+0.5f,1.0f);
    float fu1 = 1-fu2;
    float fv2 = fmod((v*w)+0.5f,1.0f);
    float fv1 = 1-fv2;

    float r1,g1,b1,a1;    // top left sample
    float r2,g2,b2,a2;    // top right sample
    float r3,g3,b3,a3;    // bottom left sample
    float r4,g4,b4,a4;    // bottom right sample

    Read(u1,v1,r1,g1,b1,a1,face);
    Read(u2,v1,r2,g2,b2,a2,face);
    Read(u1,v2,r3,g3,b3,a3,face);
    Read(u2,v2,r4,g4,b4,a4,face);

    r = (r1*fu1*fv1) + (r2*fu2*fv1) + (r3*fu1*fv2) + (r4*fu2*fv2);
    g = (g1*fu1*fv1) + (g2*fu2*fv1) + (g3*fu1*fv2) + (g4*fu2*fv2);
    b = (b1*fu1*fv1) + (b2*fu2*fv1) + (b3*fu1*fv2) + (b4*fu2*fv2);
    a = (a1*fu1*fv1) + (a2*fu2*fv1) + (a3*fu1*fv2) + (a4*fu2*fv2);
}

//-----------------------------------------------------------------------------
bool Image::AdjustExposure(float fstop)
{
    float factor = powf(2.0f,fstop);

    uint32_t d = m_Depth;
    if (d==0)
    {
        d=1;
    }


    for (uint32_t f=0;f<CUBE_NUM_FACES;f++)
    {
        float* r = m_Channels[f][R];
        if (r)
        {
            float* g =  m_Channels[f][G];
            float* b =  m_Channels[f][B];

            for (uint32_t i=0;i<m_Width*m_Height*d;i++)
            {
                *r++ *= factor;
                *g++ *= factor;
                *b++ *= factor;
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
bool Image::AdjustGamma(float gamma)
{
    uint32_t d = m_Depth;
    if (d==0)
    {
        d=1;
    }

    for (uint32_t f=0;f<CUBE_NUM_FACES;f++)
    {
        float* r = m_Channels[f][R];
        if (r)
        {
            float* g =  m_Channels[f][G];
            float* b =  m_Channels[f][B];

            for (uint32_t i=0;i<m_Width*m_Height*d;i++)
            {
                *r = powf(*r, gamma);
                *g = powf(*g, gamma);
                *b = powf(*b, gamma);
                ++r;
                ++g;
                ++b;
            }
        }
    }

    return true;
}


//-----------------------------------------------------------------------------

void Image::ConvertSrgbToLinear()
{
    uint32_t d = m_Depth;
    if (d==0)
    {
        d=1;
    }

    for (uint32_t f=0;f<CUBE_NUM_FACES;f++)
    {
        float* r = m_Channels[f][R];
        if (r)
        {
            float* g =  m_Channels[f][G];
            float* b =  m_Channels[f][B];

            for (uint32_t i=0;i<m_Width*m_Height*d;i++)
            {
                *r = SrgbToLinear(*r);
                *g = SrgbToLinear(*g);
                *b = SrgbToLinear(*b);
                ++r;
                ++g;
                ++b;
            }
        }
    }
}

void Image::ConvertLinearToSrgb()
{
    uint32_t d = m_Depth;
    if (d==0)
    {
        d=1;
    }

    for (uint32_t f=0;f<CUBE_NUM_FACES;f++)
    {
        float* r = m_Channels[f][R];
        if (r)
        {
            float* g =  m_Channels[f][G];
            float* b =  m_Channels[f][B];

            for (uint32_t i=0;i<m_Width*m_Height*d;i++)
            {
                *r = LinearToSrgb(*r);
                *g = LinearToSrgb(*g);
                *b = LinearToSrgb(*b);
                ++r;
                ++g;
                ++b;
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Internal function: Convert RGBE to floating point, this is slightly different
// to out RGBE format as the exponent bias is different.
static inline void rgbe2float(float *red, float *green, float *blue, uint8_t* rgbe)
{
    float f;
    if (rgbe[3])
    {
        f = (float)ldexp(1.0,rgbe[3]-(int)(128+8));
        *red = rgbe[0] * f;
        *green = rgbe[1] * f;
        *blue = rgbe[2] * f;
    }
    else
        *red = *green = *blue = 0.0;
}

//-----------------------------------------------------------------------------
// Internal function: simple read routine for .HDR data
static Image* ReadHDRPixels(uint8_t* src, int numpixels, Image* tex)
{
    float* red   = tex->m_Channels[0][Image::R];
    float* green = tex->m_Channels[0][Image::G];
    float* blue  = tex->m_Channels[0][Image::B];
    float* alpha = tex->m_Channels[0][Image::A];

    while(numpixels-- > 0)
    {
        rgbe2float(red,green,blue,src);
        *alpha = 1.0f;
        red++;     // skip to the next components
        green++;
        blue++;
        alpha++;
        src+=4;
    }

    return tex;
}


//-----------------------------------------------------------------------------
Image* Image::LoadHDR(const void* data)
{
    char text[128];
    float tempf;
    int32_t i;

    float gamma = 1.0f;
    float exposure = 1.0f;

    bool found_format = false;
    uint8_t* header = (uint8_t*)data;
    header = ReadString((uint8_t*)header,text,128);

    if ((text[0] != '#')||(text[1] != '?'))
    {
        return 0;
    }

    for(;;)
    {
        header = ReadString(header,text,128);

        if ((text[0] == 0)||(text[0] == '\n'))
        {
            if (!found_format)
                return 0;
            break;
        }
        else if (strcmp(text,"FORMAT=32-bit_rle_rgbe\n") == 0)
        {
            found_format = true;
        }
        else if (sscanf(text,"GAMMA=%g",&tempf) == 1)
        {
            gamma = tempf;
        }
        else if (sscanf(text,"EXPOSURE=%g",&tempf) == 1)
        {
            exposure = tempf;
        }
    }

    int32_t width;
    int32_t height;
    header = ReadString(header,text,128);
    if (sscanf(text,"-Y %d +X %d",&height,&width) < 2)
        return 0;

    // We now have all the info we need to make the resulting texture class
    Image* result = new Image(width,height,CF_RGBAFLOATMAP);

    uint32_t count;
    uint8_t buf[128];

    if ((width < 8)||(width > 0x7fff))
    {
        // not allowed to be run length encoded
        return ReadHDRPixels(header,width*height,result);
    }

    uint8_t* src = header;
    uint8_t rgbe[4];
    float*  red     = result->m_Channels[0][Image::R];
    float*  green   = result->m_Channels[0][Image::G];
    float*  blue    = result->m_Channels[0][Image::B];
    float*  alpha   = result->m_Channels[0][Image::A];
    uint8_t*     scanline_buffer = 0;

    // read in each successive scanline
    while(height > 0)
    {
        rgbe[0] = src[0];
        rgbe[1] = src[1];
        rgbe[2] = src[2];
        rgbe[3] = src[3];
        src+=4;

        if ((rgbe[0] != 2)||(rgbe[1] != 2)||(rgbe[2] & 0x80))
        {
            return ReadHDRPixels(header,width*height,result);
        }

        if (( (((int)rgbe[2])<<8) | (rgbe[3])) != width)
        {
            delete result;
            return 0;
        }

        if (scanline_buffer == NULL)
            scanline_buffer = new uint8_t[4*width];
        uint8_t* ptr = scanline_buffer;
        uint8_t* ptr_end;

        // read each of the four channels for the scanline into the buffer
        for(i=0;i<4;i++)
        {
            ptr_end = &scanline_buffer[(i+1)*width];
            while(ptr < ptr_end)
            {
                buf[0] = src[0];
                buf[1] = src[1];
                src+=2;

                if (buf[0] > 128)
                {
                    // a run of the same value
                    count = buf[0]-128;
                    if ((count == 0)||((int32_t)count > ptr_end - ptr))
                    {
                        delete result;
                        delete scanline_buffer;
                        return 0;
                    }
                    while(count-- > 0)
                        *ptr++ = buf[1];
                }
                else
                {
                    // a non-run
                    count = buf[0];
                    if ((count == 0)||((int32_t)count > ptr_end - ptr))
                    {
                        delete result;
                        delete scanline_buffer;
                        return 0;
                    }
                    *ptr++ = buf[1];
                    if (--count > 0)
                    {
                        memcpy(ptr,src,count);
                        src+=count;
                        ptr+=count;
                    }
                }
            }
        }

        //now convert data from buffer into floats
        for(i=0;i<width;i++)
        {
            rgbe[0] = scanline_buffer[i];
            rgbe[1] = scanline_buffer[i+width];
            rgbe[2] = scanline_buffer[i+2*width];
            rgbe[3] = scanline_buffer[i+3*width];
            rgbe2float(red,green,blue,rgbe);
            *alpha = 1.0f;

            red++;     // skip to the next components
            green++;
            blue++;
            alpha++;
        }
        height--;
    }
    delete scanline_buffer;
    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////
static inline nv::FloatImage::WrapMode ConvertIGWrapModeToNV(Helium::UVAddressMode mode)
{
    switch(mode)
    {
    case UV_WRAP:
        return nv::FloatImage::WrapMode_Repeat;
    case UV_MIRROR:
        return nv::FloatImage::WrapMode_Mirror;
    }

    //Default
    return nv::FloatImage::WrapMode_Clamp;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////
void Image::InvertColors(uint32_t face)
{
    for (uint32_t c = 0; c < 4; ++c)
    {
        float32_t* curr_data = GetFacePtr(face, c);
        if (!curr_data)
            continue;

        for (uint32_t i = 0; i < m_Width * m_Height; i++)
        {
            curr_data[i] = 1.0f - curr_data[i];
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////
void Image::DoubleContrast(uint32_t face)
{
    for (uint32_t c = 0; c < 4; ++c)
    {
        float32_t* curr_data = GetFacePtr(face, c);
        if (!curr_data)
            continue;

        float total_color = 0.0f;
        for (uint32_t i = 0; i < m_Width * m_Height; i++)
        {
            total_color += curr_data[i];
        }

        float mean = total_color / float(m_Width * m_Height);

        for (uint32_t i = 0; i < m_Width * m_Height; i++)
        {
            float color = mean + ((curr_data[i] - mean) * 2.0f);
            curr_data[i] = Math::Clamp(color, 0.0f, 1.0f);
        }
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////
bool Image::BlendImageFace(const Image* blend_source, float blend_strength, bool* channel_masks, uint32_t src_face, uint32_t dst_face)
{
    if (blend_source->m_Width != m_Width)
        return false;
    if (blend_source->m_Height != m_Height)
        return false;
    if (blend_strength == 0.0f)
        return false;

    for (uint32_t c = 0; c < 4; ++c)
    {
        float32_t* curr_data = GetFacePtr(dst_face, c);
        float32_t* blend_data = blend_source->GetFacePtr(src_face, c);
        if (channel_masks && !channel_masks[c])
            continue;
        if (!curr_data || !blend_data)
            continue;

        for (uint32_t i = 0; i < m_Width * m_Height; i++)
        {
            curr_data[i] = (blend_data[i] * blend_strength) + (curr_data[i] * (1.0f - blend_strength));
        }
    }
    return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////
bool Image::OverlayBlendImageFace(const Image* base_source, const Image* overlay_source, const bool* channel_masks, uint32_t face, uint32_t base_face, uint32_t overlay_face)
{
    if ((base_source->m_Width != m_Width) || (base_source->m_Height != m_Height))
        return false;
    if ((overlay_source->m_Width != m_Width) || (overlay_source->m_Height != m_Height))
        return false;

    for (uint32_t c = 0; c < 4; ++c)
    {
        float32_t* curr_data = GetFacePtr(face, c);
        float32_t* base_data = base_source->GetFacePtr(base_face, c);
        float32_t* overlay_data = overlay_source->GetFacePtr(overlay_face, c);
        if (channel_masks && !channel_masks[c])
            continue;
        if (!curr_data || !base_data || !overlay_data)
            continue;

        for (uint32_t i = 0; i < m_Width * m_Height; i++)
        {
            float base_color = base_data[i];
            float overlay_color = overlay_data[i];

            float final_color;
            if (overlay_color < 0.5f)
                final_color = base_color * overlay_color * 2.0f;
            else
                final_color = 1.0f - ((1.0f - base_color) * (1.0f - overlay_color) * 2.0f);

            curr_data[i] = final_color;
        }
    }
    return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////
static inline nv::Filter* GetImageFilter(FilterType filter)
{
    switch(filter)
    {
    case MIP_FILTER_NONE:
    case MIP_FILTER_POINT:
        return NULL;

    case MIP_FILTER_BOX:
        return new nv::BoxFilter();

    case MIP_FILTER_TRIANGLE:
        return new nv::TriangleFilter();

    case MIP_FILTER_QUADRATIC:
        return new nv::QuadraticFilter();

    case MIP_FILTER_CUBIC:
    case MIP_FILTER_POINT_COMPOSITE:  // later we will mix in 50% of a point sampled mip
        return new nv::CubicFilter();

    case MIP_FILTER_MITCHELL:
        return new nv::MitchellFilter();

    case MIP_FILTER_KAISER:
        {
            nv::Filter* filter = new nv::KaiserFilter(3);
            ((nv::KaiserFilter *)filter)->setParameters(4.0f, 1.0f);
            return filter;
        }

    case MIP_FILTER_SINC:
        return new nv::LanczosFilter();

        // temporary -- unimplemented cases
    case MIP_FILTER_GAUSSIAN:
        {
            return new nv::MitchellFilter();
        }
    }
    return NULL;
}

namespace nv
{
    void ScaleImage( float* rgba_input,
        const uint w_input,
        const uint h_input,
        float* rgba_output,
        const uint w_output, 
        const uint h_output,
        const Filter** rgba_filters,
        const FloatImage::WrapMode u_wrap_mode,
        const FloatImage::WrapMode v_wrap_mode )
    {
        // @@ Use monophase filters when frac(m_width / w) == 0
        AutoPtr<FloatImage> tmp_image( new FloatImage() ); 

        FloatImage src_image;
        //Do some aliasing
        src_image.m_componentNum  = 4;
        src_image.m_height        = h_input;
        src_image.m_width         = w_input;
        src_image.m_count         = src_image.m_height *  src_image.m_width * 4; 
        src_image.m_mem           = rgba_input;

        FloatImage dst_image;
        //Do some aliasing
        dst_image.m_componentNum  = 4;
        dst_image.m_height        = h_output;
        dst_image.m_width         = w_output;
        dst_image.m_count         = dst_image.m_height *  dst_image.m_width * 4; 
        dst_image.m_mem           = rgba_output;

        {
            tmp_image->allocate(4, w_output, h_input); 

            Array<float> tmp_column(h_output);
            tmp_column.resize(h_output);

            for (uint c = 0; c < 4; c++)
            {
                //If we have a valid filter, apply it
                if(rgba_filters[c])
                {

                    PolyphaseKernel xkernel(*rgba_filters[c], w_input, w_output, 32);
                    PolyphaseKernel ykernel(*rgba_filters[c], h_input, h_output, 32);

                    float * tmp_channel = tmp_image->channel(c);

                    for (uint y = 0; y < h_input; y++) 
                    {
                        src_image.applyKernelHorizontal(xkernel, y, c, u_wrap_mode, tmp_channel + y * w_output);
                    }

                    float * dst_channel = dst_image.channel(c);

                    for (uint x = 0; x < w_output; x++) 
                    {
                        tmp_image->applyKernelVertical(ykernel, x, c, v_wrap_mode, tmp_column.unsecureBuffer());

                        for (uint y = 0; y < h_output; y++) 
                        {
                            dst_channel[y * w_output + x] = tmp_column[y];
                        }
                    } 
                }else
                    //Otherwise default to point filtering
                {
                    float  y_scale  = 1.0f/h_output;
                    float  x_scale  = 1.0f/w_output;
                    float* dst_data = dst_image.channel(c);

                    for (uint y = 0, idx = 0; y < h_output; ++y) 
                    {
                        for (uint x = 0; x < w_output; ++x, ++idx) 
                        {
                            dst_data[idx]  = src_image.sampleNearestClamp(x*x_scale, y*y_scale, c);
                        }
                    } 
                }
            }
        }

        //Remove aliasing
        dst_image.m_mem     = NULL;
        src_image.m_mem     = NULL;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// ScaleImage()
//
//
////////////////////////////////////////////////////////////////////////////////////////////////
Image* Image::ScaleImage(uint32_t           width,
                         uint32_t           height,
                         ColorFormat   new_format,
                         FilterType    filter,
                         UVAddressMode u_wrap_mode,
                         UVAddressMode v_wrap_mode) const
{
    const FilterType filters_duplicate[] = { filter, filter, filter, filter};
    return ScaleImage(width, height, new_format, filters_duplicate, u_wrap_mode, v_wrap_mode);
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// ScaleImage()
//
//
////////////////////////////////////////////////////////////////////////////////////////////////
Image* Image::ScaleImage(uint32_t               width,
                         uint32_t               height,
                         ColorFormat       new_format,
                         const FilterType* filters,
                         UVAddressMode     u_wrap_mode,
                         UVAddressMode     v_wrap_mode) const
{
    if (IsVolumeImage())
    {
        // cannot scale volume maps
        return 0;
    }

    // this scale will have no effect so don't do it, just clone the result and convert the format
    if ( (width == m_Width) && (height == m_Height) )
    {
        Image* result = Clone();
        //Only switch the format
        result->m_NativeFormat  = new_format;
        return result;
    }

    //Choose the appropriate filters
    const nv::Filter*   nv_filters[] = { NULL, NULL, NULL, NULL};

    nv_filters[R] = GetImageFilter(filters[R]);
    nv_filters[G] = GetImageFilter(filters[G]);
    nv_filters[B] = GetImageFilter(filters[B]);
    nv_filters[A] = GetImageFilter(filters[A]);

    Image*      dest_img   = new Image(width, height, m_Depth, new_format);

    nv::FloatImage::WrapMode  nv_u_wrap_mode  = ConvertIGWrapModeToNV(u_wrap_mode);
    nv::FloatImage::WrapMode  nv_v_wrap_mode  = ConvertIGWrapModeToNV(v_wrap_mode);

    for (uint32_t f=0;f<CUBE_NUM_FACES;f++)
    {
        if (m_Channels[f][R])
        {
            nv::ScaleImage( m_Channels[f][R],
                m_Width,
                m_Height,
                dest_img->GetFacePtr(f, R),
                dest_img->m_Width,
                dest_img->m_Height,
                nv_filters,
                nv_u_wrap_mode,
                nv_v_wrap_mode );
        }
    }

    //Clean up the filters
    delete nv_filters[R];
    delete nv_filters[G];
    delete nv_filters[B];
    delete nv_filters[A];

    return dest_img;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// ScaleImageFace()
//
//
////////////////////////////////////////////////////////////////////////////////////////////////
Image* Image::ScaleImageFace(uint32_t           width,
                             uint32_t           height,
                             uint32_t           face,
                             ColorFormat   new_format,
                             FilterType    filter,
                             UVAddressMode u_wrap_mode,
                             UVAddressMode v_wrap_mode) const
{
    const FilterType filters_duplicate[] = { filter, filter, filter, filter};
    return ScaleImageFace(width, height, face, new_format, filters_duplicate, u_wrap_mode, v_wrap_mode);
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// ScaleImageFace()
//
//
////////////////////////////////////////////////////////////////////////////////////////////////
Image* Image::ScaleImageFace(uint32_t               width,
                             uint32_t               height,
                             uint32_t               face,
                             ColorFormat       new_format,
                             const FilterType* filters,
                             UVAddressMode     u_wrap_mode,
                             UVAddressMode     v_wrap_mode) const
{
    if( IsVolumeImage() || (m_Channels[face][R] == NULL) )
    {
        // cannot scale volume maps or NULL textures
        return NULL;
    }

    // this scale will have no effect so don't do it, just clone the result and convert the format
    if ( (width == m_Width) && (height == m_Height) )
    {
        //Switch the format
        Image* result = new Image(width, height, new_format);
        result->FillFaceData(0, CF_RGBAFLOATMAP, (uint8_t*)m_Channels[face]);
        return result;
    }

    //Choose the appropriate filter
    const nv::Filter*   nv_filters[] = { NULL, NULL, NULL, NULL};

    nv_filters[R] = GetImageFilter(filters[R]);
    nv_filters[G] = GetImageFilter(filters[G]);
    nv_filters[B] = GetImageFilter(filters[B]);
    nv_filters[A] = GetImageFilter(filters[A]);

    Image*  dest_img  = new Image(width, height, new_format);

    nv::FloatImage::WrapMode  nv_u_wrap_mode  = ConvertIGWrapModeToNV(u_wrap_mode);
    nv::FloatImage::WrapMode  nv_v_wrap_mode  = ConvertIGWrapModeToNV(v_wrap_mode);

    float32_t* output = dest_img->GetFacePtr(0, R);

    if ( output )
    {
        nv::ScaleImage( m_Channels[face][R],
            m_Width,
            m_Height,
            dest_img->GetFacePtr(0, R),
            dest_img->m_Width,
            dest_img->m_Height,
            nv_filters,
            nv_u_wrap_mode,
            nv_v_wrap_mode );
    }

    //Clean up the filters
    delete nv_filters[R];
    delete nv_filters[G];
    delete nv_filters[B];
    delete nv_filters[A];

    return dest_img;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// AdjustToNextPowerOf2()
//
// This will pad the texture to a power of two, the padding is full of black
//
////////////////////////////////////////////////////////////////////////////////////////////////
Image* Image::AdjustToNextPowerOf2() const
{
    if (Math::IsPowerOfTwo(m_Width) && Math::IsPowerOfTwo(m_Height))
    {
        return Clone();
    }

    if (IsVolumeImage())
        return 0;

    uint32_t width = Math::NextPowerOfTwo(m_Width);
    uint32_t height = Math::NextPowerOfTwo(m_Height);

    // make a new texture
    Image* text = new Image(width,height,m_Depth,m_NativeFormat);


    for (uint32_t f=0;f<CUBE_NUM_FACES;f++)
    {
        if (text->m_Channels[f][R] ==0)
        {
            continue;
        }
        // zero everything to black
        memset(text->m_Channels[f][R], 0,text->m_DataSize);

        float32_t r, g, b, a;
        for (uint32_t y=0;y<m_Height;++y)
        {
            for (uint32_t x=0;x<m_Width;++x)
            {
                Read(x, y, r, g, b, a, f);
                text->Write(x, y, r, g, b, a, f);
            }
        }
    }

    // return the new texture
    return text;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
// PrepareFor2ChannelNormalMap()
//
// Used to convert ARGB data into a format suitable for creating switched green/alpha 2-channel
// type normal maps.
// Also note that we normalize the incoming normal before converting it to 2 channels. This is
// because it may not be normalized but when it gets decompressed in the shader it definitely
// will be normalized.
//
// Detail maps are output in partial derivating format.
// All other maps are output in 2 component parabolic xy coords since:
//  a) it distributes precision more uniformly over the full range of solid angles
//  b) it's faster to decode in a fragment program
//
////////////////////////////////////////////////////////////////////////////////////////////////

void Image::PrepareFor2ChannelNormalMap(bool is_detail_map, bool is_detail_map_only)
{
    //Fix up the flag
    is_detail_map_only = is_detail_map ? is_detail_map_only : false;

    for (uint32_t f=0;f<CUBE_NUM_FACES;f++)
    {
        const uint32_t num_texels = m_Width * m_Height;

        if (m_Channels[f][R])
        {
            float* r_ptr = m_Channels[f][R];
            float* g_ptr = m_Channels[f][G];
            float* b_ptr = m_Channels[f][B];
            float* a_ptr = m_Channels[f][A];

            for (uint32_t i = 0; i < num_texels; ++i)
            {
                float red   = *r_ptr;
                float green = *g_ptr;
                float blue  = *b_ptr;

                // this computation can never return zero vector so it is always safe to normalize
                red   = red   *  2.f - 1.f;
                green = green *  2.f - 1.f;

                if(is_detail_map)
                {
                    float d = red * red + green * green;
                    if(d < 1.f)
                    {
                        blue = sqrtf(1.f - d);
                    }
                    else
                    {
                        blue = 0.f;
                    }
                }
                else
                {
                    blue = blue * 2.f - 1.f;
                }

                Math::Vector3 normal(red, green, blue);
                normal.Normalize();

                if(is_detail_map)
                {
                    // we can't encode any values in xy that are greater than z in absolute length, so
                    // scale z such that it equals the longest absolute length of x or y.  That should
                    // give us the best possible encoding when we have out of range values in x or y.
                    normal.z = MAX( normal.z, MAX( ABS( normal.x ), ABS( normal.y ) ) );

                    if(normal.z < 0.001f)
                    {
                        normal.z = 0.001f;
                    }

                    normal /= -normal.z;
                    normal *= Math::Vector3(0.5f);
                    normal += Math::Vector3(0.5f);
                    normal.x = Math::Clamp(normal.x, 0.f, 1.f);
                    normal.y = Math::Clamp(normal.y, 0.f, 1.f);

                    if(is_detail_map_only == true)
                    {
                        *r_ptr = normal.y;
                        *g_ptr = normal.y;
                        *b_ptr = normal.y;
                        *a_ptr = normal.x;
                    }
                    else
                    {
                        *r_ptr = normal.x;
                        *g_ptr = normal.y;
                    }
                }
                else
                {
                    //
                    // paraboloid is z = 1 - x^2 - y^2
                    //
                    //	ray is 	x' = x * t
                    //          y' = y * t
                    //          z' = z * t		... where x, y, and z are the components of a unit normal
                    //
                    // substituting one into the other we get:
                    //
                    //  zt = 1 - (xt)^2 - (yt)^2
                    //  zt = 1 - x^2*t^2 - y^2*t^2
                    //
                    //  x^2*t^2 + y^2*t^2 + zt - 1 = 0		... solve for t
                    //
                    //  at^2 + bt + c = 0
                    //
                    //  a = x^2 + b^2
                    //  b = z
                    //  c = -1
                    //
                    // now just use the quadratic formula...
                    //
                    float a = (normal.x * normal.x) + (normal.y * normal.y);
                    float b = normal.z;
                    float c = -1.f;

                    if(a < 0.00001f)
                        a = 0.00001f;

                    float discriminant = b*b - 4.f*a*c;
                    HELIUM_ASSERT(discriminant >= 0.f);
                    float t = (-b + sqrtf(discriminant)) / (2.f * a);

                    float para_x = (normal.x * t) * 0.5f + 0.5f;
                    float para_y = (normal.y * t) * 0.5f + 0.5f;
                    para_x = Math::Clamp(para_x, 0.f, 1.f);
                    para_y = Math::Clamp(para_y, 0.f, 1.f);

                    *r_ptr = para_y;
                    *g_ptr = para_y;
                    *b_ptr = para_y;
                    *a_ptr = para_x;
                }
                ++r_ptr;
                ++g_ptr;
                ++b_ptr;
                ++a_ptr;
            }
        }
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////
// Returns true if any pixel satisfies the < or > check against the threshold value
// (based off of ShaderGenerator::IsTexChannelDataValid)
////////////////////////////////////////////////////////////////////////////////////////////////
bool Image::IsChannelDataSet( uint32_t face, uint32_t channel, float32_t threshold_value, bool valid_if_greater ) const
{
    HELIUM_ASSERT( (m_Depth == 0) || (face == 0) ); // either we're a cube map, or we're checking from the first depth layer
    float32_t* color_channel = GetFacePtr(face, channel);
    uint32_t depth = (m_Depth == 0) ? 1 : m_Depth;
    uint32_t image_size = m_Width * m_Height * depth;

    if(valid_if_greater)
    {
        for(uint32_t i = 0; i < image_size; i++)
        {
            if(color_channel[i] > threshold_value)
                return true;
        }
    }
    else
    {
        for(uint32_t i = 0; i < image_size; i++)
        {
            if(color_channel[i] < threshold_value)
                return true;
        }
    }

    return false;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
// GenerateMipSet()
//
// Generate the mipset for this texture.
//
////////////////////////////////////////////////////////////////////////////////////////////////
MipSet* Image::GenerateMipSet(const MipGenOptions& options, const MipSet::RuntimeSettings& runtime) const
{
    const MipGenOptions*  mipgen_options[] = { &options,  &options, &options, &options};
    return GenerateMipSet(mipgen_options, runtime);
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// GenerateMipSet()
//
// Generate the mipset for this texture.
//
////////////////////////////////////////////////////////////////////////////////////////////////
MipSet* Image::GenerateMipSet(const MipGenOptions** options_rgb, const MipSet::RuntimeSettings& runtime) const
{
    const uint32_t         num_channels = 4;
    MipGenOptions     mipgen_options[num_channels];
    OutputColorFormat outputFormat;
    DXTOptions        dxtOptions;

    outputFormat                        = options_rgb[R]->m_OutputFormat;
    dxtOptions.m_texture                = 0;

    // Propagate the runtime wrap settings to those used to generate the mip maps
    for(uint32_t options_index = 0; options_index < num_channels; options_index++)
    {
        mipgen_options[options_index] = *options_rgb[options_index];
        mipgen_options[options_index].m_UAddressMode = runtime.m_wrap_u;
        mipgen_options[options_index].m_VAddressMode = runtime.m_wrap_v;

        dxtOptions.m_mip_gen_options[options_index] = &mipgen_options[options_index];
    }

    dxtOptions.m_mips                   = new MipSet;
    dxtOptions.m_mips->m_texture_type   = (uint32_t)Type();
    dxtOptions.m_mips->m_width          = m_Width;
    dxtOptions.m_mips->m_height         = m_Height;
    dxtOptions.m_mips->m_depth          = m_Depth;
    dxtOptions.m_mips->m_format         = outputFormat;
    dxtOptions.m_mips->m_runtime        = runtime;

    dxtOptions.m_mips->m_runtime.m_srgb_expand_r  = options_rgb[R]->m_ConvertToSrgb;
    dxtOptions.m_mips->m_runtime.m_srgb_expand_g  = options_rgb[G]->m_ConvertToSrgb;
    dxtOptions.m_mips->m_runtime.m_srgb_expand_b  = options_rgb[B]->m_ConvertToSrgb;
    dxtOptions.m_mips->m_runtime.m_srgb_expand_a  = false;  // special case for alpha

    const Image*  src_img       = this;
    bool            clone         = false;
    bool            np2_compress  = false;
    uint32_t             o_width;
    uint32_t             o_height;

    // if the output format is compressed
    if( (outputFormat == Helium::OUTPUT_CF_DXT1) ||
        (outputFormat == Helium::OUTPUT_CF_DXT3) ||
        (outputFormat == Helium::OUTPUT_CF_DXT5))
    {
        if( (outputFormat == Helium::OUTPUT_CF_DXT3) || (outputFormat == Helium::OUTPUT_CF_DXT5) )
        {
            // Check if the texture actually has alpha. If not, force to DXT1
            const float32_t upper_alpha_threshold = 0.99f;
            const float32_t lower_alpha_threshold = 0.01f;

            // 0 = don't force to DXT1
            // 1 = force to DXT1, set alpha to white
            // 2 = force to DXT1, set alpha to black
            uint32_t force_to_dxt1 = 0;
            if(m_Depth == 0) // cube map
            {
                uint32_t num_white_faces = 0;
                uint32_t num_black_faces = 0;

                for(uint32_t face = 0; face < 6; face++)
                {
                    if(src_img->m_Channels[face][R])
                    {
                        if(!src_img->IsChannelDataSet(face, A, upper_alpha_threshold, false))
                        {
                            ++num_white_faces;
                        }
                        else if(!src_img->IsChannelDataSet(0, A, lower_alpha_threshold, true))
                        {
                            ++num_black_faces;
                        }
                    }
                }

                if(num_white_faces == 6)
                    force_to_dxt1 = 1;
                else if(num_black_faces == 6)
                    force_to_dxt1 = 2;
            }
            else // 2d or volume map
            {
                if(!src_img->IsChannelDataSet(0, A, upper_alpha_threshold, false))
                {
                    force_to_dxt1 = 1;
                }
                else if(!src_img->IsChannelDataSet(0, A, lower_alpha_threshold, true))
                {
                    force_to_dxt1 = 2;
                }
            }

            if(force_to_dxt1)
            {
                HELIUM_ASSERT(force_to_dxt1 <= 2);

                outputFormat                = Helium::OUTPUT_CF_DXT1;
                dxtOptions.m_mips->m_format = Helium::OUTPUT_CF_DXT1;

                if(force_to_dxt1 == 1)
                {
                    Log::Bullet bullet ( Log::Streams::Normal, Log::Levels::Verbose, TXT( "Forced DXT5 to DXT1 - setting alpha channel to 1.\n" ) );

                    dxtOptions.m_mips->m_runtime.m_alpha_channel = Helium::COLOR_CHANNEL_FORCE_ONE;
                }
                if(force_to_dxt1 == 2)
                {
                    Log::Bullet bullet ( Log::Streams::Normal, Log::Levels::Verbose, TXT( "Forced DXT5 to DXT1 - setting alpha channel to 0.\n" ) );

                    dxtOptions.m_mips->m_runtime.m_alpha_channel = Helium::COLOR_CHANNEL_FORCE_ZERO;
                }
            }
            else
            {
                // Check for RGB being all 1, which means we can pack alpha into color of a dxt1 and swizzle
                const float32_t color_set_threshold = 0.99f;

                bool force_to_swizzled_dxt1 = false;
                if(m_Depth == 0) // cube map
                {
                    for(uint32_t face = 0; face < 6; face++)
                    {
                        if(src_img->m_Channels[face][R])
                        {
                            if( !src_img->IsChannelDataSet(face, R, color_set_threshold, false) &&
                                !src_img->IsChannelDataSet(face, G, color_set_threshold, false) &&
                                !src_img->IsChannelDataSet(face, B, color_set_threshold, false) )
                            {
                                force_to_swizzled_dxt1 = true;
                                break;
                            }
                        }
                    }
                }
                else // 2d or volume map
                {
                    if( !src_img->IsChannelDataSet(0, R, color_set_threshold, false) &&
                        !src_img->IsChannelDataSet(0, G, color_set_threshold, false) &&
                        !src_img->IsChannelDataSet(0, B, color_set_threshold, false) )
                    {
                        force_to_swizzled_dxt1 = true;
                    }
                }

                if(force_to_swizzled_dxt1)
                {
                    Log::Bullet bullet ( Log::Streams::Normal, Log::Levels::Verbose, TXT( "Swizzled alpha only texture to DXT1\n" ) );

                    uint32_t depth = (m_Depth == 0) ? 1 : m_Depth;
                    uint32_t channel_size = m_Width * m_Height * depth * sizeof(float32_t);

                    // Copy alpha to the color channels
                    for(uint32_t face = 0; face < 6; face++)
                    {
                        if(src_img->m_Channels[face][R])
                        {
                            float32_t* alpha_channel  = src_img->m_Channels[face][A];
                            float32_t* red_channel    = src_img->m_Channels[face][R];
                            float32_t* green_channel  = src_img->m_Channels[face][G];
                            float32_t* blue_channel   = src_img->m_Channels[face][B];

                            memcpy(red_channel, alpha_channel, channel_size);
                            memcpy(green_channel, alpha_channel, channel_size);
                            memcpy(blue_channel, alpha_channel, channel_size);
                        }
                    }

                    outputFormat                = Helium::OUTPUT_CF_DXT1;
                    dxtOptions.m_mips->m_format = Helium::OUTPUT_CF_DXT1;
                    dxtOptions.m_mips->m_runtime.m_alpha_channel = Helium::COLOR_CHANNEL_GET_FROM_G; // DXT stores packed colors as 5:6:5, so green has one more bit of accuracy in it
                    dxtOptions.m_mips->m_runtime.m_red_channel = Helium::COLOR_CHANNEL_FORCE_ONE;
                    dxtOptions.m_mips->m_runtime.m_green_channel = Helium::COLOR_CHANNEL_FORCE_ONE;
                    dxtOptions.m_mips->m_runtime.m_blue_channel = Helium::COLOR_CHANNEL_FORCE_ONE;

                    // Because we're getting alpha from a color channel, don't do sRGB
                    {
                        for(int channel = 0; channel < num_channels; channel++)
                        {
                            mipgen_options[channel].m_ConvertToSrgb = false;
                        }
                    }
                }
            }
        }

        //If we don't have at least a 4x4 block, choose a different format
        if((m_Width < 4 ) || (m_Height < 4 ))
        {
            //DXT.cpp will handle this case
        }
        else
        {
            // if the dimensions are not a power of 2
            // then expand to a power of 2 and compress that, later on we will extract
            // the compression blocks manually.
            if (!Math::IsPowerOfTwo(m_Width) || !Math::IsPowerOfTwo(m_Height))
            {
                // calculate a valid original width/height, this is what will be extracted from the final
                o_width   = m_Width;
                o_height  = m_Height;

                Image* local = src_img->AdjustToNextPowerOf2();
                src_img       = local;
                clone         = true;
                np2_compress  = true;
            }
        }
    }

    bool first  = true;

    for (uint32_t f=0;f<CUBE_NUM_FACES;f++)
    {
        if (src_img->m_Channels[f][R])
        {
            dxtOptions.m_face   = f;
            dxtOptions.m_count  = first;
            first               = false;

            //Verify the results
            if(!DXTGenerateMipSet( src_img, &dxtOptions))
            {
                if (clone)
                {
                    delete src_img;
                }

                return 0;
            }
        }
    }

    if (np2_compress)
    {
        dxtOptions.m_mips->ExtractNonePowerOfTwo(o_width,o_height,1);
    }

    if ( clone )
    {
        delete src_img;
    }

    return  dxtOptions.m_mips;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////
static inline void AdjustWidthAndHeight(uint32_t& width, uint32_t& height, const uint32_t max_size)
{
    //Adjust width and height
    if (!Math::IsPowerOfTwo(width))
    {
        width = Math::NextPowerOfTwo(width);
    }

    if (!Math::IsPowerOfTwo(height))
    {
        height = Math::NextPowerOfTwo(height);
    }

    //Clamp width and height
    if (width>max_size)
    {
        width = max_size;
    }

    if (height>max_size)
    {
        height = max_size;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// GenerateMultiChannelSettingMipSet()
//
// - applies settings defined by ImageGenerationSettings if not all channels have same mip gen settings
// - scales texture to a power of 2
// - generates mips set
// - it is the clients responsibility to delete resultant mipset
// - assumed normal map output texture is never hit
////////////////////////////////////////////////////////////////////////////////////////////////
MipSet* Image::GenerateMultiChannelSettingMipSet(const ImageGenerationSettings&  settings,
                                                 const MipSet::RuntimeSettings&    runtime_settings )
{
    const  MipGenOptions* mip_opt_ptrs[4];
    MipGenOptions         mip_opts[4];
    MipSet*               mips;

    for(uint32_t i = 0; i < 4; ++i)
    {
        mip_opt_ptrs[i]              = &mip_opts[i];
        mip_opts[i].m_Filter         = settings.m_mip_filter[i];
        mip_opts[i].m_PostFilter     = settings.m_image_filter[i];
        mip_opts[i].m_OutputFormat   = settings.m_output_format;
        mip_opts[i].m_Levels         = settings.m_generate_mips ? 0 : 1;
        mip_opts[i].m_ConvertToSrgb  = runtime_settings.ShouldConvertToSrgb();
        mip_opts[i].m_UAddressMode   = runtime_settings.m_wrap_u;
        mip_opts[i].m_VAddressMode   = runtime_settings.m_wrap_v;

        for (uint32_t t=0;t<MAX_TEXTURE_MIPS;t++)
        {
            mip_opts[i].m_ApplyPostFilter[t] = settings.m_ifilter_cnt[i][t];
        }
    }

    // perform any pre-scaling, and force the texture to be a power of 2
    uint32_t width   = (uint32_t)(m_Width   * settings.m_scale);
    uint32_t height  = (uint32_t)(m_Height  * settings.m_scale);

    //Adjust width and height
    AdjustWidthAndHeight(width, height, settings.m_max_size);

    const bool  diff_size = ( (width != m_Width) || (height != m_Height) );
    Image*    mip_src   = this;

    //Scale the texture size
    if( diff_size )
    {
        const FilterType filters[] = { mip_opts[0].m_Filter, mip_opts[1].m_Filter,
            mip_opts[2].m_Filter, mip_opts[3].m_Filter};

        // does a clone if sizes are the same
        mip_src = ScaleImage( width, height, m_NativeFormat, filters );
    }

    //Generate the mips
    mips  = mip_src->GenerateMipSet(mip_opt_ptrs, runtime_settings);

    //Delete the mips source texture if it's different from the current class
    if(mip_src != this)
    {
        delete mip_src;
    }

    //Return the mips
    return mips;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
// GenerateFinalizedMipSet()
//
// - applies settings defined by ImageGenerationSettings
// - scales texture to a power of 2
// - compresses normal maps, only touches x and y channels
// - generates mips set
// - it is the clients responsibility to delete resultant mip set
//
////////////////////////////////////////////////////////////////////////////////////////////////
MipSet* Image::GenerateFinalizedMipSet(const ImageGenerationSettings&  settings,
                                       const MipSet::RuntimeSettings&    runtime_settings,
                                       const bool                        is_normal_map,
                                       const bool                        is_detail_map )
{
    //If  settings are not same across all chaneels then call GenerateMultiChannelSettingMipSet
    if (!is_normal_map && !is_detail_map && !settings.AreMipSettingsEqual())
    {
        MipSet* mips =  GenerateMultiChannelSettingMipSet( settings, runtime_settings );

        //attach the runtime settings
        mips->m_runtime = runtime_settings;

        return mips;
    }

    //Either normal map or all channel mip settings are same
    MipGenOptions mip_opt;
    Image*      mip_src   = this;
    MipSet*       mips      = NULL;

    //Fill up the mip_opt structure
    mip_opt.m_Filter        = settings.m_mip_filter[0];
    mip_opt.m_PostFilter    = settings.m_image_filter[0];
    mip_opt.m_Levels        = settings.m_generate_mips ? 0 : 1;
    mip_opt.m_OutputFormat  = settings.m_output_format;

    mip_opt.m_UAddressMode  = runtime_settings.m_wrap_u;
    mip_opt.m_VAddressMode  = runtime_settings.m_wrap_v;
    // for now, it all or nothing, this can be broken up if needed
    mip_opt.m_ConvertToSrgb = runtime_settings.ShouldConvertToSrgb();

    for (uint32_t t=0;t<MAX_TEXTURE_MIPS;t++)
    {
        mip_opt.m_ApplyPostFilter[t] = settings.m_ifilter_cnt[0][t];
    }


    // perform any pre-scaling, and force the texture to be a power of 2
    uint32_t width   = (uint32_t)(m_Width   * settings.m_scale);
    uint32_t height  = (uint32_t)(m_Height  * settings.m_scale);

    //Adjust width and height
    AdjustWidthAndHeight(width, height, settings.m_max_size);

    //Determine if we need a clone
    const bool  diff_size       = ( (width != m_Width) || (height != m_Height) );
    const bool  is_special_map  = is_normal_map || is_detail_map;

    if( diff_size || is_special_map)
    {
        // does a clone if sizes are the same
        mip_src = ScaleImage( width, height, m_NativeFormat, mip_opt.m_Filter );
    }

    //Treat normal and detail maps differently
    if(is_special_map)
    {
        mip_src->PrepareFor2ChannelNormalMap(is_detail_map);
    }

    //Generate the mips
    mips                    = mip_src->GenerateMipSet(mip_opt, runtime_settings);

    //Delete the mips source texture if it's different from the current class
    if(mip_src != this)
    {
        delete mip_src;
    }

    //Return the mips
    return mips;
}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
Image*  Image::FilterImageFace(const PostMipImageFilter *filters, uint32_t face, uint32_t mip_index) const
{
    if(m_Channels[face][R] == NULL)
    {
        HELIUM_ASSERT(!"NULL face.");
        return NULL;
    }

    Image* result = new Image(m_Width, m_Height, m_NativeFormat);
    result->FilterImage(filters, this, 0, face, mip_index);

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// GenerateFormatData()
//
// Allocates data for the new format and does the conversion.
// The user is responsible for cleaning up the data he acquired
//
////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t* Image::GenerateFormatData(const Image* src_tex, ColorFormat dest_fmt, uint32_t face, bool convert_to_srgb)
{
    HELIUM_ASSERT(src_tex != NULL );

    float32_t*  r =   src_tex->GetFacePtr(face, R);

    if(r == NULL)
    {
        return NULL;
    }

    float32_t*  g =   src_tex->GetFacePtr(face, G);
    float32_t*  b =   src_tex->GetFacePtr(face, B);
    float32_t*  a =   src_tex->GetFacePtr(face, A);

    uint32_t d= src_tex->m_Depth;
    if (d==0)
    {
        d=1;
    }

    uint32_t dest_fmt_pixel_byte_size  = (ColorFormatBits(dest_fmt) >> 3);
    uint32_t space_required            = dest_fmt_pixel_byte_size * d * src_tex->m_Width * src_tex->m_Height;
    uint8_t* new_surface               = new uint8_t[space_required];
    HELIUM_ASSERT(new_surface != NULL );

    MakeColorFormatBatch(new_surface, src_tex->m_Width*src_tex->m_Height*d, dest_fmt, r, g, b, a, convert_to_srgb);

    return new_surface;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// The following look up tables are generated from Photoshop color curves
//
//
////////////////////////////////////////////////////////////////////////////////////////////////
const uint8_t  c_more_contrast[] = {   0x00,0x00,0x01,0x01,0x02,0x02,0x02,0x03,0x03,0x04,0x04,0x05,0x05,0x06,0x06,0x07,
0x08,0x08,0x09,0x09,0x0a,0x0b,0x0b,0x0c,0x0d,0x0d,0x0e,0x0f,0x0f,0x10,0x11,0x12,
0x12,0x13,0x14,0x15,0x16,0x17,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1e,0x1f,
0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x30,
0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,0x41,0x42,
0x43,0x44,0x45,0x46,0x48,0x49,0x4a,0x4b,0x4d,0x4e,0x4f,0x50,0x52,0x53,0x54,0x55,
0x57,0x58,0x59,0x5a,0x5c,0x5d,0x5e,0x60,0x61,0x62,0x63,0x65,0x66,0x67,0x69,0x6a,
0x6b,0x6d,0x6e,0x6f,0x71,0x72,0x73,0x74,0x76,0x77,0x78,0x7a,0x7b,0x7c,0x7e,0x7f,
0x80,0x82,0x83,0x84,0x86,0x87,0x88,0x8a,0x8b,0x8c,0x8e,0x8f,0x90,0x92,0x93,0x94,
0x95,0x97,0x98,0x99,0x9b,0x9c,0x9d,0x9f,0xa0,0xa1,0xa2,0xa4,0xa5,0xa6,0xa8,0xa9,
0xaa,0xab,0xad,0xae,0xaf,0xb0,0xb2,0xb3,0xb4,0xb5,0xb6,0xb8,0xb9,0xba,0xbb,0xbc,
0xbe,0xbf,0xc0,0xc1,0xc2,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcd,0xce,0xcf,
0xd0,0xd1,0xd2,0xd3,0xd4,0xd5,0xd6,0xd7,0xd8,0xd9,0xda,0xdb,0xdc,0xdd,0xde,0xdf,
0xe0,0xe1,0xe2,0xe3,0xe4,0xe4,0xe5,0xe6,0xe7,0xe8,0xe9,0xea,0xea,0xeb,0xec,0xed,
0xed,0xee,0xef,0xf0,0xf0,0xf1,0xf2,0xf3,0xf3,0xf4,0xf5,0xf5,0xf6,0xf6,0xf7,0xf8,
0xf8,0xf9,0xf9,0xfa,0xfa,0xfb,0xfb,0xfc,0xfc,0xfd,0xfd,0xfd,0xfe,0xfe,0xff,0xff };

const uint8_t  c_less_contrast[] = {   0x00,0x02,0x03,0x05,0x06,0x08,0x09,0x0b,0x0c,0x0e,0x0f,0x11,0x12,0x13,0x15,0x16,
0x18,0x19,0x1a,0x1c,0x1d,0x1e,0x20,0x21,0x22,0x23,0x25,0x26,0x27,0x28,0x2a,0x2b,
0x2c,0x2d,0x2e,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x3a,0x3b,0x3c,0x3d,
0x3e,0x3f,0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4b,0x4c,
0x4d,0x4e,0x4f,0x50,0x51,0x52,0x53,0x54,0x54,0x55,0x56,0x57,0x58,0x59,0x59,0x5a,
0x5b,0x5c,0x5d,0x5e,0x5e,0x5f,0x60,0x61,0x61,0x62,0x63,0x64,0x65,0x65,0x66,0x67,
0x68,0x68,0x69,0x6a,0x6a,0x6b,0x6c,0x6d,0x6d,0x6e,0x6f,0x70,0x70,0x71,0x72,0x72,
0x73,0x74,0x75,0x75,0x76,0x77,0x77,0x78,0x79,0x79,0x7a,0x7b,0x7c,0x7c,0x7d,0x7e,
0x7e,0x7f,0x80,0x80,0x81,0x82,0x83,0x83,0x84,0x85,0x85,0x86,0x87,0x87,0x88,0x89,
0x8a,0x8a,0x8b,0x8c,0x8c,0x8d,0x8e,0x8f,0x8f,0x90,0x91,0x92,0x92,0x93,0x94,0x95,
0x95,0x96,0x97,0x98,0x98,0x99,0x9a,0x9b,0x9c,0x9c,0x9d,0x9e,0x9f,0xa0,0xa0,0xa1,
0xa2,0xa3,0xa4,0xa5,0xa6,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xae,0xaf,
0xb0,0xb1,0xb2,0xb3,0xb4,0xb5,0xb6,0xb7,0xb8,0xb9,0xba,0xbb,0xbc,0xbd,0xbe,0xbf,
0xc0,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcc,0xcd,0xce,0xcf,0xd0,0xd2,
0xd3,0xd4,0xd5,0xd7,0xd8,0xd9,0xda,0xdc,0xdd,0xde,0xe0,0xe1,0xe3,0xe4,0xe5,0xe7,
0xe8,0xea,0xeb,0xec,0xee,0xef,0xf1,0xf2,0xf4,0xf5,0xf7,0xf9,0xfa,0xfc,0xfd,0xff};


const uint8_t  c_lighten[] =       {   0x00,0x01,0x03,0x04,0x06,0x07,0x08,0x0a,0x0b,0x0d,0x0e,0x0f,0x11,0x12,0x13,0x15,
0x16,0x18,0x19,0x1a,0x1c,0x1d,0x1e,0x20,0x21,0x22,0x24,0x25,0x26,0x28,0x29,0x2a,
0x2c,0x2d,0x2e,0x30,0x31,0x32,0x34,0x35,0x36,0x38,0x39,0x3a,0x3c,0x3d,0x3e,0x3f,
0x41,0x42,0x43,0x45,0x46,0x47,0x48,0x4a,0x4b,0x4c,0x4d,0x4f,0x50,0x51,0x53,0x54,
0x55,0x56,0x57,0x59,0x5a,0x5b,0x5c,0x5e,0x5f,0x60,0x61,0x63,0x64,0x65,0x66,0x67,
0x69,0x6a,0x6b,0x6c,0x6d,0x6f,0x70,0x71,0x72,0x73,0x74,0x76,0x77,0x78,0x79,0x7a,
0x7b,0x7c,0x7e,0x7f,0x80,0x81,0x82,0x83,0x84,0x85,0x87,0x88,0x89,0x8a,0x8b,0x8c,
0x8d,0x8e,0x8f,0x90,0x91,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,0x9d,
0x9e,0x9f,0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,
0xae,0xaf,0xb0,0xb1,0xb2,0xb3,0xb4,0xb5,0xb5,0xb6,0xb7,0xb8,0xb9,0xba,0xbb,0xbc,
0xbd,0xbe,0xbf,0xbf,0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,
0xcb,0xcb,0xcc,0xcd,0xce,0xcf,0xcf,0xd0,0xd1,0xd2,0xd3,0xd3,0xd4,0xd5,0xd6,0xd6,
0xd7,0xd8,0xd9,0xda,0xda,0xdb,0xdc,0xdc,0xdd,0xde,0xdf,0xdf,0xe0,0xe1,0xe2,0xe2,
0xe3,0xe4,0xe4,0xe5,0xe6,0xe6,0xe7,0xe8,0xe8,0xe9,0xea,0xea,0xeb,0xec,0xec,0xed,
0xee,0xee,0xef,0xef,0xf0,0xf1,0xf1,0xf2,0xf2,0xf3,0xf4,0xf4,0xf5,0xf5,0xf6,0xf6,
0xf7,0xf8,0xf8,0xf9,0xf9,0xfa,0xfa,0xfb,0xfb,0xfc,0xfc,0xfd,0xfd,0xfe,0xff,0xff};

const uint8_t  c_darken[] =        {   0x00,0x01,0x01,0x02,0x03,0x03,0x04,0x05,0x06,0x06,0x07,0x08,0x08,0x09,0x0a,0x0a,
0x0b,0x0c,0x0d,0x0d,0x0e,0x0f,0x0f,0x10,0x11,0x12,0x12,0x13,0x14,0x14,0x15,0x16,
0x17,0x17,0x18,0x19,0x1a,0x1a,0x1b,0x1c,0x1d,0x1d,0x1e,0x1f,0x20,0x20,0x21,0x22,
0x23,0x23,0x24,0x25,0x26,0x26,0x27,0x28,0x29,0x2a,0x2a,0x2b,0x2c,0x2d,0x2d,0x2e,
0x2f,0x30,0x31,0x31,0x32,0x33,0x34,0x35,0x35,0x36,0x37,0x38,0x39,0x39,0x3a,0x3b,
0x3c,0x3d,0x3e,0x3e,0x3f,0x40,0x41,0x42,0x43,0x44,0x44,0x45,0x46,0x47,0x48,0x49,
0x4a,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,0x50,0x51,0x52,0x52,0x53,0x54,0x55,0x56,0x57,
0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5d,0x5e,0x5f,0x60,0x61,0x62,0x63,0x64,0x65,0x66,
0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,0x70,0x71,0x72,0x73,0x74,0x75,0x76,
0x77,0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x7f,0x80,0x81,0x83,0x84,0x85,0x86,0x87,
0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x99,
0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa8,0xa9,0xaa,0xab,
0xac,0xae,0xaf,0xb0,0xb1,0xb2,0xb4,0xb5,0xb6,0xb7,0xb9,0xba,0xbb,0xbc,0xbd,0xbf,
0xc0,0xc1,0xc3,0xc4,0xc5,0xc6,0xc8,0xc9,0xca,0xcb,0xcd,0xce,0xcf,0xd1,0xd2,0xd3,
0xd5,0xd6,0xd7,0xd8,0xda,0xdb,0xdc,0xde,0xdf,0xe0,0xe2,0xe3,0xe5,0xe6,0xe7,0xe9,
0xea,0xeb,0xed,0xee,0xf0,0xf1,0xf2,0xf4,0xf5,0xf6,0xf8,0xf9,0xfb,0xfc,0xfe,0xff};

const float c_sharpen_gradual[] = { 92.0f, 46.0f, 8.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

////////////////////////////////////////////////////////////////////////////////////////////////
//
// CurveEvaluate()
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////////
float32_t CurveEvaluate(float32_t input, const uint8_t* lookup_table)
{
    //If the lookup table is null, don't modify the input and return it as is
    if(lookup_table == NULL)
    {
        return input;
    }
    //Make sure we are in the 0.0-1.0 range
    input   = Math::Clamp(input, 0.0f, 1.0f);
    //Generate the entry index
    uint32_t idx =  uint32_t(input*255.0f + 0.5f);

    const float32_t c_inv_255 = 1.0f/255.0f;
    return float32_t(lookup_table[idx])*c_inv_255;
}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
void  Image::HighPassFilterImage(const bool*                channel_mask,
                                 uint32_t                        face,
                                 UVAddressMode              u_wrap_mode,
                                 UVAddressMode              v_wrap_mode)
{
    // NOTE - this modifies the texture in place!

    // high-pass post filter -- custom multi-step process based on Photoshop high-pass filter plus overlay blend
    //  - Make a blurred copy of the source image (ideally using a Gaussian blur with a radius of 0.7 based on artist suggestions)
    //  - Invert the colors on this blurred copy and blend it 50-50 with the original image
    //  - Double the contrast of this image
    //  - Do an 'overlay' style blend (see photoshop layer blend modes) to combine this image with the original source

    const nv::Filter*   nv_filters[] = { NULL, NULL, NULL, NULL };

    //
    // here we're trying to approximate the photoshop Gaussian blur with radius 0.7 by running the cubic filter twice
    //
    for (uint32_t ic = 0; ic < 4; ic++)
        nv_filters[ic] = GetImageFilter(Helium::MIP_FILTER_QUADRATIC);

    Image* blur_tex = new Image(m_Width, m_Height, m_NativeFormat);

    nv::ScaleImage( m_Channels[face][R],
        m_Width,
        m_Height,
        blur_tex->GetFacePtr(0, R),
        blur_tex->m_Width,
        blur_tex->m_Height,
        nv_filters,
        ConvertIGWrapModeToNV(u_wrap_mode),
        ConvertIGWrapModeToNV(v_wrap_mode) );

    for (uint32_t ic = 0; ic < 4; ic++)
    {
        delete nv_filters[ic];
        nv_filters[ic] = GetImageFilter(Helium::MIP_FILTER_CUBIC);
    }

    Image* overlay_tex = new Image(m_Width, m_Height, m_NativeFormat);

    nv::ScaleImage( blur_tex->m_Channels[0][R],
        blur_tex->m_Width,
        blur_tex->m_Height,
        overlay_tex->GetFacePtr(0, R),
        overlay_tex->m_Width,
        overlay_tex->m_Height,
        nv_filters,
        ConvertIGWrapModeToNV(u_wrap_mode),
        ConvertIGWrapModeToNV(v_wrap_mode) );

    delete blur_tex;

    for (uint32_t ic = 0; ic < 4; ic++)
        delete nv_filters[ic];

    overlay_tex->InvertColors();
    overlay_tex->BlendImageFace(this, 0.5f, NULL, face, 0);
    overlay_tex->DoubleContrast();

    OverlayBlendImageFace(this, overlay_tex, channel_mask, face, face, 0);

    delete overlay_tex;
}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
void  Image::FilterImage(const PostMipImageFilter*  filters,
                         const Image*             src_tex,
                         uint32_t                        dst_face,
                         uint32_t                        src_face,
                         uint32_t                        mip_index)
{
    uint32_t       kernel_sizes[4]      = {    0,    0,    0,    0 };

    float32_t       center_weight[4]     = { 1.0f, 1.0f, 1.0f, 1.0f };
    float32_t       corner_weight[4]     = { 0.0f, 0.0f, 0.0f, 0.0f };
    float32_t       side_weight[4]       = { 0.0f, 0.0f, 0.0f, 0.0f };
    float32_t       lerp_coef[4]         = { 0.0f, 0.0f, 0.0f, 0.0f };

    const uint8_t* color_curves[4]      = { NULL, NULL, NULL, NULL };

    for(uint32_t i = 0; i < 4; ++i)
    {
        switch(filters[i])
        {
        case IMAGE_FILTER_LIGHTER:
            {
                color_curves[i]  =  c_lighten;
                lerp_coef[i]     =  1.0f;
            }
            break;

        case IMAGE_FILTER_DARKER:
            {
                color_curves[i]  =  c_darken;
                lerp_coef[i]     =  1.0f;
            }
            break;

        case IMAGE_FILTER_MORE_CONTRAST:
            {
                color_curves[i]  =  c_more_contrast;
                lerp_coef[i]     =  1.0f;
            }
            break;

        case IMAGE_FILTER_LESS_CONTRAST:
            {
                color_curves[i]  =  c_less_contrast;
                lerp_coef[i]     =  1.0f;
            }
            break;

        case IMAGE_FILTER_SMOOTH:
            {
                kernel_sizes[i]   =  8;

                center_weight[i]  = 12.0f;
                corner_weight[i]  =  1.0f;
                side_weight[i]    =  2.0f;
                lerp_coef[i]      =  1.0f;
            }
            break;

        case IMAGE_FILTER_SHARPEN_GRADUAL:
            {
                kernel_sizes[i]   =  8;

                center_weight[i]  = 23.0f + c_sharpen_gradual[mip_index];  // less sharpening for initial mips
                corner_weight[i]  = -1.0f;
                side_weight[i]    = -2.0f;
                lerp_coef[i]      =  1.0f;
            }
            break;

        case IMAGE_FILTER_SHARPEN1X:
            {
                kernel_sizes[i]   =  8;

                center_weight[i]  = 23.0f;
                corner_weight[i]  = -1.0f;
                side_weight[i]    = -2.0f;
                lerp_coef[i]      =  1.0f;
            }
            break;

        case IMAGE_FILTER_SHARPEN2X:
            {
                kernel_sizes[i]   =  8;

                center_weight[i]  = 19.0f;
                corner_weight[i]  = -1.0f;
                side_weight[i]    = -2.0f;
                lerp_coef[i]      =  1.0f;
            }
            break;

        case IMAGE_FILTER_SHARPEN3X:
            {
                kernel_sizes[i]   =  8;

                center_weight[i]  = 17.0f;
                corner_weight[i]  = -1.0f;
                side_weight[i]    = -2.0f;
                lerp_coef[i]      =  1.0f;
            }
            break;
        }
    }

    //Setup
    float32_t     src[4];
    float32_t     final[4];
    float32_t     channels[4];
    float32_t     total_weights[4];

    const int32_t read_offsets[8][2]  = {  {-1,-1}, { 0,-1}, { 1,-1},
    {-1, 0},          { 1, 0},
    {-1, 1}, { 0, 1}, { 1, 1} };

    float32_t read_weights[8][4];
    uint32_t kernel_size       = 0;

    for(uint32_t i = 0; i < 4; ++i)
    {
        read_weights[0][i] =  corner_weight[i];  read_weights[1][i] = side_weight[i]; read_weights[2][i] = corner_weight[i];
        read_weights[3][i] =  side_weight[i];                                         read_weights[4][i] = side_weight[i];
        read_weights[5][i] =  corner_weight[i];  read_weights[6][i] = side_weight[i]; read_weights[7][i] = corner_weight[i];
        kernel_size        =  MAX(kernel_sizes[i], kernel_size);
    }

    const  int32_t y_min_limit  = 0;
    const  int32_t x_min_limit  = 0;

    const  int32_t y_max_limit  = m_Height - 1;
    const  int32_t x_max_limit  = m_Width  - 1;

    for(int32_t y = 0; y < (int32_t)m_Height; ++y)
    {
        for(int32_t x = 0; x < (int32_t)m_Width; ++x)
        {
            //Center pixel
            src_tex->Read(x, y, src[R], src[G], src[B], src[A], src_face);

            //Do some curve evaluation
            src[R]            = CurveEvaluate(src[R], color_curves[R]);
            src[G]            = CurveEvaluate(src[G], color_curves[G]);
            src[B]            = CurveEvaluate(src[B], color_curves[B]);
            src[A]            = CurveEvaluate(src[A], color_curves[A]);

            //Scale the colors
            final[R]          = src[R]*center_weight[R];
            final[G]          = src[G]*center_weight[G];
            final[B]          = src[B]*center_weight[B];
            final[A]          = src[A]*center_weight[A];

            //Total weights
            total_weights[R]  =  center_weight[R];
            total_weights[G]  =  center_weight[G];
            total_weights[B]  =  center_weight[B];
            total_weights[A]  =  center_weight[A];

            //Neighboring pixels
            for(uint32_t samp_idx = 0; samp_idx < kernel_size; ++samp_idx)
            {
                src_tex->Read(nv::clamp(x + read_offsets[samp_idx][0], x_min_limit, x_max_limit),
                    nv::clamp(y + read_offsets[samp_idx][1], y_min_limit, y_max_limit),
                    channels[R], channels[G], channels[B], channels[A],
                    src_face);

                final[R]  +=  channels[R]*read_weights[samp_idx][R];
                final[G]  +=  channels[G]*read_weights[samp_idx][G];
                final[B]  +=  channels[B]*read_weights[samp_idx][B];
                final[A]  +=  channels[A]*read_weights[samp_idx][A];

                total_weights[R] +=  read_weights[samp_idx][R];
                total_weights[G] +=  read_weights[samp_idx][G];
                total_weights[B] +=  read_weights[samp_idx][B];
                total_weights[A] +=  read_weights[samp_idx][A];
            }

            final[R] = nv::clamp(final[R]/total_weights[R], 0.0f, 1.0f);
            final[G] = nv::clamp(final[G]/total_weights[G], 0.0f, 1.0f);
            final[B] = nv::clamp(final[B]/total_weights[B], 0.0f, 1.0f);
            final[A] = nv::clamp(final[A]/total_weights[A], 0.0f, 1.0f);

            final[R] = nv::lerp(src[R], final[R], lerp_coef[R]);
            final[G] = nv::lerp(src[G], final[G], lerp_coef[G]);
            final[B] = nv::lerp(src[B], final[B], lerp_coef[B]);
            final[A] = nv::lerp(src[A], final[A], lerp_coef[A]);

            Write(x, y, final[R], final[G], final[B], final[A], dst_face);
        }
    }
}
