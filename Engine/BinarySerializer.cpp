////----------------------------------------------------------------------------------------------------------------------
//// BinarySerializer.cpp
////
//// Copyright (C) 2010 WhiteMoon Dreams, Inc.
//// All Rights Reserved
////----------------------------------------------------------------------------------------------------------------------
//
#include "EnginePch.h"
//#include "Engine/BinarySerializer.h"
//
//using namespace Helium;
//
///// Constructor.
//BinarySerializer::BinarySerializer()
//: m_bAllowObjectReferences( false )
//{
//    // Initialize the byte-swapping stream to utilize the direct property stream.
//    m_byteSwappingStream.Open( &m_directPropertyStream );
//
//    // Don't swap bytes by default.
//    m_pPropertyStream = &m_directPropertyStream;
//}
//
///// Destructor.
//BinarySerializer::~BinarySerializer()
//{
//}
//
///// Set whether byte swapping should be performed during serialization.
/////
///// @param[in] bSwapBytes  True to swap bytes, false not to.
/////
///// @see GetByteSwapping()
//void BinarySerializer::SetByteSwapping( bool bSwapBytes )
//{
//    m_pPropertyStream =
//        ( bSwapBytes
//        ? static_cast< Stream* >( &m_byteSwappingStream )
//        : static_cast< Stream* >( &m_directPropertyStream ) );
//}
//
///// @copydoc Serializer::Serialize()
//bool BinarySerializer::Serialize( GameObject* pObject )
//{
//    HELIUM_ASSERT( pObject );
//
//    BeginSerialize( true );
//
//    // Serialize the object type reference.
//    const GameObjectType* pType = pObject->GetGameObjectType();
//    HELIUM_ASSERT( pType );
//    uint32_t typeIndex = ResolveTypeDependency( pType->GetName() );
//    HELIUM_ASSERT( IsValid( typeIndex ) );
//    m_pPropertyStream->Write( &typeIndex, sizeof( typeIndex ), 1 );
//
//    // Serialize the object template.
//    uint32_t templateIndex;
//    SetInvalid( templateIndex );
//
//    if( !pObject->IsDefaultTemplate() )
//    {
//        GameObject* pTemplate = Reflect::AssertCast< GameObject >( pObject->GetTemplate() );
//        HELIUM_ASSERT( pTemplate );
//        templateIndex = ResolveObjectDependency( pTemplate->GetPath() );
//        HELIUM_ASSERT( IsValid( templateIndex ) );
//    }
//
//    m_pPropertyStream->Write( &templateIndex, sizeof( templateIndex ), 1 );
//
//    // Serialize the object owner.
//    uint32_t ownerIndex;
//    SetInvalid( ownerIndex );
//
//    GameObject* pOwner = pObject->GetOwner();
//    if( pOwner )
//    {
//        ownerIndex = ResolveObjectDependency( pOwner->GetPath() );
//        HELIUM_ASSERT( IsValid( ownerIndex ) );
//    }
//
//    m_pPropertyStream->Write( &ownerIndex, sizeof( ownerIndex ), 1 );
//
//    // Serialize the object properties.
//    pObject->Serialize( *this );
//
//    EndSerialize();
//
//    return true;
//}
//
///// @copydoc Serializer::GetMode()
//Serializer::EMode BinarySerializer::GetMode() const
//{
//    return MODE_SAVE;
//}
//
///// @copydoc Serializer::SerializeTag()
//void BinarySerializer::SerializeTag( const Tag& /*rTag*/ )
//{
//    // Tags are ignored for binary serialization.
//}
//
///// @copydoc Serializer::CanResolveTags()
//bool BinarySerializer::CanResolveTags() const
//{
//    return false;
//}
//
///// @copydoc Serializer::SerializeBool()
//void BinarySerializer::SerializeBool( bool& rValue )
//{
//    if( ShouldSerializeCurrentProperty() )
//    {
//        m_pPropertyStream->Write( &rValue, sizeof( rValue ), 1 );
//    }
//}
//
///// @copydoc Serializer::SerializeInt8()
//void BinarySerializer::SerializeInt8( int8_t& rValue )
//{
//    if( ShouldSerializeCurrentProperty() )
//    {
//        m_pPropertyStream->Write( &rValue, sizeof( rValue ), 1 );
//    }
//}
//
///// @copydoc Serializer::SerializeUint8()
//void BinarySerializer::SerializeUint8( uint8_t& rValue )
//{
//    if( ShouldSerializeCurrentProperty() )
//    {
//        m_pPropertyStream->Write( &rValue, sizeof( rValue ), 1 );
//    }
//}
//
///// @copydoc Serializer::SerializeInt16()
//void BinarySerializer::SerializeInt16( int16_t& rValue )
//{
//    if( ShouldSerializeCurrentProperty() )
//    {
//        m_pPropertyStream->Write( &rValue, sizeof( rValue ), 1 );
//    }
//}
//
///// @copydoc Serializer::SerializeUint16()
//void BinarySerializer::SerializeUint16( uint16_t& rValue )
//{
//    if( ShouldSerializeCurrentProperty() )
//    {
//        m_pPropertyStream->Write( &rValue, sizeof( rValue ), 1 );
//    }
//}
//
///// @copydoc Serializer::SerializeInt32()
//void BinarySerializer::SerializeInt32( int32_t& rValue )
//{
//    if( ShouldSerializeCurrentProperty() )
//    {
//        m_pPropertyStream->Write( &rValue, sizeof( rValue ), 1 );
//    }
//}
//
///// @copydoc Serializer::SerializeUint32()
//void BinarySerializer::SerializeUint32( uint32_t& rValue )
//{
//    if( ShouldSerializeCurrentProperty() )
//    {
//        m_pPropertyStream->Write( &rValue, sizeof( rValue ), 1 );
//    }
//}
//
///// @copydoc Serializer::SerializeInt64()
//void BinarySerializer::SerializeInt64( int64_t& rValue )
//{
//    if( ShouldSerializeCurrentProperty() )
//    {
//        m_pPropertyStream->Write( &rValue, sizeof( rValue ), 1 );
//    }
//}
//
///// @copydoc Serializer::SerializeUint64()
//void BinarySerializer::SerializeUint64( uint64_t& rValue )
//{
//    if( ShouldSerializeCurrentProperty() )
//    {
//        m_pPropertyStream->Write( &rValue, sizeof( rValue ), 1 );
//    }
//}
//
///// @copydoc Serializer::SerializeFloat32()
//void BinarySerializer::SerializeFloat32( float32_t& rValue )
//{
//    if( ShouldSerializeCurrentProperty() )
//    {
//        m_pPropertyStream->Write( &rValue, sizeof( rValue ), 1 );
//    }
//}
//
///// @copydoc Serializer::SerializeFloat64()
//void BinarySerializer::SerializeFloat64( float64_t& rValue )
//{
//    if( ShouldSerializeCurrentProperty() )
//    {
//        m_pPropertyStream->Write( &rValue, sizeof( rValue ), 1 );
//    }
//}
//
///// @copydoc Serializer::SerializeBuffer()
//void BinarySerializer::SerializeBuffer( void* pBuffer, size_t elementSize, size_t count )
//{
//    if( ShouldSerializeCurrentProperty() )
//    {
//        m_pPropertyStream->Write( pBuffer, elementSize, count );
//    }
//}
//
///// @copydoc Serializer::SerializeEnum()
//void BinarySerializer::SerializeEnum( int32_t& rValue, uint32_t /*nameCount*/, const tchar_t* const* /*ppNames*/ )
//{
//    if( ShouldSerializeCurrentProperty() )
//    {
//        m_pPropertyStream->Write( &rValue, sizeof( rValue ), 1 );
//    }
//}
//
///// @copydoc Serializer::SerializeEnum()
//void BinarySerializer::SerializeEnum( int32_t& rValue, const Helium::Reflect::Enumeration* /*pEnumeration*/ )
//{
//    if( ShouldSerializeCurrentProperty() )
//    {
//        m_pPropertyStream->Write( &rValue, sizeof( rValue ), 1 );
//    }
//}
//
///// @copydoc Serializer::SerializeCharName()
//void BinarySerializer::SerializeCharName( CharName& rValue )
//{
//    if( ShouldSerializeCurrentProperty() )
//    {
//        CharString nameString( *rValue );
//        SerializeCharString( nameString );
//    }
//}
//
///// @copydoc Serializer::SerializeWideName()
//void BinarySerializer::SerializeWideName( WideName& rValue )
//{
//    if( ShouldSerializeCurrentProperty() )
//    {
//        WideString nameString( *rValue );
//        SerializeWideString( nameString );
//    }
//}
//
///// @copydoc Serializer::SerializeCharString()
//void BinarySerializer::SerializeCharString( CharString& rValue )
//{
//    if( ShouldSerializeCurrentProperty() )
//    {
//        HELIUM_ASSERT( rValue.GetSize() <= UINT32_MAX );
//        uint32_t stringLength = static_cast< uint32_t >( rValue.GetSize() );
//        m_pPropertyStream->Write( &stringLength, sizeof( stringLength ), 1 );
//
//        m_pPropertyStream->Write( rValue.GetData(), sizeof( char ), stringLength );
//    }
//}
//
///// @copydoc Serializer::SerializeWideString()
//void BinarySerializer::SerializeWideString( WideString& rValue )
//{
//    if( ShouldSerializeCurrentProperty() )
//    {
//        HELIUM_ASSERT( rValue.GetSize() <= UINT32_MAX );
//        uint32_t stringLength = static_cast< uint32_t >( rValue.GetSize() );
//        m_pPropertyStream->Write( &stringLength, sizeof( stringLength ), 1 );
//
//        m_pPropertyStream->Write( rValue.GetData(), sizeof( wchar_t ), stringLength );
//    }
//}
//
///// @copydoc Serializer::SerializeObjectReference()
//void BinarySerializer::SerializeObjectReference( const GameObjectType* /*pType*/, GameObjectPtr& rspObject )
//{
//    if( ShouldSerializeCurrentProperty() )
//    {
//        HELIUM_ASSERT( m_bAllowObjectReferences );
//        if( !m_bAllowObjectReferences )
//        {
//            HELIUM_TRACE(
//                TRACE_ERROR,
//                ( TXT( "BinarySerializer: Attempted to serialize an object reference using a serializer with " )
//                TXT( "which it is not allowed.\n" ) ) );
//
//            return;
//        }
//
//        uint32_t objectIndex;
//        SetInvalid( objectIndex );
//
//        GameObject* pObject = rspObject.Get();
//        if( pObject )
//        {
//            objectIndex = ResolveObjectDependency( pObject->GetPath() );
//            HELIUM_ASSERT( IsValid( objectIndex ) );
//        }
//
//        m_pPropertyStream->Write( &objectIndex, sizeof( objectIndex ), 1 );
//    }
//}
//
///// @copydoc Serializer::PushPropertyFlags()
//void BinarySerializer::PushPropertyFlags( uint32_t flags )
//{
//    m_propertyFlagStack.Push( GetCurrentPropertyFlags() | flags );
//}
//
///// @copydoc Serializer::PopPropertyFlags()
//void BinarySerializer::PopPropertyFlags()
//{
//    HELIUM_ASSERT( !m_propertyFlagStack.IsEmpty() );
//    m_propertyFlagStack.Pop();
//}
//
///// Prepare this serializer for general serializing of binary data.
/////
///// @param[in] bAllowObjectReferences  True if object references can be serialized, false if not.
/////
///// @see EndSerialize()
//void BinarySerializer::BeginSerialize( bool bAllowObjectReferences )
//{
//    HELIUM_ASSERT( !m_directPropertyStream.IsOpen() );
//
//    m_typeDependencies.Clear();
//    m_objectDependencies.Clear();
//
//    m_propertyStreamBuffer.Resize( 0 );
//    m_directPropertyStream.Open( &m_propertyStreamBuffer );
//
//    m_propertyFlagStack.Resize( 0 );
//
//    m_bAllowObjectReferences = bAllowObjectReferences;
//}
//
///// Finish binary data serialization originally started using BeginSerialize().
/////
///// @see BeginSerialize()
//void BinarySerializer::EndSerialize()
//{
//    HELIUM_ASSERT( m_directPropertyStream.IsOpen() );
//
//    m_directPropertyStream.Close();
//}
//
///// Write the serialized property data to the given stream.
/////
///// @param[in] pStream  Stream to which the property data should be written.
//void BinarySerializer::WriteToStream( Stream* pStream ) const
//{
//    HELIUM_ASSERT( pStream );
//
//    // Write the type and object link tables first.
//    uint32_t dependencyCount;
//    uint_fast32_t dependencyCountFast;
//
//    HELIUM_ASSERT( m_typeDependencies.GetSize() <= UINT32_MAX );
//    dependencyCount = static_cast< uint32_t >( m_typeDependencies.GetSize() );
//    pStream->Write( &dependencyCount, sizeof( dependencyCount ), 1 );
//
//    dependencyCountFast = dependencyCount;
//    for( uint_fast32_t dependencyIndex = 0; dependencyIndex < dependencyCountFast; ++dependencyIndex )
//    {
//        const tchar_t* pTypeName = *m_typeDependencies[ dependencyIndex ];
//
//        size_t typeNameSize = StringLength( pTypeName );
//        HELIUM_ASSERT( typeNameSize <= UINT32_MAX );
//        uint32_t typeNameSize32 = static_cast< uint32_t >( typeNameSize );
//        pStream->Write( &typeNameSize32, sizeof( typeNameSize32 ), 1 );
//        if( typeNameSize )
//        {
//            pStream->Write( pTypeName, sizeof( tchar_t ), typeNameSize );
//        }
//    }
//
//    HELIUM_ASSERT( m_objectDependencies.GetSize() <= UINT32_MAX );
//    dependencyCount = static_cast< uint32_t >( m_objectDependencies.GetSize() );
//    pStream->Write( &dependencyCount, sizeof( dependencyCount ), 1 );
//
//    String pathString;
//
//    dependencyCountFast = dependencyCount;
//    for( uint_fast32_t dependencyIndex = 0; dependencyIndex < dependencyCountFast; ++dependencyIndex )
//    {
//        m_objectDependencies[ dependencyIndex ].ToString( pathString );
//
//        size_t pathStringSize = pathString.GetSize();
//        HELIUM_ASSERT( pathStringSize <= UINT32_MAX );
//        uint32_t pathStringSize32 = static_cast< uint32_t >( pathString.GetSize() );
//        pStream->Write( &pathStringSize32, sizeof( pathStringSize32 ), 1 );
//        if( pathStringSize )
//        {
//            pStream->Write( pathString.GetData(), sizeof( tchar_t ), pathStringSize );
//        }
//    }
//
//    // Write the serialized data to the output stream.
//    pStream->Write( m_propertyStreamBuffer.GetData(), 1, m_propertyStreamBuffer.GetSize() );
//}
//
///// Resolve a dependency on a type reference.
/////
///// @param[in] typeName  Type name.
/////
///// @return  Dependency index.
/////
///// @see ResolveObjectDependency()
//uint32_t BinarySerializer::ResolveTypeDependency( Name typeName )
//{
//    uint32_t typeIndex;
//    SetInvalid( typeIndex );
//
//    if( !typeName.IsEmpty() )
//    {
//        size_t dependencyCount = m_typeDependencies.GetSize();
//        for( size_t dependencyIndex = 0; dependencyIndex < dependencyCount; ++dependencyIndex )
//        {
//            if( m_typeDependencies[ dependencyIndex ] == typeName )
//            {
//                return static_cast< uint32_t >( dependencyIndex );
//            }
//        }
//
//        HELIUM_ASSERT( dependencyCount < UINT32_MAX );
//        m_typeDependencies.Push( typeName );
//        typeIndex = static_cast< uint32_t >( dependencyCount );
//    }
//
//    return typeIndex;
//}
//
///// Resolve a dependency on an object reference.
/////
///// @param[in] path  Object path.
/////
///// @return  Dependency index.
/////
///// @see ResolveTypeDependency()
//uint32_t BinarySerializer::ResolveObjectDependency( GameObjectPath path )
//{
//    uint32_t objectIndex;
//    SetInvalid( objectIndex );
//
//    if( !path.IsEmpty() )
//    {
//        size_t dependencyCount = m_objectDependencies.GetSize();
//        for( size_t dependencyIndex = 0; dependencyIndex < dependencyCount; ++dependencyIndex )
//        {
//            if( m_objectDependencies[ dependencyIndex ] == path )
//            {
//                return static_cast< uint32_t >( dependencyIndex );
//            }
//        }
//
//        HELIUM_ASSERT( dependencyCount < UINT32_MAX );
//        m_objectDependencies.Push( path );
//        objectIndex = static_cast< uint32_t >( dependencyCount );
//    }
//
//    return objectIndex;
//}
//
///// Get the current set of property flags.
/////
///// @return  Current property flag set.
//uint32_t BinarySerializer::GetCurrentPropertyFlags() const
//{
//    size_t flagStackSize = m_propertyFlagStack.GetSize();
//
//    return ( flagStackSize == 0 ? 0 : m_propertyFlagStack[ flagStackSize - 1 ] );
//}
//
///// Get whether the current property should be serialized based on the current property flags.
/////
///// @return  True if the current property should be serialized, false if not.
//bool BinarySerializer::ShouldSerializeCurrentProperty() const
//{
//    return ( !( GetCurrentPropertyFlags() & FLAG_EDITOR_ONLY ) );
//}
