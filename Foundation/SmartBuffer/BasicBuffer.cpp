#include "FoundationPch.h"
#include "BasicBuffer.h"

#include "Platform/Exception.h"

#include "Math/Float16.h"
#include "Math/FpuVector3.h"
#include "Math/FpuVector4.h"

#include "Foundation/Log.h"

using namespace Helium;

//
// To turn on debug tracking/logging capability, set this to "1".
//  It may run slightly slower, and will use a ton of memory.
//

#define BASIC_BUFFER_DEBUG_INFO 0

#if BASIC_BUFFER_DEBUG_INFO

#define ADD_DEBUG_INFO(blockType, blockSize)          \
{                                                     \
    va_list args;                                       \
    va_start(args, dbgStr);                             \
    AddDebugInfo(blockType, blockSize, dbgStr, args, 0);\
}

#define ADD_DEBUG_INFO_SKIP(blockType, blockSize)     \
{                                                     \
    va_list args;                                       \
    va_start(args, dbgStr);                             \
    AddDebugInfo(blockType, blockSize, dbgStr, args, 1);\
}

static tchar_t g_DebugString[2048];

#else

#define ADD_DEBUG_INFO(blockType, blockSize)
#define ADD_DEBUG_INFO_SKIP(blockType, blockSize)

#endif

bool BasicBuffer::IsDebugInfoEnabled()
{
#if BASIC_BUFFER_DEBUG_INFO
    return true;
#else
    return false;
#endif
}

void BasicBuffer::AddDebugInfo(BasicBufferDebugInfo::BlockType blockType, unsigned int blockSize, const tchar_t *dbgStr, va_list argptr, tchar_t ignore_next)
{
#if BASIC_BUFFER_DEBUG_INFO
    BasicBufferDebugInfo debug_info;
    tchar_t* dbg_str;

    static tchar_t s_ignore_next = 0;

    if ( s_ignore_next )
    {
        s_ignore_next += ignore_next - 1;  // - 1 because we are ignoring current...

        return;
    }
    else
    {
        s_ignore_next += ignore_next;
    }

    if ( dbgStr )
    {
        static tchar_t prtbuf[2048];
        vsprintf(prtbuf, dbgStr, argptr);
        dbg_str = prtbuf;
    }
    else
    {
        dbg_str = "(NONE)";
    }

    debug_info.m_BlockType = blockType;
    debug_info.m_DebugString = dbg_str;
    debug_info.m_BlockSize = blockSize;
    debug_info.m_FileSize = GetSize();

    m_DebugInfo.push_back(debug_info);
#endif
}

