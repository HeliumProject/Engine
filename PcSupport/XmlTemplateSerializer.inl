//----------------------------------------------------------------------------------------------------------------------
// XmlTemplateSerializer.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Lunar
{
    /// Get the number of serialized properties.
    ///
    /// @return  Property count.
    ///
    /// @see GetPropertyData()
    size_t XmlTemplateSerializer::GetPropertyCount() const
    {
        return m_properties.GetSize();
    }

    /// Get the data for the property at the given index.
    ///
    /// @param[in] index  Property index.
    ///
    /// @return  Property information.
    ///
    /// @see GetPropertyCount()
    const XmlTemplateSerializer::PropertyData& XmlTemplateSerializer::GetPropertyData( size_t index ) const
    {
        HELIUM_ASSERT( index < m_properties.GetSize() );

        return m_properties[ index ];
    }

    /// Get the property name.
    ///
    /// @return  Property name.
    ///
    /// @see GetData(), GetSize()
    Name XmlTemplateSerializer::PropertyData::GetName() const
    {
        return m_name;
    }

    /// Get the property data.
    ///
    /// @return  Pointer to the start of the property data.
    ///
    /// @see GetName(), GetSize()
    const void* XmlTemplateSerializer::PropertyData::GetData() const
    {
        return m_pData;
    }

    /// Get the property data size.
    ///
    /// @return  Property size, in bytes.
    ///
    /// @see GetName(), GetData()
    size_t XmlTemplateSerializer::PropertyData::GetSize() const
    {
        return m_size;
    }
}
