//----------------------------------------------------------------------------------------------------------------------
// Atomic.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_CORE_ATOMIC_H
#define LUNAR_CORE_ATOMIC_H

#include "Core/Core.h"

namespace Lunar
{
    /// @defgroup atomic Atomic Variable Support
    ///
    /// All supported platforms provide support for 32-bit integer types and pointer types (either 32-bit or 64-bit).
    /// Note that most operations come in four different variants: standard, with "acquire" access semantics, with
    /// "release" access semantics, and with "unsafe" access semantics.
    /// - Standard variants (those not suffixed with "Acquire", "Release", or "Unsafe") operate with full memory
    ///   barriers before and after the atomic operation is performed.  This guarantees all memory reads and writes
    ///   performed before the atomic update are made visible on all threads before the atomic update is made visible,
    ///   and all memory reads and writes performed after the atomic operation is performed are made visible on all
    ///   threads after the atomic update is made visible.  While this is the safest to use on platforms where memory
    ///   reads and writes can be reordered, it is also the slowest.
    /// - "Acquire" variants (those suffixed with "Acquire") operate with a memory barrier placed after the atomic
    ///   operation is performed.  This guarantees that all memory reads and writes performed after the atomic update
    ///   are made visible on all threads after the atomic update itself is made visible.
    /// - "Release" variants (those suffixed with "Release") operate with a memory barrier placed before the atomic
    ///   operation is performed.  This guarantees that all memory reads and writes performed before the atomic update
    ///   are made visible on all threads before the atomic update itself is made visible.
    /// - "Unsafe" variants (those suffixed with "Unsafe") do not guarantee any memory barrier placement, and as such
    ///   can be reordered.  This yields the best performance, although it forces the user to be aware of any necessary
    ///   placement of memory barriers in their code to avoid any unwanted side-effects.
    //@{

    /// Atomically swap the current value of a 32-bit integer with another value, with full memory barriers.
    ///
    /// @param[in] rAtomic  Integer to update.
    /// @param[in] value    Value to swap.
    ///
    /// @return  Original integer value upon swapping.
    inline int32_t AtomicExchange( int32_t volatile & rAtomic, int32_t value );

    /// Atomically swap the current value of a 32-bit integer with another value, with acquire semantics.
    ///
    /// @param[in] rAtomic  Integer to update.
    /// @param[in] value    Value to swap.
    ///
    /// @return  Original integer value upon swapping.
    inline int32_t AtomicExchangeAcquire( int32_t volatile & rAtomic, int32_t value );

    /// Atomically swap the current value of a 32-bit integer with another value, with release semantics.
    ///
    /// @param[in] rAtomic  Integer to update.
    /// @param[in] value    Value to swap.
    ///
    /// @return  Original integer value upon swapping.
    inline int32_t AtomicExchangeRelease( int32_t volatile & rAtomic, int32_t value );

    /// Atomically swap the current value of a 32-bit integer with another value, without any memory barriers.
    ///
    /// @param[in] rAtomic  Integer to update.
    /// @param[in] value    Value to swap.
    ///
    /// @return  Original integer value upon swapping.
    inline int32_t AtomicExchangeUnsafe( int32_t volatile & rAtomic, int32_t value );

    /// Atomically compare the current value of a 32-bit integer with another value, swapping in a different value if
    /// the values match, with full memory barriers.
    ///
    /// @param[in] rAtomic  Integer to update.
    /// @param[in] value    Value to swap.
    /// @param[in] compare  Value against which to compare.
    ///
    /// @return  Original integer value when the comparison occurred.  If this is the same as @c compare, the integer's
    ///          value will have been updated to match @c value, otherwise it will have been left unchanged.
    inline int32_t AtomicCompareExchange( int32_t volatile & rAtomic, int32_t value, int32_t compare );

    /// Atomically compare the current value of a 32-bit integer with another value, swapping in a different value if
    /// the values match, with acquire semantics.
    ///
    /// @param[in] rAtomic  Integer to update.
    /// @param[in] value    Value to swap.
    /// @param[in] compare  Value against which to compare.
    ///
    /// @return  Original integer value when the comparison occurred.  If this is the same as @c compare, the integer's
    ///          value will have been updated to match @c value, otherwise it will have been left unchanged.
    inline int32_t AtomicCompareExchangeAcquire( int32_t volatile & rAtomic, int32_t value, int32_t compare );

