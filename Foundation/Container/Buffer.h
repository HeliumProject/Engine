#pragma once

#include "Array.h"

namespace Nocturnal
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
      Nocturnal::ConstructObject<T>( data );
      
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
}
