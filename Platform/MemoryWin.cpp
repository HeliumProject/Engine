#include "PlatformPch.h"
#include "Platform/Memory.h"

using namespace Helium;

/// Allocate memory pages of at least the specified size.
///
/// Memory allocated using this interface can later be returned to the system using Free().
///
/// @param[in] size  Allocation size, in bytes.
///
/// @return  Address of the allocation if successfully allocated, null if not.
///
/// @see Free()
void* PhysicalMemory::Allocate( size_t size )
{
    void* pMemory = VirtualAlloc( NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE );

#if HELIUM_ENABLE_MEMORY_TRACKING
    if( pMemory )
    {
        MEMORY_BASIC_INFORMATION memoryInfo;
        SIZE_T result = VirtualQuery( pMemory, &memoryInfo, sizeof( memoryInfo ) );
        if( result != 0 )
        {
#if HELIUM_WORDSIZE == 32
            InterlockedExchangeAdd(
                reinterpret_cast< LONG volatile* >( &sm_bytesAllocated ),
                static_cast< LONG >( memoryInfo.RegionSize ) );
#else
            InterlockedExchangeAdd64(
                reinterpret_cast< LONGLONG volatile* >( &sm_bytesAllocated ),
                static_cast< LONGLONG >( memoryInfo.RegionSize ) );
#endif
        }
    }
#endif

    return pMemory;
}

/// Free memory previously allocated using Allocate().
///
/// All memory pages allocated within the region starting at the given memory address up to the number of bytes
/// specified will be freed.  Multiple contiguous memory pages allocated via multiple contiguous calls to Allocate()
/// can be freed at once.
///
/// @param[in] pMemory  Base address of the allocation to free.
/// @param[in] size     Size of the range of pages to free.
///
/// @return  True if all pages were freed successfully, false if not.
///
/// @see Allocate()
bool PhysicalMemory::Free( void* pMemory, size_t size )
{
    HELIUM_ASSERT( pMemory );
    HELIUM_ASSERT( size != 0 );

    uint8_t* pCurrentBase = static_cast< uint8_t* >( pMemory );
    HELIUM_ASSERT( pCurrentBase + size == 0 || pCurrentBase + size > pCurrentBase );  // Check address space bounds.

    MEMORY_BASIC_INFORMATION memoryInfo;
    while( size != 0 )
    {
        size_t queryResult = VirtualQuery( pCurrentBase, &memoryInfo, sizeof( memoryInfo ) );
        HELIUM_ASSERT( queryResult != 0 );
        if( queryResult == 0 )
        {
            return false;
        }

        HELIUM_ASSERT( memoryInfo.BaseAddress == pCurrentBase );
        HELIUM_ASSERT( memoryInfo.AllocationBase == pCurrentBase );
        HELIUM_ASSERT( memoryInfo.State == MEM_COMMIT );
        HELIUM_ASSERT( memoryInfo.RegionSize <= size );
        if( memoryInfo.BaseAddress != pCurrentBase ||
            memoryInfo.AllocationBase != pCurrentBase ||
            memoryInfo.State != MEM_COMMIT ||
            memoryInfo.RegionSize > size )
        {
            return false;
        }

        BOOL freeResult = VirtualFree( pCurrentBase, 0, MEM_RELEASE );
        HELIUM_ASSERT( freeResult );
        if( !freeResult )
        {
            return false;
        }

        pCurrentBase += memoryInfo.RegionSize;
        size -= memoryInfo.RegionSize;

#if HELIUM_ENABLE_MEMORY_TRACKING
#if HELIUM_WORDSIZE == 32
        InterlockedExchangeAdd(
            reinterpret_cast< LONG volatile* >( &sm_bytesAllocated ),
            -static_cast< LONG >( memoryInfo.RegionSize ) );
#else
        InterlockedExchangeAdd64(
            reinterpret_cast< LONGLONG volatile* >( &sm_bytesAllocated ),
            -static_cast< LONGLONG >( memoryInfo.RegionSize ) );
#endif
#endif
    }

    return true;
}

/// Get the page size of memory allocated through this function.
///
/// @return  Current platform page size.
size_t PhysicalMemory::GetPageSize()
{
    SYSTEM_INFO systemInfo;
    GetSystemInfo( &systemInfo );

    return systemInfo.dwPageSize;
}
