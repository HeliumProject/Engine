#pragma once

#include "RenderingGL/RenderingGL.h"
#include "Rendering/RTexture2d.h"

namespace Helium
{
	/// OpenGL 2D texture implementation.
	class GLTexture2d : public RTexture2d
	{
	public:
		/// @name Construction/Destruction
		//@{
		GLTexture2d( GLuint texture, uint32_t mipCount, ERendererPixelFormat format );
		//@}

		/// @name Base Texture Information
		//@{
		uint32_t GetMipCount() const;
		//@}

		/// @name Data Access
		//@{
		virtual void* Map( uint32_t mipLevel, size_t& rPitch, ERendererBufferMapHint hint );
		virtual void Unmap( uint32_t mipLevel );
		bool CanMapWholeResource() const;

		uint32_t GetWidth( uint32_t mipLevel ) const;
		uint32_t GetHeight( uint32_t mipLevel ) const;
		ERendererPixelFormat GetPixelFormat() const;
		bool IsSrgb() const;

		RSurface* GetSurface( uint32_t mipLevel );

		inline GLuint GetGLTexture() const;
		//@}

	protected:
		/// OpenGL texture handle.
		GLuint m_texture;
		/// Number of mip levels in this texture.
		uint32_t m_mipCount;
		/// Pixel format of our texture.
		ERendererPixelFormat m_format;

		/// @name Construction/Destruction
		//@{
		virtual ~GLTexture2d();
		//@}
	};
}

#include "RenderingGL/GLTexture2d.inl"
