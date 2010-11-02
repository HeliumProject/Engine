//----------------------------------------------------------------------------------------------------------------------
// ArrayIterator.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_CORE_ARRAY_ITERATOR_H
#define LUNAR_CORE_ARRAY_ITERATOR_H

#include "Core/Core.h"

namespace Lunar
{
    /// Constant general-purpose array iterator.
    template< typename T >
    class ConstArrayIterator
    {
    public:
        /// Type for iterator values.
        typedef T ValueType;

        /// Type for pointers to iterated elements.
        typedef T* PointerType;
        /// Type for references to iterated elements.
        typedef T& ReferenceType;
        /// Type for constant pointers to iterated elements.
        typedef const T* ConstPointerType;
        /// Type for constant references to iterated elements.
        typedef const T& ConstReferenceType;

        /// @name Construction/Destruction
        //@{
        ConstArrayIterator();
        explicit ConstArrayIterator( const T* pElement );
        //@}

        /// @name Overloaded Operators
        //@{
        ConstReferenceType operator*() const;
        ConstPointerType operator->() const;

        ConstArrayIterator& operator++();
        ConstArrayIterator operator++( int );
        ConstArrayIterator& operator--();
        ConstArrayIterator operator--( int );

        ConstArrayIterator& operator+=( ptrdiff_t offset );
        ConstArrayIterator operator+( ptrdiff_t offset ) const;
        ConstArrayIterator& operator-=( ptrdiff_t offset );
        ConstArrayIterator operator-( ptrdiff_t offset ) const;

        ptrdiff_t operator-( const ConstArrayIterator& rOther ) const;

        bool operator==( const ConstArrayIterator& rOther ) const;
        bool operator!=( const ConstArrayIterator& rOther ) const;
        bool operator<( const ConstArrayIterator& rOther ) const;
        bool operator>( const ConstArrayIterator& rOther ) const;
        bool operator<=( const ConstArrayIterator& rOther ) const;
        bool operator>=( const ConstArrayIterator& rOther ) const;
        //@}

    protected:
        /// Current array element.
        PointerType m_pElement;
    };

    /// Non-constant general-purpose array iterator.
    template< typename T >
    class ArrayIterator : public ConstArrayIterator< T >
    {
    public:
        /// Type for iterator values.
        typedef T ValueType;

        /// Type for pointers to iterated elements.
        typedef T* PointerType;
        /// Type for references to iterated elements.
        typedef T& ReferenceType;
        /// Type for constant pointers to iterated elements.
        typedef const T* ConstPointerType;
        /// Type for constant references to iterated elements.
        typedef const T& ConstReferenceType;

        /// @name Construction/Destruction
        //@{
        ArrayIterator();
        explicit ArrayIterator( T* pElement );
        //@}

        /// @name Overloaded Operators
        //@{
        ReferenceType operator*() const;
        PointerType operator->() const;

        ArrayIterator& operator++();
        ArrayIterator operator++( int );
        ArrayIterator& operator--();
        ArrayIterator operator--( int );

        ArrayIterator& operator+=( ptrdiff_t offset );
        ArrayIterator operator+( ptrdiff_t offset ) const;
        ArrayIterator& operator-=( ptrdiff_t offset );
        ArrayIterator operator-( ptrdiff_t offset ) const;
        //@}
    };
}

#include "Core/ArrayIterator.inl"

#endif  // LUNAR_CORE_ARRAY_ITERATOR_H
