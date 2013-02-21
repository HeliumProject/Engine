////----------------------------------------------------------------------------------------------------------------------
//// DirectDeserializer.cpp
////
//// Copyright (C) 2010 WhiteMoon Dreams, Inc.
//// All Rights Reserved
////----------------------------------------------------------------------------------------------------------------------
//
#include "EnginePch.h"
//#include "Engine/DirectDeserializer.h"
//
//using namespace Helium;
//
///// Constructor.
/////
///// @param[in] rBuffer      Buffer from which the data should be de-serialized.
///// @param[in] startOffset  Byte offset within the buffer from which to start de-serialization.
//DirectDeserializer::DirectDeserializer( const DynamicArray< uint8_t >& rBuffer, size_t startOffset )
//: m_rBuffer( rBuffer )
//{
//    // Check if we have attempted to start past the end of the stream.
//    size_t bufferSize = rBuffer.GetSize();
//    HELIUM_ASSERT( startOffset <= bufferSize );
//    m_bEndOfStream = ( startOffset > bufferSize );
//    if( m_bEndOfStream )
//    {
//        startOffset = bufferSize;
//    }
//
//    m_offset = startOffset;
//}
//
///// @copydoc Serializer::Serialize()
//bool DirectDeserializer::Serialize( Asset* pObject )
//{
//    HELIUM_ASSERT( pObject );
//
//    HELIUM_ASSERT( m_offset <= m_rBuffer.GetSize() );
//
//    // Serialize the object.
//    pObject->Serialize( *this );
//
//    return !m_bEndOfStream;
//}
//
///// @copydoc Serializer::GetMode()
//Serializer::EMode DirectDeserializer::GetMode() const
//{
//    return MODE_LOAD;
//}
//
///// @copydoc Serializer::SerializeTag()
//void DirectDeserializer::SerializeTag( const Tag& /*rTag*/ )
//{
//    // Tags are not serialized.
//}
//
///// @copydoc Serializer::CanResolveTags()
//bool DirectDeserializer::CanResolveTags() const
//{
//    return false;
//}
//
///// @copydoc Serializer::SerializeBool()
//void DirectDeserializer::SerializeBool( bool& rValue )
//{
//    ReadValue( rValue );
//}
//
///// @copydoc Serializer::SerializeInt8()
//void DirectDeserializer::SerializeInt8( int8_t& rValue )
//{
//    ReadValue( rValue );
//}
//
///// @copydoc Serializer::SerializeUint8()
//void DirectDeserializer::SerializeUint8( uint8_t& rValue )
//{
//    ReadValue( rValue );
//}
//
///// @copydoc Serializer::SerializeInt16()
//void DirectDeserializer::SerializeInt16( int16_t& rValue )
//{
//    ReadValue( rValue );
//}
//
///// @copydoc Serializer::SerializeUint16()
//void DirectDeserializer::SerializeUint16( uint16_t& rValue )
//{
//    ReadValue( rValue );
//}
//
///// @copydoc Serializer::SerializeInt32()
//void DirectDeserializer::SerializeInt32( int32_t& rValue )
//{
//    ReadValue( rValue );
//}
//
///// @copydoc Serializer::SerializeUint32()
//void DirectDeserializer::SerializeUint32( uint32_t& rValue )
//{
//    ReadValue( rValue );
//}
//
///// @copydoc Serializer::SerializeInt64()
//void DirectDeserializer::SerializeInt64( int64_t& rValue )
//{
//    ReadValue( rValue );
//}
//
///// @copydoc Serializer::SerializeUint64()
//void DirectDeserializer::SerializeUint64( uint64_t& rValue )
//{
//    ReadValue( rValue );
//}
//
///// @copydoc Serializer::SerializeFloat32()
//void DirectDeserializer::SerializeFloat32( float32_t& rValue )
//{
//    ReadValue( rValue );
//}
//
///// @copydoc Serializer::SerializeFloat64()
//void DirectDeserializer::SerializeFloat64( float64_t& rValue )
//{
//    ReadValue( rValue );
//}
//
///// @copydoc Serializer::SerializeBuffer()
//void DirectDeserializer::SerializeBuffer( void* pBuffer, size_t elementSize, size_t count )
//{
//    ReadBytes( pBuffer, elementSize * count );
//}
//
///// @copydoc Serializer::SerializeEnum()
//void DirectDeserializer::SerializeEnum( int32_t& rValue, uint32_t /*nameCount*/, const tchar_t* const* /*ppNames*/ )
//{
//    ReadValue( rValue );
//}
//
///// @copydoc Serializer::SerializeEnum()
//void DirectDeserializer::SerializeEnum( int32_t& rValue, const Helium::Reflect::Enumeration* /*pEnumeration*/ )
//{
//    ReadValue( rValue );
//}
//
///// @copydoc Serializer::SerializeName()
//void DirectDeserializer::SerializeName( Name& rValue )
//{
//    if( m_bEndOfStream )
//    {
//        return;
//    }
//
//    size_t count = 0;
//    ReadValue( count );
//    if( m_bEndOfStream )
//    {
//        return;
//    }
//
//    if( !count )
//    {
//        rValue.Set( NULL );
//        return;
//    }
//
//    ThreadLocalStackAllocator stackAllocator;
//
//    size_t byteCount = sizeof( char ) * count;
//    char* pNameString = static_cast< char* >( stackAllocator.Allocate( byteCount + sizeof( char ) ) );
//    HELIUM_ASSERT( pNameString );
//    ReadBytes( pNameString, byteCount );
//    if( !m_bEndOfStream )
//    {
//        pNameString[ count ] = '\0';
//        rValue.Set( pNameString );
//    }
//
//    stackAllocator.Free( pNameString );
//}
//
///// @copydoc Serializer::SerializeString()
//void DirectDeserializer::SerializeString( String& rValue )
//{
//    ReadString( rValue );
//}
//
///// @copydoc Serializer::SerializeObjectReference()
//void DirectDeserializer::SerializeObjectReference( const AssetType* pType, AssetPtr& rspObject )
//{
//    HELIUM_UNREF( pType );
//
//    if( m_bEndOfStream )
//    {
//        return;
//    }
//
//    Asset* pObject = NULL;
//    ReadValue( pObject );
//    if( m_bEndOfStream )
//    {
//        return;
//    }
//
//    HELIUM_ASSERT( !pObject || pObject->IsClass( pType ) );
//    rspObject = pObject;
//}
//
///// Read bytes directly from the serialization buffer.
/////
///// @param[out] pData      Byte buffer in which to store the read data.
///// @param[in]  byteCount  Number of bytes to read.
//void DirectDeserializer::ReadBytes( void* pData, size_t byteCount )
//{
//    if( m_bEndOfStream )
//    {
//        return;
//    }
//
//    HELIUM_ASSERT( pData );
//    HELIUM_ASSERT( byteCount != 0 );
//
//    size_t bufferSize = m_rBuffer.GetSize();
//    size_t bufferRemaining = bufferSize - m_offset;
//    if( bufferRemaining < byteCount )
//    {
//        HELIUM_TRACE( TraceLevels::Error, TXT( "DirectDeserializer error: End of buffer reached during de-serialization.\n" ) );
//
//        m_offset = bufferSize;
//        m_bEndOfStream = true;
//
//        return;
//    }
//
//    const uint8_t* pBuffer = m_rBuffer.GetData();
//    HELIUM_ASSERT( pBuffer );
//    MemoryCopy( pData, pBuffer + m_offset, byteCount );
//    m_offset += byteCount;
//}
//
///// Read the byte contents of a value directly from the serialization buffer.
/////
///// @param[in] rValue  Value in which to read.
//template< typename T >
//void DirectDeserializer::ReadValue( T& rValue )
//{
//    ReadBytes( &rValue, sizeof( rValue ) );
//}
//
///// Read a string from the serialization buffer.
/////
///// @param[in] rString  String in which to read.
//template< typename CharType, typename Allocator >
//void DirectDeserializer::ReadString( StringBase< CharType, Allocator >& rString )
//{
//    if( m_bEndOfStream )
//    {
//        return;
//    }
//
//    size_t count = 0;
//    ReadValue( count );
//    if( m_bEndOfStream )
//    {
//        return;
//    }
//
//    if( !count )
//    {
//        rString.Clear();
//        return;
//    }
//
//    rString.Remove( 0, rString.GetSize() );
//    rString.Reserve( count );
//    rString.Add( static_cast< CharType >( ' ' ), count );
//    rString.Trim();
//
//    CharType* pStringBuffer = &rString[ 0 ];
//    HELIUM_ASSERT( pStringBuffer );
//    ReadBytes( pStringBuffer, sizeof( CharType ) * count );
//    if( m_bEndOfStream )
//    {
//        rString.Clear();
//    }
//}
