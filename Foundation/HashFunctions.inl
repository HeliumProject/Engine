/// Default hash function.
///
/// @param[in] rKey  Key for which to compute a hash value.
///
/// @return  Hash value.
template< typename T >
size_t Helium::Hash< T >::operator()( const T& rKey ) const
{
    return static_cast< size_t >( rKey );
}

/// Default pointer hash.
///
/// @param[in] pKey  Key for which to compute a hash value.
///
/// @return  Hash value.
template< typename T >
size_t Helium::Hash< T* >::operator()( const T* pKey ) const
{
    return reinterpret_cast< uintptr_t >( pKey );
}

/// Default pointer hash.
///
/// @param[in] pKey  Key for which to compute a hash value.
///
/// @return  Hash value.
template< typename T >
size_t Helium::Hash< const T* >::operator()( const T* pKey ) const
{
    return reinterpret_cast< uintptr_t >( pKey );
}

#if HELIUM_WORDSIZE == 32
/// Default 64-bit signed integer hash function.
///
/// @param[in] key  Key for which to compute a hash value.
///
/// @return  Hash value.
size_t Helium::Hash< int64_t >::operator()( int64_t key ) const
{
    return static_cast< size_t >( key ) ^ static_cast< size_t >( key >> 32 );
}

/// Default 64-bit unsigned integer hash function.
///
/// @param[in] key  Key for which to compute a hash value.
///
/// @return  Hash value.
size_t Helium::Hash< uint64_t >::operator()( uint64_t key ) const
{
    return static_cast< size_t >( key ) ^ static_cast< size_t >( key >> 32 );
}
#endif

/// Default 32-bit floating-point value hash function.
///
/// @param[in] key  Key for which to compute a hash value.
///
/// @return  Hash value.
size_t Helium::Hash< float32_t >::operator()( float32_t key ) const
{
    union
    {
        float32_t key;
        uint32_t hash;
    } pun;

    pun.key = key;

    return static_cast< size_t >( pun.hash );
}

/// Default 64-bit floating-point value hash function.
///
/// @param[in] key  Key for which to compute a hash value.
///
/// @return  Hash value.
size_t Helium::Hash< float64_t >::operator()( float64_t key ) const
{
    union
    {
        float64_t key;
        uint64_t hash;
    } pun;

    pun.key = key;

#if HELIUM_WORDSIZE == 32
    return static_cast< size_t >( pun.hash ) ^ static_cast< size_t >( pun.hash >> 32 );
#else
    return static_cast< size_t >( pun.hash );
#endif
}
