#include "RenderingGLPch.h"
#include "RenderingGL/GLMainContext.h"

#include "GLFW/glfw3.h"
#include "RenderingGL/GLSurface.h"

using namespace Helium;

/// Constructor.
GLMainContext::GLMainContext( GLFWwindow* pGlfwWindow )
: m_pGlfwWindow( pGlfwWindow )
, m_spBackBufferSurface( NULL )
{
	HELIUM_ASSERT( pGlfwWindow );
}

/// Destructor.
GLMainContext::~GLMainContext()
{
	m_pGlfwWindow = NULL;
}

/// @copydoc RRenderContext::GetBackBufferSurface()
RSurface* GLMainContext::GetBackBufferSurface()
{
	// Create the back buffer surface reference if it does not yet exist.
	if( !m_spBackBufferSurface )
	{
		m_spBackBufferSurface = new GLSurface( m_pGlfwWindow );
	}

	return m_spBackBufferSurface;
}

/// @copydoc RRenderContext::Swap()
void GLMainContext::Swap()
{
	// Present the scene.
	glfwSwapBuffers( m_pGlfwWindow );
}
