/// Default "equals" function.
///
/// @param[in] rA  First value to compare.
/// @param[in] rB  Second value to compare.
///
/// @return  True if the two values are equal to each other, false if not.
template< typename T >
bool Helium::Equals< T >::operator()( const T& rA, const T& rB ) const
{
    return( rA == rB );
}

/// Default "less-than" function.
///
/// @param[in] rA  First value to compare.
/// @param[in] rB  Second value to compare.
///
/// @return  True if the first value is less than the second, false if not.
template< typename T >
bool Helium::Less< T >::operator()( const T& rA, const T& rB ) const
{
    return( rA < rB );
}
