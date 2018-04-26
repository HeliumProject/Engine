#include "Precompile.h"
#include "RenderingGL/GLSurface.h"

using namespace Helium;

/// Constructor.
GLSurface::GLSurface( GLuint target, GLenum attachType, bool isTexture )
: m_target( target )
, m_attachType( attachType )
, m_isTexture( isTexture )
{
	HELIUM_ASSERT( target );
}

/// Destructor.
GLSurface::~GLSurface()
{
	m_target = 0;
	m_attachType = 0;
}

GLuint GLSurface::GetGLSurface() const
{
	return m_target;
}

GLenum GLSurface::GetGLAttachmentType() const
{
	return m_attachType;
}

bool GLSurface::GetIsTexture() const
{
	return m_isTexture;
}
