#pragma once

#include "PcSupport/PcSupport.h"

#if HELIUM_TOOLS

#include "Engine/AssetLoader.h"

#include "PcSupport/LoosePackageLoaderMap.h"

namespace Helium
{
	class LooseAssetLoader;

#if HELIUM_TOOLS
	class LooseAssetFileWatcher;
#endif

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
		virtual bool CacheObject( Asset* pObject, bool bEvictPlatformPreprocessedResourceData );
		//@}

		/// @name Static Initialization
		//@{
		static bool InitializeStaticInstance();
		//@}

#if HELIUM_TOOLS
		virtual void EnumerateRootPackages( DynamicArray< AssetPath > &packagePaths );
#endif

		static void OnPackagePreloaded( LoosePackageLoader *pPackageLoader );

	private:
		/// XML package loader map.
		LoosePackageLoaderMap m_packageLoaderMap;

		/// @name Loading Implementation
		//@{
		virtual PackageLoader* GetPackageLoader( AssetPath path );
		virtual void TickPackageLoaders();

		virtual void OnPrecacheReady( Asset* pObject, PackageLoader* pPackageLoader );
		virtual void OnLoadComplete( AssetPath path, Asset* pObject, PackageLoader* pPackageLoader );
		//@}
	};
}

#endif  // HELIUM_TOOLS
