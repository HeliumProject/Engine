#pragma once

#include "RenderingGL/RenderingGL.h"
#include "Rendering/RRasterizerState.h"

#include "GL/glew.h"

namespace Helium
{
	/// OpenGL rasterizer state object.
	class GLRasterizerState : public RRasterizerState
	{
	public:
		/// @name Construction/Destruction
		//@{
		GLRasterizerState();
		//@}

		/// Fill mode (point, line, fill).
		GLenum m_fillMode;
		/// Cull enable.
		bool m_cullEnable;
		/// Cull mode.
		GLenum m_cullMode;
		/// Face winding.
		GLenum m_winding;
		/// Depth bias enable.
		bool m_depthBiasEnable;
		/// Depth bias mode.
		GLenum m_depthBiasMode;
		/// Depth bias.
		float32_t m_depthBias;
		/// Slope-scaled depth bias.
		float32_t m_slopeScaledDepthBias;

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
		~GLRasterizerState();
		//@}
	};
}
