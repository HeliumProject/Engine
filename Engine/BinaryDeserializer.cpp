//----------------------------------------------------------------------------------------------------------------------
// BinaryDeserializer.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "EnginePch.h"
#include "Engine/BinaryDeserializer.h"

namespace Lunar
{
    /// Constructor.
    BinaryDeserializer::BinaryDeserializer()
        : m_bEndOfStream( false )
    {
        // Initialize the byte-swapping stream to utilize the direct memory stream.
        m_byteSwappingStream.Open( &m_directStream );

        // Don't swap bytes by default.
        m_pStream = &m_directStream;
    }

    /// Destructor.
    BinaryDeserializer::~BinaryDeserializer()
    {
    }

    /// Prepare for deserializing an object from the given data.
    ///
    /// @param[in] pData  Buffer containing the object data.  This should point to the first byte immediately after the
    ///                   serialized linker table references for the object type, template, and owner.
    /// @param[in] size   Size of the object data buffer.
    void BinaryDeserializer::Prepare( const void* pData, size_t size )
    {
        // We can cast away the "const" modifier here since we will not be calling Write() on the stream.
        m_directStream.Open( const_cast< void* >( pData ), size );
    }

    /// Set whether byte swapping should be performed during deserialization.
    ///
    /// @param[in] bSwapBytes  True to swap bytes, false not to.
    ///
    /// @see GetByteSwapping()
    void BinaryDeserializer::SetByteSwapping( bool bSwapBytes )
    {
        m_pStream =
            ( bSwapBytes
              ? static_cast< Stream* >( &m_byteSwappingStream )
              : static_cast< Stream* >( &m_directStream ) );
    }

    /// @copydoc Serializer::Serialize()
    bool BinaryDeserializer::Serialize( GameObject* pObject )
    {
        HELIUM_ASSERT( pObject );

        BeginSerialize();

        pObject->Serialize( *this );

        return EndSerialize();
    }

    /// @copydoc Serializer::GetMode()
    Serializer::EMode BinaryDeserializer::GetMode() const
    {
        return MODE_LOAD;
    }

    /// @copydoc Serializer::SerializeTag()
    void BinaryDeserializer::SerializeTag( const Tag& /*rTag*/ )
    {
        // Tags are ignored for binary serialization.
    }

    /// @copydoc Serializer::CanResolveTags()
    bool BinaryDeserializer::CanResolveTags() const
    {
        return false;
    }

    /// @copydoc Serializer::SerializeBool()
    void BinaryDeserializer::SerializeBool( bool& rValue )
    {
        if( ShouldSerializeCurrentProperty() )
        {
            DeserializeValue( rValue );
        }
    }

    /// @copydoc Serializer::SerializeInt8()
    void BinaryDeserializer::SerializeInt8( int8_t& rValue )
    {
        if( ShouldSerializeCurrentProperty() )
        {
            DeserializeValue( rValue );
        }
    }

    /// @copydoc Serializer::SerializeUint8()
    void BinaryDeserializer::SerializeUint8( uint8_t& rValue )
    {
        if( ShouldSerializeCurrentProperty() )
        {
            DeserializeValue( rValue );
        }
    }

    /// @copydoc Serializer::SerializeInt16()
    void BinaryDeserializer::SerializeInt16( int16_t& rValue )
    {
        if( ShouldSerializeCurrentProperty() )
        {
            DeserializeValue( rValue );
        }
    }

    /// @copydoc Serializer::SerializeUint16()
    void BinaryDeserializer::SerializeUint16( uint16_t& rValue )
    {
        if( ShouldSerializeCurrentProperty() )
        {
            DeserializeValue( rValue );
        }
    }

    /// @copydoc Serializer::SerializeInt32()
    void BinaryDeserializer::SerializeInt32( int32_t& rValue )
    {
        if( ShouldSerializeCurrentProperty() )
        {
            DeserializeValue( rValue );
        }
    }

    /// @copydoc Serializer::SerializeUint32()
    void BinaryDeserializer::SerializeUint32( uint32_t& rValue )
    {
        if( ShouldSerializeCurrentProperty() )
        {
            DeserializeValue( rValue );
        }
    }