    /// Atomically compare the current value of a 32-bit integer with another value, swapping in a different value if
    /// the values match, with release semantics.
    ///
    /// @param[in] rAtomic  Integer to update.
    /// @param[in] value    Value to swap.
    /// @param[in] compare  Value against which to compare.
    ///
    /// @return  Original integer value when the comparison occurred.  If this is the same as @c compare, the integer's
    ///          value will have been updated to match @c value, otherwise it will have been left unchanged.
    inline int32_t AtomicCompareExchangeRelease( int32_t volatile & rAtomic, int32_t value, int32_t compare );

    /// Atomically compare the current value of a 32-bit integer with another value, swapping in a different value if
    /// the values match, without any memory barriers.
    ///
    /// @param[in] rAtomic  Integer to update.
    /// @param[in] value    Value to swap.
    /// @param[in] compare  Value against which to compare.
    ///
    /// @return  Original integer value when the comparison occurred.  If this is the same as @c compare, the integer's
    ///          value will have been updated to match @c value, otherwise it will have been left unchanged.
    inline int32_t AtomicCompareExchangeUnsafe( int32_t volatile & rAtomic, int32_t value, int32_t compare );

    /// Atomically increment a 32-bit integer, with full memory barriers.
    ///
    /// @param[in] rAtomic  Integer to update.
    ///
    /// @return  New value after incrementing.
    inline int32_t AtomicIncrement( int32_t volatile & rAtomic );

    /// Atomically increment a 32-bit integer, with acquire semantics.
    ///
    /// @param[in] rAtomic  Integer to update.
    ///
    /// @return  New value after incrementing.
    inline int32_t AtomicIncrementAcquire( int32_t volatile & rAtomic );

    /// Atomically increment a 32-bit integer, with release semantics.
    ///
    /// @param[in] rAtomic  Integer to update.
    ///
    /// @return  New value after incrementing.
    inline int32_t AtomicIncrementRelease( int32_t volatile & rAtomic );

    /// Atomically increment a 32-bit integer, without any memory barriers.
    ///
    /// @param[in] rAtomic  Integer to update.
    ///
    /// @return  New value after incrementing.
    inline int32_t AtomicIncrementUnsafe( int32_t volatile & rAtomic );

    /// Atomically decrement a 32-bit integer, with full memory barriers.
    ///
    /// @param[in] rAtomic  Integer to update.
    ///
    /// @return  New value after decrementing.
    inline int32_t AtomicDecrement( int32_t volatile & rAtomic );

    /// Atomically decrement a 32-bit integer, with acquire semantics.
    ///
    /// @param[in] rAtomic  Integer to update.
    ///
    /// @return  New value after decrementing.
    inline int32_t AtomicDecrementAcquire( int32_t volatile & rAtomic );

    /// Atomically decrement a 32-bit integer, with release semantics.
    ///
    /// @param[in] rAtomic  Integer to update.
    ///
    /// @return  New value after decrementing.
    inline int32_t AtomicDecrementRelease( int32_t volatile & rAtomic );

    /// Atomically decrement a 32-bit integer, without any memory barriers.
    ///
    /// @param[in] rAtomic  Integer to update.
    ///
    /// @return  New value after decrementing.
    inline int32_t AtomicDecrementUnsafe( int32_t volatile & rAtomic );

    /// Atomically add a value to a 32-bit integer, with full memory barriers.
    ///
    /// @param[in] rAtomic  Integer to update.
    /// @param[in] value    Value to add.
    ///
    /// @return  Original integer value prior to updating.
    inline int32_t AtomicAdd( int32_t volatile & rAtomic, int32_t value );

    /// Atomically add a value to a 32-bit integer, with acquire semantics.
    ///
    /// @param[in] rAtomic  Integer to update.
    /// @param[in] value    Value to add.
    ///
    /// @return  Original integer value prior to updating.
    inline int32_t AtomicAddAcquire( int32_t volatile & rAtomic, int32_t value );

    /// Atomically add a value to a 32-bit integer, with release semantics.
    ///
    /// @param[in] rAtomic  Integer to update.
    /// @param[in] value    Value to add.
    ///
    /// @return  Original integer value prior to updating.
    inline int32_t AtomicAddRelease( int32_t volatile & rAtomic, int32_t value );

    /// Atomically add a value to a 32-bit integer, without any memory barriers.
    ///
    /// @param[in] rAtomic  Integer to update.
    /// @param[in] value    Value to add.
    ///
    /// @return  Original integer value prior to updating.
    inline int32_t AtomicAddUnsafe( int32_t volatile & rAtomic, int32_t value );

