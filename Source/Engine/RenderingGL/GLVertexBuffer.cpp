#include "Precompile.h"

#include "RenderingGL.h"
#include "RenderingGL/GLVertexBuffer.h"

#include "GL/glew.h"

using namespace Helium;

/// Constructor.
///
/// @param[in] vbo  OpenGL buffer object to wrap.  It will be deleted when this object is destroyed.
GLVertexBuffer::GLVertexBuffer( unsigned vbo )
: m_vbo( vbo )
{
	HELIUM_ASSERT( vbo != 0 );
}

/// Destructor.
GLVertexBuffer::~GLVertexBuffer()
{
	if( m_vbo )
	{
		glDeleteBuffers( 1, &m_vbo );
		m_vbo = 0;
	}
}

/// @copydoc RVertexBuffer::Map()
void* GLVertexBuffer::Map( ERendererBufferMapHint hint )
{
	if( !m_vbo )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			"GLVertexBuffer::Map(): Attempted to map an invalid OpenGL buffer object.\n" );
		return NULL;
	}

	// Determine usage hints for mapping the vertex buffer.
	// "Discard" assumes only write operations, while "NO_OVERWRITE" assumes
	// only read operations.
	GLbitfield accessFlags = 0;
	if( hint == RENDERER_BUFFER_MAP_HINT_DISCARD )
	{
		accessFlags |= GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT;
	}
	else if( hint == RENDERER_BUFFER_MAP_HINT_NO_OVERWRITE )
	{
		accessFlags |= GL_MAP_READ_BIT;
	}

	// Map the buffer to client memory.
	glBindBuffer( GL_ARRAY_BUFFER, m_vbo );
	void* pData = glMapBuffer( GL_ARRAY_BUFFER, accessFlags );
	if( !pData )
	{
		HELIUM_TRACE( TraceLevels::Error, "GLVertexBuffer::Map(): Failed to map OpenGL buffer.\n" );
		return NULL;
	}

	// Return a pointer to the mapped buffer data.
	HELIUM_ASSERT( pData );
	return pData;
}

/// @copydoc RVertexBuffer::Unmap()
void GLVertexBuffer::Unmap()
{
	if( !m_vbo )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			"GLVertexBuffer::Unmap(): Attempted to unmap an invalid OpenGL buffer object.\n" );
		return;
	}

	// Unbind the buffer from client memory.
	glBindBuffer( GL_ARRAY_BUFFER, m_vbo );
	GLboolean result = glUnmapBuffer( GL_ARRAY_BUFFER );
	if( result == GL_FALSE )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			"GLVertexBuffer::Unmap(): An error occurred while unmapping a vertex buffer.\n" );
	}
}
