/// Get a pointer to the dynamic array currently in use as this stream's memory buffer.
///
/// @return  Dynamic array in use by this stream.
Lunar::DynArray< uint8_t >* Helium::DynamicMemoryStream::GetBuffer() const
{
    return m_pBuffer;
}