    /// @copydoc Serializer::SerializeInt64()
    void BinaryDeserializer::SerializeInt64( int64_t& rValue )
    {
        if( ShouldSerializeCurrentProperty() )
        {
            DeserializeValue( rValue );
        }
    }

    /// @copydoc Serializer::SerializeUint64()
    void BinaryDeserializer::SerializeUint64( uint64_t& rValue )
    {
        if( ShouldSerializeCurrentProperty() )
        {
            DeserializeValue( rValue );
        }
    }

    /// @copydoc Serializer::SerializeFloat32()
    void BinaryDeserializer::SerializeFloat32( float32_t& rValue )
    {
        if( ShouldSerializeCurrentProperty() )
        {
            DeserializeValue( rValue );
        }
    }

    /// @copydoc Serializer::SerializeFloat64()
    void BinaryDeserializer::SerializeFloat64( float64_t& rValue )
    {
        if( ShouldSerializeCurrentProperty() )
        {
            DeserializeValue( rValue );
        }
    }

    /// @copydoc Serializer::SerializeBuffer()
    void BinaryDeserializer::SerializeBuffer( void* pBuffer, size_t elementSize, size_t count )
    {
        if( ShouldSerializeCurrentProperty() )
        {
            Deserialize( pBuffer, elementSize, count );
        }
    }

    /// @copydoc Serializer::SerializeEnum()
    void BinaryDeserializer::SerializeEnum( int32_t& rValue, uint32_t nameCount, const tchar_t* const* /*ppNames*/ )
    {
        if( ShouldSerializeCurrentProperty() )
        {
            if( DeserializeValue( rValue ) )
            {
                if( rValue != -1 && static_cast< uint32_t >( rValue ) >= nameCount )
                {
                    HELIUM_TRACE(
                        TRACE_WARNING,
                        ( TXT( "BinaryDeserializer: Enum value %" ) TPRId32 TXT( " exceeds the maximum supported " )
                          TXT( "range (%" ) TPRIu32 TXT( ").  Setting to invalid (-1).\n" ) ),
                        rValue,
                        nameCount );

                    rValue = -1;
                }
            }
        }
    }

    /// @copydoc Serializer::SerializeCharName()
    void BinaryDeserializer::SerializeCharName( CharName& rValue )
    {
        if( ShouldSerializeCurrentProperty() && !m_bEndOfStream )
        {
            CharString nameString;
            SerializeCharString( nameString );
            if( !m_bEndOfStream )
            {
                rValue.Set( nameString );
            }
        }
    }

    /// @copydoc Serializer::SerializeWideName()
    void BinaryDeserializer::SerializeWideName( WideName& rValue )
    {
        if( ShouldSerializeCurrentProperty() && !m_bEndOfStream )
        {
            WideString nameString;
            SerializeWideString( nameString );
            if( !m_bEndOfStream )
            {
                rValue.Set( nameString );
            }
        }
    }

    /// @copydoc Serializer::SerializeCharString()
    void BinaryDeserializer::SerializeCharString( CharString& rValue )
    {
        if( ShouldSerializeCurrentProperty() && !m_bEndOfStream )
        {
            uint32_t stringLength = 0;
            if( !DeserializeValue( stringLength ) )
            {
                return;
            }

            uint_fast32_t stringLengthFast = stringLength;

            rValue.Clear();
            rValue.Reserve( stringLengthFast );
            for( uint_fast32_t characterIndex = 0; characterIndex < stringLengthFast; ++characterIndex )
            {
                char character;
                if( !DeserializeValue( character ) )
                {
                    return;
                }

                rValue.Add( character );
            }
        }
    }

    /// @copydoc Serializer::SerializeWideString()
    void BinaryDeserializer::SerializeWideString( WideString& rValue )
    {
        if( ShouldSerializeCurrentProperty() && !m_bEndOfStream )
        {
            uint32_t stringLength = 0;
            if( !DeserializeValue( stringLength ) )
            {
                return;
            }

            uint_fast32_t stringLengthFast = stringLength;

            rValue.Clear();
            rValue.Reserve( stringLengthFast );
            for( uint_fast32_t characterIndex = 0; characterIndex < stringLengthFast; ++characterIndex )
            {
                wchar_t character;
                if( !DeserializeValue( character ) )
                {
                    return;
                }

                rValue.Add( character );
            }
        }
    }

