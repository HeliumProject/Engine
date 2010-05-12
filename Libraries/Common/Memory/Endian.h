#pragma once

#include "../Types.h"
#include "../Assert.h"

////////////////////////////////////////////////////////////////////////////////////////////////
union U64F64
{
  f64 m_f64;
  u64 m_u64;
};

union U32F32
{
  f32 m_f32;
  u32 m_u32;
};

////////////////////////////////////////////////////////////////////////////////////////////////
inline u64 ConvertEndian(u64 val, bool endian = true)
{
  if (endian)
  {
#ifdef WIN32
    _asm mov eax,DWORD PTR [val]
    _asm mov ebx,DWORD PTR [val+4]
    _asm bswap eax
    _asm bswap ebx
    _asm mov DWORD PTR [val+4],eax
    _asm mov DWORD PTR [val],ebx
#else
    NOC_BREAK();
#endif
  }
  return val;
}

////////////////////////////////////////////////////////////////////////////////////////////////
inline i64 ConvertEndian(i64 val, bool endian = true)
{
  return ConvertEndian((u64)val, endian);
}

////////////////////////////////////////////////////////////////////////////////////////////////
inline f64 ConvertEndian(f64 val, bool endian = true)
{
  if (endian)
  {
    U64F64  types_union;
    types_union.m_f64 = val;
    types_union.m_u64 = ConvertEndian(types_union.m_u64, endian);
    return types_union.m_f64;
  }
  return val;
}

////////////////////////////////////////////////////////////////////////////////////////////////
inline u64 ConvertEndianDoubleToU64(f64 val, bool endian = true)
{
  U64F64  types_union;
  types_union.m_f64 = val;  
  
  if (endian)
  {
    types_union.m_u64 = ConvertEndian(types_union.m_u64, endian);
  }

  return types_union.m_u64;
}

////////////////////////////////////////////////////////////////////////////////////////////////
inline u32 ConvertEndian(u32 val, bool endian = true)
{
  if (endian)
  {
#ifdef WIN32
    _asm mov eax,val
    _asm bswap eax
    _asm mov val,eax
#else
    NOC_BREAK();
#endif
  }
  return val;
}

////////////////////////////////////////////////////////////////////////////////////////////////
inline i32 ConvertEndian(i32 val, bool endian = true)
{
  return ConvertEndian((u32)val, endian);
}

////////////////////////////////////////////////////////////////////////////////////////////////
inline f32 ConvertEndian(f32 val, bool endian = true)
{
  if (endian)
  {
    U32F32 types_union;
    types_union.m_f32 = val;
    types_union.m_u32 = ConvertEndian(types_union.m_u32, endian);
    return types_union.m_f32;
  }
  return val;
}

////////////////////////////////////////////////////////////////////////////////////////////////
inline u32 ConvertEndianFloatToU32(f32 val, bool endian = true)
{
  U32F32 types_union;
  types_union.m_f32 = val;

  if (endian)
  {
    types_union.m_u32 = ConvertEndian(types_union.m_u32, endian);
  }
  return types_union.m_u32;
}

////////////////////////////////////////////////////////////////////////////////////////////////
inline u16 ConvertEndian(u16 val, bool endian = true)
{
  if (endian)
  {
#ifdef WIN32
    _asm mov ax,val
    _asm xchg al,ah
    _asm mov val,ax
#else
    NOC_BREAK();
#endif
  }
  return val;
}

////////////////////////////////////////////////////////////////////////////////////////////////
inline i16 ConvertEndian(i16 val, bool endian = true)
{
	return ConvertEndian((u16)val,endian);
}

////////////////////////////////////////////////////////////////////////////////////////////////
inline u8 ConvertEndian(u8 val, bool endian = true)
{
  return val;
}

////////////////////////////////////////////////////////////////////////////////////////////////
inline i8 ConvertEndian(i8 val, bool endian = true)
{
  return val;
}

////////////////////////////////////////////////////////////////////////////////////////////////

namespace Nocturnal
{
  template<class T>
  inline void Swizzle(T& val, bool swizzle = true)
  {
    NOC_BREAK();
  }

  template<> inline void Swizzle<bool>(bool& val, bool swizzle)
  {

  }

  template<> inline void Swizzle<u8>(u8& val, bool swizzle)
  {

  }
  template<> inline void Swizzle<i8>(i8& val, bool swizzle)
  {

  }

  template<> inline void Swizzle<u16>(u16& val, bool swizzle)
  {
    val = ConvertEndian(val, swizzle);
  }
  template<> inline void Swizzle<i16>(i16& val, bool swizzle)
  {
    val = ConvertEndian(val, swizzle);
  }

  template<> inline void Swizzle<u32>(u32& val, bool swizzle)
  {
    val = ConvertEndian(val, swizzle);
  }
  template<> inline void Swizzle<i32>(i32& val, bool swizzle)
  {
    val = ConvertEndian(val, swizzle);
  }

  template<> inline void Swizzle<u64>(u64& val, bool swizzle)
  {
    val = ConvertEndian(val, swizzle);
  }
  template<> inline void Swizzle<i64>(i64& val, bool swizzle)
  {
    val = ConvertEndian(val, swizzle);
  }

  template<> inline void Swizzle<f32>(f32& val, bool swizzle)
  {
    val = ConvertEndian(val, swizzle);
  }
  template<> inline void Swizzle<f64>(f64& val, bool swizzle)
  {
    val = ConvertEndian(val, swizzle);
  }
}
