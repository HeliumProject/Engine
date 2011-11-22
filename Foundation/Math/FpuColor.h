#pragma once

#include "Foundation/API.h"
#include "Foundation/Math/Common.h"

namespace Helium
{
    inline float SRGBToLinear(float srgb)
    {
        if(srgb <= 0.04045f)
            return srgb / 12.92f;
        else
            return powf((srgb + 0.055f) / 1.055f, 2.4f);
    }

    inline float LinearToSRGB(float linear)
    {
        if(linear <= 0.0031308f)
            return linear * 12.92f;
        else
            return 1.055f * powf(linear, 1.f / 2.4f) - 0.055f;
    }
}