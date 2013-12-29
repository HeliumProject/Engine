#include "RenderingGLPch.h"
#include "RenderingGL/GLMainContext.h"

#include "GL/glew.h"
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

/// Initialization.
void GLMainContext::Initialize()
{
	// Initialize GLEW for this OpenGL rendering context.
	GLFWwindow *currentContext = glfwGetCurrentContext();
	glfwMakeContextCurrent( m_pGlfwWindow );
	glewExperimental = GL_TRUE;
	HELIUM_ASSERT( GLEW_OK == glewInit() );
	glfwMakeContextCurrent( currentContext );
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
