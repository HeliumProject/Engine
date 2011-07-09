#pragma once

#include "ImageFilter.h"

namespace Helium
{
  class HELIUM_PIPELINE_API DilationFilter : public ImageFilter
  {
  public:
    DilationFilter(const tchar_t* inputfile, const tchar_t* outputfile, unsigned int xres, unsigned int yres, unsigned int flags, bool smoothSeams);

    virtual ~DilationFilter(void) {}

    virtual void filter(void);

  private:
    bool DilateHorizontal(Helium::Image *dst, Helium::Image *src);
    bool DilateVertical(Helium::Image *dst, Helium::Image *src);
    void Normalize(Helium::Image *input);

    // was pure virtual in base class...
    virtual Vector4 generateFilteredPixel(unsigned int x, unsigned int y) { return Vector4::Zero; };

    Helium::ColorFormat outputFormat;
    bool m_smoothSeams;
  };
}