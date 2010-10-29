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

        const static uint32_t s_BlockSize = 2048;

        BasicBufferDebugInfoVector m_DebugInfo;

    public:
        static bool IsDebugInfoEnabled();
        void AddDebugInfo(BasicBufferDebugInfo::BlockType blockType, unsigned int blockSize, const tchar *dbgStr, va_list argptr, tchar ignore_next);
        void DumpDebugInfo(FILE* file);

        uint32_t AddBuffer( const uint8_t* buffer, uint32_t size, const tchar* dbgStr = NULL, ... );
        uint32_t AddBuffer( const SmartBufferPtr& buffer,bool add_fixups );

        uint32_t AddFile( const tchar* filename );
        uint32_t AddFile( const tstring& filename );

        uint32_t AddI8(int8_t val, const tchar* dbgStr = NULL, ...);
        uint32_t AddU8(uint8_t val, const tchar* dbgStr = NULL, ...);

        uint32_t AddI16(int16_t val, const tchar* dbgStr = NULL, ...);
        uint32_t AddU16(uint16_t val, const tchar* dbgStr = NULL, ...);

        uint32_t AddI32(int32_t val, const tchar* dbgStr = NULL, ...);
        uint32_t AddU32(uint32_t val, const tchar* dbgStr = NULL, ...);

        uint32_t AddI64(int64_t val, const tchar* dbgStr = NULL, ...);
        uint32_t AddU64(uint64_t val, const tchar* dbgStr = NULL, ...);

        uint32_t AddF16(float32_t val, const tchar* dbgStr = NULL, ...);
        uint32_t AddF32(float32_t val, const tchar* dbgStr = NULL, ...);
        uint32_t AddF64(float64_t val, const tchar* dbgStr = NULL, ...);

        uint32_t AddVector3( const Math::Vector3& v, const tchar* debugStr = NULL);
        uint32_t AddVector4( const Math::Vector4& v, const tchar* debugStr = NULL);
        uint32_t AddVector4( const Math::Vector3& v, float32_t w, const tchar* debugStr = NULL);
        uint32_t AddVector4( float32_t x, float32_t y, float32_t z, float32_t w, const tchar* debugStr = NULL);

        void AddAtLocI8(int8_t val, const BufferLocation& destination);
        void AddAtLocU8(uint8_t val, const BufferLocation& destination);

        void AddAtLocI16(int16_t val, const BufferLocation& destination);
        void AddAtLocU16(uint16_t val, const BufferLocation& destination);

        void AddAtLocI32(int32_t val, const BufferLocation& destination);
        void AddAtLocU32(uint32_t val, const BufferLocation& destination);

        void AddAtLocI64(int64_t val, const BufferLocation& destination);
        void AddAtLocU64(uint64_t val, const BufferLocation& destination);

        void AddAtLocF32(float32_t val, const BufferLocation& destination);
        void AddAtLocF64(float64_t val, const BufferLocation& destination);

        void AddPad ( uint32_t pad_length );
        void PadToArb ( uint32_t align_size );
        void PadToBlock()
        {
            PadToArb( s_BlockSize );
        }

        void PadToWord()
        {
            PadToArb( sizeof( uint32_t ) );
        }

        /// Sets maximum capacity for buffer, reallocating if necessary.
        void SetCapacity(uint32_t capacity);

        /// Reserves 'size' space in the buffer and returns a BufferLocation object, advances the write location
        BufferLocation Reserve(uint32_t size, const tchar* dbgStr = NULL, ...);
        void Reserve(BufferLocation& loc, uint32_t size, const tchar* dbgStr = NULL, ...);

        /// Reserves space for a pointer in the buffer and returns a BufferLocation object, advances the write location
        // if size is zero an automatically sized pointer is reserved based on the platform otherwise size
        // must be 4 or 8 and in either case a pointer of this size will be created
        BufferLocation ReservePointer(uint32_t size, const tchar* dbgStr = NULL, ... );
        void ReservePointer(BufferLocation& loc, uint32_t size, const tchar* dbgStr = NULL, ... );

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
