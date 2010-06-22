#pragma once

#include "Platform/Types.h"
#include "Foundation/API.h"

#include <sys/types.h>

namespace Profile
{
    //
    // Allows the profiling of pools of allocated memory over time
    //

    struct MemoryPool
    {
        const char* m_Name;
        u32         m_Count;
        u64         m_Size;
        u64         m_Previous;

        MemoryPool()
            : m_Name (NULL)
            , m_Count (0)
            , m_Size (0)
            , m_Previous (0)
        {

        }
    };

    struct MemoryPoolHandle
    {
        i32 m_Index;

        MemoryPoolHandle()
            : m_Index (-1)
        {

        }

        bool Valid()
        {
            return m_Index != -1;
        }
    };

    class FOUNDATION_API Memory
    {
    private:
        static u32 s_InitCount;

    public:
        static bool Initialize();
        static void Cleanup();

        static MemoryPoolHandle CreatePool(const char* name);

        static void Allocate(MemoryPoolHandle pool, u32 size);
        static void Deallocate(MemoryPoolHandle pool, u32 size);
    };
}

