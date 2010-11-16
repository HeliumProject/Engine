//----------------------------------------------------------------------------------------------------------------------
// BinaryDeserializer.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_ENGINE_BINARY_DESERIALIZER_H
#define LUNAR_ENGINE_BINARY_DESERIALIZER_H

#include "Engine/Serializer.h"

#include "Foundation/Stream/ByteSwappingStream.h"
#include "Foundation/Stream/ExternalMemoryStream.h"

namespace Lunar
{
    /// Binary object property deserializer.
    class LUNAR_ENGINE_API BinaryDeserializer : public Serializer
    {
    public:
        /// @name Construction/Destruction
        //@{
        BinaryDeserializer();
        virtual ~BinaryDeserializer();
        //@}

        /// @name Serialization Control
        //@{
        void Prepare( const void* pData, size_t size );
        //@}

        /// @name Byte Order Setup
        //@{
        void SetByteSwapping( bool bSwapBytes );
        inline bool GetByteSwapping() const;
        //@}

        /// @name Serialization Interface
        //@{
        virtual bool Serialize( Object* pObject );
        virtual EMode GetMode() const;

        virtual void SerializeTag( const Tag& rTag );
        virtual bool CanResolveTags() const;

        virtual void SerializeBool( bool& rValue );
        virtual void SerializeInt8( int8_t& rValue );
        virtual void SerializeUint8( uint8_t& rValue );
        virtual void SerializeInt16( int16_t& rValue );
        virtual void SerializeUint16( uint16_t& rValue );
        virtual void SerializeInt32( int32_t& rValue );
        virtual void SerializeUint32( uint32_t& rValue );
        virtual void SerializeInt64( int64_t& rValue );
        virtual void SerializeUint64( uint64_t& rValue );
        virtual void SerializeFloat32( float32_t& rValue );
        virtual void SerializeFloat64( float64_t& rValue );
        virtual void SerializeBuffer( void* pBuffer, size_t elementSize, size_t count );
        virtual void SerializeEnum( int32_t& rValue, uint32_t nameCount, const tchar_t* const* ppNames );
        virtual void SerializeCharName( CharName& rValue );
        virtual void SerializeWideName( WideName& rValue );
        virtual void SerializeCharString( CharString& rValue );
        virtual void SerializeWideString( WideString& rValue );
        virtual void SerializeObjectReference( Type* pType, ObjectPtr& rspObject );

        virtual void PushPropertyFlags( uint32_t flags );
        virtual void PopPropertyFlags();
        //@}

        /// @name General Binary Deserialization Support
        //@{
        void BeginSerialize();
        bool EndSerialize();
        //@}

        /// @name Stream Information
        //@{
        inline size_t GetCurrentOffset() const;
        //@}

    private:
        /// Memory stream for deserialization.
        ExternalMemoryStream m_directStream;
        /// Byte-swapping stream interface.
        ByteSwappingStream m_byteSwappingStream;

        /// Pointer to the actual input stream to use.
        Stream* m_pStream;

        /// Property flag state stack.
        DynArray< uint32_t > m_propertyFlagStack;

        /// True if we reached the end of the memory stream early.
        bool m_bEndOfStream;

        /// @name Private Utility Functions
        //@{
        bool Deserialize( void* pBuffer, size_t elementSize, size_t count );
        template< typename T > bool DeserializeValue( T& rValue );

        uint32_t GetCurrentPropertyFlags() const;
        bool ShouldSerializeCurrentProperty() const;
        //@}
    };
}

#include "Engine/BinaryDeserializer.inl"

#endif  // LUNAR_ENGINE_BINARY_DESERIALIZER_H