    /// Atomically subtract a value from a 32-bit integer, with full memory barriers.
    ///
    /// @param[in] rAtomic  Integer to update.
    /// @param[in] value    Value to subtract.
    ///
    /// @return  Original integer value prior to updating.
    inline int32_t AtomicSubtract( int32_t volatile & rAtomic, int32_t value );

    /// Atomically subtract a value from a 32-bit integer, with acquire semantics.
    ///
    /// @param[in] rAtomic  Integer to update.
    /// @param[in] value    Value to subtract.
    ///
    /// @return  Original integer value prior to updating.
    inline int32_t AtomicSubtractAcquire( int32_t volatile & rAtomic, int32_t value );

    /// Atomically subtract a value from a 32-bit integer, with release semantics.
    ///
    /// @param[in] rAtomic  Integer to update.
    /// @param[in] value    Value to subtract.
    ///
    /// @return  Original integer value prior to updating.
    inline int32_t AtomicSubtractRelease( int32_t volatile & rAtomic, int32_t value );

    /// Atomically subtract a value from a 32-bit integer, without any memory barriers.
    ///
    /// @param[in] rAtomic  Integer to update.
    /// @param[in] value    Value to subtract.
    ///
    /// @return  Original integer value prior to updating.
    inline int32_t AtomicSubtractUnsafe( int32_t volatile & rAtomic, int32_t value );

    /// Atomically AND a 32-bit integer with another value, with full memory barriers.
    ///
    /// @param[in] rAtomic  Integer to update.
    /// @param[in] value    Value with which to AND.
    ///
    /// @return  Original integer value prior to updating.
    inline int32_t AtomicAnd( int32_t volatile & rAtomic, int32_t value );

    /// Atomically AND a 32-bit integer with another value, with acquire semantics.
    ///
    /// @param[in] rAtomic  Integer to update.
    /// @param[in] value    Value with which to AND.
    ///
    /// @return  Original integer value prior to updating.
    inline int32_t AtomicAndAcquire( int32_t volatile & rAtomic, int32_t value );

    /// Atomically AND a 32-bit integer with another value, with release semantics.
    ///
    /// @param[in] rAtomic  Integer to update.
    /// @param[in] value    Value with which to AND.
    ///
    /// @return  Original integer value prior to updating.
    inline int32_t AtomicAndRelease( int32_t volatile & rAtomic, int32_t value );

    /// Atomically AND a 32-bit integer with another value, without any memory barriers.
    ///
    /// @param[in] rAtomic  Integer to update.
    /// @param[in] value    Value with which to AND.
    ///
    /// @return  Original integer value prior to updating.
    inline int32_t AtomicAndUnsafe( int32_t volatile & rAtomic, int32_t value );

    /// Atomically OR a 32-bit integer with another value, with full memory barriers.
    ///
    /// @param[in] rAtomic  Integer to update.
    /// @param[in] value    Value with which to OR.
    ///
    /// @return  Original integer value prior to updating.
    inline int32_t AtomicOr( int32_t volatile & rAtomic, int32_t value );

    /// Atomically OR a 32-bit integer with another value, with acquire semantics.
    ///
    /// @param[in] rAtomic  Integer to update.
    /// @param[in] value    Value with which to OR.
    ///
    /// @return  Original integer value prior to updating.
    inline int32_t AtomicOrAcquire( int32_t volatile & rAtomic, int32_t value );

    /// Atomically OR a 32-bit integer with another value, with release semantics.
    ///
    /// @param[in] rAtomic  Integer to update.
    /// @param[in] value    Value with which to OR.
    ///
    /// @return  Original integer value prior to updating.
    inline int32_t AtomicOrRelease( int32_t volatile & rAtomic, int32_t value );

    /// Atomically OR a 32-bit integer with another value, without any memory barriers.
    ///
    /// @param[in] rAtomic  Integer to update.
    /// @param[in] value    Value with which to OR.
    ///
    /// @return  Original integer value prior to updating.
    inline int32_t AtomicOrUnsafe( int32_t volatile & rAtomic, int32_t value );

    /// Atomically XOR a 32-bit integer with another value, with full memory barriers.
    ///
    /// @param[in] rAtomic  Integer to update.
    /// @param[in] value    Value with which to XOR.
    ///
    /// @return  Original integer value prior to updating.
    inline int32_t AtomicXor( int32_t volatile & rAtomic, int32_t value );

    /// Atomically XOR a 32-bit integer with another value, with acquire semantics.
    ///
    /// @param[in] rAtomic  Integer to update.
    /// @param[in] value    Value with which to XOR.
    ///
    /// @return  Original integer value prior to updating.
    inline int32_t AtomicXorAcquire( int32_t volatile & rAtomic, int32_t value );

