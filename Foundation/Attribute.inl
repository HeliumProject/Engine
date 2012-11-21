template< class T >
Helium::Attribute< T >::ChangeArgs::ChangeArgs( const T& oldValue, const T& newValue )
    : m_OldValue( oldValue )
    , m_NewValue( newValue )
{

}

template< class T >
Helium::Attribute< T >::ChangingArgs::ChangingArgs( const T& oldValue, const T& newValue )
    : ChangeArgs( oldValue, newValue )
    , m_Veto( false )
{

}

template< class T >
Helium::Attribute< T >::Attribute()
    : m_Value ()
{

}

template< class T >
Helium::Attribute< T >::Attribute( const T& value )
    : m_Value ( value )
{

}

template< class T >
T& Helium::Attribute< T >::Value()
{
    return m_Value;
}

template< class T >
const T& Helium::Attribute< T >::Get() const
{
    return m_Value;
}

template< class T >
bool Helium::Attribute< T >::Set(const T& value)
{
    if ( m_Value != value )
    {
        ChangingArgs args ( m_Value, value );
        m_Changing.Raise( args );
        if ( !args.m_Veto )
        {
            T previous = m_Value;
            m_Value = value;
            m_Changed.Raise( ChangeArgs( previous, m_Value ) );
            return true;
        }
    }

    return false;
}

template< class T >
typename Helium::Attribute< T >::ChangingSignature::Event& Helium::Attribute< T >::Changing()
{
    return m_Changing;
}

template< class T >
typename Helium::Attribute< T >::ChangedSignature::Event& Helium::Attribute< T >::Changed()
{
    return m_Changed;
}

template< class T >
void Helium::Attribute< T >::RaiseChanged( const T& previous = T() )
{
    m_Changed.Raise( ChangeArgs( previous, m_Value ) );
}