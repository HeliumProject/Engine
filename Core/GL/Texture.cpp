#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "Image.h"
#include "Util.h"

using namespace Core;

GLuint Core::LoadTexture(const char *filename, GLboolean alpha, GLboolean repeat, GLboolean filtering, GLboolean mipmaps)
{
  GLuint tex = 0;
  GLubyte *data = NULL;
  int type, width, height, pixelsize;
  int filter_min, filter_mag;
  double xPow2, yPow2;
  int ixPow2, iyPow2;
  int xSize2, ySize2;
  
  GLint gl_max_texture_size = 0;
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &gl_max_texture_size);
  
  //TODO: load texture into data
  Warning("LoadTexture() failed: Unable to load texture from %s!", filename);

  if ( data )
  {
    switch(type)
    {
      case GL_RGBA:
      case GL_BGRA:
        pixelsize = 4;
        break;
        
      case GL_RGB:
#ifdef Core_OPENGL
      case GL_BGR:
#endif
        pixelsize = 3;
        break;
        
      case GL_LUMINANCE:
        pixelsize = 1;
        break;
        
      default:
        FatalError( "LoadTexture(): unknown type 0x%x", type);
        pixelsize = 0;
        break;
    }
    
    if((pixelsize*width) % 4 == 0)
    {
      glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    }
    else
    {
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    }
    
    xSize2 = width;
    if (xSize2 > gl_max_texture_size)
    {
      xSize2 = gl_max_texture_size;
    }
    ySize2 = height;
    if (ySize2 > gl_max_texture_size)
    {
      ySize2 = gl_max_texture_size;
    }
    
    /* scale image to power of 2 in height and width */
    xPow2 = log((double)xSize2) / log(2.0);
    yPow2 = log((double)ySize2) / log(2.0);
    
    ixPow2 = (int)xPow2;
    iyPow2 = (int)yPow2;
    
    if (xPow2 != (double)ixPow2)
    {
      ixPow2++;
    }
    if (yPow2 != (double)iyPow2)
    {
      iyPow2++;
    }
    
    xSize2 = 1 << ixPow2;
    ySize2 = 1 << iyPow2;
    
    DBG(Warning("gl_max_texture_size=%d / width=%d / xSize2=%d / height=%d / ySize2 = %d", gl_max_texture_size, width, xSize2, height, ySize2));
    if((width != xSize2) || (height != ySize2))
    {
#ifdef Core_OPENGLES
      DBG(Warning("non power of 2 texture"));
#endif
      
#ifdef Core_OPENGL
      /* TODO: use glTexSubImage2D instead */
      DBG(Warning("scaling texture"));
      rdata = (GLubyte*)malloc(sizeof(GLubyte) * xSize2 * ySize2 * pixelsize);
      if (!rdata)
        return 0;
      
      retval = gluScaleImage(type, width, height,
                             GL_UNSIGNED_BYTE, data,
                             xSize2, ySize2, GL_UNSIGNED_BYTE,
                             rdata);
      
      free(data);
      data = rdata;
#endif
    }
    
    glGenTextures(1, &tex);		/* Generate texture ID */
    glBindTexture(GL_TEXTURE_2D, tex);
    DBG(Warning("building texture %d",tex));
    
    if(filtering)
    {
      filter_min = (mipmaps) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;
      filter_mag = GL_LINEAR;
    }
    else
    {
      filter_min = (mipmaps) ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST;
      filter_mag = GL_NEAREST;
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter_min);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter_mag);
    
    GLint wrap = GL_REPEAT;
    
#ifdef Core_OPENGL
    if (!repeat)
      wrap = GL_CLAMP;
#endif
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
    
    if(mipmaps)
    {
      glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    }
    
    /* Clean up and return the texture ID */
    free(data);
  }
  
  return tex;
}
