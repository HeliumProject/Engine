/// Get the path of the main package in which all configuration packages are stored.
///
/// @return  FilePath of the configuration container package.
///
/// @see GetDefaultConfigPackagePath(), GetUserConfigPackagePath()
Helium::AssetPath Helium::Config::GetConfigContainerPackagePath() const
{
	return m_configContainerPackagePath;
}

/// Get the path of the default configuration package for the current platform.
///
/// @return  FilePath of the default configuration package for this platform.
///
/// @see GetUserConfigPackagePath(), GetConfigContainerPackagePath()
Helium::AssetPath Helium::Config::GetDefaultConfigPackagePath() const
{
	return m_defaultConfigPackagePath;
}

/// Returns if the given asset path is a config asset path
///
/// @return  True if the asset is a config object
///
/// @see GetConfigContainerPackagePath(), IsAssetPathInDefaultConfigPackage(), IsAssetPathInUserConfigPackage()
bool Helium::Config::IsAssetPathInConfigContainerPackage( const AssetPath &path ) const
{
	HELIUM_ASSERT( !m_configContainerPackagePath.IsEmpty() );

	return path.IsWithinAssetPath( m_configContainerPackagePath );
}

/// Get the path of the default configuration package for the current platform.
///
/// @return  FilePath of the default configuration package for this platform.
///
/// @see GetDefaultConfigPackagePath(), IsAssetPathInConfigContainerPackage(), IsAssetPathInUserConfigPackage()
bool Helium::Config::IsAssetPathInDefaultConfigPackage( const AssetPath &path ) const
{
	HELIUM_ASSERT( !m_defaultConfigPackagePath.IsEmpty() );

	return path.IsWithinAssetPath( m_defaultConfigPackagePath );
}

/// Get the name of the config object at the given index
///
/// @return  Name of the object.
Helium::Name Helium::Config::GetConfigObjectName( size_t index )
{
	return m_defaultConfigAssets[ index ]->GetName();
}

/// Get the number of loaded configuration objects.
///
/// @return  Configuration object count.
///
/// @see GetConfigObject()
size_t Helium::Config::GetConfigObjectCount() const
{
	return m_configObjects.GetSize();
}

/// Get the configuration object associated with the given index.
///
/// @param[in] index  Configuration object index.
///
/// @return  Configuration object associated with the given index.
///
/// @see GetConfigObjectCount()
template< typename T >
T* Helium::Config::GetConfigObject( size_t index ) const
{
	HELIUM_ASSERT( index < m_configObjects.GetSize() );

	Reflect::Object *pObject = m_configObjects[ index ];
	HELIUM_ASSERT( pObject );

	return Reflect::AssertCast< T >( pObject );
}

/// Get the configuration object with the given name.
///
/// @param[in] name  Configuration object name.
///
/// @return  Configuration object with the given name.
///
/// @see GetConfigObjectCount()
template< typename T >
T* Helium::Config::GetConfigObject( Name name ) const
{
	size_t configObjectCount = m_configObjects.GetSize();
	for( size_t objectIndex = 0; objectIndex < configObjectCount; ++objectIndex )
	{
		const Name &assetName = m_defaultConfigAssets[ objectIndex ]->GetName();

		if( assetName == name )
		{		
			Reflect::Object* pObject = m_configObjects[ objectIndex ];
			HELIUM_ASSERT( pObject );
			return Reflect::AssertCast< T >( pObject );
		}
	}

	return NULL;
}
