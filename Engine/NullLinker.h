////----------------------------------------------------------------------------------------------------------------------
//// NullLinker.h
////
//// Copyright (C) 2010 WhiteMoon Dreams, Inc.
//// All Rights Reserved
////----------------------------------------------------------------------------------------------------------------------
//
//#pragma once
//#ifndef HELIUM_ENGINE_NULL_LINKER_H
//#define HELIUM_ENGINE_NULL_LINKER_H
//
//#include "Engine/Serializer.h"
//
//namespace Helium
//{
//    /// Serialization linker that merely unsets the link indices on all object references.
//    ///
//    /// This is used in case we encounter an error during the deserialization process.  When such load errors occur, the
//    /// object is passed through an instance of this class in order to zero out link indices prior to destroying the
//    /// object, as the smart pointer destructor will otherwise interpret the link indices as object pointers and attempt
//    /// to dereference them (bad).
//    class HELIUM_ENGINE_API NullLinker : public Serializer
//    {
//    public:
//        /// @name Construction/Destruction
//        //@{
//        NullLinker();
//        virtual ~NullLinker();
//        //@}
//
//        /// @name Serialization Interface
//        //@{
//        virtual bool Serialize( GameObject* pObject );
//        virtual EMode GetMode() const;
//
//        virtual void SerializeTag( const Tag& rTag );
//        virtual bool CanResolveTags() const;
//
//        virtual void SerializeBool( bool& rValue );
//        virtual void SerializeInt8( int8_t& rValue );
//        virtual void SerializeUint8( uint8_t& rValue );
//        virtual void SerializeInt16( int16_t& rValue );
//        virtual void SerializeUint16( uint16_t& rValue );
//        virtual void SerializeInt32( int32_t& rValue );
//        virtual void SerializeUint32( uint32_t& rValue );
//        virtual void SerializeInt64( int64_t& rValue );
//        virtual void SerializeUint64( uint64_t& rValue );
//        virtual void SerializeFloat32( float32_t& rValue );
//        virtual void SerializeFloat64( float64_t& rValue );
//        virtual void SerializeBuffer( void* pBuffer, size_t elementSize, size_t count );
//        virtual void SerializeEnum( int32_t& rValue, uint32_t nameCount, const tchar_t* const* ppNames );
//        virtual void SerializeEnum( int32_t& rValue, const Helium::Reflect::Enumeration* pEnumeration );
//        virtual void SerializeName( Name& rValue );
//        virtual void SerializeString( String& rValue );
//        virtual void SerializeObjectReference( const GameObjectType* pType, GameObjectPtr& rspObject );
//        //@}
//    };
//}
//
//#endif  // HELIUM_ENGINE_NULL_LINKER_H
