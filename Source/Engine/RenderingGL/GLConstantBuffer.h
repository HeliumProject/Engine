#pragma once

#include "RenderingGL/RenderingGL.h"
#include "Rendering/RConstantBuffer.h"
#include "Platform/System.h"

#include "GL/glew.h"

namespace Helium
{
	/// OpenGL index buffer implementation.
	class GLConstantBuffer : public RConstantBuffer
	{
	public:
		/// @name Construction/Destruction
		//@{
		GLConstantBuffer( void* pData, uint16_t registerCount );
		//@}

		/// @name Data Access
		//@{
		void* Map( ERendererBufferMapHint hint );
		void Unmap();

		inline const void* GetData() const;
		inline uint32_t GetTag() const;
		inline uint16_t GetRegisterCount() const;
		//@}

	protected:
		/// Constant buffer data.
		void* m_pData;
		/// Map tag (incremented after each Unmap() call).
		uint32_t m_tag;
		/// Number of floating-point vector registers covered by this buffer.
		uint16_t m_registerCount;

		/// @name Construction/Destruction
		//@{
		~GLConstantBuffer();
		//@}
	};
}

#include "RenderingGL/GLConstantBuffer.inl"
