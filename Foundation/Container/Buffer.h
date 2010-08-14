#pragma once

#include "Array.h"

namespace Helium
{
    //
    // An array of bytes that holds objects of various sizes
    //

    class Buffer
    {
    public:
        Buffer( size_t size = 0, Allocator* alloc = NULL )
            : m_Data ( size, alloc )
        {

        }

        void Clear()
        {
            m_Data.Clear();
        }

        void* GetData()
        {
            return m_Data.GetData();
        }

        const void* GetData() const
        {
            return m_Data.GetData();
        }

        size_t GetSize() const
        {
            return m_Data.GetSize();
        }

        template< class T >
        T* Append()
        {
            size_t size = m_Data.GetSize();

            // grow the array, allocating if necessary
            m_Data.Resize( m_Data.GetSize() + sizeof( T ) );

            // get a pointer to where our bytes are going to go
            T* data = (T*)(m_Data.GetData() + size);

            // init the data
            Helium::ConstructObject<T>( data );

            return data;
        }

        template< class T >
        T* Append( const T& value )
        {
            T* data = Append<T>();

            // copy the data
            *data = value;

            return data;
        }

    private:
        Array<char> m_Data;
    };

    //
    // A buffer with static storage that hits an allocator on overflow
    //

    template< class T, size_t S >
    class StaticOverflowBuffer
    {
        StaticOverflowBuffer(size_t count = 0, Allocator* alloc = NULL)
            : m_DynamicStorage( count > S ? count : 0, alloc )
        {
            // must be after init of dynamic storage
            m_Data = ( count > S ? m_DynamicStorage.GetData() : &m_StaticStorage );

            if ( IsUsingStaticStorage() )
            {
                ConstructObjects<T>( m_StaticStorage, S );
            }
        }

        T* GetData()
        {
            return m_Data;
        }

        const T* GetData() const
        {
            return m_Data;
        }

        size_t GetSize() const
        {
            return IsUsingStaticStorage() ? GetStaticSize() : GetDynamicSize();
        }

        size_t GetStaticSize() const
        {
            return S;
        }

        size_t GetDynamicSize() const
        {
            return m_DynamicStorage.GetSize();
        }

        bool IsUsingStaticStorage() const
        {
            return m_Data == &m_StaticStorage;
        }

        bool IsUsingDynamicStorage() const
        {
            return m_Data != &m_StaticStorage;
        }

        void Resize( size_t count )
        {
            if ( count > S )
            {
                m_DynamicStorage.Resize( count );

                if ( IsUsingStaticStorage() )
                {
                    memcpy( m_DynamicStorage.GetData(), &m_StaticStorage, sizeof(T) * S );
                }

                m_Data = m_DynamicStorage.GetData();
            }
            else
            {
                if ( IsUsingDynamicStorage() )
                {
                    memcpy( &m_StaticStorage, m_DynamicStorage.GetData(), sizeof(T) * S );
                }

                // don't bother to discard the memory in the dynamic storage for thrashing scenarios

                m_Data = &m_StaticStorage;
            }
        }

        void FreeDynamicStorage( bool saveData = false )
        {
            if ( saveData )
            {
                Resize( S );
            }
            else
            {
                m_DynamicStorage.Clear();
                m_Data = &m_StaticStorage;
            }
        }

    private:
        T         m_StaticStorage[ S ];
        Array<T>  m_DynamicStorage;
        T*        m_Data;
    };
}
