template< class FieldT >
void Helium::Reflect::Object::FieldChanged( FieldT* fieldAddress ) const
{
    // the offset of the field is the address of the field minus the address of this object instance
    uintptr_t fieldOffset = ((uint32_t)fieldAddress - (uint32_t)this);

    // find the field in our reflection information
    const Reflect::Field* field = GetClass()->FindFieldByOffset( fieldOffset );

    // your field address probably doesn't point to the field in this instance,
    //  or your field is not exposed to Reflect, add it in your Composite function
    HELIUM_ASSERT( field );

    // notify listeners that this field changed
    RaiseChanged( field );
}

template< class ObjectT, class FieldT >
void Helium::Reflect::Object::ChangeField( FieldT ObjectT::* field, const FieldT& newValue )
{
    // set the field via pointer-to-member on the deduced templated type (!)
    this->*field = newValue;

    // find the field in our reflection information
    const Reflect::Field* field = GetClass()->FindField( field );

    // your field is not exposed to Reflect, add it in your Composite function
    HELIUM_ASSERT( field );

    // notify listeners that this field changed
    RaiseChanged( field );
}
            
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
// AssertCast type checks in debug and asserts if failure, does no type checking in release
//

template<class DerivedT>
inline DerivedT* Helium::Reflect::AssertCast( Reflect::Object* base )
{
    if ( base != NULL )
    {
        HELIUM_ASSERT( base->IsClass( GetClass<DerivedT>() ) );
    }

    return static_cast< DerivedT* >( base );
}

template<class DerivedT>
inline const DerivedT* Helium::Reflect::AssertCast(const Reflect::Object* base)
{
    if ( base != NULL )
    {
        HELIUM_ASSERT( base->IsClass( GetClass<DerivedT>() ) );
    }

    return static_cast< const DerivedT* >( base );
}

//
// ThrowCast type checks and throws if failure
//

template<class DerivedT>
inline DerivedT* Helium::Reflect::ThrowCast(Reflect::Object* base)
{
    if ( base != NULL && !base->IsClass( GetClass<DerivedT>() ) )
    {
        throw CastException ( TXT( "Object of type '%s' cannot be cast to type '%s'" ), base->GetClass()->m_Name, GetClass<DerivedT>()->m_Name );
    }

    return static_cast< DerivedT* >( base );
}

template<class DerivedT>
inline const DerivedT* Helium::Reflect::ThrowCast(const Reflect::Object* base)
{
    if ( base != NULL && !base->IsClass( GetClass<DerivedT>() ) )
    {
        throw CastException ( TXT( "Object of type '%s' cannot be cast to type '%s'" ), base->GetClass()->m_Name, GetClass<DerivedT>()->m_Name );
    }

    return static_cast< const DerivedT* >( base );
}

//
// SafeCast always type checks and returns null if failure
//

template<class DerivedT>
inline DerivedT* Helium::Reflect::SafeCast(Reflect::Object* base)
{
    if ( base != NULL && base->IsClass( GetClass<DerivedT>() ) )
    {
        return static_cast< DerivedT* >( base );
    }
    else
    {
        return NULL;
    }
}

template<class DerivedT>
inline const DerivedT* Helium::Reflect::SafeCast(const Reflect::Object* base)
{
    if ( base != NULL && base->IsClass( GetClass<DerivedT>() ) )
    {
        return static_cast< const DerivedT* >( base );
    }
    else
    {
        return NULL;
    }
}
