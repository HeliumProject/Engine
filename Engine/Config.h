//----------------------------------------------------------------------------------------------------------------------
// Config.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_ENGINE_CONFIG_H
#define LUNAR_ENGINE_CONFIG_H

#include "Engine/Engine.h"

/// Configuration container package name.
#define L_CONFIG_CONTAINER_PACKAGE TXT( "Config" )
/// Default configuration base name.
#define L_CONFIG_DEFAULT_PACKAGE_BASE TXT( "Default" )
/// User configuration base name.
#define L_CONFIG_USER_PACKAGE_BASE TXT( "User" )

/// Windows platform configuration suffix.
#define L_CONFIG_PLATFORM_SUFFIX_WIN TXT( "Win" )

/// Current platform configuration suffix.
#if HELIUM_OS_WIN
#define L_CONFIG_PLATFORM_SUFFIX L_CONFIG_PLATFORM_SUFFIX_WIN
#endif

namespace Lunar
{
    HELIUM_DECLARE_PTR( GameObject );
    HELIUM_DECLARE_PTR( Package );

    /// Configuration management.
    class LUNAR_ENGINE_API Config : NonCopyable
    {
    public:
        /// @name General Information
        //@{
        inline GameObjectPath GetConfigContainerPackagePath() const;
        inline GameObjectPath GetDefaultConfigPackagePath() const;
        inline GameObjectPath GetUserConfigPackagePath() const;
        //@}

        /// @name Loading
        //@{
        void BeginLoad();
        bool TryFinishLoad();

        void SaveUserConfig();
        //@}

        /// @name Config GameObject Access
        //@{
        inline Package* GetUserConfigPackage() const;

        inline size_t GetConfigObjectCount() const;
        template< typename T > T* GetConfigObject( size_t index ) const;
        template< typename T > T* GetConfigObject( Name name ) const;
        //@}

        /// @name Static Access
        //@{
        static Config& GetStaticInstance();
        static void DestroyStaticInstance();
        //@}

    private:
        /// Path for the overall configuration container package.
        GameObjectPath m_configContainerPackagePath;
        /// Default configuration package path.
        GameObjectPath m_defaultConfigPackagePath;
        /// User configuration package path.
        GameObjectPath m_userConfigPackagePath;

        /// Default configuration package.
        PackagePtr m_spDefaultConfigPackage;
        /// User configuration package.
        PackagePtr m_spUserConfigPackage;

        /// Default configuration objects (only used while loading).
        DynArray< GameObjectPtr > m_defaultConfigObjects;
        /// Configuration objects.
        DynArray< GameObjectPtr > m_configObjects;

        /// Async object load IDs.
        DynArray< size_t > m_objectLoadIds;
        /// True if we're waiting on an async load of the configuration package.
        bool m_bLoadingConfigPackage;

        /// Singleton instance.
        static Config* sm_pInstance;

        /// @name Construction/Destruction
        //@{
        Config();
        ~Config();
        //@}
    };
}

#include "Engine/Config.inl"

#endif  // LUNAR_ENGINE_CONFIG_H
