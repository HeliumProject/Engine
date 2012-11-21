/// Get a pointer to the memory stream data.
///
/// @return  Memory stream data.
const void* Helium::StaticMemoryStream::GetData() const
{
    return m_pStart;
}

/// Get a pointer to the dynamic array currently in use as this stream's memory buffer.
///
/// @return  Dynamic array in use by this stream.
Helium::DynamicArray< uint8_t >* Helium::DynamicMemoryStream::GetBuffer() const
{
    return m_pBuffer;
}
