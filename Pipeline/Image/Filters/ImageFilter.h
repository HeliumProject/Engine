////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ImageFilter.h
//
//  Written by: Jeff Chan
//
//  Image filter classes which do smart filtering of texture images
//
////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <iostream>

#include "Math/FpuVector2.h"
#include "Math/FpuVector3.h"
#include "Math/FpuVector4.h"

#include "Pipeline/Image/Image.h"

#define NORMAL_NORMALIZE (0x1)

namespace Helium
{
  //
  // Base class for image filtering that handles basic file and pixel operations and pixel iteration.
  // Subclasses define the filtering operation for each pixel.
  //

  class PIPELINE_API ImageFilter
  {
  public:
    ImageFilter(unsigned int width, unsigned int flags);

    // Reads input file and initializes output to be of xres by yres resolution. If xres or yres is zero, use corresponding
    // resolution from input. The filter width is specified by width and flags specify if the output should be
    // normalized for normals, etc.
    ImageFilter(const tchar_t* inputfile, const tchar_t* outputfile, unsigned int xres, unsigned int yres, unsigned int width, unsigned int flags);

    virtual ~ImageFilter(void);

    // Writes output to outputPath
    void writeOutput(void);

    // Does the filtering
    virtual void filter(void);

  protected:
    // Checks if the pixel specified by color is considered an undefined pixel
    bool isNullPixel(const Vector4& color) const;

    Helium::Image*       input;            // input image
    Helium::Image*       output;           // output image

    unsigned int        filterWidth;        // filter width
    unsigned int        opFlags;              // flags for normalizing, etc.

    // Normalizes pixel specified by color for normal maps
    void normalizeNormal(Vector4& color) const;

    // Given a image coordinate, generate filtered pixel. Overriden by subclasses which define the specific
    // filtering operation.
    virtual Vector4 generateFilteredPixel(unsigned int x, unsigned int y) = NULL;

    const tchar_t*       outputPath;         // file to write output image to
  };


  //
  // Filter which fills in undefined pixels with the average of the nearest valid pixels. Performs search up to filter width
  // from the undefined pixels.
  //

  class PIPELINE_API FillImageFilter : public ImageFilter
  {
  public:
    // Reads input file and initializes output to be of xres by yres resolution. If xres or yres is zero, use corresponding
    // resolution from input. The filter width is specified by width and flags specify if the output should be
    // normalized for normals, etc.
    FillImageFilter(const tchar_t* inputfile, const tchar_t* outputfile, unsigned int xres, unsigned int yres,
      unsigned int width, unsigned int flags) : ImageFilter(inputfile, outputfile, xres, yres, width, flags) {}

    virtual ~FillImageFilter(void) {}

  private:
    // Given a image coordinate, generate filtered pixel.
    virtual Vector4 generateFilteredPixel(unsigned int x, unsigned int y);

    // Accumulates valid pixel samples given a yoffset and absolute xoffset from basex and basey
    void accumulateSamples(int basex, int basey, int yoffset, int absxoffset, Vector4& accumcolor, unsigned int& numvalid) const;
  };


  //
  // A box filter which only filters pixels in the filter base that are contiguous with pixel at the center of the filter
  // base or if invalid, the closest valid pixel to the center. Pixels which are separated by undefined pixels are not used
  // in the filter.
  //

  class PIPELINE_API BoxImageFilter : public ImageFilter
  {
  public:
    // Reads input file and initializes output to be of xres by yres resolution. If xres or yres is zero, use corresponding
    // resolution from input. The filter width is specified by width and flags specify if the output should be
    // normalized for normals, etc.
    BoxImageFilter(const tchar_t* inputfile, const tchar_t* outputfile, unsigned int xres, unsigned int yres,
      unsigned int width, unsigned int flags);

    virtual ~BoxImageFilter(void);

  private:
    // Given a image coordinate, generate filtered pixel.
    virtual Vector4 generateFilteredPixel(unsigned int x, unsigned int y);

    // Fill in helper arrays for pixel at x and y
    void fillValues(int x, int y);

    // Detects which pixels are separated from the pixel at the center of the filter base by undefined pixels or from the closest
    // valid pixel to the center if it is invalid
    void applyPrefilter(void);

    // Actually does the box filter based on valid pixels only
    Vector4 applyBoxFilter(void) const;

    // A bunch of helper functions which checks if pixels are separated from the pixel at the center of the
    // filter base by undefined pixels or from the closest valid pixel to the center if it is invalid
    void checkSamples(int yoffset, int absxoffset, int centerx, int centery);
    void checkYOffset(int xoffset, int currentx, int currenty, int checky, unsigned int index);
    void checkXOffset(int currentx, int currenty, int checkx, int checky, unsigned int index);
    void checkY(int currentx, int checky, unsigned int index);
    void checkX(int currenty, int checkx, unsigned int index);
    void checkSamples(int currentx, int currenty, int width);

    // Helper struct
    struct VecInt2
    {
      int x;
      int y;
    };

    // Finds nearest valid pixel to center. Picks first one if more than one.
    VecInt2 findValid(int currentx, int currenty);
    VecInt2 findValid(int yoffset, int absxoffset, int centerx, int centery);

    // Gets linear index based on coordinates x and y for local arrays
    unsigned int getIndex(int x, int y) const { return (numRows * y + x); }

    bool*           valid;            // array of pixel valid indicators
    Vector4*        values;           // array of pixel values

    unsigned int        numRows;          // number of rows of pixels based on filter width
    unsigned int        numElems;         // number of pixels based on filter width
  };
}