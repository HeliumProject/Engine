//----------------------------------------------------------------------------------------------------------------------
// Type.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Lunar
{
    /// Get the parent of this type.
    ///
    /// @return  Parent type, or null if there is no parent type (should only be the case with the "Object" type).
    Type* Type::GetTypeParent() const
    {
        return m_spTypeParent;
    }

    /// Get the default template object for this type.
    ///
    /// @return  Type template object.
    Object* Type::GetTypeTemplate() const
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

    /// Get the package in which all Type object packages are stored.
    ///
    /// @return  Main type package.
    ///
    /// @see SetTypePackage()
    Package* Type::GetTypePackage()
    {
        return sm_spTypePackage;
    }

    /// Get whether this iterator is referencing a registered type.
    ///
    /// @return  True if this iterator is referencing a type, false if it is referencing nothing.
    ///
    /// @see Release()
    bool Type::ConstIterator::IsValid() const
    {
        return m_accessor.IsValid();
    }

    /// Release access to the registered type map and invalidate this iterator.
    ///
    /// @see IsValid()
    void Type::ConstIterator::Release()
    {
        m_accessor.Release();
    }

    /// Get the type referenced by this iterator.
    ///
    /// @return  Reference to the referenced type.
    Type& Type::ConstIterator::operator*() const
    {
        Type* pType = m_accessor->second;
        HELIUM_ASSERT( pType );

        return *pType;
    }

    /// Get the type referenced by this iterator.
    ///
    /// @return  Pointer to the referenced type.
    Type* Type::ConstIterator::operator->() const
    {
        Type* pType = m_accessor->second;
        HELIUM_ASSERT( pType );

        return pType;
    }

    /// Advance this iterator to the next type.
    ///
    /// @return  Reference to this iterator.
    Type::ConstIterator& Type::ConstIterator::operator++()
    {
        ++m_accessor;

        return *this;
    }

    /// Move this iterator back to the previous type.
    ///
    /// @return  Reference to this iterator.
    Type::ConstIterator& Type::ConstIterator::operator--()
    {
        --m_accessor;

        return *this;
    }

    /// Get whether this iterator is referencing the same type entry as the given iterator.
    ///
    /// @return  True if this iterator and the given iterator match, false if not.
    bool Type::ConstIterator::operator==( const ConstIterator& rOther ) const
    {
        return ( m_accessor == rOther.m_accessor );
    }

    /// Get whether this iterator is not referencing the same type entry as the given iterator.
    ///
    /// @return  True if this iterator and the given iterator do not match, false if they do match.
    bool Type::ConstIterator::operator!=( const ConstIterator& rOther ) const
    {
        return ( m_accessor != rOther.m_accessor );
    }
}
