#pragma once

#include "BasicBuffer.h"

namespace Nocturnal
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
        BufferPlatform m_Platform;
        S_SmartBufferPtr m_Buffers;

    public:
        BufferSerializer();
        BufferSerializer( BufferPlatform );

        BufferPlatform GetPlatform()
        {
            return m_Platform;
        }

        void SetPlatform(BufferPlatform platform)
        {
            m_Platform = platform;
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
        Nocturnal::SmartPtr< ObjectBuffer<T> > CreateObject()
        {
            return CreateObject<T>( -1 );
        }

        template< typename T >
        Nocturnal::SmartPtr< ObjectBuffer<T> > CreateObject( u32 type )
        {
            Nocturnal::SmartPtr< ObjectBuffer<T> > return_val = new ObjectBuffer<T>;
            return_val->SetType( type );
            return_val->SetPlatform( m_Platform );
            m_Buffers.Append( return_val );
            return return_val;
        }

        template< typename T >
        Nocturnal::SmartPtr< ObjectArrayBuffer<T> > CreateObjectArray(u32 size)
        {
            return CreateObjectArray<T>( size, -1 );
        }

        template< typename T >
        Nocturnal::SmartPtr< ObjectArrayBuffer<T> > CreateObjectArray( u32 size, u32 type )
        {
            Nocturnal::SmartPtr< ObjectArrayBuffer<T> > return_val = new ObjectArrayBuffer<T>( size );
            return_val->SetType( type );
            return_val->SetPlatform( m_Platform );
            m_Buffers.Append( return_val );
            return return_val;
        }

    public:
        BasicBufferPtr CreateBasic( u32 type = 0xffffffff, bool track = true );

        void AddBuffer( const SmartBufferPtr& buffer );
        void AddBuffers( const BufferSerializer& serializer );

    public:
        u32 ComputeSize() const;
        bool WriteToFile( const char* filename ) const;
        bool WriteToStream( std::ostream& strm ) const;
        bool ReadFromFile( const char* filename );
        bool ReadFromStream( std::istream& strm );
    };
}