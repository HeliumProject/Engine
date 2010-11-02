//----------------------------------------------------------------------------------------------------------------------
// AtomicWin.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

// Windows (non-Itanium platforms) doesn't reorder reads and writes, so memory barriers aren't needed.  As such, all
// variants of each Atomic*() function are the same.  On the downside, we can't easily debug these functions when using
// macros to generate them, and build times may be somewhat slower.
#define _GENERATE_ATOMIC_WORKER( PREFIX, OPERATION, PARAM_LIST, ACTION ) \
    PREFIX Atomic##OPERATION PARAM_LIST ACTION \
    PREFIX Atomic##OPERATION##Acquire PARAM_LIST ACTION \
    PREFIX Atomic##OPERATION##Release PARAM_LIST ACTION \
    PREFIX Atomic##OPERATION##Unsafe PARAM_LIST ACTION

namespace Lunar
{
    _GENERATE_ATOMIC_WORKER(
        int32_t,
        Exchange,
        ( int32_t volatile & rAtomic, int32_t value ),
        {
            return InterlockedExchange( reinterpret_cast< volatile LONG* >( &rAtomic ), value );
        } )
    _GENERATE_ATOMIC_WORKER(
        int32_t,
        CompareExchange,
        ( int32_t volatile & rAtomic, int32_t value, int32_t compare ),
        {
            return InterlockedCompareExchange( reinterpret_cast< volatile LONG* >( &rAtomic ), value, compare );
        } )
    _GENERATE_ATOMIC_WORKER(
        int32_t,
        Increment,
        ( int32_t volatile & rAtomic ),
        {
            return InterlockedIncrement( reinterpret_cast< volatile LONG* >( &rAtomic ) );
        } )
    _GENERATE_ATOMIC_WORKER(
        int32_t,
        Decrement,
        ( int32_t volatile & rAtomic ),
        {
            return InterlockedDecrement( reinterpret_cast< volatile LONG* >( &rAtomic ) );
        } )
    _GENERATE_ATOMIC_WORKER(
        int32_t,
        Add,
        ( int32_t volatile & rAtomic, int32_t value ),
        {
            return InterlockedExchangeAdd( reinterpret_cast< volatile LONG* >( &rAtomic ), value );
        } )
    _GENERATE_ATOMIC_WORKER(
        int32_t,
        Subtract,
        ( int32_t volatile & rAtomic, int32_t value ),
        {
            return InterlockedExchangeAdd( reinterpret_cast< volatile LONG* >( &rAtomic ), -value );
        } )
    _GENERATE_ATOMIC_WORKER(
        int32_t,
        And,
        ( int32_t volatile & rAtomic, int32_t value ),
        {
            int32_t originalValueNew = rAtomic;
            int32_t originalValueOld;
            do
            {
                originalValueOld = originalValueNew;
                originalValueNew = InterlockedCompareExchange(
                    reinterpret_cast< volatile LONG* >( &rAtomic ),
                    originalValueOld & value,
                    originalValueOld );
            } while( originalValueNew != originalValueOld );

            return originalValueNew;
        } )
    _GENERATE_ATOMIC_WORKER(
        int32_t,
        Or,
        ( int32_t volatile & rAtomic, int32_t value ),
        {
            int32_t originalValueNew = rAtomic;
            int32_t originalValueOld;
            do
            {
                originalValueOld = originalValueNew;
                originalValueNew = InterlockedCompareExchange(
                    reinterpret_cast< volatile LONG* >( &rAtomic ),
                    originalValueOld | value,
                    originalValueOld );
            } while( originalValueNew != originalValueOld );

            return originalValueNew;
        } )
    _GENERATE_ATOMIC_WORKER(
        int32_t,
        Xor,
        ( int32_t volatile & rAtomic, int32_t value ),
        {
            int32_t originalValueNew = rAtomic;
            int32_t originalValueOld;
            do
            {
                originalValueOld = originalValueNew;
                originalValueNew = InterlockedCompareExchange(
                    reinterpret_cast< volatile LONG* >( &rAtomic ),
                    originalValueOld ^ value,
                    originalValueOld );
            } while( originalValueNew != originalValueOld );

            return originalValueNew;
        } )

    _GENERATE_ATOMIC_WORKER(
        template< typename T > T*,
        Exchange,
        ( T* volatile & rAtomic, T* value ),
        {
            return static_cast< T* >( InterlockedExchangePointer(
                reinterpret_cast< void* volatile * >( &rAtomic ),
                value ) );
        } )
    _GENERATE_ATOMIC_WORKER(
        template< typename T > T*,
        CompareExchange,
        ( T* volatile & rAtomic, T* value, T* compare ),
        {
            return static_cast< T* >( InterlockedCompareExchangePointer(
                reinterpret_cast< void* volatile * >( &rAtomic ),
                value,
                compare ) );
        } )
}

#undef _GENERATE_ATOMIC_WORKER
