////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ImageFilter.cpp
//
//  Written by: Jeff Chan
//
//  Image filter classes which do smart filtering of texture images
//
////////////////////////////////////////////////////////////////////////////////////////////////
#include "ImageFilter.h"

using namespace IG;

ImageFilter::ImageFilter(unsigned int width, unsigned int flags)
: input(NULL)
, output(NULL)
, filterWidth(width)
, opFlags(flags)
, outputPath(NULL)
{

}

// Reads input file and initializes output to be of xres by yres resolution. If xres or yres is zero, use corresponding
// resolution from input. The filter width is specified by width and flags specify if the output should be
// normalized for normals, etc.

ImageFilter::ImageFilter(const char* inputfile, const char* outputfile, unsigned int xres, unsigned int yres, unsigned int width, unsigned int flags)
: output(NULL)
, filterWidth(width)
, opFlags(flags)
, outputPath(outputfile)
{
  input = IG::Texture::LoadFile(inputfile, false, NULL);    // gamma correction????

  if (!input)
  {
    std::cerr << "Unable to open input file: " << inputfile << std::endl;
    return;
  }

  if (xres > input->m_Width || yres > input->m_Height)
  {
    std::cerr << "Cannot have output resolution (" << xres << ", " << yres << ") greater than input resolution (";
    std::cerr << input->m_Width << ", " << input->m_Height << ")!" << std::endl;
  }

  if (xres == 0)
  {
    xres = input->m_Width;
  }

  if (yres == 0)
  {
    yres = input->m_Height;
  }

  output = new IG::Texture(xres, yres, input->m_NativeFormat);
}

ImageFilter::~ImageFilter(void)
{
  delete input;
  delete output;
  input = NULL;
  output = NULL;
}

// Writes output to outputPath if there is valid output

void ImageFilter::writeOutput(void)
{
  if (output)
  {
    if (!output->WriteTGA(outputPath, 0, false))   // gamma correction????
    {
      std::cerr << "Unable to write output file: " << outputPath << std::endl;
    }
  }
}

// Iterates through output pixels and calculates the center of the filter base in the input. Calls generateFilteredPixel
// with the center to get filtered pixel to write to output.

void ImageFilter::filter(void)
{
  if (input && output)
  {
    float skipX = (float) input->m_Width / output->m_Width;
    float skipY = (float) input->m_Height / output->m_Height;
    float halfskipX = skipX / 2.0f;
    float halfskipY = skipY / 2.0f;

    for (unsigned int y = 0; y < output->m_Height; y++)
    {
      unsigned int lookupY = (unsigned int) ((float) y * skipY + halfskipY);
      for (unsigned int x = 0; x < output->m_Width; x++)
      {
        unsigned int lookupX = (unsigned int) ((float) x * skipX + halfskipX);

        // Adjustments if the center is between two pixels in each dimension.
        // Alternately selects between each pixel to use as the center
        unsigned int adjustX = x & 1;
        if (halfskipX < 1.0f || floor(halfskipX) != halfskipX)
        {
          adjustX = 0;
        }

        unsigned int adjustY = (y + x) & 1;
        if (halfskipY < 1.0f || floor(halfskipY) != halfskipY)
        {
          adjustY = 0;
        }

        Math::Vector4 color = generateFilteredPixel(lookupX - adjustX, lookupY - adjustY);
        if (opFlags & NORMAL_NORMALIZE)
        {
          if (!isNullPixel(color))
          {
            normalizeNormal(color);
          }
        }

        output->Write(x, y, color.x, color.y, color.z, color.w);
      }
    }
  }
}

// Checks if the pixel specified by color is considered an undefined pixel.

bool ImageFilter::isNullPixel(const Math::Vector4& color) const
{
  if (color.w != 0 || color.x != 0 || color.y != 0 || color.z != 0)
  {
    return false;
  }
  else
  {
    return true;
  }
}

// Normalizes pixel specified by color for normal maps. The pixel is scaled and offset to [-1.0, 1.0] before
// normalizing and then returned to [0.0, 1.0] after normalizing.

