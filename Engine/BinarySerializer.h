////----------------------------------------------------------------------------------------------------------------------
//// BinarySerializer.h
////
//// Copyright (C) 2010 WhiteMoon Dreams, Inc.
//// All Rights Reserved
////----------------------------------------------------------------------------------------------------------------------
//
//#pragma once
//#ifndef HELIUM_ENGINE_BINARY_SERIALIZER_H
//#define HELIUM_ENGINE_BINARY_SERIALIZER_H
//
//#include "Engine/Serializer.h"
//
//#include "Foundation/Stream.h"
//#include "Foundation/MemoryStream.h"
//
//namespace Helium
//{
//    /// GameObject serializer for package and resource caching.
//    class HELIUM_ENGINE_API BinarySerializer : public Serializer
//    {
//    public:
//        /// @name Construction/Destruction
//        //@{
//        BinarySerializer();
//        virtual ~BinarySerializer();
//        //@}
//
//        /// @name Byte Order Setup
//        //@{
//        void SetByteSwapping( bool bSwapBytes );
//        inline bool GetByteSwapping() const;
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
//
//        virtual void PushPropertyFlags( uint32_t flags );
//        virtual void PopPropertyFlags();
//        //@}
//
//        /// @name General Binary Serialization Support
//        //@{
//        void BeginSerialize( bool bAllowObjectReferences = false );
//        void EndSerialize();
//        //@}
//
//        /// @name Result Access
//        //@{
//        inline const DynArray< uint8_t >& GetPropertyStreamBuffer() const;
//        void WriteToStream( Stream* pStream ) const;
//        //@}
//
//    private:
//        /// Type dependency table.
//        DynArray< Name > m_typeDependencies;
//        /// Object dependency table.
//        DynArray< GameObjectPath > m_objectDependencies;
//
//        /// GameObject property stream.
//        DynamicMemoryStream m_directPropertyStream;
//        /// Memory buffer for the object property stream.
//        DynArray< uint8_t > m_propertyStreamBuffer;
//        /// Byte-swapping stream interface.
//        ByteSwappingStream m_byteSwappingStream;
//
//        /// Pointer to the actual output stream to use.
//        Stream* m_pPropertyStream;
//
//        /// Property flag state stack.
//        DynArray< uint32_t > m_propertyFlagStack;
//
//        /// True if object references can be serialized.
//        bool m_bAllowObjectReferences;
//
//        /// @name Private Utility Functions
//        //@{
//        uint32_t ResolveTypeDependency( Name typeName );
//        uint32_t ResolveObjectDependency( GameObjectPath path );
//
//        uint32_t GetCurrentPropertyFlags() const;
//        bool ShouldSerializeCurrentProperty() const;
//        //@}
//    };
//}
//
//#include "Engine/BinarySerializer.inl"
//
//#endif  // HELIUM_ENGINE_BINARY_SERIALIZER_H
