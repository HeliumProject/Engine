#pragma once

#include "SmartBuffer.h"

namespace Helium
{
    namespace Math
    {
        class Vector3; 
        class Vector4; 
    }

    class BasicBuffer;
    typedef Helium::SmartPtr<BasicBuffer> BasicBufferPtr;

    //////////////////////////////////////////////////////////////////////////
    //
    // samc: I originally added BasicBufferDebugInfo to help track down
    // differences between idb2ice and mb.  In addition, it helped to document
    // what every Add*() call was writing out on the game side.
    //
    // Basically, a BasicBufferDebugInfo is created for each Add* call, and
    // inserted into a vector in the BasicBuffer.  The Add* calls now take
    // an optionally formatted debug string.  The BasicBufferDebugInfo still
    // works without the strings, but the strings really help to document
    // exactly what piece of data you are writing (in mb, I print out the game-
    // side structure and variable name each Add* call is adding).
    //
    // Then, there is a new method, DumpDebugInfo() which can dump all the
    // debug structures to a text file (easy to view and diff).  By default,
    // the debug info is turned off.  To turn it on, you need to change
    // this define in BasicBuffer.cpp:
    //
    // #define BASIC_BUFFER_DEBUG_INFO 0
    //
    // to
    //
    // #define BASIC_BUFFER_DEBUG_INFO 1
    //
    // I changed IGSerializer to automatically call DumpDebugInfo on all
    // of its BasicBuffers when you call Serialize on it.  So if you have
    // a special file, it can probably follow that format...
    //
    //////////////////////////////////////////////////////////////////////////
    class FOUNDATION_API BasicBufferDebugInfo
    {
    public:

        enum BlockType
        {
            BLOCK_TYPE_NONE = 0,
            BLOCK_TYPE_BUFFER,
            BLOCK_TYPE_I8,
            BLOCK_TYPE_U8,
            BLOCK_TYPE_I16,
            BLOCK_TYPE_U16,
            BLOCK_TYPE_I32,
            BLOCK_TYPE_U32,
            BLOCK_TYPE_I64,
            BLOCK_TYPE_U64,
            BLOCK_TYPE_F32,
            BLOCK_TYPE_F64,
            BLOCK_TYPE_RESERVE,
            BLOCK_TYPE_POINTER,
            BLOCK_TYPE_OFFSET,
        };

        tstring     m_DebugString;
        unsigned int    m_FileSize;
        unsigned int    m_BlockSize;
        BlockType       m_BlockType;
    };

    typedef std::vector<BasicBufferDebugInfo> BasicBufferDebugInfoVector;

    //
    // BasicBuffer is the most simple implementation of a SmartBuffer 
    //

    class FOUNDATION_API BasicBuffer : public SmartBuffer
    {
        friend class BufferCollection;

        const static u32 s_BlockSize = 2048;

        BasicBufferDebugInfoVector m_DebugInfo;

    public:
        static bool IsDebugInfoEnabled();
        void AddDebugInfo(BasicBufferDebugInfo::BlockType blockType, unsigned int blockSize, const tchar *dbgStr, va_list argptr, tchar ignore_next);
        void DumpDebugInfo(FILE* file);

        u32 AddBuffer( const u8* buffer, u32 size, const tchar* dbgStr = NULL, ... );
        u32 AddBuffer( const SmartBufferPtr& buffer,bool add_fixups );

        u32 AddFile( const tchar* filename );
        u32 AddFile( const tstring& filename );

        u32 AddI8(i8 val, const tchar* dbgStr = NULL, ...);
        u32 AddU8(u8 val, const tchar* dbgStr = NULL, ...);

        u32 AddI16(i16 val, const tchar* dbgStr = NULL, ...);
        u32 AddU16(u16 val, const tchar* dbgStr = NULL, ...);

        u32 AddI32(i32 val, const tchar* dbgStr = NULL, ...);
        u32 AddU32(u32 val, const tchar* dbgStr = NULL, ...);

        u32 AddI64(i64 val, const tchar* dbgStr = NULL, ...);
        u32 AddU64(u64 val, const tchar* dbgStr = NULL, ...);

        u32 AddF16(f32 val, const tchar* dbgStr = NULL, ...);
        u32 AddF32(f32 val, const tchar* dbgStr = NULL, ...);
        u32 AddF64(f64 val, const tchar* dbgStr = NULL, ...);

        u32 AddVector3( const Math::Vector3& v, const tchar* debugStr = NULL);
        u32 AddVector4( const Math::Vector4& v, const tchar* debugStr = NULL);
        u32 AddVector4( const Math::Vector3& v, f32 w, const tchar* debugStr = NULL);
        u32 AddVector4( f32 x, f32 y, f32 z, f32 w, const tchar* debugStr = NULL);

        void AddAtLocI8(i8 val, const BufferLocation& destination);
        void AddAtLocU8(u8 val, const BufferLocation& destination);

        void AddAtLocI16(i16 val, const BufferLocation& destination);
        void AddAtLocU16(u16 val, const BufferLocation& destination);

        void AddAtLocI32(i32 val, const BufferLocation& destination);
        void AddAtLocU32(u32 val, const BufferLocation& destination);

        void AddAtLocI64(i64 val, const BufferLocation& destination);
        void AddAtLocU64(u64 val, const BufferLocation& destination);

        void AddAtLocF32(f32 val, const BufferLocation& destination);
        void AddAtLocF64(f64 val, const BufferLocation& destination);

        void AddPad ( u32 pad_length );
        void PadToArb ( u32 align_size );
        void PadToBlock()
        {
            PadToArb( s_BlockSize );
        }

        void PadToWord()
        {
            PadToArb( sizeof( u32 ) );
        }

        /// Sets maximum capacity for buffer, reallocating if necessary.
        void SetCapacity(u32 capacity);

        /// Reserves 'size' space in the buffer and returns a BufferLocation object, advances the write location
        BufferLocation Reserve(u32 size, const tchar* dbgStr = NULL, ...);
        void Reserve(BufferLocation& loc, u32 size, const tchar* dbgStr = NULL, ...);

        /// Reserves space for a pointer in the buffer and returns a BufferLocation object, advances the write location
        // if size is zero an automatically sized pointer is reserved based on the platform otherwise size
        // must be 4 or 8 and in either case a pointer of this size will be created
        BufferLocation ReservePointer(u32 size, const tchar* dbgStr = NULL, ... );
        void ReservePointer(BufferLocation& loc, u32 size, const tchar* dbgStr = NULL, ... );

        /// Reserves space for an offset in the buffer and returns a BufferLocation object, advances the write location
        BufferLocation ReserveOffset(const tchar* dbgStr = NULL, ... );
        void ReserveOffset(BufferLocation& loc, const tchar* dbgStr = NULL, ... );

        /// At the current offset write a pointer to the destination, advances the write location
        void WritePointer( const BufferLocation& destination );
        void WritePointer32( const BufferLocation& destination );
        void WritePointer64( const BufferLocation& destination );

        /// At the current offset write an offset to the destination, advances the write location
        void WriteOffset( const BufferLocation& destination, bool absolute = false );
    };

    // this is a handy function for checking to make sure you have written all of the 
    // data that is required at the point you are writing it. the BasicBuffer will do this check
    // later when writing to the file, but it is easier to debug if you can catch it when you are
    // doing the writing
    // 
    template <class T>
    void AssertComplete(BasicBufferPtr& buffer)
    {
        HELIUM_ASSERT(buffer->GetCurrentLocation().first - buffer->GetHeadLocation().first == sizeof(T)); 
    }
}
