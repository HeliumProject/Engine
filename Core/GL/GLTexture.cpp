#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <windows.h> // for gl to work on windows :(

#include "GLTexture.h"
#include "GLUtil.h"

#include "Foundation/Log.h"

using namespace Helium;

GLuint GL::LoadTexture( const tstring& filename, GLboolean alpha, GLboolean repeat, GLboolean filtering, GLboolean mipmaps )
{
    GLuint tex = 0;
    GLubyte *data = NULL;
    int type = 0, width = 0, height = 0, pixelsize = 0;
    int filter_min = 0, filter_mag = 0;
    double xPow2 = 0.0, yPow2 = 0.0;
    int ixPow2 = 0, iyPow2 = 0;
    int xSize2 = 0, ySize2 = 0;

    GLint gl_max_texture_size = 0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &gl_max_texture_size);

    //TODO: load texture into data
    Log::Warning( TXT( "LoadTexture() failed: Unable to load texture from %s!" ), filename.c_str() );

    if ( data )
    {
        switch(type)
        {
        case GL_RGBA:
            pixelsize = 4;
            break;

        case GL_RGB:
            pixelsize = 3;
            break;

        case GL_LUMINANCE:
            pixelsize = 1;
            break;

        default:
            Log::Error( TXT( "LoadTexture(): unknown type 0x%x" ), type);
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

        DBG( Log::Warning( TXT("gl_max_texture_size=%d / width=%d / xSize2=%d / height=%d / ySize2 = %d"), gl_max_texture_size, width, xSize2, height, ySize2) );
        if((width != xSize2) || (height != ySize2))
        {
#ifdef Core_OPENGLES
            DBG(Warning("non power of 2 texture"));
#endif

#ifdef Core_OPENGL
            /* TODO: use glTexSubImage2D instead */
            DBG( Log::Warning( TXT("scaling texture")) );
            GLubyte* rdata = (GLubyte*)malloc(sizeof(GLubyte) * xSize2 * ySize2 * pixelsize);
            if (!rdata)
                return 0;

            int retval = gluScaleImage(type, width, height,
                GL_UNSIGNED_BYTE, data,
                xSize2, ySize2, GL_UNSIGNED_BYTE,
                rdata);

            free(data);
            data = rdata;
#endif
        }

        glGenTextures(1, &tex);		/* Generate texture ID */
        glBindTexture(GL_TEXTURE_2D, tex);
        DBG( Log::Warning( TXT("building texture %d"),tex) );

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
            Log::Error( TXT( "MIPMAPs are not supported by this version of GL" ) );
            //glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
        }

        /* Clean up and return the texture ID */
        free(data);
    }

    return tex;
}
