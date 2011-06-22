//----------------------------------------------------------------------------------------------------------------------
// DirectDeserializer.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_ENGINE_DIRECT_DESERIALIZER_H
#define LUNAR_ENGINE_DIRECT_DESERIALIZER_H

#include "Engine/Serializer.h"

#include "Foundation/Container/DynArray.h"

namespace Helium
{
    template< typename CharType, typename Allocator > class StringBase;
}

namespace Helium
{
    /// Serializer for retrieving object properties from a direct byte dump (previously created using DirectSerializer).
    ///
    /// Since tag information is not stored, the serialized data should only be de-serialized directly to an object of
    /// the same exact type.
    ///
    /// @note  This will serialize pointer values directly.  As such, it should only be used within the context of a
    ///        running instance of the application and never use data read from disk.
    class LUNAR_ENGINE_API DirectDeserializer : public Serializer
    {
    public:
        /// @name Construction/Destruction
        //@{
        DirectDeserializer( const DynArray< uint8_t >& rBuffer, size_t startOffset = 0 );
        //@}

        /// @name Data Access
        //@{
        inline size_t GetOffset() const;
        inline bool GetEndOfStream() const;
        //@}

        /// @name Serialization Interface
        //@{
        virtual bool Serialize( GameObject* pObject );
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
        virtual void SerializeEnum( int32_t& rValue, const Helium::Reflect::Enumeration* pEnumeration );
        virtual void SerializeCharName( CharName& rValue );
        virtual void SerializeWideName( WideName& rValue );
        virtual void SerializeCharString( CharString& rValue );
        virtual void SerializeWideString( WideString& rValue );
        virtual void SerializeObjectReference( const GameObjectType* pType, GameObjectPtr& rspObject );
        //@}

    private:
        /// Serialization buffer.
        const DynArray< uint8_t >& m_rBuffer;
        /// Current buffer offset.
        size_t m_offset;

        /// True if the end of the stream was reached.
        bool m_bEndOfStream;

        /// @name Private Utility Functions
        //@{
        void ReadBytes( void* pData, size_t byteCount );

        template< typename T > void ReadValue( T& rValue );
        template< typename CharType, typename Allocator > void ReadString( StringBase< CharType, Allocator >& rString );
        //@}
    };
}

#include "Engine/DirectDeserializer.inl"

#endif  // LUNAR_ENGINE_DIRECT_DESERIALIZER_H