void ImageFilter::normalizeNormal(Math::Vector4& color) const
{
  color.x = 2.0f * color.x - 1.0f;
  color.y = 2.0f * color.y - 1.0f;
  color.z = 2.0f * color.z - 1.0f;

  float lensq = color.x * color.x + color.y * color.y + color.z + color.z;
  float len = sqrt(lensq);

  color.x = (color.x / len + 1.0f) * 0.5f;
  color.y = (color.y / len + 1.0f) * 0.5f;
  color.z = (color.z / len + 1.0f) * 0.5f;
}

// Returns the pixel at the x and y coordinate if it is valid, otherwise searches in ever increasing radius from x and y
// up until filter width for valid pixels. Returns the average of the closest valid pixels. The search pattern for a
// filter width of 3 is:
// 5 4 3 4 5
// 4 2 1 2 4
// 3 1 0 1 3
// 4 2 1 2 4
// 5 4 3 4 5

Math::Vector4 FillImageFilter::generateFilteredPixel(unsigned int x, unsigned int y)
{
  Math::Vector4 color;
  input->Read(x, y, color.x, color.y, color.z, color.w);

  if (filterWidth == 1 || !isNullPixel(color))
  {
    return color;
  }

  // Only search up to a radius of filter width
  for (int maxoffset = 1; maxoffset < (int) filterWidth; maxoffset++)
  {
    int maxcombinedoffset = maxoffset * 2;

    // Looks at pixels which are closer before pixels which are further in a particular ring of pixels.
    // Accumulates the valid pixels with a particular combined offset and returns the average color if there
    // are any valid pixels.
    for (int combinedoffset = maxoffset; combinedoffset <= maxcombinedoffset; combinedoffset++)
    {
      Math::Vector4 accumcolor;
      unsigned int numvalid = 0;

      for (int yoffset = -maxoffset; yoffset <= maxoffset; yoffset += maxoffset)
      {
        int absyoffset = abs(yoffset);
        int absxoffset = combinedoffset - absyoffset;

        if (absxoffset <= maxoffset)
        {
          accumulateSamples((int) x, (int) y, yoffset, absxoffset, accumcolor, numvalid);
        }
        else
        {
          absxoffset = maxoffset;
          absyoffset = combinedoffset - absxoffset;
          if (absyoffset < maxoffset)
          {
            int incyoffset = absyoffset * 2;

            for (int newyoffset = -absyoffset; newyoffset < absyoffset; newyoffset += incyoffset)
            {
              accumulateSamples((int) x, (int) y, newyoffset, absxoffset, accumcolor, numvalid);
            }
          }
        }
      }

      if (numvalid > 0)
      {
        color = accumcolor / ((float) numvalid);
        return color;
      }
    }
  }

  return color;
}

// Accumulates valid pixel samples given a yoffset and absolute xoffset from basex and basey.

void FillImageFilter::accumulateSamples(int basex, int basey, int yoffset, int absxoffset, Math::Vector4& accumcolor, unsigned int& numvalid) const
{
  int currenty = basey + yoffset;

  // Only proceed if currenty is in the input image
  if (currenty >= 0 && currenty < (int) input->m_Height)
  {
    int incxoffset;
    if (absxoffset > 0)
    {
      incxoffset = absxoffset * 2;
    }
    else
    {
      incxoffset = 1;
    }

    for (int xoffset = -absxoffset; xoffset <= absxoffset; xoffset += incxoffset)
    {
      int currentx = basex + xoffset;

      // Only proceed if currentx is in the input image
      if (currentx >= 0 && currentx < (int) input->m_Width)
      {
        Math::Vector4 color;

        input->Read((u32) currentx, (u32) currenty, color.x, color.y, color.z, color.w);
        if (!isNullPixel(color))
        {
          accumcolor += color;
          numvalid++;
        }
      }
    }
  }
}

// Creates arrays for holding the stamp which is in the filter base

