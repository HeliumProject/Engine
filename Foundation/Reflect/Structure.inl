template< class StructureT, class BaseT >
Helium::Reflect::StructureRegistrar< StructureT, BaseT >::StructureRegistrar(const tchar_t* name)
    : TypeRegistrar( name )
{
    HELIUM_ASSERT( StructureT::s_Structure == NULL );
    TypeRegistrar::AddToList( RegistrarTypes::Structure, this );
}

template< class StructureT, class BaseT >
Helium::Reflect::StructureRegistrar< StructureT, BaseT >::~StructureRegistrar()
{
    Unregister();
    TypeRegistrar::RemoveFromList( RegistrarTypes::Structure, this );
}

template< class StructureT, class BaseT >
void Helium::Reflect::StructureRegistrar< StructureT, BaseT >::Register()
{
    if ( StructureT::s_Structure == NULL )
    {
        BaseT::s_Registrar.Register();
        AddTypeToRegistry( StructureT::CreateStructure() );
    }
}

template< class StructureT, class BaseT >
void Helium::Reflect::StructureRegistrar< StructureT, BaseT >::Unregister()
{
    if ( StructureT::s_Structure != NULL )
    {
        RemoveTypeFromRegistry( StructureT::s_Structure );
        StructureT::s_Structure = NULL;
    }
}

template< class StructureT >
Helium::Reflect::StructureRegistrar< StructureT, void >::StructureRegistrar(const tchar_t* name)
    : TypeRegistrar( name )
{
    HELIUM_ASSERT( StructureT::s_Structure == NULL );
    TypeRegistrar::AddToList( RegistrarTypes::Structure, this );
}

template< class StructureT >
Helium::Reflect::StructureRegistrar< StructureT, void >::~StructureRegistrar()
{
    Unregister();
    TypeRegistrar::RemoveFromList( RegistrarTypes::Structure, this );
}

template< class StructureT >
void Helium::Reflect::StructureRegistrar< StructureT, void >::Register()
{
    if ( StructureT::s_Structure == NULL )
    {
        AddTypeToRegistry( StructureT::CreateStructure );
    }
}

template< class StructureT >
void Helium::Reflect::StructureRegistrar< StructureT, void >::Unregister()
{
    if ( StructureT::s_Structure != NULL )
    {
        RemoveTypeFromRegistry( StructureT::s_Structure );
        StructureT::s_Structure = NULL;
    }
}