#pragma once

#include "Platform/Types.h"
#include "Platform/Utility.h"
#include <new>

/// @defgroup defaultheapmacro Default Module Memory Heap Declaration
//@{

#ifndef HELIUM_USE_MODULE_HEAPS
/// Non-zero if module-specific heaps should be enabled.
#define HELIUM_USE_MODULE_HEAPS 1
#endif

#ifndef HELIUM_USE_EXTERNAL_HEAP
/// Non-zero if a separate heap should be used for external libraries.
#define HELIUM_USE_EXTERNAL_HEAP 1
#endif

/// Define the parameter list for a dynamic memory heap, associating it with the given name in non-release builds.
///
/// @param[in] NAME_STRING  Name string to assign to the heap.
#if HELIUM_RELEASE || HELIUM_PROFILE
#define HELIUM_DYNAMIC_MEMORY_HEAP_INIT( NAME_STRING )
#else
#define HELIUM_DYNAMIC_MEMORY_HEAP_INIT( NAME_STRING ) ( NAME_STRING )
#endif

/// Define the parameter list for a fixed-capacity dynamic memory heap, associating it with the given name in
/// non-release builds.
///
/// @param[in] NAME_STRING  Name string to assign to the heap.
/// @param[in] CAPACITY     Heap capacity, in bytes.
#if HELIUM_RELEASE || HELIUM_PROFILE
#define HELIUM_DYNAMIC_MEMORY_HEAP_CAP_INIT( NAME_STRING, CAPACITY ) ( CAPACITY )
#else
#define HELIUM_DYNAMIC_MEMORY_HEAP_CAP_INIT( NAME_STRING, CAPACITY ) ( NAME_STRING, CAPACITY )
#endif

