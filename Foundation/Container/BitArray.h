#pragma once

#include "Foundation/API.h"
#include "Platform/Assert.h"
#include "Platform/Types.h"
#include "Math/Common.h"

namespace Helium
{
    /// Constant bit array element proxy.
    class FOUNDATION_API ConstBitArrayElementProxy
    {
    public:
        /// @name Construction/Destruction
        //@{
        inline ConstBitArrayElementProxy( const uint32_t& rElement, uint32_t mask );
        //@}

        /// @name Overloaded Operators
        //@{
        inline operator bool() const;

        inline bool operator==( bool bValue ) const;
        inline bool operator==( const ConstBitArrayElementProxy& rOther ) const;
        inline bool operator!=( bool bValue ) const;
        inline bool operator!=( const ConstBitArrayElementProxy& rOther ) const;
        //@}

    protected:
        /// Current array element.
        uint32_t& m_rElement;
        /// Current bit mask.
        uint32_t m_mask;

    private:
        /// @name Overloaded Operators, Private
        //@{
        ConstBitArrayElementProxy& operator=( const ConstBitArrayElementProxy& );  // Not implemented.
        //@}
    };

    /// Bit array element proxy.
    class FOUNDATION_API BitArrayElementProxy : public ConstBitArrayElementProxy
    {
    public:
        /// @name Construction/Destruction
        //@{
        inline BitArrayElementProxy( uint32_t& rElement, uint32_t mask );
        //@}

        /// @name Overloaded Operators
        //@{
        inline BitArrayElementProxy& operator=( bool bValue );
        //@}

    private:
        /// @name Overloaded Operators, Private
        //@{
        BitArrayElementProxy& operator=( const BitArrayElementProxy& );  // Not implemented.
        //@}
    };

    /// Constant bit array iterator.
    class FOUNDATION_API ConstBitArrayIterator
    {
    public:
        /// Type for iterator values.
        typedef bool ValueType;

        /// Type for references to iterated elements.
        typedef BitArrayElementProxy ReferenceType;
        /// Type for constant references to iterated elements.
        typedef ConstBitArrayElementProxy ConstReferenceType;

        /// @name Construction/Destruction
        //@{
        inline ConstBitArrayIterator();
        inline ConstBitArrayIterator( const uint32_t* pElement, uint32_t mask );
        //@}

        /// @name Overloaded Operators
        //@{
        inline ConstReferenceType operator*() const;

        inline ConstBitArrayIterator& operator++();
        inline ConstBitArrayIterator operator++( int );
        inline ConstBitArrayIterator& operator--();
        inline ConstBitArrayIterator operator--( int );

        inline ConstBitArrayIterator& operator+=( ptrdiff_t offset );
        inline ConstBitArrayIterator operator+( ptrdiff_t offset ) const;
        inline ConstBitArrayIterator& operator-=( ptrdiff_t offset );
        inline ConstBitArrayIterator operator-( ptrdiff_t offset ) const;

        inline ptrdiff_t operator-( const ConstBitArrayIterator& rOther ) const;

        inline bool operator==( const ConstBitArrayIterator& rOther ) const;
        inline bool operator!=( const ConstBitArrayIterator& rOther ) const;
        inline bool operator<( const ConstBitArrayIterator& rOther ) const;
        inline bool operator>( const ConstBitArrayIterator& rOther ) const;
        inline bool operator<=( const ConstBitArrayIterator& rOther ) const;
        inline bool operator>=( const ConstBitArrayIterator& rOther ) const;
        //@}

    protected:
        /// Current array element.
        uint32_t* m_pElement;
        /// Current bit mask.
        uint32_t m_mask;

        /// @name Utility Functions
        //@{
        inline void UnsignedIncrease( size_t offset );
        inline void UnsignedDecrease( size_t offset );
        //@}
    };

    /// Bit array iterator.
    class FOUNDATION_API BitArrayIterator : public ConstBitArrayIterator
    {
    public:
        /// Type for iterator values.
        typedef bool ValueType;

        /// Type for references to iterated elements.
        typedef BitArrayElementProxy ReferenceType;
        /// Type for constant references to iterated elements.
        typedef ConstBitArrayElementProxy ConstReferenceType;

        /// @name Construction/Destruction
        //@{
        inline BitArrayIterator();
        inline BitArrayIterator( uint32_t* pElement, uint32_t mask );
        //@}

        /// @name Overloaded Operators
        //@{
        inline ReferenceType operator*() const;

        inline BitArrayIterator& operator++();
        inline BitArrayIterator operator++( int );
        inline BitArrayIterator& operator--();
        inline BitArrayIterator operator--( int );

        inline BitArrayIterator& operator+=( ptrdiff_t offset );
        inline BitArrayIterator operator+( ptrdiff_t offset ) const;
        inline BitArrayIterator& operator-=( ptrdiff_t offset );
        inline BitArrayIterator operator-( ptrdiff_t offset ) const;
        //@}
    };

    /// Resizable bit array (not thread-safe).
    template< typename Allocator = DefaultAllocator >
    class BitArray
    {
    public:
        /// Type for array element values.
        typedef bool ValueType;

        /// Type for references to array elements.
        typedef BitArrayElementProxy ReferenceType;
        /// Type for constant references to array elements.
        typedef ConstBitArrayElementProxy ConstReferenceType;

        /// Iterator type.
        typedef BitArrayIterator Iterator;
        /// Constant iterator type.
        typedef ConstBitArrayIterator ConstIterator;

        /// @name Construction/Destruction
        //@{
        BitArray();
        BitArray( const BitArray& rSource );
        ~BitArray();
        //@}

        /// @name Array Operations
        //@{
        size_t GetSize() const;
        bool IsEmpty() const;
        void Resize( size_t size );

        size_t GetCapacity() const;
        void Reserve( size_t capacity );
        void Trim();

        void Clear();

        Iterator Begin();
        ConstIterator Begin() const;
        Iterator End();
        ConstIterator End() const;

        ReferenceType GetElement( size_t index );
        ConstReferenceType GetElement( size_t index ) const;
        void SetElement( size_t index );
        void UnsetElement( size_t index );
        void ToggleElement( size_t index );

        void Add( bool bValue, size_t count = 1 );

        void SetAll( bool bValue = true );
        void UnsetAll();
        void ToggleAll();

        ReferenceType GetFirst();
        ConstReferenceType GetFirst() const;
        ReferenceType GetLast();
        ConstReferenceType GetLast() const;
        size_t Push( bool bValue );
        void Pop();
        //@}

        /// @name Overloaded Operators
        //@{
        BitArray& operator=( const BitArray& rSource );
        template< typename OtherAllocator > BitArray& operator=( const BitArray< OtherAllocator >& rSource );

        ReferenceType operator[]( ptrdiff_t index );
        ConstReferenceType operator[]( ptrdiff_t index ) const;
        //@}

    private:
        /// Bit array buffer.
        uint32_t* m_pBuffer;
        /// Used number of bits.
        size_t m_size;
        /// Array capacity (in bits).
        size_t m_capacity;

        /// @name Private Utility Functions
        //@{
        size_t GetGrowCapacity( size_t desiredCount ) const;
        void Grow( size_t capacity );

        template< typename OtherAllocator > BitArray& Assign( const BitArray< OtherAllocator >& rSource );

        static void Fill( uint32_t* pDest, bool bValue, size_t startIndex, size_t count );
        //@}
    };
}

#include "Foundation/Container/BitArray.inl"
