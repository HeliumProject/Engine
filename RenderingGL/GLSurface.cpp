#include "RenderingGLPch.h"
#include "RenderingGL/GLSurface.h"

using namespace Helium;

/// Constructor.
GLSurface::GLSurface( GLuint renderbuffer, GLenum attachType )
: m_renderbuffer( renderbuffer )
, m_attachType( attachType )
{
	HELIUM_ASSERT( renderbuffer );
}

/// Destructor.
GLSurface::~GLSurface()
{
	m_renderbuffer = 0;
	m_attachType = 0;
}

GLuint GLSurface::GetGLSurface() const
{
	return m_renderbuffer;
}

void GLSurface::SetGLSurface( GLuint renderbuffer )
{
	m_renderbuffer = renderbuffer;
}

GLenum GLSurface::GetGLAttachmentType() const
{
	return m_attachType;
}
