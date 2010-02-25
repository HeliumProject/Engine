#pragma once

#include "Common/Memory/Endian.h"

namespace RPC
{
  typedef void (*SwizzleFunc)(void* data);

  template <class T>
  inline void Swizzle(T* data)
  {
#ifdef WIN32
    NOC_BREAK();
#endif
  }

  template<>
  inline void Swizzle<u8>(u8* data)
  {
#ifdef WIN32
    Nocturnal::Swizzle(*data, true);
#endif
  }
  template<> inline void Swizzle(i8* data)
  {
#ifdef WIN32
    Nocturnal::Swizzle(*data, true);
#endif
  }

  template<> inline void Swizzle(u16* data)
  {
#ifdef WIN32
    Nocturnal::Swizzle(*data, true);
#endif
  }
  template<> inline void Swizzle(i16* data)
  {
#ifdef WIN32
    Nocturnal::Swizzle(*data, true);
#endif
  }

  template<> inline void Swizzle(u32* data)
  {
#ifdef WIN32
    Nocturnal::Swizzle(*data, true);
#endif
  }
  template<> inline void Swizzle(i32* data)
  {
#ifdef WIN32
    Nocturnal::Swizzle(*data, true);
#endif
  }

  template<> inline void Swizzle(u64* data)
  {
#ifdef WIN32
    Nocturnal::Swizzle(*data, true);
#endif
  }
  template<> inline void Swizzle(i64* data)
  {
#ifdef WIN32
    Nocturnal::Swizzle(*data, true);
#endif
  }

  template<> inline void Swizzle(f32* data)
  {
#ifdef WIN32
    Nocturnal::Swizzle(*data, true);
#endif
  }
  template<> inline void Swizzle(f64* data)
  {
#ifdef WIN32
    Nocturnal::Swizzle(*data, true);
#endif
  }

  template <class T>
  inline void Swizzle(T& data)
  {
#ifdef WIN32
    Swizzle(&data);
#endif
  }

  template <class T>
  SwizzleFunc GetSwizzleFunc()
  {
    void (*func)(T*) = &Swizzle<T>;
    return (SwizzleFunc)(void*)func;
  }
}