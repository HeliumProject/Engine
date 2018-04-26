#include "Precompile.h"
#include "RenderingGL/GLMainContext.h"
#include "RenderingGL/GLSurface.h"

#include "GL/glew.h"
#include "GLFW/glfw3.h"

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
		GLuint newRenderbuffer = 0;
		glGenRenderbuffers( 1, &newRenderbuffer );
		HELIUM_ASSERT( newRenderbuffer != 0 );
		if( newRenderbuffer == 0 )
		{
			HELIUM_TRACE( TraceLevels::Error, "GLMainContext: Failed to generate GL back renderbuffer surface.\n" );
		}

		// Store off previously bound renderbuffer.
		GLint curRenderbuffer = 0;
		glGetIntegerv( GL_RENDERBUFFER_BINDING, &curRenderbuffer );

		// Create a new renderbuffer object for the back buffer.
		HELIUM_ASSERT( m_pGlfwWindow != NULL );
		int width, height;
		glfwGetWindowSize(m_pGlfwWindow, &width, &height);
		glBindRenderbuffer( GL_RENDERBUFFER, newRenderbuffer );
		glRenderbufferStorage( GL_RENDERBUFFER, GL_RGBA8, width, height );

		// Restore previous renderbuffer.
		glBindRenderbuffer( GL_RENDERBUFFER, curRenderbuffer );

		// Construct the GLSurface object.
		m_spBackBufferSurface = new GLSurface( newRenderbuffer, GL_COLOR_ATTACHMENT0, false );
		HELIUM_ASSERT( m_spBackBufferSurface != NULL );
	}

	return m_spBackBufferSurface;
}

/// @copydoc RRenderContext::Swap()
void GLMainContext::Swap()
{
	// Present the scene.
	glfwSwapBuffers( m_pGlfwWindow );
}
