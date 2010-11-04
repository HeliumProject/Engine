//----------------------------------------------------------------------------------------------------------------------
// DynArray.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Lunar
{
    /// Constructor.
    ///
    /// This creates an empty array.  No memory is allocated at this time.
    template< typename T, typename Allocator >
    DynArray< T, Allocator >::DynArray()
        : m_pBuffer( NULL )
        , m_size( 0 )
        , m_capacity( 0 )
    {
    }

    /// Constructor.
    ///
    /// This creates a copy of the given array.
    ///
    /// @param[in] pSource  Array from which to copy.
    /// @param[in] size     Number of elements in the given array.
    template< typename T, typename Allocator >
    DynArray< T, Allocator >::DynArray( const T* pSource, size_t size )
        : m_pBuffer( NULL )
        , m_size( size )
        , m_capacity( size )
    {
        HELIUM_ASSERT( pSource );
        if( size != 0 )
        {
            m_pBuffer = Allocate( m_size );
            HELIUM_ASSERT( m_pBuffer );
            ArrayUninitializedCopy( m_pBuffer, pSource, size );
        }
    }

    /// Copy constructor.
    ///
    /// When copying, only the memory needed to hold onto the used contents of the source array will be allocated (i.e.
    /// if the source array has 10 elements but a capacity of 20, only memory for the 10 used elements will be allocated
    /// for this copy).
    ///
    /// @param[in] rSource  Array from which to copy.
    template< typename T, typename Allocator >
    DynArray< T, Allocator >::DynArray( const DynArray& rSource )
    {
        CopyConstruct( rSource );
    }

    /// Copy constructor.
    ///
    /// When copying, only the memory needed to hold onto the used contents of the source array will be allocated (i.e.
    /// if the source array has 10 elements but a capacity of 20, only memory for the 10 used elements will be allocated
    /// for this copy).
    ///
    /// @param[in] rSource  Array from which to copy.
    template< typename T, typename Allocator >
    template< typename OtherAllocator >
    DynArray< T, Allocator >::DynArray( const DynArray< T, OtherAllocator >& rSource )
    {
        CopyConstruct( rSource );
    }

    /// Destructor.
    template< typename T, typename Allocator >
    DynArray< T, Allocator >::~DynArray()
    {
        Finalize();
    }

    /// Get the number of elements in this array.
    ///
    /// @return  Number of elements in this array.
    ///
    /// @see GetCapacity(), Resize(), IsEmpty()
    template< typename T, typename Allocator >
    size_t DynArray< T, Allocator >::GetSize() const
    {
        return m_size;
    }

    /// Get whether this array is currently empty.
    ///
    /// @return  True if this array is empty, false if not.
    ///
    /// @see GetSize()
    template< typename T, typename Allocator >
    bool DynArray< T, Allocator >::IsEmpty() const
    {
        return( m_size == 0 );
    }

    /// Resize this array, retaining any existing data that fits within the new size.
    ///
    /// If the new size is smaller than the current size, no memory will be freed for the array buffer itself, but the
    /// destructor of elements that no longer fit into the array will be called as appropriate.
    ///
    /// If the new size is larger than the current capacity, the array memory will be reallocated according to the
    /// normal array growth rules.  This can be avoided by calling Reserve() to increase the capacity to an explicit
    /// value prior to calling this function.
    ///
    /// @param[in] size  New array size.
    ///
    /// @see GetSize()
    template< typename T, typename Allocator >
    void DynArray< T, Allocator >::Resize( size_t size )
    {
        if( size != m_size )
        {
            if( size < m_size )
            {
                ArrayInPlaceDestroy( m_pBuffer + size, m_size - size );
            }
            else
            {
                Grow( size );
                ArrayInPlaceConstruct< T >( m_pBuffer + m_size, size - m_size );
            }

            m_size = size;
        }
    }

    /// Get the maximum number of elements which this array can contain without requiring reallocation of memory.
    ///
    /// @return  Current array capacity.
    ///
    /// @see GetSize(), Reserve()
    template< typename T, typename Allocator >
    size_t DynArray< T, Allocator >::GetCapacity() const
    {
        return m_capacity;
    }

    /// Explicitly increase the capacity of this array to support at least the specified number of elements.
    ///
    /// If the requested capacity is less than the current capacity, no memory will be reallocated.
    ///
    /// @param[in] capacity  Desired capacity.
    ///
    /// @see GetCapacity()
    template< typename T, typename Allocator >
    void DynArray< T, Allocator >::Reserve( size_t capacity )
    {
        if( capacity > m_capacity )
        {
            m_pBuffer = ResizeBuffer( m_pBuffer, m_size, m_capacity, capacity );
            HELIUM_ASSERT( m_pBuffer );
            m_capacity = capacity;
        }
    }

    /// Resize the allocated array memory to match the size actually in use.
    ///
    /// @see GetCapacity()
    template< typename T, typename Allocator >
    void DynArray< T, Allocator >::Trim()
    {
        if( m_capacity != m_size )
        {
            m_pBuffer = ResizeBuffer( m_pBuffer, m_size, m_capacity, m_size );
            HELIUM_ASSERT( m_pBuffer || m_size == 0 );
            m_capacity = m_size;
        }
    }

    /// Get a pointer to the base of the allocated array buffer.
    ///
    /// @return  Array buffer.
    template< typename T, typename Allocator >
    T* DynArray< T, Allocator >::GetData()
    {
        return m_pBuffer;
    }

    /// Get a pointer to the base of the allocated array buffer.
    ///
    /// @return  Array buffer.
    template< typename T, typename Allocator >
    const T* DynArray< T, Allocator >::GetData() const
    {
        return m_pBuffer;
    }

    /// Resize the array to zero and free all allocated memory.
    template< typename T, typename Allocator >
    void DynArray< T, Allocator >::Clear()
    {
        ArrayInPlaceDestroy( m_pBuffer, m_size );
        m_allocator.Free( m_pBuffer );
        m_pBuffer = NULL;
        m_size = 0;
        m_capacity = 0;
    }

    /// Retrieve an iterator referencing the beginning of this array.
    ///
    /// @return  Iterator at the beginning of this array.
    ///
    /// @see End()
    template< typename T, typename Allocator >
    typename DynArray< T, Allocator >::Iterator DynArray< T, Allocator >::Begin()
    {
        return Iterator( m_pBuffer );
    }

    /// Retrieve a constant iterator referencing the beginning of this array.
    ///
    /// @return  Constant iterator at the beginning of this array.
    ///
    /// @see End()
    template< typename T, typename Allocator >
    typename DynArray< T, Allocator >::ConstIterator DynArray< T, Allocator >::Begin() const
    {
        return ConstIterator( m_pBuffer );
    }

    /// Retrieve an iterator referencing the end of this array.
    ///
    /// @return  Iterator at the end of this array.
    ///
    /// @see Begin()
    template< typename T, typename Allocator >
    typename DynArray< T, Allocator >::Iterator DynArray< T, Allocator >::End()
    {
        return Iterator( m_pBuffer + m_size );
    }

    /// Retrieve a constant iterator referencing the end of this array.
    ///
    /// @return  Constant iterator at the end of this array.
    ///
    /// @see Begin()
    template< typename T, typename Allocator >
    typename DynArray< T, Allocator >::ConstIterator DynArray< T, Allocator >::End() const
    {
        return ConstIterator( m_pBuffer + m_size );
    }

    /// Get the array element at the specified index.
    ///
    /// @param[in] index  Array index.
    ///
    /// @return  Reference to the element at the specified index.
    template< typename T, typename Allocator >
    T& DynArray< T, Allocator >::GetElement( size_t index )
    {
        HELIUM_ASSERT( index < m_size );
        HELIUM_ASSERT( m_pBuffer );
        return m_pBuffer[ index ];
    }

    /// Get the array element at the specified index.
    ///
    /// @param[in] index  Array index.
    ///
    /// @return  Constant reference to the element at the specified index.
    template< typename T, typename Allocator >
    const T& DynArray< T, Allocator >::GetElement( size_t index ) const
    {
        HELIUM_ASSERT( index < m_size );
        HELIUM_ASSERT( m_pBuffer );
        return m_pBuffer[ index ];
    }

    /// Set this array to a copy of a C-style array.
    ///
    /// Note that this will reallocate the capacity of this array to match the given array size.
    ///
    /// @param[in] pSource  Array from which to copy.
    /// @param[in] size     Number of elements in the given array.
    template< typename T, typename Allocator >
    void DynArray< T, Allocator >::Set( const T* pSource, size_t size )
    {
        HELIUM_ASSERT( pSource );
        ArrayInPlaceDestroy( m_pBuffer, m_size );
        if( size != m_capacity )
        {
            m_pBuffer = Reallocate( m_pBuffer, size );
            HELIUM_ASSERT( m_pBuffer || size == 0 );
            m_capacity = size;
        }

        HELIUM_ASSERT( size == 0 ? m_pBuffer == NULL : m_pBuffer != NULL );

        m_size = size;
        ArrayUninitializedCopy( m_pBuffer, pSource, size );
    }

    /// Add an element to the end of this array.
    ///
    /// @param[in] rValue  Value to add.
    /// @param[in] count   Number of copies of the specified value to add.
    template< typename T, typename Allocator >
    void DynArray< T, Allocator >::Add( const T& rValue, size_t count )
    {
        size_t newSize = m_size + count;
        Grow( newSize );
        ArrayUninitializedFill( m_pBuffer + m_size, rValue, count );
        m_size = newSize;
    }

    /// Add an array of elements to the end of this array.
    ///
    /// @param[in] pValues  Array of values to add.
    /// @param[in] count    Number of elements in the given array.
    template< typename T, typename Allocator >
    void DynArray< T, Allocator >::AddArray( const T* pValues, size_t count )
    {
        HELIUM_ASSERT( pValues || count == 0 );

        size_t newSize = m_size + count;
        Grow( newSize );
        ArrayUninitializedCopy( m_pBuffer + m_size, pValues, count );
        m_size = newSize;
    }

    /// Insert an element to the middle of this array.
    ///
    /// @param[in] index   Index at which to insert values.
    /// @param[in] rValue  Value to insert.
    /// @param[in] count   Number of copies of the specified value to insert.
    template< typename T, typename Allocator >
    void DynArray< T, Allocator >::Insert( size_t index, const T& rValue, size_t count )
    {
        HELIUM_ASSERT( index <= m_size );

        size_t newSize = m_size + count;
        if( newSize > m_capacity )
        {
            size_t newCapacity = GetGrowCapacity( newSize );
            T* pNewBuffer = Allocate( newCapacity );
            HELIUM_ASSERT( pNewBuffer );
            ArrayUninitializedCopy( pNewBuffer, m_pBuffer, index );
            ArrayUninitializedFill( pNewBuffer + index, rValue, count );
            ArrayUninitializedCopy( pNewBuffer + index + count, m_pBuffer + index, m_size - index );

            m_allocator.Free( m_pBuffer );

            m_pBuffer = pNewBuffer;
            m_capacity = newCapacity;
        }
        else
        {
            size_t shiftCount = m_size - index;
            if( shiftCount <= count )
            {
                ArrayUninitializedCopy( m_pBuffer + index + count, m_pBuffer + index, shiftCount );
                ArrayUninitializedFill( m_pBuffer + m_size, rValue, count - shiftCount );

                ArraySet( m_pBuffer + index, rValue, shiftCount );
            }
            else
            {
                ArrayUninitializedCopy( m_pBuffer + m_size, m_pBuffer + m_size - count, count );

                ArrayMove( m_pBuffer + index + count, m_pBuffer + index, shiftCount - count );
                ArraySet( m_pBuffer + index, rValue, count );
            }
        }

        m_size = newSize;
    }

    /// Insert an array into to the middle of this array.
    ///
    /// @param[in] index    Index at which to insert values.
    /// @param[in] pValues  Array of values to insert.
    /// @param[in] count    Number of elements to copy from the array.
    template< typename T, typename Allocator >
    void DynArray< T, Allocator >::InsertArray( size_t index, const T* pValues, size_t count )
    {
        HELIUM_ASSERT( index <= m_size );
        HELIUM_ASSERT( pValues || count == 0 );

        size_t newSize = m_size + count;
        if( newSize > m_capacity )
        {
            size_t newCapacity = GetGrowCapacity( newSize );
            T* pNewBuffer = Allocate( newCapacity );
            HELIUM_ASSERT( pNewBuffer );
            ArrayUninitializedCopy( pNewBuffer, m_pBuffer, index );
            ArrayUninitializedCopy( pNewBuffer + index, pValues, count );
            ArrayUninitializedCopy( pNewBuffer + index + count, m_pBuffer + index, m_size - index );

            m_allocator.Free( m_pBuffer );

            m_pBuffer = pNewBuffer;
            m_capacity = newCapacity;
        }
        else
        {
            size_t shiftCount = m_size - index;
            if( shiftCount <= count )
            {
                ArrayUninitializedCopy( m_pBuffer + index + count, m_pBuffer + index, shiftCount );
                ArrayUninitializedCopy( m_pBuffer + m_size, pValues + shiftCount, count - shiftCount );

                ArrayCopy( m_pBuffer + index, pValues, shiftCount );
            }
            else
            {
                ArrayUninitializedCopy( m_pBuffer + m_size, m_pBuffer + m_size - count, count );

                ArrayMove( m_pBuffer + index + count, m_pBuffer + index, shiftCount - count );
                ArrayCopy( m_pBuffer + index, pValues, count );
            }
        }

        m_size = newSize;
    }

    /// Remove elements from this array.
    ///
    /// This will retain the order of elements in this array, although a degree of overhead is occurred to shift
    /// elements in order to reduce holes.  If order is not a concern, RemoveSwap() can be used instead to reduce the
    /// copy overhead.
    ///
    /// @param[in] index  Index from which to remove elements.
    /// @param[in] count  Number of elements to remove.
    ///
    /// @see RemoveSwap()
    template< typename T, typename Allocator >
    void DynArray< T, Allocator >::Remove( size_t index, size_t count )
    {
        HELIUM_ASSERT( index <= m_size );

        size_t shiftStartIndex = index + count;
        HELIUM_ASSERT( shiftStartIndex <= m_size );

        size_t newSize = m_size - count;

        ArrayMove( m_pBuffer + index, m_pBuffer + shiftStartIndex, m_size - shiftStartIndex );
        ArrayInPlaceDestroy( m_pBuffer + newSize, count );
        m_size = newSize;
    }

    /// Remove elements from this array, swapping in elements from the end of the array in order to reduce the amount of
    /// data copied.
    ///
    /// Note that the order of existing elements in this array is not guaranteed to be retained when using this
    /// function.  If ordering needs to be retained (at the cost of additional overhead from shifting around array
    /// elements in order to remove holes), Remove() should be used instead.
    ///
    /// @param[in] index  Index from which to remove elements.
    /// @param[in] count  Number of elements to remove.
    ///
    /// @see Remove()
    template< typename T, typename Allocator >
    void DynArray< T, Allocator >::RemoveSwap( size_t index, size_t count )
    {
        HELIUM_ASSERT( index <= m_size );

        size_t shiftStartIndex = index + count;
        HELIUM_ASSERT( shiftStartIndex <= m_size );

        size_t newSize = m_size - count;

        size_t trailingCount = m_size - shiftStartIndex;
        if( trailingCount <= count )
        {
            // We're removing more elements from the array than exist past the end of the range being removed, so
            // perform a normal shift and destroy.
            ArrayMove( m_pBuffer + index, m_pBuffer + shiftStartIndex, trailingCount );
        }
        else
        {
            // Swap elements from the end of the array into the empty space.
            ArrayCopy( m_pBuffer + index, m_pBuffer + newSize, count );
        }

        ArrayInPlaceDestroy( m_pBuffer + newSize, count );
        m_size = newSize;
    }

    /// Get the first element in this array.
    ///
    /// @return  Reference to the first element in this array.
    ///
    /// @see GetLast()
    template< typename T, typename Allocator >
    T& DynArray< T, Allocator >::GetFirst()
    {
        HELIUM_ASSERT( m_size != 0 );
        HELIUM_ASSERT( m_pBuffer );

        return m_pBuffer[ 0 ];
    }

    /// Get the first element in this array.
    ///
    /// @return  Constant reference to the first element in this array.
    ///
    /// @see GetLast()
    template< typename T, typename Allocator >
    const T& DynArray< T, Allocator >::GetFirst() const
    {
        HELIUM_ASSERT( m_size != 0 );
        HELIUM_ASSERT( m_pBuffer );

        return m_pBuffer[ 0 ];
    }

    /// Get the last element in this array.
    ///
    /// @return  Reference to the last element in this array.
    ///
    /// @see GetFirst()
    template< typename T, typename Allocator >
    T& DynArray< T, Allocator >::GetLast()
    {
        HELIUM_ASSERT( m_size != 0 );
        HELIUM_ASSERT( m_pBuffer );

        return m_pBuffer[ m_size - 1 ];
    }

    /// Get the last element in this array.
    ///
    /// @return  Constant reference to the last element in this array.
    ///
    /// @see GetFirst()
    template< typename T, typename Allocator >
    const T& DynArray< T, Allocator >::GetLast() const
    {
        HELIUM_ASSERT( m_size != 0 );
        HELIUM_ASSERT( m_pBuffer );

        return m_pBuffer[ m_size - 1 ];
    }

    /// Push an element onto the end of this array.
    ///
    /// @param[in] rValue  Value to push.
    ///
    /// @return  Index of the pushed element.
    ///
    /// @see Pop()
    template< typename T, typename Allocator >
    size_t DynArray< T, Allocator >::Push( const T& rValue )
    {
        size_t index = m_size;
        Add( rValue );

        return index;
    }

    /// Remove the last element from this array.
    ///
    /// @see Push()
    template< typename T, typename Allocator >
    void DynArray< T, Allocator >::Pop()
    {
        HELIUM_ASSERT( m_size != 0 );
        Remove( m_size - 1 );
    }

    /// Swap the contents of this array with another array.
    ///
    /// @param[in] rArray  Array with which to swap.
    template< typename T, typename Allocator >
    void DynArray< T, Allocator >::Swap( DynArray& rArray )
    {
        T* pBuffer = m_pBuffer;
        size_t size = m_size;
        size_t capacity = m_capacity;

        m_pBuffer = rArray.m_pBuffer;
        m_size = rArray.m_size;
        m_capacity = rArray.m_capacity;

        rArray.m_pBuffer = pBuffer;
        rArray.m_size = size;
        rArray.m_capacity = capacity;
    }

    /// Allocate a new object as a new element in this array.
    ///
    /// @return  Pointer to the new object.
    template< typename T, typename Allocator >
    T* DynArray< T, Allocator >::New()
    {
        size_t newSize = m_size + 1;
        Grow( newSize );

        T* pObject = new( m_pBuffer + m_size ) T;
        HELIUM_ASSERT( pObject );

        m_size = newSize;

        return pObject;
    }

#define L_IMPLEMENT_DYNARRAY_NEW_Z( Z, N, DATA ) \
    template< typename T, typename Allocator > \
    template< BOOST_PP_ENUM_PARAMS_Z( Z, N, typename Param ) > \
    T* DynArray< T, Allocator >::New( BOOST_PP_ENUM_BINARY_PARAMS_Z( Z, N, const Param, &rParam ) ) \
    { \
        size_t newSize = m_size + 1; \
        Grow( newSize ); \
        \
        T* pObject = new( m_pBuffer + m_size ) T( BOOST_PP_ENUM_PARAMS_Z( Z, N, rParam ) ); \
        HELIUM_ASSERT( pObject ); \
        \
        m_size = newSize; \
        \
        return pObject; \
    }

    BOOST_PP_REPEAT_FROM_TO( 1, 17, L_IMPLEMENT_DYNARRAY_NEW_Z, )

#undef L_IMPLEMENT_DYNARRAY_NEW_Z

    /// Set this array to the contents of the given array.
    ///
    /// If the given array is not the same as this array, this will always destroy the current contents of this array
    /// and allocate a fresh array whose capacity matches the size of the given array.
    ///
    /// @param[in] rSource  Array from which to copy.
    ///
    /// @return  Reference to this array.
    template< typename T, typename Allocator >
    DynArray< T, Allocator >& DynArray< T, Allocator >::operator=( const DynArray& rSource )
    {
        return Assign( rSource );
    }

    /// Set this array to the contents of the given array.
    ///
    /// If the given array is not the same as this array, this will always destroy the current contents of this array
    /// and allocate a fresh array whose capacity matches the size of the given array.
    ///
    /// @param[in] rSource  Array from which to copy.
    ///
    /// @return  Reference to this array.
    template< typename T, typename Allocator >
    template< typename OtherAllocator >
    DynArray< T, Allocator >& DynArray< T, Allocator >::operator=( const DynArray< T, OtherAllocator >& rSource )
    {
        return Assign( rSource );
    }

    /// Get the array element at the specified index.
    ///
    /// @param[in] index  Array index.
    ///
    /// @return  Reference to the element at the specified index.
    template< typename T, typename Allocator >
    T& DynArray< T, Allocator >::operator[]( ptrdiff_t index )
    {
        HELIUM_ASSERT( static_cast< size_t >( index ) < m_size );
        HELIUM_ASSERT( m_pBuffer );
        return m_pBuffer[ index ];
    }

    /// Get the array element at the specified index.
    ///
    /// @param[in] index  Array index.
    ///
    /// @return  Constant reference to the element at the specified index.
    template< typename T, typename Allocator >
    const T& DynArray< T, Allocator >::operator[]( ptrdiff_t index ) const
    {
        HELIUM_ASSERT( static_cast< size_t >( index ) < m_size );
        HELIUM_ASSERT( m_pBuffer );
        return m_pBuffer[ index ];
    }

    /// Get the capacity to which this array should grow if growing to support the desired number of elements.
    ///
    /// @param[in] desiredCount  Desired minimum capacity.
    ///
    /// @return  Recommended capacity.
    template< typename T, typename Allocator >
    size_t DynArray< T, Allocator >::GetGrowCapacity( size_t desiredCount ) const
    {
        HELIUM_ASSERT( desiredCount > m_capacity );
        return Max< size_t >( desiredCount, m_capacity + m_capacity / 2 + 1 );
    }

    /// Increase the capacity of this array according to the normal growth rules.
    ///
    /// @param[in] capacity  New capacity.
    template< typename T, typename Allocator >
    void DynArray< T, Allocator >::Grow( size_t capacity )
    {
        if( capacity > m_capacity )
        {
            capacity = GetGrowCapacity( capacity );
            m_pBuffer = ResizeBuffer( m_pBuffer, m_size, m_capacity, capacity );
            HELIUM_ASSERT( m_pBuffer );

            m_capacity = capacity;
        }
    }

    /// Allocate and construct a copy of the specified object, assuming all data in this object is uninitialized.
    ///
    /// @param[in] rSource  Object to copy.
    template< typename T, typename Allocator >
    template< typename OtherAllocator >
    void DynArray< T, Allocator >::CopyConstruct( const DynArray< T, OtherAllocator >& rSource )
    {
        m_pBuffer = NULL;

        size_t size = rSource.m_size;
        m_size = size;
        m_capacity = size;

        if( size )
        {
            m_pBuffer = Allocate( size );
            HELIUM_ASSERT( m_pBuffer );
            HELIUM_ASSERT( rSource.m_pBuffer );
            ArrayUninitializedCopy( m_pBuffer, rSource.m_pBuffer, size );
        }
    }

    /// Free all allocated resources, but don't clear out any variables unless necessary.
    template< typename T, typename Allocator >
    void DynArray< T, Allocator >::Finalize()
    {
        ArrayInPlaceDestroy( m_pBuffer, m_size );
        m_allocator.Free( m_pBuffer );
    }

    /// Assignment operator implementation.
    ///
    /// This is separated out to help deal with the fact that the default (shallow-copy) assignment operator is used if
    /// we define just a template assignment operator overload for any allocator type in the source array and not one
    /// that only takes the same exact DynArray type (non-templated) as well.
    ///
    /// @param[in] rSource  Array from which to copy.
    ///
    /// @return  Reference to this array.
    template< typename T, typename Allocator >
    template< typename OtherAllocator >
    DynArray< T, Allocator >& DynArray< T, Allocator >::Assign( const DynArray< T, OtherAllocator >& rSource )
    {
        if( this != &rSource )
        {
            Finalize();
            CopyConstruct( rSource );
        }

        return *this;
    }

    /// Allocate memory for the specified number of elements, accounting for non-standard alignment requirements.
    ///
    /// @param[in] count  Number of elements for which to allocate.
    ///
    /// @return  Pointer to the allocated memory.
    ///
    /// @see Reallocate()
    template< typename T, typename Allocator >
    T* DynArray< T, Allocator >::Allocate( size_t count )
    {
        return Allocate( count, boost::integral_constant< bool, ( boost::alignment_of< T >::value > 8 ) >() );
    }

    /// Allocate() implementation for types requiring a specific alignment.
    ///
    /// @param[in] count            Number of elements for which to allocate.
    /// @param[in] rNeedsAlignment  boost::true_type.
    ///
    /// @return  Pointer to the allocated memory.
    ///
    /// @see Reallocate()
    template< typename T, typename Allocator >
    T* DynArray< T, Allocator >::Allocate( size_t count, const boost::true_type& /*rNeedsAlignment*/ )
    {
        return static_cast< T* >( m_allocator.AllocateAligned( boost::alignment_of< T >::value, sizeof( T ) * count ) );
    }

    /// Allocate() implementation for types that can use the default alignment.
    ///
    /// @param[in] count            Number of elements for which to allocate.
    /// @param[in] rNeedsAlignment  boost::false_type.
    ///
    /// @return  Pointer to the allocated memory.
    ///
    /// @see Reallocate()
    template< typename T, typename Allocator >
    T* DynArray< T, Allocator >::Allocate( size_t count, const boost::false_type& /*rNeedsAlignment*/ )
    {
        return static_cast< T* >( m_allocator.Allocate( sizeof( T ) * count ) );
    }

    /// Reallocate memory for the specified number of elements, accounting for non-standard alignment requirements.
    ///
    /// @param[in] pMemory  Base address of the allocation to reallocate.
    /// @param[in] count    Number of elements for which to reallocate.
    ///
    /// @return  Pointer to the allocated memory.
    ///
    /// @see Allocate()
    template< typename T, typename Allocator >
    T* DynArray< T, Allocator >::Reallocate( T* pMemory, size_t count )
    {
        return Reallocate(
            pMemory,
            count,
            boost::integral_constant< bool, ( boost::alignment_of< T >::value > 8 ) >() );
    }

    /// Reallocate() implementation for types requiring a specific alignment.
    ///
    /// @param[in] pMemory          Base address of the allocation to reallocate.
    /// @param[in] count            Number of elements for which to reallocate.
    /// @param[in] rNeedsAlignment  boost::true_type.
    ///
    /// @return  Pointer to the allocated memory.
    ///
    /// @see Reallocate()
    template< typename T, typename Allocator >
    T* DynArray< T, Allocator >::Reallocate( T* pMemory, size_t count, const boost::true_type& /*rNeedsAlignment*/ )
    {
        size_t existingSize = m_allocator.GetMemorySize( pMemory );
        size_t newSize = sizeof( T ) * count;
        if( existingSize != newSize )
        {
            T* pNewMemory = static_cast< T* >( m_allocator.AllocateAligned(
                boost::alignment_of< T >::value,
                newSize ) );
            HELIUM_ASSERT( pNewMemory || newSize == 0 );
            MemoryCopy( pNewMemory, pMemory, Min( existingSize, newSize ) );
            m_allocator.Free( pMemory );
            pMemory = pNewMemory;
        }

        return pMemory;
    }

    /// Reallocate() implementation for types that can use the default alignment.
    ///
    /// @param[in] pMemory          Base address of the allocation to reallocate.
    /// @param[in] count            Number of elements for which to reallocate.
    /// @param[in] rNeedsAlignment  boost::false_type.
    ///
    /// @return  Pointer to the allocated memory.
    ///
    /// @see Reallocate()
    template< typename T, typename Allocator >
    T* DynArray< T, Allocator >::Reallocate( T* pMemory, size_t count, const boost::false_type& /*rNeedsAlignment*/ )
    {
        return static_cast< T* >( m_allocator.Reallocate( pMemory, sizeof( T ) * count ) );
    }

    /// Resize an array of elements.
    ///
    /// @param[in] pMemory       Base address of the array being resized.
    /// @param[in] elementCount  Number of elements that have actually been constructed in the buffer.
    /// @param[in] oldCapacity   Current array capacity.
    /// @param[in] newCapacity   New array capacity.
    ///
    /// @return  Pointer to the resized array.
    template< typename T, typename Allocator >
    T* DynArray< T, Allocator >::ResizeBuffer( T* pMemory, size_t elementCount, size_t oldCapacity, size_t newCapacity )
    {
        return ResizeBuffer(
            pMemory,
            elementCount,
            oldCapacity,
            newCapacity,
            boost::integral_constant<
                bool, boost::has_trivial_copy< T >::value && boost::has_trivial_destructor< T >::value >() );
    }

    /// ResizeBuffer() implementation for types with both a trivial copy constructor and trivial destructor.
    ///
    /// @param[in] pMemory                       Base address of the array being resized.
    /// @param[in] elementCount                  Number of elements that have actually been constructed in the buffer.
    /// @param[in] oldCapacity                   Current array capacity.
    /// @param[in] newCapacity                   New array capacity.
    /// @param[in] rHasTrivialCopyAndDestructor  boost::true_type.
    ///
    /// @return  Pointer to the resized array.
    template< typename T, typename Allocator >
    T* DynArray< T, Allocator >::ResizeBuffer(
        T* pMemory,
        size_t /*elementCount*/,
        size_t /*oldCapacity*/,
        size_t newCapacity,
        const boost::true_type& /*rHasTrivialCopyAndDestructor*/ )
    {
        return Reallocate( pMemory, newCapacity );
    }

    /// ResizeBuffer() implementation for types without either a trivial copy constructor or a trivial destructor.
    ///
    /// @param[in] pMemory                       Base address of the array being resized.
    /// @param[in] elementCount                  Number of elements that have actually been constructed in the buffer.
    /// @param[in] oldCapacity                   Current array capacity.
    /// @param[in] newCapacity                   New array capacity.
    /// @param[in] rHasTrivialCopyAndDestructor  boost::false_type.
    ///
    /// @return  Pointer to the resized array.
    template< typename T, typename Allocator >
    T* DynArray< T, Allocator >::ResizeBuffer(
        T* pMemory,
        size_t elementCount,
        size_t oldCapacity,
        size_t newCapacity,
        const boost::false_type& /*rHasTrivialCopyAndDestructor*/ )
    {
        HELIUM_ASSERT( elementCount <= oldCapacity );
        HELIUM_ASSERT( elementCount <= newCapacity );

        T* pNewMemory = pMemory;

        if( oldCapacity != newCapacity )
        {
            pNewMemory = NULL;
            if( newCapacity )
            {
                pNewMemory = Allocate( newCapacity );
                HELIUM_ASSERT( pNewMemory );
                ArrayUninitializedCopy( pNewMemory, pMemory, elementCount );
            }

            ArrayInPlaceDestroy( pMemory, elementCount );
            m_allocator.Free( pMemory );
        }

        return pNewMemory;
    }
}