void BasicBuffer::DumpDebugInfo(FILE* file)
{
    if ( file )
    {
        fprintf(file, "ByteOrder   : %s\n", m_ByteOrder==ByteOrders::LittleEndian ? "LittleEndian" : "BigEndian" );

        for (BasicBufferDebugInfoVector::iterator i=m_DebugInfo.begin(); i!=m_DebugInfo.end(); i++)
        {
            tchar_t* blockTypeStr = NULL;

            switch(i->m_BlockType)
            {
            case BasicBufferDebugInfo::BLOCK_TYPE_BUFFER:  blockTypeStr = TXT( "Buffer" ); break;
            case BasicBufferDebugInfo::BLOCK_TYPE_I8:      blockTypeStr = TXT( "int8_t" ); break;
            case BasicBufferDebugInfo::BLOCK_TYPE_U8:      blockTypeStr = TXT( "uint8_t" ); break;
            case BasicBufferDebugInfo::BLOCK_TYPE_I16:     blockTypeStr = TXT( "int16_t" ); break;
            case BasicBufferDebugInfo::BLOCK_TYPE_U16:     blockTypeStr = TXT( "uint16_t" ); break;
            case BasicBufferDebugInfo::BLOCK_TYPE_I32:     blockTypeStr = TXT( "int32_t" ); break;
            case BasicBufferDebugInfo::BLOCK_TYPE_U32:     blockTypeStr = TXT( "uint32_t" ); break;
            case BasicBufferDebugInfo::BLOCK_TYPE_I64:     blockTypeStr = TXT( "int64_t" ); break;
            case BasicBufferDebugInfo::BLOCK_TYPE_U64:     blockTypeStr = TXT( "uint64_t" ); break;
            case BasicBufferDebugInfo::BLOCK_TYPE_F32:     blockTypeStr = TXT( "float32_t" ); break;
            case BasicBufferDebugInfo::BLOCK_TYPE_F64:     blockTypeStr = TXT( "float64_t" ); break;
            case BasicBufferDebugInfo::BLOCK_TYPE_RESERVE: blockTypeStr = TXT( "Reserve" ); break;
            case BasicBufferDebugInfo::BLOCK_TYPE_POINTER: blockTypeStr = TXT( "Pointer" ); break;
            case BasicBufferDebugInfo::BLOCK_TYPE_OFFSET:  blockTypeStr = TXT( "Offset" ); break;
            default:                                       blockTypeStr = TXT( "Unknown" ); break;
            }

            fprintf(file, "Block Label   : %s\n", i->m_DebugString.c_str());
            fprintf(file, "Block Type    : %s, Size: %d\n", blockTypeStr, i->m_BlockSize);
            fprintf(file, "Block BufferLocation: 0x%08X\n", i->m_FileSize);
            fprintf(file, "Contents      : ");

            uint8_t* data = m_Data + i->m_FileSize;

            switch(i->m_BlockType)
            {
            case BasicBufferDebugInfo::BLOCK_TYPE_I8:      fprintf(file, "%02X\n",*((int8_t*)data)); break;
            case BasicBufferDebugInfo::BLOCK_TYPE_U8:      fprintf(file, "%02X\n",*((uint8_t*)data)); break;
            case BasicBufferDebugInfo::BLOCK_TYPE_I16:     fprintf(file, "%04X\n",ConvertEndian( *((int16_t*)data),IsPlatformBigEndian())); break;
            case BasicBufferDebugInfo::BLOCK_TYPE_U16:     fprintf(file, "%04X\n",ConvertEndian( *((uint16_t*)data),IsPlatformBigEndian())); break;
            case BasicBufferDebugInfo::BLOCK_TYPE_I32:     fprintf(file, "%08X\n",ConvertEndian( *((int32_t*)data),IsPlatformBigEndian())); break;
            case BasicBufferDebugInfo::BLOCK_TYPE_U32:     fprintf(file, "%08X\n",ConvertEndian( *((uint32_t*)data),IsPlatformBigEndian())); break;
            case BasicBufferDebugInfo::BLOCK_TYPE_I64:     fprintf(file, "0x%I64x\n",ConvertEndian( *((uint64_t*)data),IsPlatformBigEndian())); break;
            case BasicBufferDebugInfo::BLOCK_TYPE_U64:     fprintf(file, "0x%I64x\n",ConvertEndian( *((uint64_t*)data),IsPlatformBigEndian())); break;
            case BasicBufferDebugInfo::BLOCK_TYPE_F32:     fprintf(file, "%.3f\n",ConvertEndian( *((float32_t*)data),IsPlatformBigEndian())); break;
            case BasicBufferDebugInfo::BLOCK_TYPE_F64:     fprintf(file, "%.3f\n",ConvertEndian( *((float64_t*)data),IsPlatformBigEndian())); break;
            case BasicBufferDebugInfo::BLOCK_TYPE_OFFSET:  fprintf(file, "0x%08x\n", *((uint32_t*)data)); break;
            case BasicBufferDebugInfo::BLOCK_TYPE_POINTER: if (GetPlatformPtrSize() == 8) fprintf(file, "0x%I64x\n", *((uint64_t*)data)); else fprintf(file, "0x%08x\n", *((uint32_t*)data)); break;
            case BasicBufferDebugInfo::BLOCK_TYPE_RESERVE:
                {
                    bool handled = false;

                    switch (i->m_BlockSize) 
                    {
                    case 1: handled = true; fprintf(file, "%d\n", *((uint8_t*)data)); break;
                    case 2: handled = true; fprintf(file, "%d\n", *((uint16_t*)data)); break;
                    case 4: handled = true; fprintf(file, "%d\n", *((uint32_t*)data)); break;
                    }

                    if ( handled )
                    {
                        break;
                    }
                }

            case BasicBufferDebugInfo::BLOCK_TYPE_BUFFER:
                {
                    unsigned int num_blocks = i->m_BlockSize;
                    unsigned int j;

                    // limit the length of this output..
                    if (num_blocks > 1024)
                        num_blocks = 1024;

                    // just output hex...
                    for (j=0; j<num_blocks; j++)
                    {
                        fprintf(file, "0x%02X ", data[j]);
                    }

                    fprintf(file, "\n");
                    fprintf(file, "              : ");

                    // output string...
                    for (j=0; j<num_blocks; j++)
                    {
                        if ( data[j] >= 32 && data[j] <= 126 )
                        {
                            fprintf(file, "%c", data[j]);
                        }
                        else
                        {
                            fprintf(file, "." );
                        }
                    }

                    fprintf(file, "\n");
                }
                break;

            default:
                {
                    fprintf(file, "PARADOX ERR\n");
                    break;
                }
            }

            fprintf(file, "\n");
        }
    }
}

