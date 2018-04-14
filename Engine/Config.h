#pragma once

#include "Engine/Engine.h"
#include "Engine/Asset.h"
#include "Engine/ConfigAsset.h"

// Configuration container package name.
#define HELIUM_CONFIG_CONTAINER_PACKAGE "Config"
// Default configuration base name.
#define HELIUM_CONFIG_DEFAULT_PACKAGE_BASE "Default"
// User configuration base name.
#define HELIUM_CONFIG_USER_PACKAGE_BASE "User"

// Windows platform configuration suffix.
#define HELIUM_CONFIG_PLATFORM_SUFFIX_WIN "Win"
// Mac platform configuration suffix.
#define HELIUM_CONFIG_PLATFORM_SUFFIX_MAC "Mac"
// Linux platform configuration suffix.
#define HELIUM_CONFIG_PLATFORM_SUFFIX_LINUX "Lin"

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

		inline bool IsAssetPathInConfigContainerPackage( const AssetPath &path ) const;
		inline bool IsAssetPathInDefaultConfigPackage( const AssetPath &path ) const;
		inline bool IsAssetPathInUserConfigPackage( const AssetPath &path ) const;
		//@}

		/// @name Loading
		//@{
		void BeginLoad();
		bool TryFinishLoad();
		//@}

		inline Name GetConfigObjectName( size_t index );
		FilePath GetUserConfigObjectFilePath( Name name );

		/// @name Config Asset Access
		//@{
		inline size_t GetConfigObjectCount() const;
		template< typename T > T* GetConfigObject( size_t index ) const;
		template< typename T > T* GetConfigObject( Name name ) const;
		//@}

		/// @name Static Access
		//@{
		static Config* GetInstance();
		static void Startup();
		static void Shutdown();
		//@}

	private:
		/// FilePath for the overall configuration container package.
		AssetPath m_configContainerPackagePath;
		/// Default configuration package path.
		AssetPath m_defaultConfigPackagePath;
		/// User configuration objects directory
		FilePath m_userDataDirectory;

		/// Default configuration package.
		PackagePtr m_spDefaultConfigPackage;

		/// Default configuration objects
		DynamicArray< ConfigAssetPtr > m_defaultConfigAssets;
		/// Configuration objects (coming from user objects, or defaults if no user object is found)
		DynamicArray< Reflect::ObjectPtr > m_configObjects;

		/// Async object load IDs.
		DynamicArray< size_t > m_assetLoadIds;
		/// Async file reads
		DynamicArray< size_t > m_fileLoadIds;

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
