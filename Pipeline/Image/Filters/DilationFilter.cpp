#include "DilationFilter.h"

using namespace Helium;

////////////////////////////////////////////////////////////////////////////////////////////////
//
// Dilation filter
//
////////////////////////////////////////////////////////////////////////////////////////////////

#if OPTIMIZE_DILATION_FILTER_EXECUTION_TIME

// Lower quality but faster...

#define FILTER_RADIUS_PER_PASS    6
const double g_GaussianWeights[] =
{
    0.199471,
    0.176033,
    0.120985,
    0.064759,
    0.026995,
    0.008764,
    0.002216,
};

#else

// Better quality...

#define FILTER_RADIUS_PER_PASS    3
const double g_GaussianWeights[] =
{
    0.2888f,
    0.2232f,
    0.1034f,
    0.0288f,
};

#endif

HELIUM_COMPILE_ASSERT( sizeof(g_GaussianWeights)/sizeof(g_GaussianWeights[0]) == FILTER_RADIUS_PER_PASS + 1 );

DilationFilter::DilationFilter(const tchar_t* inputfile, const tchar_t* outputfile, unsigned int xres, unsigned int yres, unsigned int flags, bool smoothSeams) : ImageFilter(1, flags), m_smoothSeams(smoothSeams)
{
    ImageFilter::outputPath = outputfile;

    bool convert_to_linear = true;
    input = Helium::Image::LoadFile(inputfile, convert_to_linear, NULL);
    if (!input)
    {
        std::cerr << "Unable to open input file: " << inputfile << std::endl;
        return;
    }

    outputFormat = input->m_NativeFormat;

    if (xres == 0)
        xres = input->m_Width;
    if (yres == 0)
        yres = input->m_Height;

    Helium::Image *scaled_input = input->ScaleImage(xres, yres, Helium::CF_RGBAFLOATMAP, Helium::MIP_FILTER_GAUSSIAN);
    delete input;
    input = scaled_input;

    if(opFlags & NORMAL_NORMALIZE)
        Normalize(input);

    output = new Helium::Image(xres, yres, Helium::CF_RGBAFLOATMAP);
}


void DilationFilter::filter(void)
{
    // resize input image to match output before filtering

    //
    // define null pixels using the alpha channel:
    //
    //  0.0 = not dilated yet
    //  0.5 = currently being dilated
    //  0.9 = newly created valid texels - don't touch
    //  1.0 = original valid texels - don't touch
    //

    for( uint32_t y = 0; y < input->m_Height; ++y )
    {
        for( uint32_t x = 0; x < input->m_Width; ++x )
        {
            float r, g, b, a;
            input->Read(x, y, r, g, b, a);
            Vector4 color(r, g, b, a);
            if(!isNullPixel(color))
            {
                input->Write(x, y, r, g, b, 1.f);
            }
        }
    }

    bool finished = false;
    while(!finished)
    {
        finished = DilateVertical(output, input);
        finished |= DilateHorizontal(input, output);
    }

    if(opFlags & NORMAL_NORMALIZE)
        Normalize(input);

    // clear out alpha channel to aid potential compression
    // alternatively, we may want to leave a mask of the original valid texels...
    for( uint32_t y = 0; y < input->m_Height; ++y )
    {
        for( uint32_t x = 0; x < input->m_Width; ++x )
        {
            float r, g, b, a;
            input->Read(x, y, r, g, b, a);
            input->Write(x, y, r, g, b, 0.f);
        }
    }

    // point output to the final result
    delete output;
    output = input;
    input = NULL;

    output->m_NativeFormat = outputFormat;
}

bool DilationFilter::DilateHorizontal(Helium::Image *dst, Helium::Image *src)
{
    bool finished = true;

    for( uint32_t y = 0; y < dst->m_Height; ++y )
    {
        for( uint32_t x = 0; x < dst->m_Width; ++x )
        {
            float r, g, b, a;
            src->Read(x, y, r, g, b, a);

            if( a < 0.999f )
            {
                double total_r = 0.0;
                double total_g = 0.0;
                double total_b = 0.0;
                double total_a = 0.0;
                double total_weight = 0.0;

                for( int dx = int(x) - FILTER_RADIUS_PER_PASS; dx <= int(x) + FILTER_RADIUS_PER_PASS; ++dx )
                {
                    if( dx >= 0 && dx < int(dst->m_Width) )
                    {
                        src->Read(uint32_t(dx), y, r, g, b, a);
                        if( a > 0.4999f )
                        {
                            int weight_idx = int(x) - dx;
                            if(weight_idx < 0) weight_idx = -weight_idx;
                            double weight = g_GaussianWeights[weight_idx];

                            total_r += double(r) * weight;
                            total_g += double(g) * weight;
                            total_b += double(b) * weight;
                            total_weight += weight;
                        }
                    }
                }

                if( total_weight > 0.0 )
                {
                    double mult = (1.0 / total_weight);
                    r = float(total_r * mult);
                    g = float(total_g * mult);
                    b = float(total_b * mult);
                    a = m_smoothSeams ? 0.9f : 1.f;
                }
                else
                {
                    finished = false;
                }
            }
            dst->Write(x, y, r, g, b, a);
        }
    }
    return finished;
}

bool DilationFilter::DilateVertical(Helium::Image *dst, Helium::Image *src)
{
    bool finished = true;

    for( uint32_t y = 0; y < dst->m_Height; ++y )
    {
        for( uint32_t x = 0; x < dst->m_Width; ++x )
        {
            float r, g, b, a;
            src->Read(x, y, r, g, b, a);

            if( a < 0.999f )
            {
                double total_r = 0.0;
                double total_g = 0.0;
                double total_b = 0.0;
                double total_a = 0.0;
                double total_weight = 0.0;

                for( int dy = int(y) - FILTER_RADIUS_PER_PASS; dy <= int(y) + FILTER_RADIUS_PER_PASS; ++dy )
                {
                    if( dy >= 0 && dy < int(dst->m_Height) )
                    {
                        src->Read(x, uint32_t(dy), r, g, b, a);
                        if( a > 0.4999f )
                        {
                            int weight_idx = int(y) - dy;
                            if(weight_idx < 0) weight_idx = -weight_idx;
                            double weight = g_GaussianWeights[weight_idx];

                            total_r += double(r) * weight;
                            total_g += double(g) * weight;
                            total_b += double(b) * weight;
                            total_weight += weight;
                        }
                    }
                }

                if( total_weight > 0.0 )
                {
                    double mult = (1.0 / total_weight);
                    r = float(total_r * mult);
                    g = float(total_g * mult);
                    b = float(total_b * mult);
                    a = m_smoothSeams ? 0.9f : 1.f;
                }
                else
                {
                    finished = false;
                }
            }
            dst->Write(x, y, r, g, b, a);
        }
    }
    return finished;
}

void DilationFilter::Normalize(Helium::Image *tex)
{
    HELIUM_ASSERT(tex)

        for( uint32_t y = 0; y < tex->m_Height; ++y )
        {
            for( uint32_t x = 0; x < tex->m_Width; ++x )
            {
                float r, g, b, a;
                tex->Read(x, y, r, g, b, a);
                if(a > 0.f)
                {
                    Vector4 color(r, g, b, 0.f);
                    normalizeNormal(color);
                    tex->Write(x, y, color.x, color.y, color.z, a);
                }
            }
        }
}