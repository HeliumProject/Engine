#pragma once

#include "PcSupport/PcSupport.h"

#if HELIUM_TOOLS

#include "Engine/AssetLoader.h"

#include "PcSupport/LoosePackageLoaderMap.h"

namespace Helium
{
	class LooseAssetLoader;
	class LooseAssetFileWatcher;

	/// Archive-based object loader.
	class HELIUM_PC_SUPPORT_API LooseAssetLoader : public AssetLoader
	{
	public:
		/// @name Construction/Destruction
		//@{
		LooseAssetLoader();
		~LooseAssetLoader();
		//@}

		/// @name Loading Interface
		//@{
		virtual bool CacheObject( const AssetPath &path, Asset* pObject, bool bEvictPlatformPreprocessedResourceData );
		//@}

		/// @name Static Initialization
		//@{
		static bool InitializeStaticInstance();
		//@}

		virtual void EnumerateRootPackages( DynamicArray< AssetPath > &packagePaths );

		static void OnPackagePreloaded( LoosePackageLoader *pPackageLoader );

	private:
		LoosePackageLoaderMap m_packageLoaderMap;

		/// @name Loading Implementation
		//@{
		virtual PackageLoader* GetPackageLoader( AssetPath path );
		virtual void TickPackageLoaders();

		virtual void OnPrecacheReady( const AssetPath &path, Asset* pObject, PackageLoader* pPackageLoader );
		virtual void OnLoadComplete( const AssetPath &path, Asset* pObject, PackageLoader* pPackageLoader );
		//@}
	};
}

#endif  // HELIUM_TOOLS
