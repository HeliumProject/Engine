//----------------------------------------------------------------------------------------------------------------------
// DirectSerializer.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "EnginePch.h"
#include "Engine/DirectSerializer.h"

using namespace Lunar;

/// Constructor.
///
/// @param[in] rBuffer  Buffer to which the data should be serialized.  Note that serialization will begin from the
///                     current end of the array, with all existing data left intact.
DirectSerializer::DirectSerializer( DynArray< uint8_t >& rBuffer )
: m_rBuffer( rBuffer )
{
}

/// @copydoc Serializer::Serialize()
bool DirectSerializer::Serialize( GameObject* pObject )
{
    HELIUM_ASSERT( pObject );

    pObject->Serialize( *this );

    return true;
}

/// @copydoc Serializer::GetMode()
Serializer::EMode DirectSerializer::GetMode() const
{
    return MODE_SAVE;
}

/// @copydoc Serializer::SerializeTag()
void DirectSerializer::SerializeTag( const Tag& /*rTag*/ )
{
    // Tags are not serialized.
}

/// @copydoc Serializer::CanResolveTags()
bool DirectSerializer::CanResolveTags() const
{
    return false;
}

/// @copydoc Serializer::SerializeBool()
void DirectSerializer::SerializeBool( bool& rValue )
{
    WriteValue( rValue != false );
}

/// @copydoc Serializer::SerializeInt8()
void DirectSerializer::SerializeInt8( int8_t& rValue )
{
    WriteValue( rValue );
}

/// @copydoc Serializer::SerializeUint8()
void DirectSerializer::SerializeUint8( uint8_t& rValue )
{
    WriteValue( rValue );
}

/// @copydoc Serializer::SerializeInt16()
void DirectSerializer::SerializeInt16( int16_t& rValue )
{
    WriteValue( rValue );
}

/// @copydoc Serializer::SerializeUint16()
void DirectSerializer::SerializeUint16( uint16_t& rValue )
{
    WriteValue( rValue );
}

/// @copydoc Serializer::SerializeInt32()
void DirectSerializer::SerializeInt32( int32_t& rValue )
{
    WriteValue( rValue );
}

/// @copydoc Serializer::SerializeUint32()
void DirectSerializer::SerializeUint32( uint32_t& rValue )
{
    WriteValue( rValue );
}

/// @copydoc Serializer::SerializeInt64()
void DirectSerializer::SerializeInt64( int64_t& rValue )
{
    WriteValue( rValue );
}

/// @copydoc Serializer::SerializeUint64()
void DirectSerializer::SerializeUint64( uint64_t& rValue )
{
    WriteValue( rValue );
}

/// @copydoc Serializer::SerializeFloat32()
void DirectSerializer::SerializeFloat32( float32_t& rValue )
{
    WriteValue( rValue );
}

/// @copydoc Serializer::SerializeFloat64()
void DirectSerializer::SerializeFloat64( float64_t& rValue )
{
    WriteValue( rValue );
}

/// @copydoc Serializer::SerializeBuffer()
void DirectSerializer::SerializeBuffer( void* pBuffer, size_t elementSize, size_t count )
{
    WriteBytes( pBuffer, elementSize * count );
}

/// @copydoc Serializer::SerializeEnum()
void DirectSerializer::SerializeEnum( int32_t& rValue, uint32_t /*nameCount*/, const tchar_t* const* /*ppNames*/ )
{
    WriteValue( rValue );
}

/// @copydoc Serializer::SerializeEnum()
void DirectSerializer::SerializeEnum( int32_t& rValue, const Helium::Reflect::Enumeration* /*pEnumeration*/ )
{
    WriteValue( rValue );
}

/// @copydoc Serializer::SerializeCharName()
void DirectSerializer::SerializeCharName( CharName& rValue )
{
    const char* pNameString = rValue.Get();
    HELIUM_ASSERT( pNameString );
    size_t nameLength = StringLength( pNameString );

    WriteString( pNameString, nameLength );
}

/// @copydoc Serializer::SerializeWideName()
void DirectSerializer::SerializeWideName( WideName& rValue )
{
    const wchar_t* pNameString = rValue.Get();
    HELIUM_ASSERT( pNameString );
    size_t nameLength = StringLength( pNameString );

    WriteString( pNameString, nameLength );
}

/// @copydoc Serializer::SerializeCharString()
void DirectSerializer::SerializeCharString( CharString& rValue )
{
    WriteString( rValue.GetData(), rValue.GetSize() );
}

/// @copydoc Serializer::SerializeWideString()
void DirectSerializer::SerializeWideString( WideString& rValue )
{
    WriteString( rValue.GetData(), rValue.GetSize() );
}

/// @copydoc Serializer::SerializeObjectReference()
void DirectSerializer::SerializeObjectReference( const GameObjectType* /*pType*/, GameObjectPtr& rspObject )
{
    GameObject* pObject = rspObject;
    WriteValue( pObject );
}

/// Write bytes directly to the serialization buffer.
///
/// @param[in] pData      Byte buffer to write.
/// @param[in] byteCount  Number of bytes to write.
void DirectSerializer::WriteBytes( const void* pData, size_t byteCount )
{
    HELIUM_ASSERT( pData );
    HELIUM_ASSERT( byteCount != 0 );

    size_t bufferOffset = m_rBuffer.GetSize();
    m_rBuffer.Resize( bufferOffset + byteCount );

    uint8_t* pBuffer = m_rBuffer.GetData();
    HELIUM_ASSERT( pBuffer );
    MemoryCopy( pBuffer + bufferOffset, pData, byteCount );
}

/// Write the byte contents of a value directly to the serialization buffer.
///
/// @param[in] rValue  Value to write.
template< typename T >
void DirectSerializer::WriteValue( const T& rValue )
{
    WriteBytes( &rValue, sizeof( rValue ) );
}

/// Write a string directly to the serialization buffer.
///
/// @param[in] pString  Contents of the string to write.
/// @param[in] count    Number of characters in the given string buffer.
template< typename T >
void DirectSerializer::WriteString( const T* pString, size_t count )
{
    WriteValue( count );
    if( count )
    {
        HELIUM_ASSERT( pString );
        WriteBytes( pString, sizeof( pString[ 0 ] ) * count );
    }
}
