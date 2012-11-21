#pragma once

#include "Foundation/API.h"
#include "Foundation/DynamicArray.h"
#include "Foundation/Pair.h"

namespace Helium
{
    /// Simple table backed by a dynamic array (not thread-safe).
    template< typename Value, typename Key, typename ExtractKey, typename EqualKey = Equals< Key >, typename Allocator = DefaultAllocator, typename InternalValue = Value >
    class Table
    {
    public:
        /// Type for table element values.
        typedef Value ValueType;
        /// Type for table element keys.
        typedef Key KeyType;

        /// Internal value type (type used for actual value storage).
        typedef InternalValue InternalValueType;

        /// Type for testing two keys for equality.
        typedef EqualKey KeyEqualType;
        /// Allocator type.
        typedef Allocator AllocatorType;

        /// Iterator type.
        typedef ArrayIterator< Value > Iterator;
        /// Constant iterator type.
        typedef ConstArrayIterator< Value > ConstIterator;

        /// @name Construction/Destruction
        //@{
        Table();
        Table( const Table& rSource );
        template< typename OtherAllocator > Table(
            const Table< Value, Key, ExtractKey, EqualKey, OtherAllocator, InternalValue >& rSource );
        //@}

        /// @name Map Operations
        //@{
        size_t GetSize() const;
        bool IsEmpty() const;

        size_t GetCapacity() const;
        void Reserve( size_t capacity );
        void Trim();

        void Clear();

        Iterator Begin();
        ConstIterator Begin() const;
        Iterator End();
        ConstIterator End() const;

        Iterator Find( const Key& rKey );
        ConstIterator Find( const Key& rKey ) const;

        Pair< Iterator, bool > Insert( const ValueType& rValue );
        bool Insert( ConstIterator& rIterator, const ValueType& rValue );

        bool Remove( const Key& rKey );
        void Remove( Iterator iterator );
        void Remove( Iterator start, Iterator end );

        void Swap( Table& rTable );
        //@}

        /// @name Overloaded Operators
        //@{
        Table& operator=( const Table& rSource );
        template< typename OtherAllocator > Table& operator=(
            const Table< Value, Key, ExtractKey, EqualKey, OtherAllocator, InternalValue >& rSource );

        bool operator==( const Table& rOther ) const;
        template< typename OtherAllocator > bool operator==(
            const Table< Value, Key, ExtractKey, EqualKey, OtherAllocator, InternalValue >& rOther ) const;
        bool operator!=( const Table& rOther ) const;
        template< typename OtherAllocator > bool operator!=(
            const Table< Value, Key, ExtractKey, EqualKey, OtherAllocator, InternalValue >& rOther ) const;
        //@}

    private:
        /// Internal array of table elements.
        DynamicArray< InternalValue, Allocator > m_elements;
    };
}

#include "Foundation/Table.inl"
