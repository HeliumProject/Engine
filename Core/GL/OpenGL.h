#pragma once

#ifdef __APPLE__
# include "TargetConditionals.h"
# ifdef TARGET_OS_IPHONE
#  include <OpenGLES/ES1/gl.h>
#  include <OpenGLES/ES1/glext.h>
#  define Core_OPENGLES
# else
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
# define Core_OPENGL
# endif
#else
# include <GL/gl.h>
# include <GL/glu.h>
# define Core_OPENGL
#endif