    /// @copydoc Serializer::SerializeObjectReference()
    void BinaryDeserializer::SerializeObjectReference( Type* /*pType*/, GameObjectPtr& rspObject )
    {
        // ALWAYS release the current object reference first.  This is done in case we encounter an error during the
        // deserialization process.  When such load errors occur, the object is passed through a NullLinker to zero out
        // link indices prior to destroying the object, as the smart pointer destructor will otherwise interpret the
        // link indices as object pointers and attempt to dereference them (bad).  NullLinker can't tell which object
        // references are holding link indices and which are holding actual object references, so releasing all object
        // references that we encounter, even after an error occurs, will allow us to still use the NullLinker without
        // leaking memory.
        rspObject.Release();

        uint32_t objectIndex;
        SetInvalid( objectIndex );

        if( ShouldSerializeCurrentProperty() && !m_bEndOfStream )
        {
            DeserializeValue( objectIndex );
        }

        rspObject.SetLinkIndex( objectIndex );
    }

    /// @copydoc Serializer::PushPropertyFlags()
    void BinaryDeserializer::PushPropertyFlags( uint32_t flags )
    {
        m_propertyFlagStack.Push( GetCurrentPropertyFlags() | flags );
    }

    /// @copydoc Serializer::PopPropertyFlags()
    void BinaryDeserializer::PopPropertyFlags()
    {
        HELIUM_ASSERT( !m_propertyFlagStack.IsEmpty() );
        m_propertyFlagStack.Pop();
    }

    /// Prepare this serializer for general deserializing of binary data.
    ///
    /// @see EndSerialize()
    void BinaryDeserializer::BeginSerialize()
    {
        m_directStream.Seek( 0, Stream::SEEK_ORIGIN_BEGIN );
        m_propertyFlagStack.Resize( 0 );
        m_bEndOfStream = false;
    }

    /// Finish binary data deserialization originally started using BeginSerialize().
    ///
    /// @return  True if deserialization occurred successfully, false if an error occurred.
    ///
    /// @see BeginSerialize()
    bool BinaryDeserializer::EndSerialize()
    {
        return !m_bEndOfStream;
    }

    /// Deserialize an array of data.
    ///
    /// @param[in] pBuffer      Buffer into which the deserialized data should be written.
    /// @param[in] elementSize  Size of each element to deserialize.
    /// @param[in] count        Number of elements to deserialize.
    ///
    /// @return  True if deserialization was successful, false if an error occurred.
    bool BinaryDeserializer::Deserialize( void* pBuffer, size_t elementSize, size_t count )
    {
        if( !m_bEndOfStream )
        {
            size_t readCount = m_pStream->Read( pBuffer, elementSize, count );
            if( readCount != count )
            {
                HELIUM_TRACE(
                    TRACE_ERROR,
                    ( TXT( "BinaryDeserializer: End of stream reached when trying to deserialize %" ) TPRIuSZ
                      TXT( " elements of %" ) TPRIuSZ TXT( " bytes each.\n" ) ),
                    count,
                    elementSize );

                m_bEndOfStream = true;
            }
        }

        return !m_bEndOfStream;
    }

    /// Deserialize a single POD value.
    ///
    /// @param[out] rValue  Deserialized value.
    ///
    /// @return  True if deserialization was successful, false if an error occurred.
    template< typename T >
    bool BinaryDeserializer::DeserializeValue( T& rValue )
    {
        return Deserialize( &rValue, sizeof( rValue ), 1 );
    }

    /// Get the current set of property flags.
    ///
    /// @return  Current property flag set.
    uint32_t BinaryDeserializer::GetCurrentPropertyFlags() const
    {
        size_t flagStackSize = m_propertyFlagStack.GetSize();

        return ( flagStackSize == 0 ? 0 : m_propertyFlagStack[ flagStackSize - 1 ] );
    }

    /// Get whether the current property should be serialized based on the current property flags.
    ///
    /// @return  True if the current property should be serialized, false if not.
    bool BinaryDeserializer::ShouldSerializeCurrentProperty() const
    {
        return ( !( GetCurrentPropertyFlags() & FLAG_EDITOR_ONLY ) );
    }
}
