#pragma once

#include "RenderingGL/RenderingGL.h"
#include "Rendering/RVertexBuffer.h"
#include "Platform/System.h"

namespace Helium
{
	/// OpenGL vertex buffer implementation.
	class GLVertexBuffer : public RVertexBuffer
	{
	public:
		/// @name Construction/Destruction
		//@{
		GLVertexBuffer( unsigned vbo );
		//@}

		/// @name Data Access
		//@{
		virtual void* Map( ERendererBufferMapHint hint ) override;
		virtual void Unmap() override;

		inline unsigned GetGLBuffer() const;
		//@}

	protected:
		/// Vertex buffer instance.
		unsigned m_vbo;

		/// @name Construction/Destruction
		//@{
		virtual ~GLVertexBuffer();
		//@}
	};
}

#include "RenderingGL/GLVertexBuffer.inl"
