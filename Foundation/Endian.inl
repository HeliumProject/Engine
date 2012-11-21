uint64_t Helium::ConvertEndian(uint64_t val, bool endian)
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

int64_t Helium::ConvertEndian(int64_t val, bool endian)
{
    return ConvertEndian((uint64_t)val, endian);
}

float64_t Helium::ConvertEndian(float64_t val, bool endian)
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

uint64_t Helium::ConvertEndianDoubleToU64(float64_t val, bool endian)
{
    U64F64  types_union;
    types_union.m_f64 = val;  

    if (endian)
    {
        types_union.m_u64 = ConvertEndian(types_union.m_u64, endian);
    }

    return types_union.m_u64;
}

uint32_t Helium::ConvertEndian(uint32_t val, bool endian)
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

int32_t Helium::ConvertEndian(int32_t val, bool endian)
{
    return ConvertEndian((uint32_t)val, endian);
}

float32_t Helium::ConvertEndian(float32_t val, bool endian)
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

uint32_t Helium::ConvertEndianFloatToU32(float32_t val, bool endian)
{
    U32F32 types_union;
    types_union.m_f32 = val;

    if (endian)
    {
        types_union.m_u32 = ConvertEndian(types_union.m_u32, endian);
    }

    return types_union.m_u32;
}

uint16_t Helium::ConvertEndian(uint16_t val, bool endian)
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

int16_t Helium::ConvertEndian(int16_t val, bool endian)
{
    return ConvertEndian((uint16_t)val,endian);
}

uint8_t Helium::ConvertEndian(uint8_t val, bool endian)
{
    return val;
}

int8_t Helium::ConvertEndian(int8_t val, bool endian)
{
    return val;
}

template<class T>
void Helium::Swizzle(T& val, bool swizzle)
{
    if ( sizeof( T ) > 1 && swizzle )
    {
        ReverseByteOrder( &val, &val, sizeof( T ) );
    }
}

template<>
inline void Helium::Swizzle<uint16_t>(uint16_t& val, bool swizzle)
{
    val = ConvertEndian(val, swizzle);
}

template<>
inline void Helium::Swizzle<int16_t>(int16_t& val, bool swizzle)
{
    val = ConvertEndian(val, swizzle);
}

template<>
inline void Helium::Swizzle<uint32_t>(uint32_t& val, bool swizzle)
{
    val = ConvertEndian(val, swizzle);
}

template<>
inline void Helium::Swizzle<int32_t>(int32_t& val, bool swizzle)
{
    val = ConvertEndian(val, swizzle);
}

template<>
inline void Helium::Swizzle<uint64_t>(uint64_t& val, bool swizzle)
{
    val = ConvertEndian(val, swizzle);
}
template<>
inline void Helium::Swizzle<int64_t>(int64_t& val, bool swizzle)
{
    val = ConvertEndian(val, swizzle);
}

template<>
inline void Helium::Swizzle<float32_t>(float32_t& val, bool swizzle)
{
    val = ConvertEndian(val, swizzle);
}

template<>
inline void Helium::Swizzle<float64_t>(float64_t& val, bool swizzle)
{
    val = ConvertEndian(val, swizzle);
}