#pragma once

#include "RenderingGL/RenderingGL.h"
#include "Rendering/RDepthStencilState.h"

#include "GL/glew.h"

namespace Helium
{
	/// OpenGL Depth/Stencil state object.
	class GLDepthStencilState : public RDepthStencilState
	{
	public:
		/// @name Construction/Destruction
		//@{
		GLDepthStencilState();
		//@}

		/// True to enable depth testing.
		bool m_depthTestEnable;
		/// True to enable depth writing.
		bool m_depthWriteEnable;
		/// Depth comparison function.
		GLenum m_depthFunction;

		/// True to enable stencil testing.
		bool m_stencilTestEnable;
		/// Stencil comparison function.
		GLenum m_stencilFunction;
		/// Stencil operation to perform when stencil testing fails.
		GLenum m_stencilFailOperation;
		/// Stencil operation to perform when stencil testing passes and depth testing fails.
		GLenum m_stencilDepthFailOperation;
		/// Stencil operation to perform when both stencil testing and depth testing pass.
		GLenum m_stencilDepthPassOperation;
		/// Stencil read mask. (argument to glStencilFunc)
		GLuint m_stencilReadMask;
		/// Stencil write mask. (argument to glStencilMask)
		GLuint m_stencilWriteMask;

		/// @name Initialization
		//@{
		bool Initialize( const Description& rDescription );
		//@}

		/// @name State Information
		//@{
		void GetDescription( Description& rDescription ) const;
		//@}

	private:
		/// @name Construction/Destruction
		//@{
		~GLDepthStencilState();
		//@}
	};
}