uint32_t BasicBuffer::AddBuffer( const uint8_t* buffer, uint32_t size, const tchar_t* dbgStr, ... )
{
    ADD_DEBUG_INFO(BasicBufferDebugInfo::BLOCK_TYPE_BUFFER, size);

    if ( (size + m_Size) > m_Capacity )
    {
        GrowBy( size );
    }

    memcpy( m_Data + m_Size, buffer, size );
    m_Size += size;

    return ( m_Size - size );
}

uint32_t BasicBuffer::AddBuffer( const SmartBufferPtr& buffer, bool add_fixups )
{
    // platform types have to match
    HELIUM_ASSERT(buffer->GetByteOrder() == m_ByteOrder);

    // first bring in all the data from the incoming buffer
    uint32_t return_val = AddBuffer( buffer->GetData(), buffer->GetSize() );

    if (add_fixups)
    {
        // inherit all the incoming / outgoing fixups from this buffer with out new offset
        InheritFixups( buffer, return_val );
    }

    return return_val;
}

uint32_t BasicBuffer::AddFile( const tstring& filename )
{
    return AddFile( filename.c_str() );
}

uint32_t BasicBuffer::AddFile( const tchar_t* filename )
{
    FILE *pfile = _tfopen( filename, TXT( "rb" ) );
    if ( pfile == NULL )
    {
        throw Helium::Exception( TXT( "Could not open file '%s' to add to membuf '%s'." ), filename, m_Name.c_str() );
    }

    fseek(pfile,0,SEEK_END);
    long filesize = ftell(pfile);
    fseek(pfile,0,SEEK_SET);

    if(filesize == -1)
    {
        throw Helium::Exception( TXT( "Could not get file size for file '%s' to add to membuf '%s'." ), filename, m_Name.c_str() );
    }

    if ( (filesize + m_Size) > m_Capacity )
        GrowBy(filesize);

    size_t file_read = fread( m_Data + m_Size, 1, filesize, pfile );
    if(file_read != (size_t)filesize)
    {
        Log::Warning( TXT( "Could not read entire file '%s' to add to membuf '%s'.\n" ), filename, m_Name.c_str() );
    }

    fclose(pfile);

    m_Size += (uint32_t)file_read;
    return m_Size - (uint32_t)file_read;
}

uint32_t BasicBuffer::AddI8( int8_t val, const tchar_t* dbgStr, ... )
{
    ADD_DEBUG_INFO_SKIP(BasicBufferDebugInfo::BLOCK_TYPE_I8, 1);

    return AddBuffer( (uint8_t*)&val, sizeof( int8_t ) );
}

uint32_t BasicBuffer::AddU8( uint8_t val, const tchar_t* dbgStr, ... )
{
    ADD_DEBUG_INFO_SKIP(BasicBufferDebugInfo::BLOCK_TYPE_U8, 1);

    return AddBuffer( (uint8_t*)&val, sizeof( uint8_t ) );
}

uint32_t BasicBuffer::AddU16( uint16_t val, const tchar_t* dbgStr, ... )
{
    ADD_DEBUG_INFO_SKIP(BasicBufferDebugInfo::BLOCK_TYPE_U16, 2);

    val = ConvertEndian(val,IsPlatformBigEndian());
    return AddBuffer( (uint8_t*)&val, sizeof( uint16_t ) );
}

