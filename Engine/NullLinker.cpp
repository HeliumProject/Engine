////----------------------------------------------------------------------------------------------------------------------
//// NullLinker.cpp
////
//// Copyright (C) 2010 WhiteMoon Dreams, Inc.
//// All Rights Reserved
////----------------------------------------------------------------------------------------------------------------------
//
#include "EnginePch.h"
//#include "Engine/NullLinker.h"
//
//using namespace Helium;
//
///// Constructor.
//NullLinker::NullLinker()
//{
//}
//
///// Destructor.
//NullLinker::~NullLinker()
//{
//}
//
///// @name Serializer::Serialize()
//bool NullLinker::Serialize( GameObject* pObject )
//{
//    HELIUM_ASSERT( pObject );
//
//    pObject->Serialize( *this );
//
//    return true;
//}
//
///// @name Serializer::GetMode()
//Serializer::EMode NullLinker::GetMode() const
//{
//    return MODE_LINK;
//}
//
///// @name Serializer::SerializeTag()
//void NullLinker::SerializeTag( const Tag& /*rTag*/ )
//{
//}
//
///// @copydoc Serializer::CanResolveTags()
//bool NullLinker::CanResolveTags() const
//{
//    return false;
//}
//
///// @name Serializer::SerializeBool()
//void NullLinker::SerializeBool( bool& /*rValue*/ )
//{
//}
//
///// @name Serializer::SerializeInt8()
//void NullLinker::SerializeInt8( int8_t& /*rValue*/ )
//{
//}
//
///// @name Serializer::SerializeUint8()
//void NullLinker::SerializeUint8( uint8_t& /*rValue*/ )
//{
//}
//
///// @name Serializer::SerializeInt16()
//void NullLinker::SerializeInt16( int16_t& /*rValue*/ )
//{
//}
//
///// @name Serializer::SerializeUint16()
//void NullLinker::SerializeUint16( uint16_t& /*rValue*/ )
//{
//}
//
///// @name Serializer::SerializeInt32()
//void NullLinker::SerializeInt32( int32_t& /*rValue*/ )
//{
//}
//
///// @name Serializer::SerializeUint32()
//void NullLinker::SerializeUint32( uint32_t& /*rValue*/ )
//{
//}
//
///// @name Serializer::SerializeInt64()
//void NullLinker::SerializeInt64( int64_t& /*rValue*/ )
//{
//}
//
///// @name Serializer::SerializeUint64()
//void NullLinker::SerializeUint64( uint64_t& /*rValue*/ )
//{
//}
//
///// @name Serializer::SerializeFloat32()
//void NullLinker::SerializeFloat32( float32_t& /*rValue*/ )
//{
//}
//
///// @name Serializer::SerializeFloat64()
//void NullLinker::SerializeFloat64( float64_t& /*rValue*/ )
//{
//}
//
///// @name Serializer::SerializeBuffer()
//void NullLinker::SerializeBuffer( void* /*pBuffer*/, size_t /*elementSize*/, size_t /*count*/ )
//{
//}
//
///// @name Serializer::SerializeEnum()
//void NullLinker::SerializeEnum( int32_t& /*rValue*/, uint32_t /*nameCount*/, const tchar_t* const* /*ppNames*/ )
//{
//}
//
///// @name Serializer::SerializeEnum()
//void NullLinker::SerializeEnum( int32_t& /*rValue*/, const Helium::Reflect::Enumeration* /*pEnumeration*/ )
//{
//}
//
///// @name Serializer::SerializeName()
//void NullLinker::SerializeName( Name& /*rValue*/ )
//{
//}
//
///// @name Serializer::SerializeString()
//void NullLinker::SerializeString( String& /*rValue*/ )
//{
//}
//
///// @name Serializer::SerializeObjectReference()
//void NullLinker::SerializeObjectReference( const GameObjectType* /*pType*/, GameObjectPtr& rspObject )
//{
//    rspObject.ClearLinkIndex();
//}