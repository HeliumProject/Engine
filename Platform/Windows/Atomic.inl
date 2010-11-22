// Windows (non-Itanium platforms) doesn't reorder reads and writes, so memory barriers aren't needed.  As such, all
// variants of each Atomic*() function are the same.  On the downside, we can't easily debug these functions when using
// macros to generate them, and build times may be somewhat slower.
#define _GENERATE_ATOMIC_WORKER( PREFIX, OPERATION, PARAM_LIST, ACTION ) \
    PREFIX Helium::Atomic##OPERATION PARAM_LIST ACTION \
    PREFIX Helium::Atomic##OPERATION##Acquire PARAM_LIST ACTION \
    PREFIX Helium::Atomic##OPERATION##Release PARAM_LIST ACTION \
    PREFIX Helium::Atomic##OPERATION##Unsafe PARAM_LIST ACTION

_GENERATE_ATOMIC_WORKER(
    int32_t,
    Exchange,
    ( int32_t volatile & rAtomic, int32_t value ),
    {
        return _InterlockedExchange( reinterpret_cast< volatile long* >( &rAtomic ), value );
    } )
_GENERATE_ATOMIC_WORKER(
    int32_t,
    CompareExchange,
    ( int32_t volatile & rAtomic, int32_t value, int32_t compare ),
    {
        return _InterlockedCompareExchange( reinterpret_cast< volatile long* >( &rAtomic ), value, compare );
    } )
_GENERATE_ATOMIC_WORKER(
    int32_t,
    Increment,
    ( int32_t volatile & rAtomic ),
    {
        return _InterlockedIncrement( reinterpret_cast< volatile long* >( &rAtomic ) );
    } )
_GENERATE_ATOMIC_WORKER(
    int32_t,
    Decrement,
    ( int32_t volatile & rAtomic ),
    {
        return _InterlockedDecrement( reinterpret_cast< volatile long* >( &rAtomic ) );
    } )
_GENERATE_ATOMIC_WORKER(
    int32_t,
    Add,
    ( int32_t volatile & rAtomic, int32_t value ),
    {
        return _InterlockedExchangeAdd( reinterpret_cast< volatile long* >( &rAtomic ), value );
    } )
_GENERATE_ATOMIC_WORKER(
    int32_t,
    Subtract,
    ( int32_t volatile & rAtomic, int32_t value ),
    {
        return _InterlockedExchangeAdd( reinterpret_cast< volatile long* >( &rAtomic ), -value );
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
            originalValueNew = _InterlockedCompareExchange(
                reinterpret_cast< volatile long* >( &rAtomic ),
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
            originalValueNew = _InterlockedCompareExchange(
                reinterpret_cast< volatile long* >( &rAtomic ),
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
            originalValueNew = _InterlockedCompareExchange(
                reinterpret_cast< volatile long* >( &rAtomic ),
                originalValueOld ^ value,
                originalValueOld );
        } while( originalValueNew != originalValueOld );

        return originalValueNew;
    } )

#if HELIUM_CPU_X86
_GENERATE_ATOMIC_WORKER(
    template< typename T > T*,
    Exchange,
    ( T* volatile & rAtomic, T* value ),
    {
        return reinterpret_cast< T* >( _InterlockedExchange(
            reinterpret_cast< volatile long* >( &rAtomic ),
            reinterpret_cast< long >( value ) ) );
    } )
_GENERATE_ATOMIC_WORKER(
    template< typename T > T*,
    CompareExchange,
    ( T* volatile & rAtomic, T* value, T* compare ),
    {
        return reinterpret_cast< T* >( _InterlockedCompareExchange(
            reinterpret_cast< volatile long* >( &rAtomic ),
            reinterpret_cast< long >( value ),
            reinterpret_cast< long >( compare ) ) );
    } )
#else  // HELIUM_CPU_X86
_GENERATE_ATOMIC_WORKER(
    template< typename T > T*,
    Exchange,
    ( T* volatile & rAtomic, T* value ),
    {
        return static_cast< T* >( _InterlockedExchangePointer(
            reinterpret_cast< void* volatile * >( &rAtomic ),
            value ) );
    } )
_GENERATE_ATOMIC_WORKER(
    template< typename T > T*,
    CompareExchange,
    ( T* volatile & rAtomic, T* value, T* compare ),
    {
        return static_cast< T* >( _InterlockedCompareExchangePointer(
            reinterpret_cast< void* volatile * >( &rAtomic ),
            value,
            compare ) );
    } )
#endif  // HELIUM_CPU_X86

#undef _GENERATE_ATOMIC_WORKER
