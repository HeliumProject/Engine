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

    inline uint64_t ConvertEndian(uint64_t val, bool endian = true);
    inline int64_t ConvertEndian(int64_t val, bool endian = true);
    inline float64_t ConvertEndian(float64_t val, bool endian = true);
    inline uint64_t ConvertEndianDoubleToU64(float64_t val, bool endian = true);

    inline uint32_t ConvertEndian(uint32_t val, bool endian = true);
    inline int32_t ConvertEndian(int32_t val, bool endian = true);
    inline float32_t ConvertEndian(float32_t val, bool endian = true);
    inline uint32_t ConvertEndianFloatToU32(float32_t val, bool endian = true);
    
	inline uint16_t ConvertEndian(uint16_t val, bool endian = true);
    inline int16_t ConvertEndian(int16_t val, bool endian = true);
    
	inline uint8_t ConvertEndian(uint8_t val, bool endian = true);
	inline int8_t ConvertEndian(int8_t val, bool endian = true);

    template<class T>
    inline void Swizzle(T& val, bool swizzle = true);
}

#include "Foundation/Endian.inl"