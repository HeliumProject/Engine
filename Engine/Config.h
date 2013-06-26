#pragma once

#include "Engine/Engine.h"
#include "Engine/Asset.h"

// Configuration container package name.
#define HELIUM_CONFIG_CONTAINER_PACKAGE TXT( "Config" )
// Default configuration base name.
#define HELIUM_CONFIG_DEFAULT_PACKAGE_BASE TXT( "Default" )
// User configuration base name.
#define HELIUM_CONFIG_USER_PACKAGE_BASE TXT( "User" )

// Windows platform configuration suffix.
#define HELIUM_CONFIG_PLATFORM_SUFFIX_WIN TXT( "Win" )
// Mac platform configuration suffix.
#define HELIUM_CONFIG_PLATFORM_SUFFIX_MAC TXT( "Mac" )
// Linux platform configuration suffix.
#define HELIUM_CONFIG_PLATFORM_SUFFIX_LINUX TXT( "Linux" )

// Current platform configuration suffix.
#if HELIUM_OS_WIN
# define HELIUM_CONFIG_PLATFORM_SUFFIX HELIUM_CONFIG_PLATFORM_SUFFIX_WIN
#elif HELIUM_OS_MAC
# define HELIUM_CONFIG_PLATFORM_SUFFIX HELIUM_CONFIG_PLATFORM_SUFFIX_MAC
#elif HELIUM_OS_LINUX
# define HELIUM_CONFIG_PLATFORM_SUFFIX HELIUM_CONFIG_PLATFORM_SUFFIX_LINUX
#endif

namespace Helium
{
    /// Configuration management.
    class HELIUM_ENGINE_API Config : NonCopyable
    {
    public:
        /// @name General Information
        //@{
        inline AssetPath GetConfigContainerPackagePath() const;
        inline AssetPath GetDefaultConfigPackagePath() const;
        inline AssetPath GetUserConfigPackagePath() const;
        //@}

        /// @name Loading
        //@{
        void BeginLoad();
        bool TryFinishLoad();
        //@}

        /// @name Config Asset Access
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
        AssetPath m_configContainerPackagePath;
        /// Default configuration package path.
        AssetPath m_defaultConfigPackagePath;
        /// User configuration package path.
        AssetPath m_userConfigPackagePath;

        /// Default configuration package.
        PackagePtr m_spDefaultConfigPackage;
        /// User configuration package.
        PackagePtr m_spUserConfigPackage;

        /// Default configuration objects (only used while loading).
        DynamicArray< AssetPtr > m_defaultConfigObjects;
        /// Configuration objects.
        DynamicArray< AssetPtr > m_configObjects;

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
