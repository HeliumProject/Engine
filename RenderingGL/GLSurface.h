#pragma once

#include "RenderingD3D9/RenderingD3D9.h"
#include "Rendering/RSurface.h"

struct GLFWwindow;

namespace Helium
{
	/// Wrapper for an OpenGL surface.
	class GLSurface : public RSurface
	{
	public:
		/// @name Construction/Destruction
		//@{
		GLSurface( GLuint renderbuffer );
		//@}

		/// @name Data Access
		//@{
		GLuint GetGLSurface() const;
		void SetGLSurface( GLuint renderbuffer );
		//@}

	protected:
		/// Handle to the OpenGL renderbuffer.
		GLuint m_renderbuffer;

		/// @name Construction/Destruction
		//@{
		virtual ~GLSurface();
		//@}
	};
}
