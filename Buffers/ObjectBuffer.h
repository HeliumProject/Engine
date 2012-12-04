#pragma once

#include "SmartBuffer.h"

#define OB_TYPEDEF( T ) \
  typedef Helium::SmartPtr< Helium::ObjectBuffer< T > >      T##Buffer; \
  typedef Helium::SmartPtr< Helium::ObjectArrayBuffer< T > > T##ArrayBuffer; \

namespace Helium
{
  //
  // The ObjectBuffer classes are a more complex implementation of a SmartBuffer.  
  //

  template <typename T>
  class ObjectBuffer : public SmartBuffer
  {
  public:
    ObjectBuffer() 
      : SmartBuffer()
    {
      Reserve( sizeof( T ) );
      m_Size = sizeof( T );
    }

    inline operator T* ()
    {
      return (T*)(m_Data);
    }
  
    inline T* operator-> ()
    {
      return (T*)(m_Data);
    }
  };

  template <typename T>
  class ObjectArrayBuffer : public SmartBuffer
  {
  protected:
    uint32_t m_array_size;

  public:
    ObjectArrayBuffer(uint32_t size) 
      : SmartBuffer()
      , m_array_size( size )
    {
      if ( size > 0 )
      {
        GrowBy( sizeof( T ) * size );
        m_Size = sizeof( T ) * size;
      }
    }

    inline uint32_t Count() const
    {
      return m_array_size;
    }

    inline void Reserve(uint32_t size)
    {
      if ( m_array_size < size )
      {
        uint32_t diff = size - m_array_size;
        GrowBy( sizeof( T ) * diff );
        m_Size += sizeof( T ) * diff;
        m_array_size = size;
      }
    }

    inline T& operator[] (uint32_t index)
    {
      return At(index);
    }

    inline T& At(uint32_t index)
    {
      Reserve( index + 1 );
      return *((T*)(m_Data + ( sizeof( T ) * index ) ));
    }

    inline StrongBufferLocation GetIndexOffsetLocation( uint32_t index, uint32_t offset )
    {
      return GetOffsetLocation( ( sizeof(T) * index ) + offset );
    }
  };
}