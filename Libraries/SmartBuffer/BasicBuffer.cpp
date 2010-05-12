#include "BasicBuffer.h"
#include "Fixup.h"

#include "Common/Exception.h"
#include "Console/Console.h"

#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Half.h"

using namespace Nocturnal;

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

static char g_DebugString[2048];

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

void BasicBuffer::AddDebugInfo(BasicBufferDebugInfo::BlockType blockType, unsigned int blockSize, const char *dbgStr, va_list argptr, char ignore_next)
{
#if BASIC_BUFFER_DEBUG_INFO
  BasicBufferDebugInfo debug_info;
  char* dbg_str;

  static char s_ignore_next = 0;

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
    static char prtbuf[2048];
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
    fprintf(file, "BufferPlatform   : %s\n", m_Platform==BufferPlatforms::x86?"PC":"PS3");

    for (BasicBufferDebugInfoVector::iterator i=m_DebugInfo.begin(); i!=m_DebugInfo.end(); i++)
    {
      char* blockTypeStr = NULL;

      switch(i->m_BlockType)
      {
      case BasicBufferDebugInfo::BLOCK_TYPE_BUFFER:  blockTypeStr = "Buffer"; break;
      case BasicBufferDebugInfo::BLOCK_TYPE_I8:      blockTypeStr = "i8"; break;
      case BasicBufferDebugInfo::BLOCK_TYPE_U8:      blockTypeStr = "u8"; break;
      case BasicBufferDebugInfo::BLOCK_TYPE_I16:     blockTypeStr = "i16"; break;
      case BasicBufferDebugInfo::BLOCK_TYPE_U16:     blockTypeStr = "u16"; break;
      case BasicBufferDebugInfo::BLOCK_TYPE_I32:     blockTypeStr = "i32"; break;
      case BasicBufferDebugInfo::BLOCK_TYPE_U32:     blockTypeStr = "u32"; break;
      case BasicBufferDebugInfo::BLOCK_TYPE_I64:     blockTypeStr = "i64"; break;
      case BasicBufferDebugInfo::BLOCK_TYPE_U64:     blockTypeStr = "u64"; break;
      case BasicBufferDebugInfo::BLOCK_TYPE_F32:     blockTypeStr = "f32"; break;
      case BasicBufferDebugInfo::BLOCK_TYPE_F64:     blockTypeStr = "f64"; break;
      case BasicBufferDebugInfo::BLOCK_TYPE_RESERVE: blockTypeStr = "Reserve"; break;
      case BasicBufferDebugInfo::BLOCK_TYPE_POINTER: blockTypeStr = "Pointer"; break;
      case BasicBufferDebugInfo::BLOCK_TYPE_OFFSET:  blockTypeStr = "Offset"; break;
      default:                                       blockTypeStr = "Unknown"; break;
      }

      fprintf(file, "Block Label   : %s\n", i->m_DebugString.c_str());
      fprintf(file, "Block Type    : %s, Size: %d\n", blockTypeStr, i->m_BlockSize);
      fprintf(file, "Block Location: 0x%08X\n", i->m_FileSize);
      fprintf(file, "Contents      : ");

      u8* data = m_Data + i->m_FileSize;

      switch(i->m_BlockType)
      {
      case BasicBufferDebugInfo::BLOCK_TYPE_I8:      fprintf(file, "%02X\n",*((i8*)data)); break;
      case BasicBufferDebugInfo::BLOCK_TYPE_U8:      fprintf(file, "%02X\n",*((u8*)data)); break;
      case BasicBufferDebugInfo::BLOCK_TYPE_I16:     fprintf(file, "%04X\n",ConvertEndian( *((i16*)data),IsPlatformBigEndian())); break;
      case BasicBufferDebugInfo::BLOCK_TYPE_U16:     fprintf(file, "%04X\n",ConvertEndian( *((u16*)data),IsPlatformBigEndian())); break;
      case BasicBufferDebugInfo::BLOCK_TYPE_I32:     fprintf(file, "%08X\n",ConvertEndian( *((i32*)data),IsPlatformBigEndian())); break;
      case BasicBufferDebugInfo::BLOCK_TYPE_U32:     fprintf(file, "%08X\n",ConvertEndian( *((u32*)data),IsPlatformBigEndian())); break;
      case BasicBufferDebugInfo::BLOCK_TYPE_I64:     fprintf(file, "0x%I64x\n",ConvertEndian( *((u64*)data),IsPlatformBigEndian())); break;
      case BasicBufferDebugInfo::BLOCK_TYPE_U64:     fprintf(file, "0x%I64x\n",ConvertEndian( *((u64*)data),IsPlatformBigEndian())); break;
      case BasicBufferDebugInfo::BLOCK_TYPE_F32:     fprintf(file, "%.3f\n",ConvertEndian( *((f32*)data),IsPlatformBigEndian())); break;
      case BasicBufferDebugInfo::BLOCK_TYPE_F64:     fprintf(file, "%.3f\n",ConvertEndian( *((f64*)data),IsPlatformBigEndian())); break;
      case BasicBufferDebugInfo::BLOCK_TYPE_OFFSET:  fprintf(file, "0x%08x\n", *((u32*)data)); break;
      case BasicBufferDebugInfo::BLOCK_TYPE_POINTER: if (GetPlatformPtrSize() == 8) fprintf(file, "0x%I64x\n", *((u64*)data)); else fprintf(file, "0x%08x\n", *((u32*)data)); break;
      case BasicBufferDebugInfo::BLOCK_TYPE_RESERVE:
        {
          bool handled = false;

          switch (i->m_BlockSize) 
          {
          case 1: handled = true; fprintf(file, "%d\n", *((u8*)data)); break;
          case 2: handled = true; fprintf(file, "%d\n", *((u16*)data)); break;
          case 4: handled = true; fprintf(file, "%d\n", *((u32*)data)); break;
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

u32 BasicBuffer::AddBuffer( const u8* buffer, u32 size, const char* dbgStr, ... )
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

u32 BasicBuffer::AddBuffer( const SmartBufferPtr& buffer, bool add_fixups )
{
  // platform types have to match
  NOC_ASSERT(buffer->GetPlatform() == m_Platform);

  // first bring in all the data from the incoming buffer
  u32 return_val = AddBuffer( buffer->GetData(), buffer->GetSize() );

  if (add_fixups)
  {
    // inherit all the incoming / outgoing fixups from this buffer with out new offset
    InheritFixups( buffer, return_val );
  }

  return return_val;
}

u32 BasicBuffer::AddFile( const std::string& filename )
{
  return AddFile( filename.c_str() );
}

u32 BasicBuffer::AddFile( const char* filename )
{
  FILE *pfile = fopen( filename, "rb" );
  if ( pfile == NULL )
  {
    throw Nocturnal::Exception( "Could not open file '%s' to add to membuf '%s'.", filename, m_Name.c_str() );
  }

  fseek(pfile,0,SEEK_END);
  long filesize = ftell(pfile);
  fseek(pfile,0,SEEK_SET);

  if(filesize == -1)
  {
    throw Nocturnal::Exception("Could not get file size for file '%s' to add to membuf '%s'.", filename, m_Name.c_str() );
  }

  if ( (filesize + m_Size) > m_Capacity )
    GrowBy(filesize);

  size_t file_read = fread( m_Data + m_Size, 1, filesize, pfile );
  if(file_read != (size_t)filesize)
  {
    Console::Warning("Could not read entire file '%s' to add to membuf '%s'.\n", filename, m_Name.c_str() );
  }

  fclose(pfile);

  m_Size += (u32)file_read;
  return m_Size - file_read;
}

u32 BasicBuffer::AddI8( i8 val, const char* dbgStr, ... )
{
  ADD_DEBUG_INFO_SKIP(BasicBufferDebugInfo::BLOCK_TYPE_I8, 1);

  return AddBuffer( (u8*)&val, sizeof( i8 ) );
}

u32 BasicBuffer::AddU8( u8 val, const char* dbgStr, ... )
{
  ADD_DEBUG_INFO_SKIP(BasicBufferDebugInfo::BLOCK_TYPE_U8, 1);

  return AddBuffer( (u8*)&val, sizeof( u8 ) );
}

u32 BasicBuffer::AddU16( u16 val, const char* dbgStr, ... )
{
  ADD_DEBUG_INFO_SKIP(BasicBufferDebugInfo::BLOCK_TYPE_U16, 2);

  val = ConvertEndian(val,IsPlatformBigEndian());
  return AddBuffer( (u8*)&val, sizeof( u16 ) );
}

u32 BasicBuffer::AddI16( i16 val, const char* dbgStr, ... )
{
  ADD_DEBUG_INFO_SKIP(BasicBufferDebugInfo::BLOCK_TYPE_I16, 2);

  val = ConvertEndian(val,IsPlatformBigEndian());
  return AddBuffer( (u8*)&val, sizeof( i16 ) );
}

u32 BasicBuffer::AddI32( i32 val, const char* dbgStr, ... )
{
  ADD_DEBUG_INFO_SKIP(BasicBufferDebugInfo::BLOCK_TYPE_I32, 4);

  val = ConvertEndian(val,IsPlatformBigEndian());
  return AddBuffer( (u8*)&val, sizeof( i32 ) );
}

u32 BasicBuffer::AddU32( u32 val, const char* dbgStr, ... )
{
  ADD_DEBUG_INFO_SKIP(BasicBufferDebugInfo::BLOCK_TYPE_U32, 4);

  val = ConvertEndian(val,IsPlatformBigEndian());
  return AddBuffer( (u8*)&val, sizeof( u32 ) );
}

u32 BasicBuffer::AddI64( i64 val, const char* dbgStr, ... )
{
  ADD_DEBUG_INFO_SKIP(BasicBufferDebugInfo::BLOCK_TYPE_I64, 8);

  val = ConvertEndian(val,IsPlatformBigEndian());
  return AddBuffer( (u8*)&val, sizeof( i64 ) );
}

u32 BasicBuffer::AddU64( u64 val, const char* dbgStr, ... )
{
  ADD_DEBUG_INFO_SKIP(BasicBufferDebugInfo::BLOCK_TYPE_U64, 8);

  val = ConvertEndian(val,IsPlatformBigEndian());
  return AddBuffer( (u8*)&val, sizeof( u64 ) );
}

u32 BasicBuffer::AddF16( f32 val, const char* dbgStr, ... )
{
  i16 half = ::Math::FloatToHalf( val );
  ADD_DEBUG_INFO_SKIP(BasicBufferDebugInfo::BLOCK_TYPE_I16, 2);

  half = ConvertEndian(half,IsPlatformBigEndian());
  return AddBuffer( (u8*)&half, sizeof( i16 ) );
}

u32 BasicBuffer::AddF32( f32 val, const char* dbgStr, ... )
{
  u32 i = *(reinterpret_cast<u32 *>(&val));

  // handle case for "negative" zero..  this was
  // causing diffs to vary between reflect/non-xml
  //
  // IEEE format for a f32 is this:
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

u32 BasicBuffer::AddF64( f64 val, const char* dbgStr, ... )
{
  u64 i = *(reinterpret_cast<u64 *>(&val));

  // handle case for "negative" zero..  this was
  // causing diffs to vary between reflect/non-xml
  //
  // IEEE format for a f32 is this:
  // Sign   Exponent  Fraction
  //Bit 64 [63 -- 56] [55 -- 0]
  if ( (i & 0x7FFFFFFFFFFFFFFF) == 0x0 )
  {
    i = 0x0;
  }

  ADD_DEBUG_INFO_SKIP(BasicBufferDebugInfo::BLOCK_TYPE_F64, 8);

  return AddU64( i );
}

u32 BasicBuffer::AddVector3( const Math::Vector3& v, const char* debugStr )
{
  u32 ret = AddF32(v.x, debugStr);
  AddF32(v.y, debugStr);
  AddF32(v.z, debugStr);
  return ret;
}

u32 BasicBuffer::AddVector4( const Math::Vector4& v, const char* debugStr )
{
  u32 ret = AddF32(v.x, debugStr);
  AddF32(v.y, debugStr);
  AddF32(v.z, debugStr);
  AddF32(v.w, debugStr);
  return ret;
}

u32 BasicBuffer::AddVector4( const Math::Vector3& v, f32 w, const char* debugStr )
{
  u32 ret = AddF32(v.x, debugStr);
  AddF32(v.y, debugStr);
  AddF32(v.z, debugStr);
  AddF32(w, debugStr);
  return ret;
}

u32 BasicBuffer::AddVector4( f32 x, f32 y, f32 z, f32 w, const char* debugStr )
{
  u32 ret = AddF32(x, debugStr);
  AddF32(y, debugStr);
  AddF32(z, debugStr);
  AddF32(w, debugStr);
  return ret;
}

void BasicBuffer::AddAtLocI8( i8 val, const Location& destination )
{
  Write( destination, (u8*)&val, sizeof( i8 ) );
}

void BasicBuffer::AddAtLocU8( u8 val, const Location& destination )
{
  Write( destination, (u8*)&val, sizeof( u8 ) );
}

void BasicBuffer::AddAtLocI16( i16 val, const Location& destination )
{
  val = ConvertEndian(val,IsPlatformBigEndian());
  Write( destination, (u8*)&val, sizeof( i16 ) );
}

void BasicBuffer::AddAtLocU16( u16 val, const Location& destination )
{
  val = ConvertEndian(val,IsPlatformBigEndian());
  Write( destination, (u8*)&val, sizeof( u16 ) );
}

void BasicBuffer::AddAtLocI32( i32 val, const Location& destination )
{
  val = ConvertEndian(val,IsPlatformBigEndian());
  Write( destination, (u8*)&val, sizeof( i32 ) );
}

void BasicBuffer::AddAtLocU32( u32 val, const Location& destination )
{
  val = ConvertEndian(val,IsPlatformBigEndian());
  Write( destination, (u8*)&val, sizeof( u32 ) );
}

void BasicBuffer::AddAtLocI64( i64 val, const Location& destination )
{
  val = ConvertEndian(val,IsPlatformBigEndian());
  Write( destination, (u8*)&val, sizeof( i64 ) );
}

void BasicBuffer::AddAtLocU64( u64 val, const Location& destination )
{
  val = ConvertEndian(val,IsPlatformBigEndian());
  Write( destination, (u8*)&val, sizeof( u64 ) );
}

void BasicBuffer::AddAtLocF32( f32 val, const Location& destination )
{
  u32 i = *(reinterpret_cast<u32 *>(&val));

  // handle case for "negative" zero..  this was
  // causing diffs to vary between reflect/non-xml
  //
  // IEEE format for a f32 is this:
  // Sign   Exponent  Fraction
  //    0   00000000  00000000000000000000000
  //Bit 31 [30 -- 23] [22 -- 0]
  if ( (i & 0x7FFFFFFF) == 0x0 )
  {
    i = 0x0;
  }

  AddAtLocU32( i, destination );
}

void BasicBuffer::AddAtLocF64( f64 val, const Location& destination )
{
  u64 i = *(reinterpret_cast<u64 *>(&val));

  // handle case for "negative" zero..  this was
  // causing diffs to vary between reflect/non-xml
  //
  // IEEE format for a f32 is this:
  // Sign   Exponent  Fraction
  //Bit 64 [63 -- 56] [55 -- 0]
  if ( (i & 0x7FFFFFFFFFFFFFFF) == 0x0 )
  {
    i = 0x0;
  }

  AddAtLocU64( i, destination );
}

void BasicBuffer::AddPad( u32 pad_length )
{
  if ( (pad_length + m_Size) > m_Capacity )
  {
    GrowBy( pad_length );
  }

  for ( u32 i = 0; i < pad_length; ++i )
  {
    AddU8( 0 );
  }
}

void BasicBuffer::PadToArb( u32 align_size )
{
  i32 pad_length = (i32)align_size - ( m_Size % align_size );
  if ( pad_length != align_size )
  {
    NOC_ASSERT( pad_length > 0 );
    AddPad( (u32)pad_length );
  }
}

void BasicBuffer::SetCapacity(u32 capacity)
{
  if (capacity > m_Capacity)
  {
    GrowBy(capacity - m_Capacity);
  }  
}

SmartBuffer::Location BasicBuffer::Reserve(u32 size, const char* dbgStr, ...)
{
  ADD_DEBUG_INFO(BasicBufferDebugInfo::BLOCK_TYPE_RESERVE, size);

  Location return_val = GetCurrentLocation();

  if ( (size + m_Size) > m_Capacity )
  {
    GrowBy( size );
  }

  memset( m_Data + m_Size, 0, size );

  m_Size += size;

  return return_val;
}

void BasicBuffer::Reserve(Location& loc, u32 size, const char* dbgStr, ...)
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

SmartBuffer::Location BasicBuffer::ReservePointer(u32 size, const char* dbgStr, ... )
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

void BasicBuffer::ReservePointer(SmartBuffer::Location& loc, u32 size, const char* dbgStr, ... )
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

SmartBuffer::Location BasicBuffer::ReserveOffset( const char* dbgStr, ... )
{
  ADD_DEBUG_INFO_SKIP(BasicBufferDebugInfo::BLOCK_TYPE_OFFSET, 4);

  return Reserve( 4 );
}

void BasicBuffer::ReserveOffset(SmartBuffer::Location& loc, const char* dbgStr, ... )
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

void BasicBuffer::WritePointer( const Location& destination )
{
  AddPointerFixup( ReservePointer(0), destination,GetPlatformPtrSize() );
}

void BasicBuffer::WritePointer32( const Location& destination )
{
  AddPointerFixup( ReservePointer(4), destination,4 );
}

void BasicBuffer::WritePointer64( const Location& destination )
{
  AddPointerFixup( ReservePointer(8), destination,8 );
}

void BasicBuffer::WriteOffset( const Location& destination, bool absolute )
{
  AddOffsetFixup( ReserveOffset(), destination, absolute );
}