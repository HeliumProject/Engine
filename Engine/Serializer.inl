//----------------------------------------------------------------------------------------------------------------------
// Serializer.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Helium
{
    /// Constructor.
    ///
    /// @param[in] rStructure  Structure to wrap for serialization.
    template< typename T >
    StructSerializeProxy< T >::StructSerializeProxy( T& rStructure )
        : m_rStructure( rStructure )
    {
    }

    /// Constructor.
    ///
    /// @param[in] rArray  Static array to wrap for serialization.
    template< typename T, size_t Size >
    ArraySerializeProxy< T, Size >::ArraySerializeProxy( T ( &rArray )[ Size ] )
        : m_rArray( rArray )
    {
    }

    /// Constructor.
    ///
    /// @param[in] rArray  Dynamic array to wrap for serialization.
    template< typename T >
    DynamicArraySerializeProxy< T >::DynamicArraySerializeProxy( DynamicArray< T >& rArray )
        : m_rArray( rArray )
    {
    }

    /// Constructor.
    ///
    /// @param[in] rArray  Static array of structs to wrap for serialization.
    template< typename T, size_t Size >
    StructArraySerializeProxy< T, Size >::StructArraySerializeProxy( T ( &rArray )[ Size ] )
        : m_rArray( rArray )
    {
    }

    /// Constructor.
    ///
    /// @param[in] rArray  Dynamic array of structs to wrap for serialization.
    template< typename T >
    StructDynamicArraySerializeProxy< T >::StructDynamicArraySerializeProxy( DynamicArray< T >& rArray )
        : m_rArray( rArray )
    {
    }

    /// Wrap a struct for serialization.
    ///
    /// @param[in] rStruct  Struct to wrap.
    ///
    /// @return  Structure serialization proxy.
    ///
    /// @see WrapArray(), WrapDynamicArray(), WrapStructArray(), WrapStructDynamicArray()
    template< typename T >
    StructSerializeProxy< T > Serializer::WrapStruct( T& rStruct )
    {
        return StructSerializeProxy< T >( rStruct );
    }

    /// Wrap a static array for serialization.
    ///
    /// @param[in] rArray  Array to wrap.
    ///
    /// @return  Array serialization proxy.
    ///
    /// @see WrapStruct(), WrapDynamicArray(), WrapStructArray(), WrapStructDynamicArray()
    template< typename T, size_t Size >
    ArraySerializeProxy< T, Size > Serializer::WrapArray( T ( &rArray )[ Size ] )
    {
        return ArraySerializeProxy< T, Size >( rArray );
    }

    /// Wrap a dynamic array for serialization.
    ///
    /// @param[in] rArray  Array to wrap.
    ///
    /// @return  Dynamic array serialization proxy.
    ///
    /// @see WrapStruct(), WrapArray(), WrapStructArray(), WrapStructDynamicArray()
    template< typename T >
    DynamicArraySerializeProxy< T > Serializer::WrapDynamicArray( DynamicArray< T >& rArray )
    {
        return DynamicArraySerializeProxy< T >( rArray );
    }

    /// Wrap a static array of structs for serialization.
    ///
    /// @param[in] rArray  Array to wrap.
    ///
    /// @return  Struct array serialization proxy.
    ///
    /// @see WrapStruct(), WrapArray(), WrapDynamicArray(), WrapStructDynamicArray()
    template< typename T, size_t Size >
    StructArraySerializeProxy< T, Size > Serializer::WrapStructArray( T ( &rArray )[ Size ] )
    {
        return StructArraySerializeProxy< T, Size >( rArray );
    }

    /// Wrap a dynamic array of structs for serialization.
    ///
    /// @param[in] rArray  Array to wrap.
    ///
    /// @return  Struct dynamic array serialization proxy.
    ///
    /// @see WrapStruct(), WrapArray(), WrapDynamicArray(), WrapStructArray()
    template< typename T >
    StructDynamicArraySerializeProxy< T > Serializer::WrapStructDynamicArray( DynamicArray< T >& rArray )
    {
        return StructDynamicArraySerializeProxy< T >( rArray );
    }

    /// Apply a tag for an upcoming property.
    ///
    /// @param[in] rTag  Tag to serialize.
    ///
    /// @return  Reference to this object.
    Serializer& Serializer::operator<<( const Tag& rTag )
    {
        SerializeTag( rTag );
        return *this;
    }

    /// Serialize a boolean value.
    ///
    /// @param[in,out] rValue  Boolean value.
    ///
    /// @return  Reference to this object.
    Serializer& Serializer::operator<<( bool& rValue )
    {
        SerializeBool( rValue );
        return *this;
    }

    /// Serialize a signed 8-bit integer.
    ///
    /// @param[in,out] rValue  Signed 8-bit integer.
    ///
    /// @return  Reference to this object.
    Serializer& Serializer::operator<<( int8_t& rValue )
    {
        SerializeInt8( rValue );
        return *this;
    }

    /// Serialize an unsigned 8-bit integer.
    ///
    /// @param[in,out] rValue  Unsigned 8-bit integer.
    ///
    /// @return  Reference to this object.
    Serializer& Serializer::operator<<( uint8_t& rValue )
    {
        SerializeUint8( rValue );
        return *this;
    }

    /// Serialize a signed 16-bit integer.
    ///
    /// @param[in,out] rValue  Signed 16-bit integer.
    ///
    /// @return  Reference to this object.
    Serializer& Serializer::operator<<( int16_t& rValue )
    {
        SerializeInt16( rValue );
        return *this;
    }

    /// Serialize an unsigned 16-bit integer.
    ///
    /// @param[in,out] rValue  Unsigned 16-bit integer.
    ///
    /// @return  Reference to this object.
    Serializer& Serializer::operator<<( uint16_t& rValue )
    {
        SerializeUint16( rValue );
        return *this;
    }

    /// Serialize a signed 32-bit integer.
    ///
    /// @param[in,out] rValue  Signed 32-bit integer.
    ///
    /// @return  Reference to this object.
    Serializer& Serializer::operator<<( int32_t& rValue )
    {
        SerializeInt32( rValue );
        return *this;
    }

    /// Serialize an unsigned 32-bit integer.
    ///
    /// @param[in,out] rValue  Unsigned 32-bit integer.
    ///
    /// @return  Reference to this object.
    Serializer& Serializer::operator<<( uint32_t& rValue )
    {
        SerializeUint32( rValue );
        return *this;
    }

    /// Serialize a signed 64-bit integer.
    ///
    /// @param[in,out] rValue  Signed 64-bit integer.
    ///
    /// @return  Reference to this object.
    Serializer& Serializer::operator<<( int64_t& rValue )
    {
        SerializeInt64( rValue );
        return *this;
    }

    /// Serialize an unsigned 64-bit integer.
    ///
    /// @param[in,out] rValue  Unsigned 64-bit integer.
    ///
    /// @return  Reference to this object.
    Serializer& Serializer::operator<<( uint64_t& rValue )
    {
        SerializeUint64( rValue );
        return *this;
    }

    /// Serialize a single-precision floating-point value.
    ///
    /// @param[in,out] rValue  Single-precision floating-point value.
    ///
    /// @return  Reference to this object.
    Serializer& Serializer::operator<<( float32_t& rValue )
    {
        SerializeFloat32( rValue );
        return *this;
    }

    /// Serialize a double-precision floating-point value.
    ///
    /// @param[in,out] rValue  Double-precision floating-point value.
    ///
    /// @return  Reference to this object.
    Serializer& Serializer::operator<<( float64_t& rValue )
    {
        SerializeFloat64( rValue );
        return *this;
    }

    /// Serialize an enumeration value.
    ///
    /// @param[in,out] rValue  Serializable enumeration value.
    ///
    /// @return  Reference to this object.
    template< typename EnumInfo, typename ValueType >
    Serializer& Serializer::operator<<( Enum< EnumInfo, ValueType >& rValue )
    {
        int32_t valueLocal = static_cast< int32_t >( rValue );

        uint32_t nameCount = rValue.GetCount();
        HELIUM_ASSERT( nameCount != 0 );

        const tchar_t* const* ppNames = rValue.GetNames();
        HELIUM_ASSERT( ppNames );

        SerializeEnum( valueLocal, nameCount, ppNames );

        rValue = static_cast< ValueType >( valueLocal );

        return *this;
    }

    /// Serialize an 8-bit character name.
    ///
    /// @param[in,out] rValue  Name.
    ///
    /// @return  Reference to this object.
    Serializer& Serializer::operator<<( Name& rValue )
    {
        SerializeName( rValue );
        return *this;
    }

    /// Serialize an 8-bit character string.
    ///
    /// @param[in,out] rValue  String.
    ///
    /// @return  Reference to this object.
    Serializer& Serializer::operator<<( String& rValue )
    {
        SerializeString( rValue );
        return *this;
    }

    /// Serialize an object reference.
    ///
    /// @param[in,out] rspObject  GameObject smart pointer.
    ///
    /// @return  Reference to this object.
    template< typename T >
    Serializer& Serializer::operator<<( StrongPtr< T >& rspObject )
    {
        SerializeObjectReference( T::GetStaticType(), reinterpret_cast< GameObjectPtr& >( rspObject ) );

        return *this;
    }

    /// Serialize a structure value.
    ///
    /// @param[in] rValue  Serialization proxy for the structure to serialize.
    ///
    /// @return  Reference to this object.
    template< typename T >
    Serializer& Serializer::operator<<( const StructSerializeProxy< T >& rValue )
    {
        BeginStruct();
        rValue.m_rStructure.Serialize( *this );
        EndStruct();

        return *this;
    }

    /// Serialize a static array.
    ///
    /// @param[in] rValue  Serialization proxy for the array to serialize.
    ///
    /// @return  Reference to this object.
    template< typename T, size_t Size >
    Serializer& Serializer::operator<<( const ArraySerializeProxy< T, Size >& rValue )
    {
        HELIUM_COMPILE_ASSERT( Size <= static_cast< size_t >( UINT32_MAX ) );
        BeginArray( static_cast< uint32_t >( Size ) );

        for( size_t index = 0; index < Size; ++index )
        {
            *this << rValue.m_rArray[ index ];
        }

        EndArray();

        return *this;
    }

    /// Serialize a dynamic array.
    ///
    /// @param[in] rValue  Serialization proxy for the array to serialize.
    ///
    /// @return  Reference to this object.
    template< typename T >
    Serializer& Serializer::operator<<( const DynamicArraySerializeProxy< T >& rValue )
    {
        BeginDynamicArray();

        HELIUM_ASSERT( rValue.m_rArray.GetSize() <= static_cast< size_t >( UINT32_MAX ) );
        uint32_t size = static_cast< uint32_t >( rValue.m_rArray.GetSize() );
        *this << size;

        if( GetMode() == MODE_LOAD )
        {
            rValue.m_rArray.Reserve( size );
            rValue.m_rArray.Resize( size );
            rValue.m_rArray.Trim();
        }

        for( size_t index = 0; index < size; ++index )
        {
            *this << rValue.m_rArray[ index ];
        }

        EndDynamicArray();

        return *this;
    }

    /// Serialize a static array of structs.
    ///
    /// @param[in] rValue  Serialization proxy for the array to serialize.
    ///
    /// @return  Reference to this object.
    template< typename T, size_t Size >
    Serializer& Serializer::operator<<( const StructArraySerializeProxy< T, Size >& rValue )
    {
        HELIUM_COMPILE_ASSERT( Size <= static_cast< size_t >( UINT32_MAX ) );
        BeginArray( static_cast< uint32_t >( Size ) );

        for( size_t index = 0; index < Size; ++index )
        {
            BeginStruct();
            rValue.m_rArray[ index ].Serialize( *this );
            EndStruct();
        }

        EndArray();

        return *this;
    }

    /// Serialize a dynamic array of structs.
    ///
    /// @param[in] rValue  Serialization proxy for the array to serialize.
    ///
    /// @return  Reference to this object.
    template< typename T >
    Serializer& Serializer::operator<<( const StructDynamicArraySerializeProxy< T >& rValue )
    {
        BeginDynamicArray();

        HELIUM_ASSERT( rValue.m_rArray.GetSize() <= static_cast< size_t >( UINT32_MAX ) );
        uint32_t size = static_cast< uint32_t >( rValue.m_rArray.GetSize() );
        *this << size;

        if( GetMode() == MODE_LOAD )
        {
            rValue.m_rArray.Reserve( size );
            rValue.m_rArray.Resize( size );
            rValue.m_rArray.Trim();
        }

        for( size_t index = 0; index < size; ++index )
        {
            BeginStruct();
            rValue.m_rArray[ index ].Serialize( *this );
            EndStruct();
        }

        EndDynamicArray();

        return *this;
    }

    /// Constructor.
    ///
    /// @param[in] pName  Property name.  Note that this object will directly hold onto the given string pointer during
    ///                   its lifetime.
    Serializer::Tag::Tag( const tchar_t* pName )
        : m_pName( pName )
    {
    }

    /// Get the property name.
    ///
    /// @return  Property name string.
    const tchar_t* Serializer::Tag::Get() const
    {
        return m_pName;
    }

    /// Constructor.
    ///
    /// This creates an enumeration interface with an uninitialized state.  The interface will need to be manually set
    /// to a value before use.
    template< typename EnumInfo, typename ValueType >
    Serializer::Enum< EnumInfo, ValueType >::Enum()
    {
    }

    /// Constructor.
    ///
    /// @param[in] value  Enumeration value.
    template< typename EnumInfo, typename ValueType >
    Serializer::Enum< EnumInfo, ValueType >::Enum( ValueType value )
        : m_value( value )
    {
    }

    /// Get the array of value name strings.
    ///
    /// @return  Array of value name strings.
    ///
    /// @see GetCount(), GetName(), FindNames()
    template< typename EnumInfo, typename ValueType >
    const tchar_t* const* Serializer::Enum< EnumInfo, ValueType >::GetNames() const
    {
        return EnumInfo::GetNames();
    }

    /// Get the number of enumeration values.
    ///
    /// @return  Number of valid enumeration values.
    ///
    /// @see GetNames(), GetName(), FindName()
    template< typename EnumInfo, typename ValueType >
    uint32_t Serializer::Enum< EnumInfo, ValueType >::GetCount() const
    {
        return EnumInfo::GetCount();
    }

    /// Get the name string for the specified value.
    ///
    /// @param[in] value  Enumeration value.
    ///
    /// @return  Name string if the value is valid, null if the value is not valid.
    ///
    /// @see FindName(), GetNames(), GetCount()
    template< typename EnumInfo, typename ValueType >
    const tchar_t* Serializer::Enum< EnumInfo, ValueType >::GetName( ValueType value ) const
    {
        uint32_t valueCount = EnumInfo::GetCount();
        if( static_cast< uint32_t >( value ) >= valueCount )
        {
            return NULL;
        }

        const tchar_t* const* ppNames = EnumInfo::GetNames();
        HELIUM_ASSERT( ppNames );

        return ppNames[ value ];
    }

    /// Search for the enumeration value with the given name.
    ///
    /// @param[in] pName  Name of the value to locate.
    ///
    /// @return  Enumeration value if found, "static_cast< ValueType >( -1 )" if not found.
    ///
    /// @see GetName(), GetNames(), GetCount()
    template< typename EnumInfo, typename ValueType >
    ValueType Serializer::Enum< EnumInfo, ValueType >::FindName( const tchar_t* pName ) const
    {
        HELIUM_ASSERT( pName );

        const tchar_t* const* ppNames = EnumInfo::GetNames();
        HELIUM_ASSERT( ppNames );

        uint32_t valueCount = EnumInfo::GetCount();
        for( uint32_t valueIndex = 0; valueIndex < valueCount; ++valueIndex )
        {
            if( CompareString( ppNames[ valueIndex ], pName ) == 0 )
            {
                return static_cast< ValueType >( valueIndex );
            }
        }

        return static_cast< ValueType >( -1 );
    }

    /// Cast to a constant enumeration value type reference.
    ///
    /// @return  Constant reference to the enumeration value.
    template< typename EnumInfo, typename ValueType >
    Serializer::Enum< EnumInfo, ValueType >::
        operator typename Serializer::Enum< EnumInfo, ValueType >::ConstValueReferenceType() const
    {
        return m_value;
    }
}
