#pragma once

#include "BasicBuffer.h"

namespace Helium
{
    template< typename T > class ObjectBuffer;
    template< typename T > class ObjectArrayBuffer;

    class FOUNDATION_API BufferSerializer
    {
        // Profile interface
#ifdef PROFILE_ACCUMULATION
        static Profile::Accumulator s_ReadAccum;
        static Profile::Accumulator s_WriteAccum;
#endif

    protected:
        ByteOrder m_ByteOrder;
        S_SmartBufferPtr m_Buffers;

    public:
        BufferSerializer();
        BufferSerializer( ByteOrder );

        ByteOrder GetByteOrder()
        {
            return m_ByteOrder;
        }

        void SetByteOrder(ByteOrder platform)
        {
            m_ByteOrder = platform;
        }

        S_SmartBufferPtr::Iterator begin() const
        {
            return m_Buffers.Begin();
        }

        S_SmartBufferPtr::Iterator end() const
        {
            return m_Buffers.End();
        }

        void Reset()
        {
            m_Buffers.Clear();
        }

        template< typename T >
        Helium::SmartPtr< ObjectBuffer<T> > CreateObject()
        {
            return CreateObject<T>( -1 );
        }

        template< typename T >
        Helium::SmartPtr< ObjectBuffer<T> > CreateObject( uint32_t type )
        {
            Helium::SmartPtr< ObjectBuffer<T> > return_val = new ObjectBuffer<T>;
            return_val->SetType( type );
            return_val->SetByteOrder( m_ByteOrder );
            m_Buffers.Append( return_val );
            return return_val;
        }

        template< typename T >
        Helium::SmartPtr< ObjectArrayBuffer<T> > CreateObjectArray(uint32_t size)
        {
            return CreateObjectArray<T>( size, -1 );
        }

        template< typename T >
        Helium::SmartPtr< ObjectArrayBuffer<T> > CreateObjectArray( uint32_t size, uint32_t type )
        {
            Helium::SmartPtr< ObjectArrayBuffer<T> > return_val = new ObjectArrayBuffer<T>( size );
            return_val->SetType( type );
            return_val->SetByteOrder( m_ByteOrder );
            m_Buffers.Append( return_val );
            return return_val;
        }

    public:
        BasicBufferPtr CreateBasic( uint32_t type = 0xffffffff, bool track = true );

        void AddBuffer( const SmartBufferPtr& buffer );
        void AddBuffers( const BufferSerializer& serializer );

    public:
        uint32_t ComputeSize() const;
        bool WriteToFile( const tchar* filename ) const;
        bool WriteToStream( tostream& strm ) const;
        bool ReadFromFile( const tchar* filename );
        bool ReadFromStream( tistream& strm );
    };
}