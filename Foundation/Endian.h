#pragma once

#include "Platform/Types.h"
#include "Platform/Assert.h"
#include "Platform/Utility.h"

namespace Helium
{
    namespace ByteOrders
    {
        enum ByteOrder
        {
            LittleEndian = 0,   // little endian
            BigEndian,          // big endian
            Unknown             // used to indicate that we do not yet know a byte order, not counted towards the 'Count'
        };
    }
    typedef ByteOrders::ByteOrder ByteOrder;

#if HELIUM_ENDIAN_LITTLE
    const static ByteOrder PlatformByteOrder = ByteOrders::LittleEndian;
#else
    const static ByteOrder PlatformByteOrder = ByteOrders::BigEndian;
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////
    union U64F64
    {
        float64_t m_f64;
        uint64_t m_u64;
    };

    union U32F32
    {
        float32_t m_f32;
        uint32_t m_u32;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    inline uint64_t ConvertEndian(uint64_t val, bool endian = true)
    {
        if (endian)
        {
#if defined( HELIUM_CC_CL ) && defined ( HELIUM_CPU_X86_32 )
            _asm mov eax,DWORD PTR [val];
            _asm mov ebx,DWORD PTR [val+4];
            _asm bswap eax;
            _asm bswap ebx;
            _asm mov DWORD PTR [val+4],eax;
            _asm mov DWORD PTR [val],ebx;
#else
            ReverseByteOrder( &val, &val, sizeof( val ) );
#endif
        }

        return val;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    inline int64_t ConvertEndian(int64_t val, bool endian = true)
    {
        return ConvertEndian((uint64_t)val, endian);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    inline float64_t ConvertEndian(float64_t val, bool endian = true)
    {
        if (endian)
        {
            U64F64 types_union;
            types_union.m_f64 = val;
            types_union.m_u64 = ConvertEndian(types_union.m_u64, endian);
            return types_union.m_f64;
        }

        return val;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    inline uint64_t ConvertEndianDoubleToU64(float64_t val, bool endian = true)
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
    inline uint32_t ConvertEndian(uint32_t val, bool endian = true)
    {
        if (endian)
        {
#if defined( HELIUM_CC_CL ) && defined ( HELIUM_CPU_X86_32 )
            _asm mov eax,val;
            _asm bswap eax;
            _asm mov val,eax;
#else
            ReverseByteOrder( &val, &val, sizeof( val ) );
#endif
        }

        return val;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    inline int32_t ConvertEndian(int32_t val, bool endian = true)
    {
        return ConvertEndian((uint32_t)val, endian);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    inline float32_t ConvertEndian(float32_t val, bool endian = true)
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
    inline uint32_t ConvertEndianFloatToU32(float32_t val, bool endian = true)
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
    inline uint16_t ConvertEndian(uint16_t val, bool endian = true)
    {
        if (endian)
        {
#if defined( HELIUM_CC_CL ) && defined ( HELIUM_CPU_X86_32 )
            _asm mov ax,val;
            _asm xchg al,ah;
            _asm mov val,ax;
#else
            ReverseByteOrder( &val, &val, sizeof( val ) );
#endif
        }

        return val;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    inline int16_t ConvertEndian(int16_t val, bool endian = true)
    {
        return ConvertEndian((uint16_t)val,endian);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    inline uint8_t ConvertEndian(uint8_t val, bool endian = true)
    {
        return val;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    inline int8_t ConvertEndian(int8_t val, bool endian = true)
    {
        return val;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////

    template<class T>
    inline void Swizzle(T& val, bool swizzle = true)
    {
        if ( sizeof( T ) > 1 && swizzle )
        {
            ReverseByteOrder( &val, &val, sizeof( T ) );
        }
    }

    template<> inline void Swizzle<uint16_t>(uint16_t& val, bool swizzle)
    {
        val = ConvertEndian(val, swizzle);
    }
    template<> inline void Swizzle<int16_t>(int16_t& val, bool swizzle)
    {
        val = ConvertEndian(val, swizzle);
    }

    template<> inline void Swizzle<uint32_t>(uint32_t& val, bool swizzle)
    {
        val = ConvertEndian(val, swizzle);
    }
    template<> inline void Swizzle<int32_t>(int32_t& val, bool swizzle)
    {
        val = ConvertEndian(val, swizzle);
    }

    template<> inline void Swizzle<uint64_t>(uint64_t& val, bool swizzle)
    {
        val = ConvertEndian(val, swizzle);
    }
    template<> inline void Swizzle<int64_t>(int64_t& val, bool swizzle)
    {
        val = ConvertEndian(val, swizzle);
    }

    template<> inline void Swizzle<float32_t>(float32_t& val, bool swizzle)
    {
        val = ConvertEndian(val, swizzle);
    }
    template<> inline void Swizzle<float64_t>(float64_t& val, bool swizzle)
    {
        val = ConvertEndian(val, swizzle);
    }
}
