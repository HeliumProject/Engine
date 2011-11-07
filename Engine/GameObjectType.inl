//----------------------------------------------------------------------------------------------------------------------
// GameObjectType.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

/// Get the name of this type.
///
/// @return  Type name.
Helium::Name Helium::GameObjectType::GetName() const
{
    return m_name;
}

/// Get the name of this type.
///
/// @return  Type name.
const Helium::Reflect::Class* Helium::GameObjectType::GetClass() const
{
    return m_class;
}

/// Get the parent of this type, cast to a GameObjectType.
///
/// @return  Base type, or null if the parent is not a GameObjectType type (should only be the case with the
///          "GameObject" type itself).
const Helium::GameObjectType* Helium::GameObjectType::GetBaseType() const
{
    const Reflect::Composite* pBase = m_class->m_Base;
    HELIUM_ASSERT( pBase );
    return static_cast< const GameObjectType* >( pBase->m_Tag );
}

/// Get the default template object for this type.
///
/// @return  Type template object.
Helium::GameObject* Helium::GameObjectType::GetTemplate() const
{
    return const_cast< GameObject* >( static_cast< const GameObject* >( m_class->m_Default.Get() ) );
}

/// Get the flags associated with this type.
///
/// @return  Type flags.
uint32_t Helium::GameObjectType::GetFlags() const
{
    return m_flags;
}

/// Get the package in which all template object packages are stored.
///
/// @return  Main type package.
///
/// @see SetTypePackage()
Helium::Package* Helium::GameObjectType::GetTypePackage()
{
    return sm_spTypePackage;
}

/// Constructor.
///
/// Creates an uninitialized iterator.  Using this is not safe until it is initialized.
Helium::GameObjectType::ConstIterator::ConstIterator()
{
}

/// Constructor.
///
/// @param[in] iterator  Type map iterator from which to initialize this iterator.
Helium::GameObjectType::ConstIterator::ConstIterator( LookupMap::ConstIterator iterator )
    : m_iterator( iterator )
{
}

/// Get the type referenced by this iterator.
///
/// @return  Reference to the referenced type.
const Helium::GameObjectType& Helium::GameObjectType::ConstIterator::operator*() const
{
    GameObjectType* pType = m_iterator->Second();
    HELIUM_ASSERT( pType );

    return *pType;
}

/// Get the type referenced by this iterator.
///
/// @return  Pointer to the referenced type.
const Helium::GameObjectType* Helium::GameObjectType::ConstIterator::operator->() const
{
    GameObjectType* pType = m_iterator->Second();
    HELIUM_ASSERT( pType );

    return pType;
}

/// Advance this iterator to the next type.
///
/// @return  Reference to this iterator.
Helium::GameObjectType::ConstIterator& Helium::GameObjectType::ConstIterator::operator++()
{
    ++m_iterator;

    return *this;
}

/// Advance this iterator to the next type.
///
/// @return  Copy of this iterator prior to advancing.
Helium::GameObjectType::ConstIterator Helium::GameObjectType::ConstIterator::operator++( int )
{
    ConstIterator result = *this;
    ++m_iterator;

    return result;
}

/// Move this iterator back to the previous type.
///
/// @return  Reference to this iterator.
Helium::GameObjectType::ConstIterator& Helium::GameObjectType::ConstIterator::operator--()
{
    --m_iterator;

    return *this;
}

/// Move this iterator back to the previous type.
///
/// @return  Copy of this iterator prior to decrementing.
Helium::GameObjectType::ConstIterator Helium::GameObjectType::ConstIterator::operator--( int )
{
    ConstIterator result = *this;
    --m_iterator;

    return result;
}

/// Get whether this iterator is referencing the same type entry as the given iterator.
///
/// @param[in] rOther  Iterator with which to compare.
///
/// @return  True if this iterator and the given iterator match, false if not.
bool Helium::GameObjectType::ConstIterator::operator==( const ConstIterator& rOther ) const
{
    return ( m_iterator == rOther.m_iterator );
}

/// Get whether this iterator is not referencing the same type entry as the given iterator.
///
/// @param[in] rOther  Iterator with which to compare.
///
/// @return  True if this iterator and the given iterator do not match, false if they do match.
bool Helium::GameObjectType::ConstIterator::operator!=( const ConstIterator& rOther ) const
{
    return ( m_iterator != rOther.m_iterator );
}

/// Get whether this iterator is referencing a type entry prior to the given iterator.
///
/// @param[in] rOther  Iterator with which to compare.
///
/// @return  True if this iterator is referencing a type entry prior to the given iterator, false if not.
bool Helium::GameObjectType::ConstIterator::operator<( const ConstIterator& rOther ) const
{
    return ( m_iterator < rOther.m_iterator );
}

/// Get whether this iterator is referencing a type entry after the given iterator.
///
/// @param[in] rOther  Iterator with which to compare.
///
/// @return  True if this iterator is referencing a type entry after the given iterator, false if not.
bool Helium::GameObjectType::ConstIterator::operator>( const ConstIterator& rOther ) const
{
    return ( m_iterator > rOther.m_iterator );
}

/// Get whether this iterator is referencing a type entry prior to or the same as the given iterator.
///
/// @param[in] rOther  Iterator with which to compare.
///
/// @return  True if this iterator is referencing the same or a prior type entry, false if not.
bool Helium::GameObjectType::ConstIterator::operator<=( const ConstIterator& rOther ) const
{
    return ( m_iterator <= rOther.m_iterator );
}

/// Get whether this iterator is referencing a type entry after or the same as the given iterator.
///
/// @param[in] rOther  Iterator with which to compare.
///
/// @return  True if this iterator is referencing the same or a later type entry, false if not.
bool Helium::GameObjectType::ConstIterator::operator>=( const ConstIterator& rOther ) const
{
    return ( m_iterator >= rOther.m_iterator );
}
