#pragma once

#include "ImageFilter.h"

namespace Helium
{
  class PIPELINE_API DilationFilter : public ImageFilter
  {
  public:
    DilationFilter(const tchar* inputfile, const tchar* outputfile, unsigned int xres, unsigned int yres, unsigned int flags, bool smoothSeams);

    virtual ~DilationFilter(void) {}

    virtual void filter(void);

  private:
    bool DilateHorizontal(Helium::Image *dst, Helium::Image *src);
    bool DilateVertical(Helium::Image *dst, Helium::Image *src);
    void Normalize(Helium::Image *input);

    // was pure virtual in base class...
    virtual Math::Vector4 generateFilteredPixel(unsigned int x, unsigned int y) { return Math::Vector4::Zero; };

    Helium::ColorFormat outputFormat;
    bool m_smoothSeams;
  };
}