/// Constructor.
template< typename T1, typename T2 >
Helium::KeyValue< T1, T2 >::KeyValue()
{
}

/// Constructor.
///
/// @param[in] rFirst   Value to which the first pair element should be initialized.
/// @param[in] rSecond  Value to which the second pair element should be initialized.
template< typename T1, typename T2 >
Helium::KeyValue< T1, T2 >::KeyValue( const T1& rFirst, const T2& rSecond )
    : m_first( rFirst )
    , m_second( rSecond )
{
}

/// Get the first element in this pair.
///
/// @return  Constant reference to the first pair element.
///
/// @see Second()
template< typename T1, typename T2 >
const T1& Helium::KeyValue< T1, T2 >::First() const
{
    return m_first;
}

/// Get the second element in this pair.
///
/// @return  Reference to the second pair element.
///
/// @see First()
template< typename T1, typename T2 >
T2& Helium::KeyValue< T1, T2 >::Second()
{
    return m_second;
}

/// Get the second element in this pair.
///
/// @return  Constant reference to the second pair element.
///
/// @see First()
template< typename T1, typename T2 >
const T2& Helium::KeyValue< T1, T2 >::Second() const
{
    return m_second;
}

/// Less-than comparison.
///
/// @return  If the first element in this object is less than the first element in the given object, this will return
///          true.  If the first element in the given object less than the first element in this object, this will
///          return false.  If neither element is less than the other, this will return true if the second element in
///          this object is less than the second element in the given object, and false otherwise.
template< typename T1, typename T2 >
bool Helium::KeyValue< T1, T2 >::operator<( const KeyValue& rOther ) const
{
    if( m_first < rOther.m_first )
    {
        return true;
    }

    if( rOther.m_first < m_first )
    {
        return false;
    }

    return ( m_second < rOther.m_second );
}

/// Equality comparison.
///
/// @return  True if both elements in this object are equal to both elements in the given object, false otherwise.
template< typename T1, typename T2 >
bool Helium::KeyValue< T1, T2 >::operator==( const KeyValue& rOther ) const
{
    return ( m_first == rOther.m_first && m_second == rOther.m_second );
}

/// Constructor.
template< typename T1, typename T2 >
Helium::Pair< T1, T2 >::Pair()
{
}

/// Constructor.
///
/// @param[in] rFirst   Value to which the first pair element should be initialized.
/// @param[in] rSecond  Value to which the second pair element should be initialized.
template< typename T1, typename T2 >
Helium::Pair< T1, T2 >::Pair( const T1& rFirst, const T2& rSecond )
    : KeyValue( rFirst, rSecond )
{
}

/// Constructor.
///
/// @param[in] rKeyValue  Pair from which to initialize this pair.
template< typename T1, typename T2 >
Helium::Pair< T1, T2 >::Pair( const KeyValue< T1, T2 >& rKeyValue )
    : KeyValue( rKeyValue )
{
}

/// Get the first element in this pair.
///
/// @return  Reference to the first pair element.
///
/// @see Second()
template< typename T1, typename T2 >
T1& Helium::Pair< T1, T2 >::First()
{
    return m_first;
}

/// Assignment operator.
///
/// @param[in] rOther  Pair from which to copy.
///
/// @return  Reference to this object.
template< typename T1, typename T2 >
Helium::Pair< T1, T2 >& Helium::Pair< T1, T2 >::operator=( const Pair< T1, T2 >& rOther )
{
    m_first = rOther.m_first;
    m_second = rOther.m_second;

    return *this;
}

/// Assignment operator.
///
/// @param[in] rOther  Pair from which to copy.
///
/// @return  Reference to this object.
template< typename T1, typename T2 >
Helium::Pair< T1, T2 >& Helium::Pair< T1, T2 >::operator=( const KeyValue< T1, T2 >& rOther )
{
    m_first = rOther.m_first;
    m_second = rOther.m_second;

    return *this;
}

/// Extract the first value from a heterogeneous pair.
///
/// Note that this expects the interface provided by KeyValue and Pair, and is not compatible with std::pair.
///
/// @param[in] rPair  Reference to the value pair.
///
/// @return  Reference to the first value in the pair.
template< typename PairType >
typename PairType::FirstType& Helium::SelectFirst< PairType >::operator()( PairType& rPair ) const
{
    return rPair.First();
}

/// Extract the first value from a heterogeneous pair.
///
/// Note that this expects the interface provided by KeyValue and Pair, and is not compatible with std::pair.
///
/// @param[in] rPair  Constant reference to the value pair.
///
/// @return  Constant reference to the first value in the pair.
template< typename PairType >
const typename PairType::FirstType& Helium::SelectFirst< PairType >::operator()( const PairType& rPair ) const
{
    return rPair.First();
}

/// Extract the second value from a heterogeneous pair.
///
/// Note that this expects the interface provided by KeyValue and Pair, and is not compatible with std::pair.
///
/// @param[in] rPair  Reference to the value pair.
///
/// @return  Reference to the second value in the pair.
template< typename PairType >
typename PairType::SecondType& Helium::SelectSecond< PairType >::operator()( PairType& rPair ) const
{
    return rPair.Second();
}

/// Extract the second value from a heterogeneous pair.
///
/// Note that this expects the interface provided by KeyValue and Pair, and is not compatible with std::pair.
///
/// @param[in] rPair  Constant reference to the value pair.
///
/// @return  Constant reference to the second value in the pair.
template< typename PairType >
const typename PairType::SecondType& Helium::SelectSecond< PairType >::operator()( const PairType& rPair ) const
{
    return rPair.Second();
}

/// Extract the first value from a key-value pair.
///
/// Note that this expects the interface provided by KeyValue and Pair, and is not compatible with std::pair.
///
/// @param[in] rPair  Constant reference to the value pair.
///
/// @return  Constant reference to the first value in the pair.
template< typename PairType >
const typename PairType::FirstType& Helium::SelectKey< PairType >::operator()( const PairType& rPair ) const
{
    return rPair.First();
}
