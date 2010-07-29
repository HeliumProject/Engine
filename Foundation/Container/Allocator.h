#pragma once

#include <stdlib.h>
#include <string.h>
#include <new>

namespace Helium
{
  //
  // Basic allocator interface: alloc, realloc, and free
  //
  
  class Allocator
  {
  public:
    virtual void* Allocate(size_t) = 0;

    virtual void* Reallocate(void* addr, size_t old_size, size_t new_size)
    {
      void* result = Allocate( new_size );
      memcpy( result, addr, old_size );
      Free( addr );
      return result;
    }

    virtual void Free(void*) = 0;
  };

  //
  // Default allocator interfaces with the CRT heap
  //
  
  class DefaultAllocator : public Allocator
  {
  public:
    virtual void* Allocate(size_t size)
    {
      return ::malloc( size );
    }
    
    virtual void* Reallocate(void* addr, size_t old_size, size_t new_size)
    {
      return ::realloc( addr, new_size );
    }
    
    virtual void Free(void* addr)
    {
      ::free( addr );
    }
  };
  
  //
  // Makes handling null allocators easier (defer to default allocator)
  //

  inline void* Allocate(size_t size, Allocator* alloc = NULL)
  {
    return alloc ? alloc->Allocate( size ) : DefaultAllocator ().Allocate( size ); 
  }

  inline void* Reallocate(void* addr, size_t old_size, size_t new_size, Allocator* alloc = NULL)
  {
    return alloc ? alloc->Reallocate( addr, old_size, new_size ) : DefaultAllocator ().Reallocate( addr, old_size, new_size ); 
  }

  inline void Free(void* addr, Allocator* alloc = NULL)
  {
    return alloc ? alloc->Free( addr ) : DefaultAllocator ().Free( addr ); 
  }

  //
  // Template versions
  //
  
  template <class T>
  inline T* AllocateObjects(size_t count, Allocator* alloc = NULL)
  {
    return reinterpret_cast<T*>( Allocate( sizeof(T) * count, alloc ) );
  }

  template <class T>
  inline T* ReallocateObjects(T* ptr, size_t old_count, size_t new_count, Allocator* alloc = NULL)
  {
    return reinterpret_cast<T*>( Reallocate( (void*)ptr, sizeof(T) * old_count, sizeof(T) * new_count, alloc ) );
  }

  template <class T>
  inline void FreeObjects(T* ptr, Allocator* alloc = NULL)
  {
    Free( (void*)ptr, alloc );
  }

  //
  // Construct/destruct helpers
  //
  
  template <class T>
  inline void ConstructObject(T* ptr)
  {
    // placement new will handle builtin types vs. user types
    new ((void*)ptr) T;
  }

  template <class T>
  inline void DestructObject(T* ptr)
  {
    // note that "T" here HAS to be a template parameter, otherwise builtin types will generate compile error
    // apparently only the template compiler knows how to differentiate between builtin and user types (without
    // entering builtin type specialization hell)
    (ptr)->~T();
  }

  //
  // Array construct/destruct helpers
  //
  
  template <class T>
  inline void ConstructObjects(T* ptr, size_t count)
  {
    for ( T* end = ptr + count; ptr != end; ++ptr )
    {
      ConstructObject(ptr);
    }
  }
  
  template <class T>
  inline void DestructObjects(T* ptr, size_t count)
  {
    for ( T* end = ptr + count; ptr != end; ++ptr )
    {
      DestructObject(ptr);
    }
  }
}