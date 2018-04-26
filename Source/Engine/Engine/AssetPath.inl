/// Constructor.
///
/// When using the default constructor, the internal object path entry pointer is left in an uninitialized state and
/// must be manually initialized using Set() or some other assignment before being used.
Helium::AssetPath::AssetPath()
{
}

/// Constructor.
///
/// The object path entry will be initialized to null.
Helium::AssetPath::AssetPath( ENullName )
	: m_pEntry( NULL )
{
}
	
/// Constructor.
///
/// The object path entry will be initialized to null.
Helium::AssetPath::AssetPath( const char *pString )
{
	Set( pString );
}

/// Get whether or not this asset path is equivalent, or contained within the given asset path
///
/// @return  True if asset path is equivalent, or contained within the given asset path
///
/// @see GetParent()
bool Helium::AssetPath::IsWithinAssetPath( const AssetPath &rOuterAssetPath ) const
{
	// Only cache the files we care about
	for( AssetPath testPath = *this; !testPath.IsEmpty(); testPath = testPath.GetParent() )
	{
		if( testPath == rOuterAssetPath )
		{
			return true;
		}
	}

	return false;
}

/// Get the object name (bottom-most component of the path).
///
/// @return  Asset name.
///
/// @see GetInstanceIndex(), IsPackage(), GetParent()
Helium::Name Helium::AssetPath::GetName() const
{
	return ( m_pEntry ? m_pEntry->name : Name( NULL_NAME ) );
}

/// Get the root object name (bottom-most non-package component of the path).
///
/// @return  Root asset name.
///
/// @see GetInstanceIndex(), IsPackage(), GetParent()
Helium::Name Helium::AssetPath::GetRootName() const
{
	Entry *pEntry = m_pEntry;

	while ( pEntry && pEntry->pParent && !pEntry->pParent->bPackage )
	{
		pEntry = pEntry->pParent;
	}

	return ( pEntry ? pEntry->name : Name( NULL_NAME ) );
}

/// Get the instance index associated with the object.
///
/// @return  Instance index.
///
/// @see GetName(), IsPackage(), GetParent()
uint32_t Helium::AssetPath::GetInstanceIndex() const
{
	return ( m_pEntry ? m_pEntry->instanceIndex : Invalid< uint32_t >() );
}

/// Get whether the specific object referenced by this path is a package.
///
/// @return  True if this object is a package, false if not.  This will also return false for empty path names.
///
/// @see GetName(), GetInstanceIndex(), GetParent()
bool Helium::AssetPath::IsPackage() const
{
	return ( m_pEntry && m_pEntry->bPackage );
}

/// Get the parent of this path.
///
/// @return  Parent path, or an empty path if this path has no parent or is empty.
///
/// @see GetName(), GetInstanceIndex(), IsPackage()
Helium::AssetPath Helium::AssetPath::GetParent() const
{
	AssetPath result;
	result.m_pEntry = ( m_pEntry ? m_pEntry->pParent : NULL );

	return result;
}

/// Get the parent of this path that is a package (i.e. a sub-object /My/Package:Object:InnerObject would return /My/Package
///
/// @return  Parent package path, or an empty path if this path has no parent or is empty.
///
/// @see GetName(), GetInstanceIndex(), IsPackage()
Helium::AssetPath Helium::AssetPath::GetParentPackage() const
{
	AssetPath result;
	result.m_pEntry = ( m_pEntry ? m_pEntry->pParent : NULL );
	
	while (result.m_pEntry && !result.m_pEntry->bPackage)
	{
		result.m_pEntry = result.m_pEntry->pParent;
	}

	return result;
}

/// Generate the string representation of this object path.
///
/// @return  String representation of this path.
///
/// @see Set()
Helium::String Helium::AssetPath::ToString() const
{
	String result;
	ToString( result );

	return result;
}

/// Generate a string representation of this object path with all package and object delimiters converted to valid
/// directory delimiters for the current platform.
///
/// @return  File path string representation of this path.
Helium::String Helium::AssetPath::ToFilePathString() const
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
bool Helium::AssetPath::IsEmpty() const
{
	return ( m_pEntry == NULL );
}

/// Compute a hash value for this object path (fast, should only be used with entries already in the object path
/// table).
///
/// @return  Hash value.
size_t Helium::AssetPath::ComputeHash() const
{
	// Each AssetPath entry has a unique Entry pointer, so we can use the address itself as a hash.
	return static_cast< size_t >( reinterpret_cast< uintptr_t >( m_pEntry ) );
}

/// Equality comparison operator.
///
/// @param[in] path  Asset path with which to compare.
///
/// @return  True if this path and the given path match, false if not.
bool Helium::AssetPath::operator==( AssetPath path ) const
{
	return ( m_pEntry == path.m_pEntry );
}

/// Inequality comparison operator.
///
/// @param[in] path  Asset path with which to compare.
///
/// @return  True if this path and the given path do not match, false if they do.
bool Helium::AssetPath::operator!=( AssetPath path ) const
{
	return ( m_pEntry != path.m_pEntry );
}

/// Default AssetPath hash.
///
/// @param[in] rKey  Key for which to compute a hash value.
///
/// @return  Hash value.
size_t Helium::Hash< Helium::AssetPath >::operator()( const Helium::AssetPath& rKey ) const
{
	return rKey.ComputeHash();
}
