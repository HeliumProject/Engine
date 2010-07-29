#pragma once

#include "Allocator.h"

namespace Helium
{
  //
  // A resizable array
  //
  
  template <class T>
  class Array
  {
  public:
    Array(Allocator* alloc = NULL)
    : m_Alloc (alloc)
    , m_Data (NULL)
    , m_Capacity (0)
    , m_Size (0)
    {
      
    }
    
    Array(size_t size, Allocator* alloc = NULL)
    : m_Alloc (alloc)
    , m_Data (NULL)
    , m_Capacity (size)
    , m_Size (size)
    {
      if (m_Size > 0)
      {
        // alloc memory
        m_Data = AllocateObjects<T>(m_Size, m_Alloc);
        
        // run our constructors
        ConstructObjects<T>(m_Data, m_Size);
      }
    }
    
    Array(const Array<T>& rhs)
    : m_Alloc (rhs.m_Alloc)
    , m_Data (NULL)
    , m_Capacity (rhs.m_Size)
    , m_Size (rhs.m_Size)
    {
      if (m_Size > 0)
      {
        // alloc memory
        m_Data = AllocateObjects<T>(m_Size, m_Alloc);

        // copy
        const T* itr = rhs.m_Data;
        const T* end = rhs.m_Data + rhs.m_Size;
        for ( size_t index=0; itr != end; ++itr, ++index )
        {
          // construct this element
          ConstructObject(&m_Data[index]);

          // copy data from rhs
          m_Data[index] = *itr;
        }
      }
    }
    
    ~Array()
    {
      if (m_Data)
      {
        // call destructors
        DestructObjects<T>(m_Data, m_Size);
        
        // free memory
        FreeObjects<T>(m_Data);
      }
    }
    
    Array<T>& operator=(const Array<T>& rhs)
    {
      // realloc
      Resize(rhs.GetSize());

      if (m_Size > 0)
      {
        // copy
        const T* itr = rhs.m_Data;
        const T* end = rhs.m_Data + rhs.m_Size;
        for ( size_t index=0; itr != end; ++itr, ++index )
        {
          // copy data from rhs
          m_Data[index] = *itr;
        }
      }
      
      return *this;
    }
    
    bool operator==(const Array<T>& rhs) const
    {
      // early out of counts don't match
      if (m_Size != rhs.m_Size)
      {
        return false;
      }
      
      // walk lhs
      const T* itrL = m_Data;
      const T* endL = m_Data + m_Size;
      
      // walk rhs
      const T* itrR = rhs.m_Data;
      const T* endR = rhs.m_Data + rhs.m_Size;
      
      // iterate
      for ( ; itrL != endL && itrR != endR; ++itrL, ++itrR )
      {
        // if any element does not match
        if (*itrL != *itrR)
        {
          // fail
          return false;
        }
      }
      
      return true;
    }
    
    bool operator!=(const Array<T>& rhs) const
    {
      return !operator==(rhs);
    }
    
    void Clear()
    {
      Resize( 0 );
    }
    
    T* GetData()
    {
      return m_Data;
    }
    
    const T* GetData() const
    {
      return m_Data;
    }
    
    size_t GetCapacity() const
    {
      return m_Capacity;
    }
    
    size_t GetSize() const
    {
      return m_Size;
    }
    
    bool IsEmpty() const
    {
      return GetSize() == 0;
    }
    
    void Reserve(size_t new_size)
    {
      if (new_size > m_Capacity)
      {
        // reallocate new storage
        m_Data = ReallocateObjects<T>(m_Data, m_Size, new_size);
        
        // update the capacity
        m_Capacity = new_size;
      }
    }

    void Resize(size_t new_size)
    {
      if (new_size != m_Size)
      {
        if (m_Size)
        {
          if (new_size)
          {
            if ( new_size > m_Capacity )
            {
              // we need to grow the capacity to the specified size
              m_Data = ReallocateObjects<T>(m_Data, m_Capacity, new_size, m_Alloc);
              m_Capacity = new_size;

              // run constructors for our new objects
              ConstructObjects<T>(m_Data + m_Size, new_size - m_Size);
            }
            else if ( new_size < m_Size )
            {
              // reset memory of scrapped objects
              memset(m_Data + new_size, 0, (m_Size - new_size) * sizeof( T ) );

              // run constructors for our scrapped objects
              ConstructObjects<T>(m_Data + new_size, m_Size - new_size);              
            }
          }
          else
          {
            // run our destructors
            DestructObjects<T>(m_Data, m_Size);
            
            // nonzero to zero, free our allocation
            FreeObjects<T>(m_Data);
            m_Capacity = 0;
            m_Data = NULL;
          }        
        }
        else
        {
          if (new_size > 0)
          {
            // zero to nonzero, alloc a new region
            m_Data = AllocateObjects<T>(new_size, m_Alloc);
            m_Capacity = new_size;
            
            // run our constructors
            ConstructObjects<T>(m_Data, new_size);
          }
        }
        
        m_Size = new_size;
      }
    }
    
    void Grow()
    {
      Reserve( ( m_Size ? m_Size : sizeof(T) ) << 1 );
    }

    void Append(const T& value)
    {
      if ( m_Size == m_Capacity )
      {
        Grow();
      }
      
      m_Data[ m_Size ] = value;
      m_Size++;
    }
    
  private:
    Allocator*  m_Alloc;
    T*          m_Data;
    size_t      m_Capacity;
    size_t      m_Size;
  };
}
