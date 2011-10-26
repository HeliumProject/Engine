template< class ClassT, class BaseT >
Helium::Reflect::ClassRegistrar< ClassT, BaseT >::ClassRegistrar(const tchar_t* name)
    : TypeRegistrar( name )
{
    HELIUM_ASSERT( ClassT::s_Class == NULL );
    TypeRegistrar::AddToList( RegistrarTypes::Class, this );
}

template< class ClassT, class BaseT >
Helium::Reflect::ClassRegistrar< ClassT, BaseT >::~ClassRegistrar()
{
    Unregister();
    TypeRegistrar::RemoveFromList( RegistrarTypes::Class, this );
}

template< class ClassT, class BaseT >
void Helium::Reflect::ClassRegistrar< ClassT, BaseT >::Register()
{
    if ( ClassT::s_Class == NULL )
    {
        BaseT::s_Registrar.Register();
        AddTypeToRegistry( ClassT::CreateClass() );
    }
}

template< class ClassT, class BaseT >
void Helium::Reflect::ClassRegistrar< ClassT, BaseT >::Unregister()
{
    if ( ClassT::s_Class != NULL )
    {
        RemoveTypeFromRegistry( ClassT::s_Class );
    }
}

template< class ClassT >
Helium::Reflect::ClassRegistrar< ClassT, void >::ClassRegistrar(const tchar_t* name)
    : TypeRegistrar( name )
{
    HELIUM_ASSERT( ClassT::s_Class == NULL );
    TypeRegistrar::AddToList( RegistrarTypes::Class, this );
}

template< class ClassT >
Helium::Reflect::ClassRegistrar< ClassT, void >::~ClassRegistrar()
{
    Unregister();
    TypeRegistrar::RemoveFromList( RegistrarTypes::Class, this );
}

template< class ClassT >
void Helium::Reflect::ClassRegistrar< ClassT, void >::Register()
{
    if ( ClassT::s_Class == NULL )
    {
        AddTypeToRegistry( ClassT::CreateClass() );
    }
}

template< class ClassT >
void Helium::Reflect::ClassRegistrar< ClassT, void >::Unregister()
{
    if ( ClassT::s_Class != NULL )
    {
        RemoveTypeFromRegistry( ClassT::s_Class );
    }
}