#pragma once

#include "RenderingGL/RenderingGL.h"
#include "Rendering/RIndexBuffer.h"
#include "Platform/System.h"

#include "GL/glew.h"

namespace Helium
{
	/// OpenGL index buffer implementation.
	class GLIndexBuffer : public RIndexBuffer
	{
	public:
		/// @name Construction/Destruction
		//@{
		GLIndexBuffer( GLenum elementType, unsigned buffer );
		//@}

		/// @name Data Access
		//@{
		virtual void* Map( ERendererBufferMapHint hint ) HELIUM_OVERRIDE;
		virtual void Unmap() HELIUM_OVERRIDE;
		unsigned GetGLBuffer() const;
		GLenum GetGLElementType() const;
		//@}

	protected:
		/// Buffer instance and type
		GLenum m_elementType;
		unsigned m_buffer;

		/// @name Construction/Destruction
		//@{
		virtual ~GLIndexBuffer();
		//@}
	};
}

#include "RenderingD3D9/D3D9VertexBuffer.inl"
