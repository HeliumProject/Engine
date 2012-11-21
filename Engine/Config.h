#pragma once

#include "Engine/Engine.h"
#include "Engine/GameObjectPath.h"

/// Configuration container package name.
#define HELIUM_CONFIG_CONTAINER_PACKAGE TXT( "Config" )
/// Default configuration base name.
#define HELIUM_CONFIG_DEFAULT_PACKAGE_BASE TXT( "Default" )
/// User configuration base name.
#define HELIUM_CONFIG_USER_PACKAGE_BASE TXT( "User" )

/// Windows platform configuration suffix.
#define HELIUM_CONFIG_PLATFORM_SUFFIX_WIN TXT( "Win" )

/// Current platform configuration suffix.
#if HELIUM_OS_WIN
#define HELIUM_CONFIG_PLATFORM_SUFFIX HELIUM_CONFIG_PLATFORM_SUFFIX_WIN
#endif

namespace Helium
{
    class GameObject;
    typedef Helium::StrongPtr< GameObject > GameObjectPtr;
    typedef Helium::StrongPtr< const GameObject > ConstGameObjectPtr;

    class Package;
    typedef Helium::StrongPtr< Package > PackagePtr;
    typedef Helium::StrongPtr< const Package > ConstPackagePtr;

    /// Configuration management.
    class HELIUM_ENGINE_API Config : NonCopyable
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
        /// FilePath for the overall configuration container package.
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
        DynamicArray< GameObjectPtr > m_defaultConfigObjects;
        /// Configuration objects.
        DynamicArray< GameObjectPtr > m_configObjects;

        /// Async object load IDs.
        DynamicArray< size_t > m_objectLoadIds;
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
