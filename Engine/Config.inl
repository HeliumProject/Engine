//----------------------------------------------------------------------------------------------------------------------
// Config.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Lunar
{
    /// Get the path of the main package in which all configuration packages are stored.
    ///
    /// @return  Path of the configuration container package.
    ///
    /// @see GetDefaultConfigPackagePath(), GetUserConfigPackagePath()
    ObjectPath Config::GetConfigContainerPackagePath() const
    {
        return m_configContainerPackagePath;
    }

    /// Get the path of the default configuration package for the current platform.
    ///
    /// @return  Path of the default configuration package for this platform.
    ///
    /// @see GetUserConfigPackagePath(), GetConfigContainerPackagePath()
    ObjectPath Config::GetDefaultConfigPackagePath() const
    {
        return m_defaultConfigPackagePath;
    }

    /// Get the path of the user-specific configuration package.
    ///
    /// @return  Path of the user configuration package.
    ///
    /// @see GetDefaultConfigPackagePath(), GetConfigContainerPackagePath()
    ObjectPath Config::GetUserConfigPackagePath() const
    {
        return m_userConfigPackagePath;
    }

    /// Get the user configuration package instance.
    ///
    /// @return  User configuration package.
    Package* Config::GetUserConfigPackage() const
    {
        return m_spUserConfigPackage;
    }

    /// Get the number of loaded configuration objects.
    ///
    /// @return  Configuration object count.
    ///
    /// @see GetConfigObject()
    size_t Config::GetConfigObjectCount() const
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
    T* Config::GetConfigObject( size_t index ) const
    {
        HELIUM_ASSERT( index < m_configObjects.GetSize() );

        Object* pObject = m_configObjects[ index ];
        HELIUM_ASSERT( pObject );

        return StaticCast< T >( pObject );
    }

    /// Get the configuration object with the given name.
    ///
    /// @param[in] name  Configuration object name.
    ///
    /// @return  Configuration object with the given name.
    ///
    /// @see GetConfigObjectCount()
    template< typename T >
    T* Config::GetConfigObject( Name name ) const
    {
        size_t configObjectCount = m_configObjects.GetSize();
        for( size_t objectIndex = 0; objectIndex < configObjectCount; ++objectIndex )
        {
            Object* pObject = m_configObjects[ objectIndex ];
            HELIUM_ASSERT( pObject );

            if( pObject->GetName() == name )
            {
                return StaticCast< T >( pObject );
            }
        }

        return NULL;
    }
}
