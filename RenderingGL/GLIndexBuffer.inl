namespace Helium
{
	/// Get the OpenGL index buffer.
	///
	/// @return  OpenGL index buffer handle.
	unsigned GLIndexBuffer::GetGLBuffer() const
	{
		return m_buffer;
	}

	/// Get the OpenGL index buffer.
	///
	/// @return  OpenGL index buffer handle.
	GLenum GLIndexBuffer::GetGLElementType() const
	{
		return m_elementType;
	}
}