BoxImageFilter::BoxImageFilter(const char* inputfile, const char* outputfile, unsigned int xres, unsigned int yres, unsigned int width, unsigned int flags)
: ImageFilter (inputfile, outputfile, xres, yres, width, flags)
{
  numRows = width * 2 - 1;
  numElems = numRows * numRows;

  valid = new bool[numElems];
  values = new Math::Vector4[numElems];
}

BoxImageFilter::~BoxImageFilter(void)
{
  delete [] valid;
  delete [] values;
}

// Generates filtered pixel by filling helper arrays with pixels in the filter base, removing pixels separated from the
// center of the filter base by invalid pixels or from the closest valid pixel to the center if it is invalid and averaging
// valid pixels.

Math::Vector4 BoxImageFilter::generateFilteredPixel(unsigned int x, unsigned int y)
{
  fillValues((int) x, (int) y);

  applyPrefilter();

  Math::Vector4 color = applyBoxFilter();

  return color;
}

// Fills in helper arrays with pixels in the filter base centered at x and y.

void BoxImageFilter::fillValues(int x, int y)
{
  int width = (int) filterWidth - 1;

  for (int yoffset = -width, accumy = 0; accumy < (int) numRows; yoffset++, accumy++)
  {
    int currenty = y + yoffset;

    if (currenty >= 0 && currenty < (int) input->m_Height)
    {
      for (int xoffset = -width, accumx = 0; accumx < (int) numRows; xoffset++, accumx++)
      {
        int currentx = x + xoffset;
        unsigned int index = getIndex(accumx, accumy);

        if (currentx >= 0 && currentx < (int) input->m_Width)
        {
          input->Read((unsigned int) currentx, (unsigned int) currenty, values[index].x, values[index].y, values[index].z, values[index].w);

          if (!isNullPixel(values[index]))
          {
            valid[index] = true;
          }
          else
          {
            valid[index] = false;
          }
        }
        else
        {
          valid[index] = false;
        }
      }
    }
    else
    {
      for (int accumx = 0; accumx < (int) numRows; accumx++)
      {
        unsigned int index = getIndex(accumx, accumy);

        valid[index] = false;
      }
    }
  }
}

// Checks if pixels in the filter base are separated from the pixel at the center of the filter base by invalid pixels.
// Finds nearest valid pixel to the center pixel if it is invalid and checks if pixels in the filter base are separated
// from that pixel. Also, skips the checks if the filter width is less than 3 if center pixel is valid or if the filter
// width is less than 2 if the center pixel is invalid since no pixel in the filter base can be separated from the
// center or nearest valid pixel by invalid pixels in these cases.

void BoxImageFilter::applyPrefilter(void)
{
  int center = (int) filterWidth - 1;
  unsigned int index = getIndex(center, center);

  if (filterWidth > 2)
  {
    if (valid[index])
    {
      checkSamples(center, center, filterWidth);
    }
    else
    {
      BoxImageFilter::VecInt2 nearestvalid = findValid(center, center);
      if (nearestvalid.x != -1)
      {
        int offset = abs(nearestvalid.x - center);
        offset = MAX(offset, abs(nearestvalid.y - center));
        checkSamples(nearestvalid.x, nearestvalid.y, filterWidth + offset);
      }
    }
  }
  else if (filterWidth > 1 && !valid[index])
  {
    BoxImageFilter::VecInt2 nearestvalid = findValid(center, center);
    if (nearestvalid.x != -1)
    {
      int offset = abs(nearestvalid.x - center);
      offset = MAX(offset, abs(nearestvalid.y - center));
      checkSamples(nearestvalid.x, nearestvalid.y, filterWidth + offset);
    }
  }
}

// Averages valid pixels

Math::Vector4 BoxImageFilter::applyBoxFilter(void) const
{
  Math::Vector4 color;
  unsigned int numvalid = 0;

  for (unsigned int i = 0; i < numElems; i++)
  {
    if (valid[i])
    {
      color += values[i];
      numvalid++;
    }
  }

  if (numvalid > 0)
  {
    color /= (float) numvalid;
  }

  return color;
}

