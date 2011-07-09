#pragma once

#include "Platform/Types.h"
#include "Foundation/API.h"

#include <sys/types.h>

namespace Helium
{
    namespace Profile
    {
        //
        // Allows the profiling of pools of allocated memory over time
        //

        struct MemoryPool
        {
            const tchar_t* m_Name;
            uint32_t         m_Count;
            uint64_t         m_Size;
            uint64_t         m_Previous;

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
            int32_t m_Index;

            MemoryPoolHandle()
                : m_Index (-1)
            {

            }

            bool Valid()
            {
                return m_Index != -1;
            }
        };

        class HELIUM_FOUNDATION_API Memory
        {
        private:
            static uint32_t s_InitCount;

        public:
            static bool Initialize();
            static void Cleanup();

            static MemoryPoolHandle CreatePool(const tchar_t* name);

            static void Allocate(MemoryPoolHandle pool, uint32_t size);
            static void Deallocate(MemoryPoolHandle pool, uint32_t size);
        };
    }

}