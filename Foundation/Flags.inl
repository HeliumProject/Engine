template< class T >
bool Helium::HasFlags( const T& bitMap, const T flag )
{
    return ( ( bitMap & flag ) == flag );
}

template< class T >
void Helium::SetFlag( T& bitMap, const T flag, const bool setIt )
{
    if ( setIt )
    {
        bitMap |= flag;
    }
    else // unset
    {
        bitMap &= ~flag;
    }
}