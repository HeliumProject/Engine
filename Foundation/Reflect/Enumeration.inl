template< class EnumerationT >
Helium::Reflect::EnumerationRegistrar< EnumerationT >::EnumerationRegistrar(const tchar_t* name)
    : TypeRegistrar( name )
{
    HELIUM_ASSERT( EnumerationT::s_Enumeration == NULL );
    TypeRegistrar::AddToList( RegistrarTypes::Enumeration, this );
}

template< class EnumerationT >
Helium::Reflect::EnumerationRegistrar< EnumerationT >::~EnumerationRegistrar()
{
    Unregister();
    TypeRegistrar::RemoveFromList( RegistrarTypes::Enumeration, this );
}

template< class EnumerationT >
void Helium::Reflect::EnumerationRegistrar< EnumerationT >::Register()
{
    if( EnumerationT::s_Enumeration == NULL )
    {
        AddTypeToRegistry( EnumerationT::CreateEnumeration() );
    }
}

template< class EnumerationT >
void Helium::Reflect::EnumerationRegistrar< EnumerationT >::Unregister()
{
    if( EnumerationT::s_Enumeration != NULL )
    {
        RemoveTypeFromRegistry( EnumerationT::s_Enumeration );
    }
}