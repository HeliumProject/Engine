#pragma once

#include "RenderingGL/RenderingGL.h"
#include "Rendering/RBlendState.h"

#include "GL/glew.h"

namespace Helium
{
	/// OpenGL blend state object.
	class GLBlendState : public RBlendState
	{
	public:
		/// @name Construction/Destruction
		//@{
		GLBlendState();
		//@}

		/// Color write mask.
		GLboolean m_redWriteMaskEnable;
		GLboolean m_greenWriteMaskEnable;
		GLboolean m_blueWriteMaskEnable;
		GLboolean m_alphaWriteMaskEnable;
		/// True to enable alpha blending.
		bool m_blendEnable;
		/// Blend equation.
		GLenum m_function;
		/// Source blend factor.
		GLenum m_sourceFactor;
		/// Destination blend factor.
		GLenum m_destinationFactor;

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
		~GLBlendState();
		//@}
	};
}
