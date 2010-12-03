//----------------------------------------------------------------------------------------------------------------------
// Type.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Lunar
{
    /// Perform any actions immediately prior to incrementing an object's strong reference count.
    ///
    /// @param[in] pObject  Object whose reference count is being updated.
    ///
    /// @see PreRemoveStrongRef(), PreAddWeakRef(), PreRemoveWeakRef()
    void TypeRefCountSupport::PreAddStrongRef( Type* /*pObject*/ )
    {
    }

    /// Perform any actions immediately prior to decrementing an object's strong reference count.
    ///
    /// @param[in] pObject  Object whose reference count is being updated.
    ///
    /// @see PreRemoveStrongRef(), PreAddWeakRef(), PreRemoveWeakRef()
    void TypeRefCountSupport::PreRemoveStrongRef( Type* /*pObject*/ )
    {
    }

    /// Perform any actions immediately prior to incrementing an object's weak reference count.
    ///
    /// @param[in] pObject  Object whose reference count is being updated.
    ///
    /// @see PreRemoveWeakRef(), PreAddStrongRef(), PreRemoveStrongRef()
    void TypeRefCountSupport::PreAddWeakRef( Type* /*pObject*/ )
    {
    }

    /// Perform any actions immediately prior to decrementing an object's weak reference count.
    ///
    /// @param[in] pObject  Object whose reference count is being updated.
    ///
    /// @see PreRemoveWeakRef(), PreAddStrongRef(), PreRemoveStrongRef()
    void TypeRefCountSupport::PreRemoveWeakRef( Type* /*pObject*/ )
    {
    }

    /// Perform any pre-destruction work before clearing the last strong reference to an object and destroying the
    /// object.
    ///
    /// @param[in] pObject  Object about to be destroyed.
    ///
    /// @see Destroy()
    void TypeRefCountSupport::PreDestroy( Type* /*pObject*/ )
    {
    }

    /// Destroy an object after the final strong reference to it has been cleared.
    ///
    /// @param[in] pObject  Object to destroy.
    ///
    /// @see PreDestroy()
    void TypeRefCountSupport::Destroy( Type* pObject )
    {
        HELIUM_ASSERT( pObject );

        delete pObject;
    }

    /// Get the name of this type.
    ///
    /// @return  Type name.
    Name Type::GetName() const
    {
        return m_name;
    }

    /// Get the parent of this type.
    ///
    /// @return  Parent type, or null if there is no parent type (should only be the case with the "GameObject" type).
    Type* Type::GetTypeParent() const
    {
        return m_spTypeParent;
    }

    /// Get the default template object for this type.
    ///
    /// @return  Type template object.
    GameObject* Type::GetTypeTemplate() const
    {
        return m_spTypeTemplate;
    }

    /// Get the type flags for this type.
    ///
    /// @return  Type flags.
    uint32_t Type::GetTypeFlags() const
    {
        return m_typeFlags;
    }

    /// Get the package in which all template object packages are stored.
    ///
    /// @return  Main type package.
    ///
    /// @see SetTypePackage()
    Package* Type::GetTypePackage()
    {
        return sm_spTypePackage;
    }

    /// Constructor.
    ///
    /// Creates an uninitialized iterator.  Using this is not safe until it is initialized.
    Type::ConstIterator::ConstIterator()
    {
    }

    /// Constructor.
    ///
    /// @param[in] iterator  Type map iterator from which to initialize this iterator.
    Type::ConstIterator::ConstIterator( LookupMap::ConstIterator iterator )
        : m_iterator( iterator )
    {
    }

    /// Get the type referenced by this iterator.
    ///
    /// @return  Reference to the referenced type.
    Type& Type::ConstIterator::operator*() const
    {
        Type* pType = m_iterator->Second();
        HELIUM_ASSERT( pType );

        return *pType;
    }

    /// Get the type referenced by this iterator.
    ///
    /// @return  Pointer to the referenced type.
    Type* Type::ConstIterator::operator->() const
    {
        Type* pType = m_iterator->Second();
        HELIUM_ASSERT( pType );

        return pType;
    }

    /// Advance this iterator to the next type.
    ///
    /// @return  Reference to this iterator.
    Type::ConstIterator& Type::ConstIterator::operator++()
    {
        ++m_iterator;

        return *this;
    }

    /// Advance this iterator to the next type.
    ///
    /// @return  Copy of this iterator prior to advancing.
    Type::ConstIterator Type::ConstIterator::operator++( int )
    {
        ConstIterator result = *this;
        ++m_iterator;

        return result;
    }

    /// Move this iterator back to the previous type.
    ///
    /// @return  Reference to this iterator.
    Type::ConstIterator& Type::ConstIterator::operator--()
    {
        --m_iterator;

        return *this;
    }

    /// Move this iterator back to the previous type.
    ///
    /// @return  Copy of this iterator prior to decrementing.
    Type::ConstIterator Type::ConstIterator::operator--( int )
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
    bool Type::ConstIterator::operator==( const ConstIterator& rOther ) const
    {
        return ( m_iterator == rOther.m_iterator );
    }

    /// Get whether this iterator is not referencing the same type entry as the given iterator.
    ///
    /// @param[in] rOther  Iterator with which to compare.
    ///
    /// @return  True if this iterator and the given iterator do not match, false if they do match.
    bool Type::ConstIterator::operator!=( const ConstIterator& rOther ) const
    {
        return ( m_iterator != rOther.m_iterator );
    }

    /// Get whether this iterator is referencing a type entry prior to the given iterator.
    ///
    /// @param[in] rOther  Iterator with which to compare.
    ///
    /// @return  True if this iterator is referencing a type entry prior to the given iterator, false if not.
    bool Type::ConstIterator::operator<( const ConstIterator& rOther ) const
    {
        return ( m_iterator < rOther.m_iterator );
    }

    /// Get whether this iterator is referencing a type entry after the given iterator.
    ///
    /// @param[in] rOther  Iterator with which to compare.
    ///
    /// @return  True if this iterator is referencing a type entry after the given iterator, false if not.
    bool Type::ConstIterator::operator>( const ConstIterator& rOther ) const
    {
        return ( m_iterator > rOther.m_iterator );
    }

    /// Get whether this iterator is referencing a type entry prior to or the same as the given iterator.
    ///
    /// @param[in] rOther  Iterator with which to compare.
    ///
    /// @return  True if this iterator is referencing the same or a prior type entry, false if not.
    bool Type::ConstIterator::operator<=( const ConstIterator& rOther ) const
    {
        return ( m_iterator <= rOther.m_iterator );
    }

    /// Get whether this iterator is referencing a type entry after or the same as the given iterator.
    ///
    /// @param[in] rOther  Iterator with which to compare.
    ///
    /// @return  True if this iterator is referencing the same or a later type entry, false if not.
    bool Type::ConstIterator::operator>=( const ConstIterator& rOther ) const
    {
        return ( m_iterator >= rOther.m_iterator );
    }
}
