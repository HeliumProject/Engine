/// Perform any pre-destruction work before clearing the last strong reference to an object and destroying the
/// object.
///
/// @param[in] pObject  Object about to be destroyed.
///
/// @see Destroy()
void Helium::Reflect::ObjectRefCountSupport::PreDestroy( Object* pObject )
{
    HELIUM_ASSERT( pObject );

    pObject->PreDestroy();
}

/// Destroy an object after the final strong reference to it has been cleared.
///
/// @param[in] pObject  Object to destroy.
///
/// @see PreDestroy()
void Helium::Reflect::ObjectRefCountSupport::Destroy( Object* pObject )
{
    HELIUM_ASSERT( pObject );

    pObject->Destroy();
}

//
// DangerousCast does not type checking
//

template<class DerivedT>
inline DerivedT* Helium::Reflect::DangerousCast(Reflect::Object* base)
{
    return static_cast<DerivedT*>(base);
}

template<class DerivedT>
inline const DerivedT* Helium::Reflect::DangerousCast(const Reflect::Object* base)
{
    return static_cast<const DerivedT*>(base);
}

//
// AssertCast type checks in debug and asserts if failure, does no type checking in release
//

template<class DerivedT>
inline DerivedT* Helium::Reflect::AssertCast( Reflect::Object* base )
{
    if ( base != NULL )
    {
        HELIUM_ASSERT( base->IsClass( GetClass<DerivedT>() ) );
    }

    return DangerousCast<DerivedT>( base );
}

template<class DerivedT>
inline const DerivedT* Helium::Reflect::AssertCast(const Reflect::Object* base)
{
    if ( base != NULL )
    {
        HELIUM_ASSERT( base->IsClass( GetClass<DerivedT>() ) );
    }

    return DangerousCast<DerivedT>( base );
}

//
// TryCast type checks and throws if failure
//

template<class DerivedT>
inline DerivedT* Helium::Reflect::TryCast(Reflect::Object* base)
{
    if ( base != NULL && !base->IsClass( GetClass<DerivedT>() ) )
    {
        throw CastException ( TXT( "Object of type '%s' cannot be cast to type '%s'" ), base->GetClass()->m_Name, GetClass<DerivedT>()->m_Name );
    }

    return DangerousCast<DerivedT>( base );
}

template<class DerivedT>
inline const DerivedT* Helium::Reflect::TryCast(const Reflect::Object* base)
{
    if ( base != NULL && !base->IsClass( GetClass<DerivedT>() ) )
    {
        throw CastException ( TXT( "Object of type '%s' cannot be cast to type '%s'" ), base->GetClass()->m_Name, GetClass<DerivedT>()->m_Name );
    }

    return DangerousCast<DerivedT>( base );
}

//
// ObjectCast always type checks and returns null if failure
//

template<class DerivedT>
inline DerivedT* Helium::Reflect::ObjectCast(Reflect::Object* base)
{
    if ( base != NULL && base->IsClass( GetClass<DerivedT>() ) )
    {
        return DangerousCast<DerivedT>( base );
    }
    else
    {
        return NULL;
    }
}

template<class DerivedT>
inline const DerivedT* Helium::Reflect::ObjectCast(const Reflect::Object* base)
{
    if ( base != NULL && base->IsClass( GetClass<DerivedT>() ) )
    {
        return DangerousCast<DerivedT>( base );
    }
    else
    {
        return NULL;
    }
}
