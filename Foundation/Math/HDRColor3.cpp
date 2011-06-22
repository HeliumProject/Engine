#include "FoundationPch.h"
#include "HDRColor3.h"
#include "HDRColor4.h"

using namespace Helium;

HDRColor3& HDRColor3::operator=( const HDRColor4& v )
{
    r = v.r;
    g = v.g;
    b = v.b;
    s = v.s;
    return *this;
}

HDRColor3::operator HDRColor4()
{
    return HDRColor4( r, g, b, 255, s );
}