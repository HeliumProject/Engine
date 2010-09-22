#pragma once

#include "OpenGL.h"

#include "Platform/String.h"

namespace Helium
{
    namespace Core
    {
        namespace GL
        {
            GLuint LoadTexture( const tstring& filename, GLboolean alpha, GLboolean repeat, GLboolean filtering, GLboolean mipmaps );
        }
    }
}