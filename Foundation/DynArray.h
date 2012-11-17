#pragma once

#include "Platform/Utility.h"
#include "Platform/MemoryHeap.h"
#include "Platform/Assert.h"

#include "Foundation/API.h"
#include "Foundation/Math.h"

#include "boost/preprocessor/repetition/enum_params.hpp"
#include "boost/preprocessor/repetition/enum_binary_params.hpp"
#include "boost/preprocessor/repetition/repeat_from_to.hpp"

namespace Helium
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

        ptrdiff_t operator-( const ConstArrayIterator& rOther ) const;
        //@}
    };
	
	/// Resizable array (not thread-safe).
    template< typename T, typename Allocator = DefaultAllocator >
    class DynArray
    {
    public:
        /// Type for array element values.
        typedef T ValueType;

        /// Type for pointers to array elements.
        typedef T* PointerType;
        /// Type for references to array elements.
        typedef T& ReferenceType;
        /// Type for constant pointers to array elements.
        typedef const T* ConstPointerType;
        /// Type for constant references to array elements.
        typedef const T& ConstReferenceType;

        /// Iterator type.
        typedef ArrayIterator< T > Iterator;
        /// Constant iterator type.
        typedef ConstArrayIterator< T > ConstIterator;

        /// @name Construction/Destruction
        //@{
        DynArray();
        DynArray( const T* pSource, size_t size );
        DynArray( const DynArray& rSource );
        template< typename OtherAllocator > DynArray( const DynArray< T, OtherAllocator >& rSource );
        ~DynArray();
        //@}

        /// @name Array Operations
        //@{
        size_t GetSize() const;
        bool IsEmpty() const;
        void Resize( size_t size );

        size_t GetCapacity() const;
        void Reserve( size_t capacity );
        void Trim();

        T* GetData();
        const T* GetData() const;

        void Clear();

        Iterator Begin();
        ConstIterator Begin() const;
        Iterator End();
        ConstIterator End() const;

        T& GetElement( size_t index );
        const T& GetElement( size_t index ) const;

        void Set( const T* pSource, size_t size );

        void Add( const T& rValue, size_t count = 1 );
        void AddArray( const T* pValues, size_t count );
        void Insert( size_t index, const T& rValue, size_t count = 1 );
        void InsertArray( size_t index, const T* pValues, size_t count );
        void Remove( size_t index, size_t count = 1 );
        void RemoveSwap( size_t index, size_t count = 1 );
        void RemoveAll();

        T& GetFirst();
        const T& GetFirst() const;
        T& GetLast();
        const T& GetLast() const;
        size_t Push( const T& rValue );
        void Pop();

        void Swap( DynArray& rArray );

        uint32_t GetIndexOfPointer(const T *_ptr) const;
        //@}

        /// @name In-place Object Creation
        //@{
        T* New();

#define HELIUM_DECLARE_DYNARRAY_NEW_Z( Z, N, DATA ) \
    template< BOOST_PP_ENUM_PARAMS_Z( Z, N, typename Param ) > \
    T* New( BOOST_PP_ENUM_BINARY_PARAMS_Z( Z, N, const Param, &rParam ) );

        BOOST_PP_REPEAT_FROM_TO( 1, 17, HELIUM_DECLARE_DYNARRAY_NEW_Z, )

#undef HELIUM_DECLARE_DYNARRAY_NEW_Z
        //@}

        /// @name Overloaded Operators
        //@{
        DynArray& operator=( const DynArray& rSource );
        template< typename OtherAllocator > DynArray& operator=( const DynArray< T, OtherAllocator >& rSource );

        T& operator[]( ptrdiff_t index );
        const T& operator[]( ptrdiff_t index ) const;

        bool operator==( const DynArray& rOther ) const;
        template< typename OtherAllocator > bool operator==( const DynArray< T, OtherAllocator >& rOther ) const;
        bool operator!=( const DynArray& rOther ) const;
        template< typename OtherAllocator > bool operator!=( const DynArray< T, OtherAllocator >& rOther ) const;
        //@}

    private:
        /// Allocated array buffer.
        T* m_pBuffer;
        /// Used buffer size.
        size_t m_size;
        /// Buffer capacity.
        size_t m_capacity;

        /// @name Private Utility Functions
        //@{
        size_t GetGrowCapacity( size_t desiredCount ) const;
        void Grow( size_t capacity );

        template< typename OtherAllocator > void CopyConstruct( const DynArray< T, OtherAllocator >& rSource );
        void Finalize();

        template< typename OtherAllocator > DynArray& Assign( const DynArray< T, OtherAllocator >& rSource );

        template< typename OtherAllocator > bool Equals( const DynArray< T, OtherAllocator >& rOther ) const;

        T* Allocate( size_t count );
        T* Allocate( size_t count, const std::true_type& rNeedsAlignment );
        T* Allocate( size_t count, const std::false_type& rNeedsAlignment );

        T* Reallocate( T* pMemory, size_t count );
        T* Reallocate( T* pMemory, size_t count, const std::true_type& rNeedsAlignment );
        T* Reallocate( T* pMemory, size_t count, const std::false_type& rNeedsAlignment );

        T* ResizeBuffer( T* pMemory, size_t elementCount, size_t oldCapacity, size_t newCapacity );
        T* ResizeBuffer(
            T* pMemory, size_t elementCount, size_t oldCapacity, size_t newCapacity,
            const std::true_type& rHasTrivialCopyAndDestructor );
        T* ResizeBuffer(
            T* pMemory, size_t elementCount, size_t oldCapacity, size_t newCapacity,
            const std::false_type& rHasTrivialCopyAndDestructor );
        //@}
    };
}

#include "Foundation/DynArray.inl"
