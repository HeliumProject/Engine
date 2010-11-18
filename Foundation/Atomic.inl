template< typename T >
Helium::AtomicRefCountBase< T >::AtomicRefCountBase()
    : m_RefCount( 0 )
{
}

template< typename T >
Helium::AtomicRefCountBase< T >::AtomicRefCountBase( const AtomicRefCountBase& rSource )
    : m_RefCount( 0 )
{
    // Do not copy the reference count.
}

template< typename T >
int32_t Helium::AtomicRefCountBase< T >::GetRefCount() const
{
    return m_RefCount;
}

template< typename T >
int32_t Helium::AtomicRefCountBase< T >::IncrRefCount() const
{
    int32_t newRefCount = AtomicIncrementUnsafe( m_RefCount );

    // Test for wrapping to zero.
    HELIUM_ASSERT( newRefCount != 0 );

    return newRefCount;
}

template< typename T >
int32_t Helium::AtomicRefCountBase< T >::DecrRefCount() const
{
    int32_t newRefCount = AtomicDecrementUnsafe( m_RefCount );
    if( newRefCount == 0 )
    {
        delete const_cast< T* >( static_cast< const T* >( this ) );
    }

    return newRefCount;
}

template< typename T >
Helium::AtomicRefCountBase< T >& Helium::AtomicRefCountBase< T >::operator=( const AtomicRefCountBase& rSource )
{
    // Do not copy the reference count.
    return *this;
}
