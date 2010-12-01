#pragma once

#include "Foundation/API.h"
#include "Foundation/Container/DynArray.h"
#include "Foundation/Container/Pair.h"

namespace Helium
{
    /// Associative array (not thread-safe).
    ///
    /// Map stores elements in a contiguous array, similar to DynArray.  No hashing is performed, so element lookups can
    /// take O(n) time in the worst-case scenario.
    ///
    /// Map elements are not guaranteed to remain sorted.
    template< typename Key, typename Data, typename EqualKey = Equals< Key >, typename Allocator = DefaultAllocator >
    class Map
    {
    public:
        /// Type for map element keys.
        typedef Key KeyType;
        /// Type for map element data.
        typedef Data DataType;
        /// Type for map elements.
        typedef KeyValue< Key, Data > ValueType;

        /// Type for testing two keys for equality.
        typedef EqualKey KeyEqualType;
        /// Allocator type.
        typedef Allocator AllocatorType;

        /// Iterator type.
        typedef ArrayIterator< KeyValue< Key, Data > > Iterator;
        /// Constant iterator type.
        typedef ConstArrayIterator< KeyValue< Key, Data > > ConstIterator;

        /// @name Construction/Destruction
        //@{
        Map();
        Map( const Map& rSource );
        template< typename OtherAllocator > Map( const Map< Key, Data, EqualKey, OtherAllocator >& rSource );
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

        void Swap( Map& rMap );
        //@}

        /// @name Overloaded Operators
        //@{
        Map& operator=( const Map& rSource );
        template< typename OtherAllocator > Map& operator=( const Map< Key, Data, EqualKey, OtherAllocator >& rSource );

        Data& operator[]( const Key& rKey );
        //@}

    private:
        /// Internal array of map elements.
        DynArray< Pair< Key, Data >, Allocator > m_elements;

        /// Key equal comparison functor.
        EqualKey m_keyEquals;
    };
}

#include "Foundation/Container/Map.inl"
