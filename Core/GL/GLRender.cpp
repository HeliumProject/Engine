#include <stdint.h>

#include "GLRender.h"
#include "GLUtil.h"

using namespace Core;
using namespace Core::GL::GL;

void Core::GL::GLErrorCheck()
{
    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR)
    {
#ifdef OPENGL
        Warning("GL error: %s", gluErrorString(error));
#else
        const char* message = "Unknown";
        switch ( error )
        {
        case GL_INVALID_ENUM:
            message = "GL_INVALID_ENUM";
            break;

        case GL_INVALID_VALUE:
            message = "GL_INVALID_VALUE";
            break;

        case GL_INVALID_OPERATION:
            message = "GL_INVALID_OPERATION";
            break;

        case GL_STACK_OVERFLOW:
            message = "GL_STACK_OVERFLOW";
            break;

        case GL_STACK_UNDERFLOW:
            message = "GL_STACK_UNDERFLOW";
            break;

        case GL_OUT_OF_MEMORY:
            message = "GL_OUT_OF_MEMORY";
            break;
        }

        Warning("GL error: %s (%d)", message, error);
#endif
    }
}

void SetMaterialScalar::Dump()
{
    const char* type = "Unknown";
    switch ( m_Type )
    {
    case GL_SHININESS:
        type = "GL_SHININESS";
        break;
    }

    Log( "glMaterialf( GL_FRONT_AND_BACK, %s (%d), %f )", type, m_Type, m_Param );
}

void SetMaterialScalar::Execute()
{
    glMaterialf( GL_FRONT_AND_BACK, m_Type, m_Param );
    DBG( GLErrorCheck() );
}

void SetMaterialVector::Dump()
{
    const char* type = "Unknown";
    switch ( m_Type )
    {
    case GL_AMBIENT:
        type = "GL_AMBIENT";
        break;

    case GL_DIFFUSE:
        type = "GL_DIFFUSE";
        break;

    case GL_SPECULAR:
        type = "GL_SPECULAR";
        break;
    }

    Log( "glMaterialfv( GL_FRONT_AND_BACK, %s (%d), [%f, %f, %f, %f] )", type, m_Type, m_R, m_G, m_B, m_A );
}

void SetMaterialVector::Execute()
{
    GLfloat params[] = { m_R, m_G, m_B, m_A };
    glMaterialfv( GL_FRONT_AND_BACK, m_Type, params );
    DBG( GLErrorCheck() );
}

void SetTexture::Dump()
{
    Log( "glBindTexture( GL_TEXTURE_2D, %d )", m_Texture ); 
}

void SetTexture::Execute()
{
    glBindTexture( GL_TEXTURE_2D, m_Texture ); 
    DBG( GLErrorCheck() );
}

void DrawPrimitives::Dump()
{
    const char* primitive = "Unknown";
    switch ( m_Primitive )
    {
    case GL_POINTS:
        primitive = "GL_POINTS";
        break;

    case GL_LINES:
        primitive = "GL_LINES";
        break;

    case GL_LINE_LOOP:
        primitive = "GL_LINE_LOOP";
        break;

    case GL_LINE_STRIP:
        primitive = "GL_LINE_STRIP";
        break;

    case GL_TRIANGLES:
        primitive = "GL_TRIANGLES";
        break;

    case GL_TRIANGLE_STRIP:
        primitive = "GL_TRIANGLE_STRIP";
        break;

    case GL_TRIANGLE_FAN:
        primitive = "GL_TRIANGLE_FAN";
        break;
    }

    Log( "glDrawArrays( %s (%d), %d, %d )", primitive, m_Primitive, m_Start, m_Count );
}

void DrawPrimitives::Execute()
{
    glDrawArrays( m_Primitive, m_Start, m_Count );
    DBG( GLErrorCheck() );
}

void RenderCommandBuffer::Clear()
{
    m_Buffer.Clear();
    m_Offsets.Clear();
}

void RenderCommandBuffer::Dump()
{
    uintptr_t* off = m_Offsets.GetData();
    uintptr_t* end = m_Offsets.GetData() + m_Offsets.GetSize();
    for ( ; off != end; ++off )
    {
        RenderCommand* cmd = (RenderCommand*)(*off + (uintptr_t)m_Buffer.GetData());

        cmd->Dump();
    }
}

void RenderCommandBuffer::Execute()
{
    uintptr_t* off = m_Offsets.GetData();
    uintptr_t* end = m_Offsets.GetData() + m_Offsets.GetSize();
    for ( ; off != end; ++off )
    {
        RenderCommand* cmd = (RenderCommand*)(*off + (uintptr_t)m_Buffer.GetData());

        cmd->Execute();
    }
}