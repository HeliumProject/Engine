#pragma once

#include "Fixup.h"
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
    u32 m_array_size;

  public:
    ObjectArrayBuffer(u32 size) 
      : SmartBuffer()
      , m_array_size( size )
    {
      if ( size > 0 )
      {
        GrowBy( sizeof( T ) * size );
        m_Size = sizeof( T ) * size;
      }
    }

    inline u32 Count() const
    {
      return m_array_size;
    }

    inline void Reserve(u32 size)
    {
      if ( m_array_size < size )
      {
        u32 diff = size - m_array_size;
        GrowBy( sizeof( T ) * diff );
        m_Size += sizeof( T ) * diff;
        m_array_size = size;
      }
    }

    inline T& operator[] (u32 index)
    {
      return At(index);
    }

    inline T& At(u32 index)
    {
      Reserve( index + 1 );
      return *((T*)(m_Data + ( sizeof( T ) * index ) ));
    }

    inline Location GetIndexOffsetLocation( u32 index, u32 offset )
    {
      return GetOffsetLocation( ( sizeof(T) * index ) + offset );
    }
  };
}