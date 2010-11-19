//----------------------------------------------------------------------------------------------------------------------
// DirectSerializer.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_ENGINE_DIRECT_SERIALIZER_H
#define LUNAR_ENGINE_DIRECT_SERIALIZER_H

#include "Engine/Serializer.h"

namespace Lunar
{
    /// Serializer for performing a direct byte dump of an object to a buffer.
    ///
    /// Since tag information is not stored, the serialized data should only be de-serialized directly to an object of
    /// the same exact type.
    ///
    /// @note  This will serialize pointer values directly.  As such, it should only be used within the context of a
    ///        running instance of the application and never written to disk.
    class LUNAR_ENGINE_API DirectSerializer : public Serializer
    {
    public:
        /// @name Construction/Destruction
        //@{
        DirectSerializer( DynArray< uint8_t >& rBuffer );
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
        virtual void SerializeCharName( CharName& rValue );
        virtual void SerializeWideName( WideName& rValue );
        virtual void SerializeCharString( CharString& rValue );
        virtual void SerializeWideString( WideString& rValue );
        virtual void SerializeObjectReference( Type* pType, GameObjectPtr& rspObject );
        //@}

    private:
        /// Serialization buffer.
        DynArray< uint8_t >& m_rBuffer;

        /// @name Private Utility Functions
        //@{
        void WriteBytes( const void* pData, size_t byteCount );

        template< typename T > void WriteValue( const T& rValue );
        template< typename T > void WriteString( const T* pString, size_t count );
        //@}
    };
}

#endif  // LUNAR_ENGINE_DIRECT_SERIALIZER_H