    /// Atomically XOR a 32-bit integer with another value, with release semantics.
    ///
    /// @param[in] rAtomic  Integer to update.
    /// @param[in] value    Value with which to XOR.
    ///
    /// @return  Original integer value prior to updating.
    inline int32_t AtomicXorRelease( int32_t volatile & rAtomic, int32_t value );

    /// Atomically XOR a 32-bit integer with another value, without any memory barriers.
    ///
    /// @param[in] rAtomic  Integer to update.
    /// @param[in] value    Value with which to XOR.
    ///
    /// @return  Original integer value prior to updating.
    inline int32_t AtomicXorUnsafe( int32_t volatile & rAtomic, int32_t value );

    /// Atomically swap the current value of pointer with another value, with full memory barriers.
    ///
    /// @param[in] rAtomic  Pointer to update.
    /// @param[in] value    Value to swap.
    ///
    /// @return  Original pointer value upon swapping.
    template< typename T >
    T* AtomicExchange( T* volatile & rAtomic, T* value );

    /// Atomically swap the current value of a pointer with another value, with acquire semantics.
    ///
    /// @param[in] rAtomic  Pointer to update.
    /// @param[in] value    Value to swap.
    ///
    /// @return  Original pointer value upon swapping.
    template< typename T >
    T* AtomicExchangeAcquire( T* volatile & rAtomic, T* value );

    /// Atomically swap the current value of a pointer with another value, with release semantics.
    ///
    /// @param[in] rAtomic  Pointer to update.
    /// @param[in] value    Value to swap.
    ///
    /// @return  Original pointer value upon swapping.
    template< typename T >
    T* AtomicExchangeRelease( T* volatile & rAtomic, T* value );

    /// Atomically swap the current value of a pointer with another value, without any memory barriers.
    ///
    /// @param[in] rAtomic  Pointer to update.
    /// @param[in] value    Value to swap.
    ///
    /// @return  Original pointer value upon swapping.
    template< typename T >
    T* AtomicExchangeUnsafe( T* volatile & rAtomic, T* value );

    /// Atomically compare the current value of a pointer with another value, swapping in a different value if
    /// the values match, with full memory barriers.
    ///
    /// @param[in] rAtomic  Pointer to update.
    /// @param[in] value    Value to swap.
    /// @param[in] compare  Value against which to compare.
    ///
    /// @return  Original pointer value when the comparison occurred.  If this is the same as @c compare, the pointer's
    ///          value will have been updated to match @c value, otherwise it will have been left unchanged.
    template< typename T >
    T* AtomicCompareExchange( T* volatile & rAtomic, T* value, T* compare );

    /// Atomically compare the current value of a pointer with another value, swapping in a different value if
    /// the values match, with acquire semantics.
    ///
    /// @param[in] rAtomic  Pointer to update.
    /// @param[in] value    Value to swap.
    /// @param[in] compare  Value against which to compare.
    ///
    /// @return  Original pointer value when the comparison occurred.  If this is the same as @c compare, the pointer's
    ///          value will have been updated to match @c value, otherwise it will have been left unchanged.
    template< typename T >
    T* AtomicCompareExchangeAcquire( T* volatile & rAtomic, T* value, T* compare );

    /// Atomically compare the current value of a pointer with another value, swapping in a different value if
    /// the values match, with release semantics.
    ///
    /// @param[in] rAtomic  Pointer to update.
    /// @param[in] value    Value to swap.
    /// @param[in] compare  Value against which to compare.
    ///
    /// @return  Original pointer value when the comparison occurred.  If this is the same as @c compare, the pointer's
    ///          value will have been updated to match @c value, otherwise it will have been left unchanged.
    template< typename T >
    T* AtomicCompareExchangeRelease( T* volatile & rAtomic, T* value, T* compare );

    /// Atomically compare the current value of a pointer with another value, swapping in a different value if
    /// the values match, without any memory barriers.
    ///
    /// @param[in] rAtomic  Pointer to update.
    /// @param[in] value    Value to swap.
    /// @param[in] compare  Value against which to compare.
    ///
    /// @return  Original pointer value when the comparison occurred.  If this is the same as @c compare, the pointer's
    ///          value will have been updated to match @c value, otherwise it will have been left unchanged.
    template< typename T >
    T* AtomicCompareExchangeUnsafe( T* volatile & rAtomic, T* value, T* compare );

    //@}
}

#if L_OS_WIN
#include "AtomicWin.inl"
#endif

#endif  // LUNAR_CORE_ATOMIC_H