uint32_t BasicBuffer::AddI16( int16_t val, const tchar_t* dbgStr, ... )
{
    ADD_DEBUG_INFO_SKIP(BasicBufferDebugInfo::BLOCK_TYPE_I16, 2);

    val = ConvertEndian(val,IsPlatformBigEndian());
    return AddBuffer( (uint8_t*)&val, sizeof( int16_t ) );
}

uint32_t BasicBuffer::AddI32( int32_t val, const tchar_t* dbgStr, ... )
{
    ADD_DEBUG_INFO_SKIP(BasicBufferDebugInfo::BLOCK_TYPE_I32, 4);

    val = ConvertEndian(val,IsPlatformBigEndian());
    return AddBuffer( (uint8_t*)&val, sizeof( int32_t ) );
}

uint32_t BasicBuffer::AddU32( uint32_t val, const tchar_t* dbgStr, ... )
{
    ADD_DEBUG_INFO_SKIP(BasicBufferDebugInfo::BLOCK_TYPE_U32, 4);

    val = ConvertEndian(val,IsPlatformBigEndian());
    return AddBuffer( (uint8_t*)&val, sizeof( uint32_t ) );
}

uint32_t BasicBuffer::AddI64( int64_t val, const tchar_t* dbgStr, ... )
{
    ADD_DEBUG_INFO_SKIP(BasicBufferDebugInfo::BLOCK_TYPE_I64, 8);

    val = ConvertEndian(val,IsPlatformBigEndian());
    return AddBuffer( (uint8_t*)&val, sizeof( int64_t ) );
}

uint32_t BasicBuffer::AddU64( uint64_t val, const tchar_t* dbgStr, ... )
{
    ADD_DEBUG_INFO_SKIP(BasicBufferDebugInfo::BLOCK_TYPE_U64, 8);

    val = ConvertEndian(val,IsPlatformBigEndian());
    return AddBuffer( (uint8_t*)&val, sizeof( uint64_t ) );
}

uint32_t BasicBuffer::AddF16( float32_t val, const tchar_t* dbgStr, ... )
{
    uint16_t half = FloatToHalf( val );
    ADD_DEBUG_INFO_SKIP(BasicBufferDebugInfo::BLOCK_TYPE_I16, 2);

    half = ConvertEndian(half,IsPlatformBigEndian());
    return AddBuffer( (uint8_t*)&half, sizeof( uint16_t ) );
}

uint32_t BasicBuffer::AddF32( float32_t val, const tchar_t* dbgStr, ... )
{
    uint32_t i = *(reinterpret_cast<uint32_t *>(&val));

    // handle case for "negative" zero..  this was
    // causing diffs to vary between reflect/non-xml
    //
    // IEEE format for a float32_t is this:
    // Sign   Exponent  Fraction
    //    0   00000000  00000000000000000000000
    //Bit 31 [30 -- 23] [22 -- 0]
    if ( (i & 0x7FFFFFFF) == 0x0 )
    {
        i = 0x0;
    }

    ADD_DEBUG_INFO_SKIP(BasicBufferDebugInfo::BLOCK_TYPE_F32, 4);

    return AddU32( i );
}

uint32_t BasicBuffer::AddF64( float64_t val, const tchar_t* dbgStr, ... )
{
    uint64_t i = *(reinterpret_cast<uint64_t *>(&val));

    // handle case for "negative" zero..  this was
    // causing diffs to vary between reflect/non-xml
    //
    // IEEE format for a float32_t is this:
    // Sign   Exponent  Fraction
    //Bit 64 [63 -- 56] [55 -- 0]
    if ( (i & 0x7FFFFFFFFFFFFFFF) == 0x0 )
    {
        i = 0x0;
    }

    ADD_DEBUG_INFO_SKIP(BasicBufferDebugInfo::BLOCK_TYPE_F64, 8);

    return AddU64( i );
}

uint32_t BasicBuffer::AddVector3( const Vector3& v, const tchar_t* debugStr )
{
    uint32_t ret = AddF32(v.x, debugStr);
    AddF32(v.y, debugStr);
    AddF32(v.z, debugStr);
    return ret;
}

