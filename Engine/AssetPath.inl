//----------------------------------------------------------------------------------------------------------------------
// AssetPath.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Helium
{
    /// Constructor.
    ///
    /// When using the default constructor, the internal object path entry pointer is left in an uninitialized state and
    /// must be manually initialized using Set() or some other assignment before being used.
    AssetPath::AssetPath()
    {
    }

    /// Constructor.
    ///
    /// The object path entry will be initialized to null.
    AssetPath::AssetPath( ENullName )
        : m_pEntry( NULL )
    {
    }
	
    /// Constructor.
    ///
    /// The object path entry will be initialized to null.
    AssetPath::AssetPath( const char *pString )
    {
		Set( pString );
    }

    /// Get the object name (bottom-most component of the path).
    ///
    /// @return  Asset name.
    ///
    /// @see GetInstanceIndex(), IsPackage(), GetParent()
    Name AssetPath::GetName() const
    {
        return ( m_pEntry ? m_pEntry->name : Name( NULL_NAME ) );
    }

    /// Get the instance index associated with the object.
    ///
    /// @return  Instance index.
    ///
    /// @see GetName(), IsPackage(), GetParent()
    uint32_t AssetPath::GetInstanceIndex() const
    {
        return ( m_pEntry ? m_pEntry->instanceIndex : Invalid< uint32_t >() );
    }

    /// Get whether the specific object referenced by this path is a package.
    ///
    /// @return  True if this object is a package, false if not.  This will also return false for empty path names.
    ///
    /// @see GetName(), GetInstanceIndex(), GetParent()
    bool AssetPath::IsPackage() const
    {
        return ( m_pEntry && m_pEntry->bPackage );
    }

    /// Get the parent of this path.
    ///
    /// @return  Parent path, or an empty path if this path has no parent or is empty.
    ///
    /// @see GetName(), GetInstanceIndex(), IsPackage()
    AssetPath AssetPath::GetParent() const
    {
        AssetPath result;
        result.m_pEntry = ( m_pEntry ? m_pEntry->pParent : NULL );

        return result;
    }

    /// Generate the string representation of this object path.
    ///
    /// @return  String representation of this path.
    ///
    /// @see Set()
    String AssetPath::ToString() const
    {
        String result;
        ToString( result );

        return result;
    }

    /// Generate a string representation of this object path with all package and object delimiters converted to valid
    /// directory delimiters for the current platform.
    ///
    /// @return  File path string representation of this path.
    String AssetPath::ToFilePathString() const
    {
        String result;
        ToFilePathString( result );

        return result;
    }

    /// Get whether this object path is empty.
    ///
    /// @return  True if this path is empty, false if not.
    ///
    /// @see Clear(), Set()
    bool AssetPath::IsEmpty() const
    {
        return ( m_pEntry == NULL );
    }

    /// Compute a hash value for this object path (fast, should only be used with entries already in the object path
    /// table).
    ///
    /// @return  Hash value.
    size_t AssetPath::ComputeHash() const
    {
        // Each AssetPath entry has a unique Entry pointer, so we can use the address itself as a hash.
        return static_cast< size_t >( reinterpret_cast< uintptr_t >( m_pEntry ) );
    }

    /// Equality comparison operator.
    ///
    /// @param[in] path  Asset path with which to compare.
    ///
    /// @return  True if this path and the given path match, false if not.
    bool AssetPath::operator==( AssetPath path ) const
    {
        return ( m_pEntry == path.m_pEntry );
    }

    /// Inequality comparison operator.
    ///
    /// @param[in] path  Asset path with which to compare.
    ///
    /// @return  True if this path and the given path do not match, false if they do.
    bool AssetPath::operator!=( AssetPath path ) const
    {
        return ( m_pEntry != path.m_pEntry );
    }
}

namespace Helium
{
    /// Default AssetPath hash.
    ///
    /// @param[in] rKey  Key for which to compute a hash value.
    ///
    /// @return  Hash value.
    size_t Hash< Helium::AssetPath >::operator()( const Helium::AssetPath& rKey ) const
    {
        return rKey.ComputeHash();
    }
}
