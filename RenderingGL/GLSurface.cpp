#include "RenderingGLPch.h"
#include "RenderingGL/GLSurface.h"

using namespace Helium;

/// Constructor.
GLSurface::GLSurface( GLuint renderbuffer )
: m_renderbuffer( renderbuffer )
{
	HELIUM_ASSERT( renderbuffer );
}

/// Destructor.
GLSurface::~GLSurface()
{
	m_renderbuffer = 0;
}

GLuint GLSurface::GetGLSurface() const
{
	return m_renderbuffer;
}

void GLSurface::SetGLSurface( GLuint renderbuffer )
{
	m_renderbuffer = renderbuffer;
}
