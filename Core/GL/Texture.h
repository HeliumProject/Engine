#pragma once

#include "OpenGL.h"

namespace Core
{
  GLuint LoadTexture(const char *filename, GLboolean alpha, GLboolean repeat, GLboolean filtering, GLboolean mipmaps);
}