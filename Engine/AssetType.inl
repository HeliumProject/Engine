//----------------------------------------------------------------------------------------------------------------------
// AssetType.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

/// Get the name of this type.
///
/// @return  Type name.
Helium::Name Helium::AssetType::GetName() const
{
    return m_name;
}

/// Get the name of this type.
///
/// @return  Type name.
const Helium::Reflect::Class* Helium::AssetType::GetClass() const
{
    return m_class;
}

/// Get the parent of this type, cast to a AssetType.
///
/// @return  Base type, or null if the parent is not a AssetType type (should only be the case with the
///          "Asset" type itself).
const Helium::AssetType* Helium::AssetType::GetBaseType() const
{
    const Reflect::Structure* pBase = m_class->m_Base;
    HELIUM_ASSERT( pBase );
    return static_cast< const AssetType* >( pBase->m_Tag );
}

/// Get the default template object for this type.
///
/// @return  Type template object.
Helium::Asset* Helium::AssetType::GetTemplate() const
{
    return const_cast< Asset* >( static_cast< const Asset* >( m_class->m_Default.Get() ) );
}

/// Get the flags associated with this type.
///
/// @return  Type flags.
uint32_t Helium::AssetType::GetFlags() const
{
    return m_flags;
}

/// Get the package in which all template object packages are stored.
///
/// @return  Main type package.
///
/// @see SetTypePackage()
Helium::Package* Helium::AssetType::GetTypePackage()
{
    return sm_spTypePackage;
}

/// Constructor.
///
/// Creates an uninitialized iterator.  Using this is not safe until it is initialized.
Helium::AssetType::ConstIterator::ConstIterator()
{
}

/// Constructor.
///
/// @param[in] iterator  Type map iterator from which to initialize this iterator.
Helium::AssetType::ConstIterator::ConstIterator( LookupMap::ConstIterator iterator )
    : m_iterator( iterator )
{
}

/// Get the type referenced by this iterator.
///
/// @return  Reference to the referenced type.
const Helium::AssetType& Helium::AssetType::ConstIterator::operator*() const
{
    AssetType* pType = m_iterator->Second();
    HELIUM_ASSERT( pType );

    return *pType;
}

/// Get the type referenced by this iterator.
///
/// @return  Pointer to the referenced type.
const Helium::AssetType* Helium::AssetType::ConstIterator::operator->() const
{
    AssetType* pType = m_iterator->Second();
    HELIUM_ASSERT( pType );

    return pType;
}

/// Advance this iterator to the next type.
///
/// @return  Reference to this iterator.
Helium::AssetType::ConstIterator& Helium::AssetType::ConstIterator::operator++()
{
    ++m_iterator;

    return *this;
}

/// Advance this iterator to the next type.
///
/// @return  Copy of this iterator prior to advancing.
Helium::AssetType::ConstIterator Helium::AssetType::ConstIterator::operator++( int )
{
    ConstIterator result = *this;
    ++m_iterator;

    return result;
}

/// Move this iterator back to the previous type.
///
/// @return  Reference to this iterator.
Helium::AssetType::ConstIterator& Helium::AssetType::ConstIterator::operator--()
{
    --m_iterator;

    return *this;
}

/// Move this iterator back to the previous type.
///
/// @return  Copy of this iterator prior to decrementing.
Helium::AssetType::ConstIterator Helium::AssetType::ConstIterator::operator--( int )
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
bool Helium::AssetType::ConstIterator::operator==( const ConstIterator& rOther ) const
{
    return ( m_iterator == rOther.m_iterator );
}

/// Get whether this iterator is not referencing the same type entry as the given iterator.
///
/// @param[in] rOther  Iterator with which to compare.
///
/// @return  True if this iterator and the given iterator do not match, false if they do match.
bool Helium::AssetType::ConstIterator::operator!=( const ConstIterator& rOther ) const
{
    return ( m_iterator != rOther.m_iterator );
}

/// Get whether this iterator is referencing a type entry prior to the given iterator.
///
/// @param[in] rOther  Iterator with which to compare.
///
/// @return  True if this iterator is referencing a type entry prior to the given iterator, false if not.
bool Helium::AssetType::ConstIterator::operator<( const ConstIterator& rOther ) const
{
    return ( m_iterator < rOther.m_iterator );
}

/// Get whether this iterator is referencing a type entry after the given iterator.
///
/// @param[in] rOther  Iterator with which to compare.
///
/// @return  True if this iterator is referencing a type entry after the given iterator, false if not.
bool Helium::AssetType::ConstIterator::operator>( const ConstIterator& rOther ) const
{
    return ( m_iterator > rOther.m_iterator );
}

/// Get whether this iterator is referencing a type entry prior to or the same as the given iterator.
///
/// @param[in] rOther  Iterator with which to compare.
///
/// @return  True if this iterator is referencing the same or a prior type entry, false if not.
bool Helium::AssetType::ConstIterator::operator<=( const ConstIterator& rOther ) const
{
    return ( m_iterator <= rOther.m_iterator );
}

/// Get whether this iterator is referencing a type entry after or the same as the given iterator.
///
/// @param[in] rOther  Iterator with which to compare.
///
/// @return  True if this iterator is referencing the same or a later type entry, false if not.
bool Helium::AssetType::ConstIterator::operator>=( const ConstIterator& rOther ) const
{
    return ( m_iterator >= rOther.m_iterator );
}
