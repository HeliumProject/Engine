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
		GLSurface( GLuint target, GLenum attachType, bool isTexture );
		//@}

		/// @name Data Access
		//@{
		GLuint GetGLSurface() const;
		GLenum GetGLAttachmentType() const;
		bool GetIsTexture() const;
		//@}

	protected:
		/// Handle to the OpenGL renderbuffer or texture.
		GLuint m_target;
		/// Attachment type of the OpenGL renderbuffer.
		GLenum m_attachType;
		/// True if render target is texture, false if renderbuffer.
		bool m_isTexture;

		/// @name Construction/Destruction
		//@{
		virtual ~GLSurface();
		//@}
	};
}