uint32_t BasicBuffer::AddVector4( const Vector4& v, const tchar_t* debugStr )
{
    uint32_t ret = AddF32(v.x, debugStr);
    AddF32(v.y, debugStr);
    AddF32(v.z, debugStr);
    AddF32(v.w, debugStr);
    return ret;
}

uint32_t BasicBuffer::AddVector4( const Vector3& v, float32_t w, const tchar_t* debugStr )
{
    uint32_t ret = AddF32(v.x, debugStr);
    AddF32(v.y, debugStr);
    AddF32(v.z, debugStr);
    AddF32(w, debugStr);
    return ret;
}

uint32_t BasicBuffer::AddVector4( float32_t x, float32_t y, float32_t z, float32_t w, const tchar_t* debugStr )
{
    uint32_t ret = AddF32(x, debugStr);
    AddF32(y, debugStr);
    AddF32(z, debugStr);
    AddF32(w, debugStr);
    return ret;
}

void BasicBuffer::AddAtLocI8( int8_t val, const BufferLocation& destination )
{
    Write( destination, (uint8_t*)&val, sizeof( int8_t ) );
}

void BasicBuffer::AddAtLocU8( uint8_t val, const BufferLocation& destination )
{
    Write( destination, (uint8_t*)&val, sizeof( uint8_t ) );
}

void BasicBuffer::AddAtLocI16( int16_t val, const BufferLocation& destination )
{
    val = ConvertEndian(val,IsPlatformBigEndian());
    Write( destination, (uint8_t*)&val, sizeof( int16_t ) );
}

void BasicBuffer::AddAtLocU16( uint16_t val, const BufferLocation& destination )
{
    val = ConvertEndian(val,IsPlatformBigEndian());
    Write( destination, (uint8_t*)&val, sizeof( uint16_t ) );
}

void BasicBuffer::AddAtLocI32( int32_t val, const BufferLocation& destination )
{
    val = ConvertEndian(val,IsPlatformBigEndian());
    Write( destination, (uint8_t*)&val, sizeof( int32_t ) );
}

void BasicBuffer::AddAtLocU32( uint32_t val, const BufferLocation& destination )
{
    val = ConvertEndian(val,IsPlatformBigEndian());
    Write( destination, (uint8_t*)&val, sizeof( uint32_t ) );
}

void BasicBuffer::AddAtLocI64( int64_t val, const BufferLocation& destination )
{
    val = ConvertEndian(val,IsPlatformBigEndian());
    Write( destination, (uint8_t*)&val, sizeof( int64_t ) );
}

void BasicBuffer::AddAtLocU64( uint64_t val, const BufferLocation& destination )
{
    val = ConvertEndian(val,IsPlatformBigEndian());
    Write( destination, (uint8_t*)&val, sizeof( uint64_t ) );
}

void BasicBuffer::AddAtLocF32( float32_t val, const BufferLocation& destination )
{
    uint32_t i = *(reinterpret_cast<uint32_t *>(&val));

    // handle case for "negative" zero..  this was
    // causing diffs to vary between reflect/non-xml
    //
    // IEEE format for a float32_t is this:
    // Sign   Exponent  Fraction
    //    0   00000000  00000000000000000000000
    //Bit 31 [30 -- 23] [22 -- 0]
    if ( (i & 0x7FFFFFFF) == 0x0 )
    {
        i = 0x0;
    }

    AddAtLocU32( i, destination );
}

void BasicBuffer::AddAtLocF64( float64_t val, const BufferLocation& destination )
{
    uint64_t i = *(reinterpret_cast<uint64_t *>(&val));

    // handle case for "negative" zero..  this was
    // causing diffs to vary between reflect/non-xml
    //
    // IEEE format for a float32_t is this:
    // Sign   Exponent  Fraction
    //Bit 64 [63 -- 56] [55 -- 0]
    if ( (i & 0x7FFFFFFFFFFFFFFF) == 0x0 )
    {
        i = 0x0;
    }

    AddAtLocU64( i, destination );
}

void BasicBuffer::AddPad( uint32_t pad_length )
{
    if ( (pad_length + m_Size) > m_Capacity )
    {
        GrowBy( pad_length );
    }

    for ( uint32_t i = 0; i < pad_length; ++i )
    {
        AddU8( 0 );
    }
}

