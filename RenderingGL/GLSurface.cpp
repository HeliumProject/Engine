#include "RenderingGLPch.h"
#include "RenderingGL/GLSurface.h"

using namespace Helium;

// TODO: The concept of a surface should be implemented with FBOs
// or some other appropriate way to define render targets.  We
// need to revisit this code.

/// Constructor.
GLSurface::GLSurface( GLFWwindow* pGlfwWindow )
: m_pGlfwWindow( pGlfwWindow )
{
	HELIUM_ASSERT( pGlfwWindow );

	SetGLSurface( pGlfwWindow );
}

/// Destructor.
GLSurface::~GLSurface()
{
	SetGLSurface( NULL );
}

GLFWwindow* GLSurface::GetGLSurface() const
{
	return m_pGlfwWindow;
}

void GLSurface::SetGLSurface( GLFWwindow* pGlfwWindow )
{
	m_pGlfwWindow = pGlfwWindow;
}
