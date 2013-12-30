#include "RenderingGLPch.h"

#include "RenderingGL.h"
#include "RenderingGL/GLIndexBuffer.h"

#include "GL/glew.h"

using namespace Helium;

/// Constructor.
///
/// @param[in] buffer  OpenGL buffer object to wrap.  It will be deleted when this object is destroyed.
GLIndexBuffer::GLIndexBuffer( GLenum elementType, unsigned buffer )
: m_elementType( elementType )
, m_buffer( buffer )
{
	HELIUM_ASSERT( buffer != 0 );
}

/// Destructor.
GLIndexBuffer::~GLIndexBuffer()
{
	if( m_buffer )
	{
		glDeleteBuffers( 1, &m_buffer );
		m_buffer = 0;
	}
}

/// @copydoc RIndexBuffer::Map()
void* GLIndexBuffer::Map( ERendererBufferMapHint hint )
{
	if( !m_buffer )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			"GLIndexBuffer::Map(): Attempted to map an invalid OpenGL buffer object.\n" );
		return NULL;
	}

	// Determine usage hints for mapping the buffer.
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
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_buffer );
	void* pData = glMapBuffer( GL_ELEMENT_ARRAY_BUFFER, accessFlags );
	if( !pData )
	{
		HELIUM_TRACE( TraceLevels::Error, "GLIndexBuffer::Map(): Failed to map OpenGL buffer.\n" );
		return NULL;
	}

	// Return a pointer to the mapped buffer data.
	HELIUM_ASSERT( pData );
	return pData;
}

/// @copydoc RVertexBuffer::Unmap()
void GLIndexBuffer::Unmap()
{
	if( !m_buffer )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			"GLIndexBuffer::Unmap(): Attempted to unmap an invalid OpenGL buffer object.\n" );
		return;
	}

	// Unbind the buffer from client memory.
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_buffer );
	GLboolean result = glUnmapBuffer( GL_ELEMENT_ARRAY_BUFFER );
	if( result == GL_FALSE )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			"GLIndexBuffer::Unmap(): An error occurred while unmapping a vertex buffer.\n" );
	}
}
