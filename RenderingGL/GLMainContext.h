#pragma once

#include "RenderingGL/RenderingGL.h"
#include "Rendering/RRenderContext.h"

struct GLFWwindow;

namespace Helium
{
	HELIUM_DECLARE_RPTR( GLSurface );

	/// Interface to the main GLFW render context.
	class GLMainContext : public RRenderContext
	{
	public:
		/// @name Construction/Destruction
		//@{
		GLMainContext( GLFWwindow* pGlfwWindow );
		//@}

		/// @name Initialization
		//@{
		void Initialize();
		//@}

		/// @name Render Control
		//@{
		RSurface* GetBackBufferSurface();
		void Swap();
		//@}

	private:
		/// GLFW window / OpenGL context
		GLFWwindow *m_pGlfwWindow;
        /// Active backbuffer surface.
        GLSurfacePtr m_spBackBufferSurface;

        /// @name Construction/Destruction
        //@{
        ~GLMainContext();
        //@}
    };
}