// Checks if pixels at yoffset and absolute xoffset from center are separated from the center by invalid pixels.
// Pixels can use previously checked pixels to determine if they are separated from the center by invalid pixels.
// Each filter base can be divided into 4 quadrants. Depending on which quadrant a pixel falls into, the pixel checks
// the validity of 3 adjoining pixels which are closer to the center. If no valid pixels are found, the pixel is
// marked as invalid.

void BoxImageFilter::checkSamples(int yoffset, int absxoffset, int centerx, int centery)
{
  int currenty = centery + yoffset;

  if (currenty >= 0 && currenty < (int) numRows)
  {
    int incxoffset;
    if (absxoffset > 0)
    {
      incxoffset = absxoffset * 2;
    }
    else
    {
      incxoffset = 1;
    }

    for (int xoffset = -absxoffset; xoffset <= absxoffset; xoffset += incxoffset)
    {
      int currentx = centerx + xoffset;

      if (currentx >= 0 && currentx < (int) numRows)
      {
        unsigned int index = getIndex(currentx, currenty);

        // Only do checks if current pixel is valid
        if (valid[index])
        {
          if (yoffset < 0)
          {
            checkYOffset(xoffset, currentx, currenty, currenty + 1, index);
          }
          else if (yoffset > 0)
          {
            checkYOffset(xoffset, currentx, currenty, currenty - 1, index);
          }
          else
          {
            // Pixels which are in line with the center along the horizontal direction
            if (xoffset < 0)
            {
              checkX(currenty, currentx + 1, index);
            }
            else
            {
              checkX(currenty, currentx - 1, index);
            }
          }
        }
      }
    }
  }
}

// After pixels are divided between pixels which are above and below the center, divides pixels between
// pixels which are to the left and right of the center to bucket pixels into the appropriate quadrant.

void BoxImageFilter::checkYOffset(int xoffset, int currentx, int currenty, int checky, unsigned int index)
{
  if (xoffset < 0)
  {
    checkXOffset(currentx, currenty, currentx + 1, checky, index);
  }
  else if (xoffset > 0)
  {
    checkXOffset(currentx, currenty, currentx - 1, checky, index);
  }
  else
  {
    // Pixels which are in line with the center along the vertical direction
    checkY(currentx, checky, index);
  }
}

// After the quadrant is determined, checks the validity of the 3 adjoining pixels to determine if the
// pixel is valid.

void BoxImageFilter::checkXOffset(int currentx, int currenty, int checkx, int checky, unsigned int index)
{
  unsigned int checkindex = getIndex(currentx, checky);
  if (valid[checkindex])
    return;

  checkindex = getIndex(checkx, checky);
  if (valid[checkindex])
    return;

  checkindex = (checkx, currenty);
  if (valid[checkindex])
    return;

  valid[index] = false;
}

// Checks if the pixel is valid when the pixel is in line with the center along the vertical direction by
// checking the validity of the 3 adjoining pixel which are closer to the center.

void BoxImageFilter::checkY(int currentx, int checky, unsigned int index)
{
  unsigned int checkindex = getIndex(currentx, checky);
  if (valid[checkindex])
    return;

  int checkx = currentx - 1;
  if (checkx >= 0)
  {
    checkindex = getIndex(checkx, checky);
    if (valid[checkindex])
      return;
  }

  checkx = currentx + 1;
  if (checkx < (int) numRows)
  {
    checkindex = getIndex(checkx, checky);
    if (valid[checkindex])
      return;
  }

  valid[index] = false;
}

// Checks if the pixel is valid when the pixel is in line with the center along the horizontal direction by
// checking the validity of the 3 adjoining pixel which are closer to the center.

void BoxImageFilter::checkX(int currenty, int checkx, unsigned int index)
{
  unsigned int checkindex = getIndex(checkx, currenty);
  if (valid[checkindex])
    return;

  int checky = currenty - 1;
  if (checky >= 0)
  {
    checkindex = getIndex(checkx, checky);
    if (valid[checkindex])
      return;
  }

  checky = currenty + 1;
  if (checky < (int) numRows)
  {
    checkindex = getIndex(checkx, checky);
    if (valid[checkindex])
      return;
  }

  valid[index] = false;
}

