#pragma once

#include "ImageFilter.h"

namespace IG
{
  class TEXTURE_API DilationFilter : public ImageFilter
  {
  public:
    DilationFilter(const char* inputfile, const char* outputfile, unsigned int xres, unsigned int yres, unsigned int flags, bool smoothSeams);

    virtual ~DilationFilter(void) {}

    virtual void filter(void);

  private:
    bool DilateHorizontal(IG::Texture *dst, IG::Texture *src);
    bool DilateVertical(IG::Texture *dst, IG::Texture *src);
    void Normalize(IG::Texture *input);

    // was pure virtual in base class...
    virtual Math::Vector4 generateFilteredPixel(unsigned int x, unsigned int y) { return Math::Vector4::Zero; };

    IG::ColorFormat outputFormat;
    bool m_smoothSeams;
  };
}