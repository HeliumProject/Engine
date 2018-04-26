#pragma once

#include "RenderingD3D9/RenderingD3D9.h"
#include "Rendering/RSamplerState.h"

#include "GL/glew.h"

namespace Helium
{
	/// OpenGL texture sampler state object.
	class GLSamplerState : public RSamplerState
	{
	public:
		/// @name Construction/Destruction
		//@{
		GLSamplerState();
		//@}

		/// Texture parameter target.
		GLenum m_texParameterTarget;

		/// Minification filter.
		GLenum m_minFilter;
		/// Magnification filter.
		GLenum m_magFilter;
		/// Offset from the calculated mip level.
		GLfloat m_mipLodBias;
		/// Maximum anisotropy value for anistropic texture filtering.
		GLfloat m_maxAnisotropy;

		/// Texture u-coordinate address mode.
		GLenum m_addressModeU;
		/// Texture v-coordinate address mode.
		GLenum m_addressModeV;
		/// Texture w-coordinate address mode.
		GLenum m_addressModeW;

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
		~GLSamplerState();
		//@}
	};
}