void BasicBuffer::PadToArb( uint32_t align_size )
{
    int32_t pad_length = (int32_t)align_size - ( m_Size % align_size );
    if ( pad_length != align_size )
    {
        HELIUM_ASSERT( pad_length > 0 );
        AddPad( (uint32_t)pad_length );
    }
}

void BasicBuffer::SetCapacity(uint32_t capacity)
{
    if (capacity > m_Capacity)
    {
        GrowBy(capacity - m_Capacity);
    }  
}

BufferLocation BasicBuffer::Reserve(uint32_t size, const tchar_t* dbgStr, ...)
{
    ADD_DEBUG_INFO(BasicBufferDebugInfo::BLOCK_TYPE_RESERVE, size);

    BufferLocation return_val = GetCurrentLocation();

    if ( (size + m_Size) > m_Capacity )
    {
        GrowBy( size );
    }

    memset( m_Data + m_Size, 0, size );

    m_Size += size;

    return return_val;
}

void BasicBuffer::Reserve(BufferLocation& loc, uint32_t size, const tchar_t* dbgStr, ...)
{
#if BASIC_BUFFER_DEBUG_INFO
    {
        if (dbgStr)
        {
            va_list args;
            va_start(args, dbgStr);
            assert(_vscprintf(dbgStr, args)+1 <= sizeof(g_DebugString));
            vsprintf(g_DebugString, dbgStr, args);
            dbgStr = g_DebugString;
        }
    }
#endif //BASIC_BUFFER_DEBUG_INFO

    loc = Reserve(size, dbgStr);
}

BufferLocation BasicBuffer::ReservePointer(uint32_t size, const tchar_t* dbgStr, ... )
{
    if (size==0)
    {
        size = GetPlatformPtrSize();
    }

    // 8 byte pointers are aligned to 8 byte addressess
    if ( size == 8 )
    {
        PadToArb(8);
    }

    ADD_DEBUG_INFO_SKIP(BasicBufferDebugInfo::BLOCK_TYPE_POINTER, size);

    return Reserve( size );
}

void BasicBuffer::ReservePointer(BufferLocation& loc, uint32_t size, const tchar_t* dbgStr, ... )
{
#if BASIC_BUFFER_DEBUG_INFO
    {
        if (dbgStr)
        {
            va_list args;
            va_start(args, dbgStr);
            assert(_vscprintf(dbgStr, args)+1 <= sizeof(g_DebugString));
            vsprintf(g_DebugString, dbgStr, args);
            dbgStr = g_DebugString;
        }
    }
#endif //BASIC_BUFFER_DEBUG_INFO

    loc = ReservePointer(size, dbgStr);
}

BufferLocation BasicBuffer::ReserveOffset( const tchar_t* dbgStr, ... )
{
    ADD_DEBUG_INFO_SKIP(BasicBufferDebugInfo::BLOCK_TYPE_OFFSET, 4);

    return Reserve( 4 );
}

void BasicBuffer::ReserveOffset(BufferLocation& loc, const tchar_t* dbgStr, ... )
{
#if BASIC_BUFFER_DEBUG_INFO
    {
        if (dbgStr)
        {
            va_list args;
            va_start(args, dbgStr);
            assert(_vscprintf(dbgStr, args)+1 <= sizeof(g_DebugString));
            vsprintf(g_DebugString, dbgStr, args);
            dbgStr = g_DebugString;
        }
    }
#endif //BASIC_BUFFER_DEBUG_INFO

    loc = ReserveOffset(dbgStr);
}

void BasicBuffer::WritePointer( const BufferLocation& destination )
{
    AddPointerFixup( ReservePointer(0), destination,GetPlatformPtrSize() );
}

void BasicBuffer::WritePointer32( const BufferLocation& destination )
{
    AddPointerFixup( ReservePointer(4), destination,4 );
}

void BasicBuffer::WritePointer64( const BufferLocation& destination )
{
    AddPointerFixup( ReservePointer(8), destination,8 );
}

void BasicBuffer::WriteOffset( const BufferLocation& destination, bool absolute )
{
    AddOffsetFixup( ReserveOffset(), destination, absolute );
}