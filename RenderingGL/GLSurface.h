#pragma once

#include "RenderingD3D9/RenderingD3D9.h"
#include "Rendering/RSurface.h"

struct GLFWwindow;

namespace Helium
{
	// TODO: The concept of a surface should be implemented with FBOs
	// or some other appropriate way to define render targets.  We
	// need to revisit this code.

	/// Wrapper for an OpenGL surface.
	class GLSurface : public RSurface
	{
	public:
		/// @name Construction/Destruction
		//@{
		GLSurface( GLFWwindow* pGlfwWindow );
		//@}

		/// @name Data Access
		//@{
		GLFWwindow* GetGLSurface() const;
		void SetGLSurface( GLFWwindow* pSurface );
		//@}

	protected:
		/// GLFW window / OpenGL context
		GLFWwindow *m_pGlfwWindow;

		/// @name Construction/Destruction
		//@{
		virtual ~GLSurface();
		//@}
	};
}