/// Define the default memory heap for the current module.
///
/// This must be called in the source file of a given module in order to set up the default heap to use for that module.
///
/// @param[in] MODULE_NAME  Name of the module.  This will be associated with the module's heap when debugging or
///                         viewing memory stats.
#define HELIUM_DEFINE_DEFAULT_MODULE_HEAP( MODULE_NAME ) \
    namespace Helium \
    { \
        DynamicMemoryHeap& HELIUM_MODULE_HEAP_FUNCTION() \
        { \
            static DynamicMemoryHeap* pModuleHeap = NULL; \
            if( !pModuleHeap ) \
            { \
                pModuleHeap = static_cast< DynamicMemoryHeap* >( \
                    VirtualMemory::Allocate( sizeof( DynamicMemoryHeap ) ) ); \
                new( pModuleHeap ) DynamicMemoryHeap HELIUM_DYNAMIC_MEMORY_HEAP_INIT( TXT( #MODULE_NAME ) ); \
            } \
            \
            return *pModuleHeap; \
        } \
    }

#if HELIUM_USE_MODULE_HEAPS
    /// Default memory heap.  This exposes the memory heap for any module in which this file is included.
    #define HELIUM_DEFAULT_HEAP Helium::HELIUM_MODULE_HEAP_FUNCTION()
#else
    /// Default memory heap.
    #define HELIUM_DEFAULT_HEAP Helium::GetDefaultHeap()
#endif

#if HELIUM_USE_EXTERNAL_HEAP
    /// Default dynamic memory heap for allocations from external libraries.
    #define HELIUM_EXTERNAL_HEAP Helium::GetExternalHeap()
#else
    /// Default dynamic memory heap for allocations from external libraries.
    #define HELIUM_EXTERNAL_HEAP HELIUM_DEFAULT_HEAP
#endif

//@}

/// @defgroup newdeletemacros "new"/"delete" Utility Macros
//@{

/// Create a new object using a specific heap or allocator.
///
/// @param[in] ALLOCATOR  Reference to a Helium::MemoryHeap or allocator from which to allocate memory.
/// @param[in] TYPE       Type of object to create.
/// @param[in] ...        Optional parameter list.
///
/// @return  Pointer to the newly created object, if successful.
///
/// @see HELIUM_DELETE()
#define HELIUM_NEW( ALLOCATOR, TYPE, ... ) new( ALLOCATOR ) TYPE( __VA_ARGS__ )

/// Create a new array of objects using a specific heap or allocator.
///
/// @param[in] ALLOCATOR  Reference to a Helium::MemoryHeap or allocator from which to allocate memory.
/// @param[in] TYPE       Type of object to create.
/// @param[in] COUNT      Number of elements to create.
///
/// @return  Pointer to the newly created array, if successful.
///
/// @see HELIUM_DELETE_A()
#define HELIUM_NEW_A( ALLOCATOR, TYPE, COUNT ) Helium::NewArrayHelper< T >( ALLOCATOR, COUNT )

/// Destroy a single object allocated from a specific heap or allocator.
///
/// @param[in] ALLOCATOR  Reference to a Helium::MemoryHeap or allocator from which the object memory was allocated.
/// @param[in] OBJ        Pointer to the object to destroy.
///
/// @see HELIUM_NEW()
#define HELIUM_DELETE( ALLOCATOR, OBJ ) Helium::DeleteHelper( ALLOCATOR, OBJ )

/// Destroy an array of objects allocated from a specific heap or allocator.
///
/// @param[in] ALLOCATOR  Reference to a Helium::MemoryHeap or allocator from which the object memory was allocated.
/// @param[in] OBJ        Pointer to the array to destroy.
///
/// @see HELIUM_NEW_A()
#define HELIUM_DELETE_A( ALLOCATOR, OBJ ) Helium::DeleteArrayHelper( ALLOCATOR, OBJ )

//@}

/// @defgroup memdebug Memory Debug Settings
//@{

#ifndef HELIUM_ENABLE_MEMORY_TRACKING
/// Non-zero if general memory tracking should be enabled.
#define HELIUM_ENABLE_MEMORY_TRACKING ( !HELIUM_RELEASE )
#endif

#ifndef HELIUM_ENABLE_MEMORY_TRACKING_VERBOSE
/// Non-zero if detailed allocation tracking should be enabled.
#define HELIUM_ENABLE_MEMORY_TRACKING_VERBOSE ( 0/*HELIUM_ENABLE_MEMORY_TRACKING && HELIUM_DEBUG*/ )
#endif

//@}

/// Size of SIMD vectors, in bytes.
#define HELIUM_SIMD_SIZE ( 16 )
/// Alignment of SIMD vectors, in bytes.
#define HELIUM_SIMD_ALIGNMENT ( 16 )

/// Prefix macro for declaring SIMD type or variable.
#define HELIUM_SIMD_ALIGN_PRE HELIUM_ALIGN_PRE( 16 )
/// Suffix macro for declaring SIMD type or variable alignment.
#define HELIUM_SIMD_ALIGN_POST HELIUM_ALIGN_POST( 16 )

namespace Helium
{
    class ReadWriteLock;
    class ThreadLocalPointer;

#if HELIUM_ENABLE_MEMORY_TRACKING_VERBOSE
    struct DynamicMemoryHeapVerboseTrackingData;
#endif

    /// Low-level memory allocation interface.  This is used to allocate pages of memory in the application's address
    /// space (physical memory if possible).  Typically, most application will not use this directly, but will instead
    /// allocate using one of the provided heap allocators, which provide better management for most runtime
    /// allocations.
    class HELIUM_PLATFORM_API VirtualMemory
    {
    public:
        /// @name Memory Allocation
        //@{
        static void* Allocate( size_t size );
        static bool Free( void* pMemory, size_t size );
        //@}

        /// @name Memory Information
        //@{
        static size_t GetPageSize();
        //@}

#if HELIUM_ENABLE_MEMORY_TRACKING
    private:
        /// Number of bytes of physical memory currently allocated.
        static volatile size_t sm_bytesAllocated;
#endif
    };

    /// Memory heap base class.
    ///
    /// This provides the basic interface for custom heap allocators.  The results of each function should behave
    /// similar to their C standard library counterparts, specifically:
    /// - Allocate() should return a null pointer if an allocation is unsuccessful.
    /// - Reallocate() should behave just like Allocate() if the initial memory address provided is null.
    /// - Reallocate() should behave just as if Free() was called on the memory address provided if the size is null.
    /// - The alignment parameter of AllocateAligned() should be expected to be a power of two.
    /// - Free() should run with no ill effects if a null pointer is provided for the memory address to free.
    class HELIUM_PLATFORM_API MemoryHeap : NonCopyable
    {
    public:
        /// @name Allocation Interface
        //@{
        virtual void* Allocate( size_t size ) = 0;
        virtual void* Reallocate( void* pMemory, size_t size ) = 0;
        virtual void* AllocateAligned( size_t alignment, size_t size ) = 0;
        virtual void Free( void* pMemory ) = 0;
        virtual size_t GetMemorySize( void* pMemory ) = 0;
        //@}
    };

    /// Thread-safe dynamic memory heap.
    ///
    /// This provides a thread-safe, dynamic memory heap for general purpose allocations.  Pages of memory are allocated
    /// from the system using VirtualMemory::Allocate() and VirtualMemory::Free(), and allocations within these blocks
    /// are managed internally using nedmalloc (http://www.nedprod.com/programs/portable/nedmalloc/) to provide
    /// efficient scalability across multiple threads.
    class HELIUM_PLATFORM_API DynamicMemoryHeap : public MemoryHeap
    {
    public:
#if HELIUM_ENABLE_MEMORY_TRACKING_VERBOSE
        /// Maximum number of allocations to include in an allocation backtrace.
        static const size_t BACKTRACE_DEPTH_MAX = 32;

        /// Memory allocation backtrace data.
        struct AllocationBacktrace 
        {
            /// Program counter addresses.
            void* pAddresses[ BACKTRACE_DEPTH_MAX ];
        };
#endif

        /// @name Construction/Destruction
        //@{
        DynamicMemoryHeap( size_t capacity = 0 );
#if !HELIUM_RELEASE && !HELIUM_PROFILE
        DynamicMemoryHeap( const tchar_t* pName, size_t capacity = 0 );
#endif
        virtual ~DynamicMemoryHeap();
        //@}

        /// @name Allocation Interface
        //@{
        virtual void* Allocate( size_t size );
        virtual void* Reallocate( void* pMemory, size_t size );
        virtual void* AllocateAligned( size_t alignment, size_t size );
        virtual void Free( void* pMemory );
        virtual size_t GetMemorySize( void* pMemory );
        //@}

        /// @name Global Heap List Iteration
        //@{
        inline DynamicMemoryHeap* GetPreviousHeap() const;
        inline DynamicMemoryHeap* GetNextHeap() const;
        //@}

        /// @name Debugging
        //@{
#if !HELIUM_RELEASE && !HELIUM_PROFILE
        inline const tchar_t* GetName() const;
#endif

#if HELIUM_ENABLE_MEMORY_TRACKING
        inline size_t GetAllocationCount() const;
        inline size_t GetBytesActual() const;
#endif
        //@}

        /// @name Global Heap List Access
        //@{
        static DynamicMemoryHeap* LockReadGlobalHeapList();
        static void UnlockReadGlobalHeapList();

        static void UnregisterCurrentThreadCache();
        //@}

#if HELIUM_ENABLE_MEMORY_TRACKING
        /// @name Memory Stat Support
        //@{
        static void LogMemoryStats();
        //@}
#endif

    private:
        /// mspace instance.
        void* m_pMspace;
#if !HELIUM_RELEASE && !HELIUM_PROFILE
        /// Heap name (for debugging).
        const tchar_t* m_pName;
#endif

        /// Previous dynamic memory heap in the global list.
        DynamicMemoryHeap* volatile m_pPreviousHeap;
        /// Next dynamic memory heap in the global list.
        DynamicMemoryHeap* volatile m_pNextHeap;

#if HELIUM_ENABLE_MEMORY_TRACKING
        /// Number of allocations.
        volatile size_t m_allocationCount;
        /// Total number of bytes actually allocated.
        volatile size_t m_bytesActual;
#endif

#if HELIUM_ENABLE_MEMORY_TRACKING_VERBOSE
        /// Verbose memory tracking data.
        DynamicMemoryHeapVerboseTrackingData* m_pVerboseTrackingData;
#endif

        /// Head of the global list of dynamic memory heaps.
        static DynamicMemoryHeap* volatile sm_pGlobalHeapListHead;

#if HELIUM_ENABLE_MEMORY_TRACKING_VERBOSE
        /// True to temporarily disable memory backtrace tracking.
        static volatile bool sm_bDisableBacktraceTracking;
#endif

        /// @name Private Utility Functions
        //@{
        void ConstructNoName( size_t capacity );

#if HELIUM_ENABLE_MEMORY_TRACKING
        void AddAllocation( void* pMemory );
        void RemoveAllocation( void* pMemory );
#endif
        //@}

        /// @name Private Static Utility Functions
        //@{
        static ReadWriteLock& GetGlobalHeapListLock();
#if HELIUM_ENABLE_MEMORY_TRACKING
        static DynamicMemoryHeap* GetAllocationHeap( void* pMemory );
#endif
        //@}
    };

    /// Default dynamic memory allocator.
    ///
    /// This provides access to a single, global allocator managed using DynamicMemoryHeap.  This allocator is used by
    /// the global "new" and "delete" operators.  In general, it is preferred to use specific heaps to improve
    /// performance and prevent fragmentation, but general allocations will still typically be handled more efficiently
    /// than the system's default allocator.
    class DefaultAllocator
    {
    public:
        /// @name Memory Allocation
        //@{
        HELIUM_FORCEINLINE void* Allocate( size_t size );
        HELIUM_FORCEINLINE void* Reallocate( void* pMemory, size_t size );
        HELIUM_FORCEINLINE void* AllocateAligned( size_t alignment, size_t size );
        HELIUM_FORCEINLINE void Free( void* pMemory );
        HELIUM_FORCEINLINE size_t GetMemorySize( void* pMemory );
        //@}
    };

    /// Stack-based memory heap.
    ///
    /// This provides a simple stack-based memory pool.  Allocations are performed by grabbing the current stack pointer
    /// to use as the allocation base address and incrementing the stack pointer by the size of the requested
    /// allocation.  "Freeing" an allocation simply resets the stack pointer to the allocation base address.  As such,
    /// allocations can only be freed in the reverse of the order in which they were made.  Due to the simplistic nature
    /// of the memory pool management, Reallocate() is not supported.
    ///
    /// In addition to the standard MemoryHeap interface, the StackMemoryHeap::Marker class allows for more efficient
    /// releasing of stack memory by allowing the user to mark a stack location to immediately "pop" back to, releasing
    /// all allocations after the Marker was set instantly.
    ///
    /// StackMemoryHeap is not thread-safe.  Its primary purpose is to provide a method for performing moderately
    /// efficient allocations, often with a short lifetime.
    ///
    /// Note that the base address of all blocks are aligned to HELIUM_SIMD_ALIGNMENT by default.
    template< typename Allocator = DefaultAllocator >
    class StackMemoryHeap : public MemoryHeap
    {
    public:
        /// Stack marker.
        ///
        /// Stack markers are used to track the location of the stack pointer at a given time so that the stack can be
        /// popped directly back to the original location, discarding the contents of all allocations made after the
        /// stack marker was set.  This simplifies freeing stack heap memory allocated over a given block of execution
        /// time by allowing all such allocations to be "freed" instantly.
        class Marker
        {
        public:
            /// @name Construction/Destruction
            //@{
            Marker();
            explicit Marker( StackMemoryHeap& rHeap );
            ~Marker();
            //@}

            /// @name Stack Manipulation
            //@{
            void Set( StackMemoryHeap& rHeap );
            void Pop();
            //@}

        private:
            /// Currently tracked memory heap.
            StackMemoryHeap* m_pHeap;
            /// Tracked stack pointer.
            void* m_pStackPointer;

            /// @name Construction/Destruction
            //@{
            Marker( const Marker& );  // Not implemented.
            //@}

            /// @name Overloaded Operators
            //@{
            Marker& operator=( const Marker& );  // Not implemented.
            //@}
        };

        /// @name Construction/Destruction
        //@{
        StackMemoryHeap( size_t blockSize, size_t blockCountMax = Invalid< size_t >(), size_t defaultAlignment = 8 );
        ~StackMemoryHeap();
        //@}

        /// @name Allocation Interface
        //@{
        virtual void* Allocate( size_t size );
        virtual void* Reallocate( void* pMemory, size_t size );
        virtual void* AllocateAligned( size_t alignment, size_t size );
        virtual void Free( void* pMemory );
        virtual size_t GetMemorySize( void* pMemory );
        //@}

    private:
        /// Allocated memory block.
        struct Block
        {
            /// Block memory buffer.
            void* m_pBuffer;

            /// Previous block.
            Block* m_pPreviousBlock;
            /// Next block.
            Block* m_pNextBlock;
        };

        /// Head block in the stack.
        Block* m_pHeadBlock;
        /// Tail block in the stack.
        Block* m_pTailBlock;

        /// Currently active block in the stack.
        Block* m_pCurrentBlock;
        /// Current stack pointer.
        void* m_pStackPointer;

        /// Size of each block.
        size_t m_blockSize;
        /// Default allocation alignment.
        size_t m_defaultAlignment;
        /// Remaining number of blocks that can be allocated.
        size_t m_remainingBlockCount;

        /// @name Utility Functions
        //@{
        Block* AllocateBlock();
        //@}
    };

    /// Thread-local stack-based allocator.
    ///
    /// This provides an interface to a StackMemoryHeap instance specifically for the current thread.  This heap is a
    /// growable heap that can be used for various temporary allocations.  Note that since jobs can be run on any
    /// thread, you should never attempt to leave an allocation around for another job to free.
    class HELIUM_PLATFORM_API ThreadLocalStackAllocator
    {
    public:
        /// Size of each stack block.
        static const size_t BLOCK_SIZE = 512 * 1024;

        /// @name Construction/Destruction
        //@{
        ThreadLocalStackAllocator();
        //@}

        /// @name Memory Allocation
        //@{
        void* Allocate( size_t size );
        void* AllocateAligned( size_t alignment, size_t size );
        void Free( void* pMemory );
        //@}

        /// @name Static Access
        //@{
        static StackMemoryHeap<>& GetMemoryHeap();
        static void ReleaseMemoryHeap();
        //@}

    private:
        /// Cached reference to the current thread's stack heap.
        StackMemoryHeap<>* m_pHeap;

        /// @name Thread-local Storage Access
        //@{
        static ThreadLocalPointer& GetMemoryHeapTls();
        //@}
    };

#if HELIUM_USE_MODULE_HEAPS
    /// Get the default heap to use for dynamic allocations from this module (not DLL-exported).
    extern DynamicMemoryHeap& HELIUM_MODULE_HEAP_FUNCTION();
#else
    /// Get the default heap to use for dynamic allocations.
    HELIUM_PLATFORM_API DynamicMemoryHeap& GetDefaultHeap();
#endif

#if HELIUM_USE_EXTERNAL_HEAP
    /// Get the default heap to use for dynamic allocations from external libraries.
    HELIUM_PLATFORM_API DynamicMemoryHeap& GetExternalHeap();
#endif

    /// @defgroup newdeletehelper "new"/"delete" Helper Functions
    /// Note: These are only intended to be used internally.  Do not call these functions directly.
    //@{
    template< typename T, typename Allocator > void DeleteHelper( Allocator& rAllocator, T* pObject );

    template< typename T, typename Allocator > T* NewArrayHelper( Allocator& rAllocator, size_t count );
    template< typename T, typename Allocator > T* NewArrayHelper(
        Allocator& rAllocator, size_t count, const std::true_type& rHasTrivialDestructor );
    template< typename T, typename Allocator > T* NewArrayHelper(
        Allocator& rAllocator,
        size_t count,
        const std::false_type& rHasTrivialDestructor );

    template< typename T, typename Allocator > void DeleteArrayHelper( Allocator& rAllocator, T* pArray );
    template< typename T, typename Allocator > void DeleteArrayHelper(
        Allocator& rAllocator, T* pArray, const std::true_type& rHasTrivialDestructor );
    template< typename T, typename Allocator > void DeleteArrayHelper(
        Allocator& rAllocator, T* pArray, const std::false_type& rHasTrivialDestructor );

    template< typename Allocator > void* AllocateAlignmentHelper( Allocator& rAllocator, size_t size );
    //@}
}

/// @defgroup memoryheapnew Helium::MemoryHeap "new" Overrides
//@{
inline void* operator new( size_t size, Helium::MemoryHeap& rHeap );
//@}

#include "Platform/Memory.inl"