// Checks if pixels in the filter base are separated from the current pixel by invalid pixels. Also, skips the checks if
// the width is less than 3 since no pixel in the filter base can be separated from the current pixel by invalid pixels
// in this case. Checks starting from the closest to the furthest pixels from the current pixel in the filter base. The
// further pixels can use the previously checked closer pixels to determine if they are separated from the current pixel
// by invalid pixels.

void BoxImageFilter::checkSamples(int currentx, int currenty, int width)
{
  // Only check up to a radius of width
  for (int maxoffset = 2; maxoffset < width; maxoffset++)
  {
    int maxcombinedoffset = maxoffset * 2;

    // Looks at pixels which are closer before pixels which are further in a particular ring of pixels
    for (int combinedoffset = maxoffset; combinedoffset <= maxcombinedoffset; combinedoffset++)
    {
      for (int yoffset = -maxoffset; yoffset <= maxoffset; yoffset += maxoffset)
      {
        int absyoffset = abs(yoffset);
        int absxoffset = combinedoffset - absyoffset;

        if (absxoffset <= maxoffset)
        {
          checkSamples(yoffset, absxoffset, currentx, currenty);
        }
        else
        {
          absxoffset = maxoffset;
          absyoffset = combinedoffset - absxoffset;
          if (absyoffset < maxoffset)
          {
            int incyoffset = absyoffset * 2;

            for (int newyoffset = -absyoffset; newyoffset < absyoffset; newyoffset += incyoffset)
            {
              checkSamples(newyoffset, absxoffset, currentx, currenty);
            }
          }
        }
      }
    }
  }
}

// Finds nearest valid pixel from current. Checks starting from the closest to the furthest pixels from the
// current pixel in the filter base. Returns first valid pixel in search.

BoxImageFilter::VecInt2 BoxImageFilter::findValid(int currentx, int currenty)
{
  BoxImageFilter::VecInt2 result;

  // Only search up to a radius of filter width
  for (int maxoffset = 1; maxoffset < (int) filterWidth; maxoffset++)
  {
    int maxcombinedoffset = maxoffset * 2;

    // Looks at pixels which are closer before pixels which are further in a particular ring of pixels.
    for (int combinedoffset = maxoffset; combinedoffset <= maxcombinedoffset; combinedoffset++)
    {
      for (int yoffset = -maxoffset; yoffset <= maxoffset; yoffset += maxoffset)
      {
        int absyoffset = abs(yoffset);
        int absxoffset = combinedoffset - absyoffset;

        if (absxoffset <= maxoffset)
        {
          result = findValid(yoffset, absxoffset, currentx, currenty);
          if (result.x != -1)
          {
            return result;
          }
        }
        else
        {
          absxoffset = maxoffset;
          absyoffset = combinedoffset - absxoffset;
          if (absyoffset < maxoffset)
          {
            int incyoffset = absyoffset * 2;

            for (int newyoffset = -absyoffset; newyoffset < absyoffset; newyoffset += incyoffset)
            {
              result = findValid(newyoffset, absxoffset, currentx, currenty);
              if (result.x != -1)
              {
                return result;
              }
            }
          }
        }
      }
    }
  }

  result.x = result.y = -1;
  return result;
}

// Checks if pixels at yoffset and absolute xoffset from center are valid. Returns first valid pixel in search.

BoxImageFilter::VecInt2 BoxImageFilter::findValid(int yoffset, int absxoffset, int centerx, int centery)
{
  BoxImageFilter::VecInt2 result;
  int currenty = centery + yoffset;

  int incxoffset;
  if (absxoffset > 0)
  {
    incxoffset = absxoffset * 2;
  }
  else
  {
    incxoffset = 1;
  }

  for (int xoffset = -absxoffset; xoffset <= absxoffset; xoffset += incxoffset)
  {
    int currentx = centerx + xoffset;
    unsigned int index = getIndex(currentx, currenty);

    if (valid[index])
    {
      result.x = currentx;
      result.y = currenty;
      return result;
    }
  }

  result.x = result.y = -1;
  return result;
}