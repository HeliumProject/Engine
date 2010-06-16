#pragma once

#include "ImageFilter.h"

namespace Nocturnal
{
  class PIPELINE_API DilationFilter : public ImageFilter
  {
  public:
    DilationFilter(const char* inputfile, const char* outputfile, unsigned int xres, unsigned int yres, unsigned int flags, bool smoothSeams);

    virtual ~DilationFilter(void) {}

    virtual void filter(void);

  private:
    bool DilateHorizontal(Nocturnal::Texture *dst, Nocturnal::Texture *src);
    bool DilateVertical(Nocturnal::Texture *dst, Nocturnal::Texture *src);
    void Normalize(Nocturnal::Texture *input);

    // was pure virtual in base class...
    virtual Math::Vector4 generateFilteredPixel(unsigned int x, unsigned int y) { return Math::Vector4::Zero; };

    Nocturnal::ColorFormat outputFormat;
    bool m_smoothSeams;
  };
}