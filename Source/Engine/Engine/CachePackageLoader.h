#pragma once

#include "Engine/Asset.h"
#include "Engine/PackageLoader.h"

#include "Engine/Cache.h"

namespace Helium
{
	/// Package loader for loading objects from a binary cache.
	class CachePackageLoader : public PackageLoader
	{
	public:
		/// Load request pool block size.
		static const size_t LOAD_REQUEST_POOL_BLOCK_SIZE = 16;

		/// @name Construction/Destruction
		//@{
		CachePackageLoader();
		virtual ~CachePackageLoader();
		//@}

		/// @name Initialization
		//@{
		bool Initialize( Name cacheName );
		void Shutdown();
		//@} 

		/// @name Loading
		//@{
		bool BeginPreload();
		virtual bool TryFinishPreload();

		virtual size_t BeginLoadObject( AssetPath path, Reflect::ObjectResolver *pResolver, bool forceReload = false );
		virtual bool TryFinishLoadObject( size_t requestId, AssetPtr& rspObject );

		virtual void Tick();
		//@}

		/// @name Data Access
		//@{
		virtual size_t GetObjectCount() const;
		virtual AssetPath GetAssetPath( size_t index ) const;

		inline Cache* GetCache() const;
		//@}

	private:
		/// Load request flags.
		enum ELoadFlag
		{
			/// Set once object preloading has completed.
			LOAD_FLAG_PRELOADED = 1 << 0,
			/// Set when an error has occurred in the load process.
			LOAD_FLAG_ERROR = 1 << 1
		};

		/// Asset load request data.
		struct LoadRequest
		{
			/// Cache entry.
			const Cache::Entry* pEntry;
			/// Resolver from top-level request
			Reflect::ObjectResolver *pResolver;
			/// Temporary object reference (hold while loading is in progress).
			AssetPtr spObject;

			/// Async load ID.
			size_t asyncLoadId;
			/// Async load buffer.
			uint8_t* pAsyncLoadBuffer;

			/// Pointer to where the property data begins within the pAsyncLoadBuffer
			uint8_t* pPropertyDataBegin;
			/// End of the property data
			uint8_t* pPropertyDataEnd;
			/// Pointer to where the persistent resource data begins within the pAsyncLoadBuffer
			uint8_t* pPersistentResourceDataBegin;
			/// End of the persistent resource data.
			uint8_t* pPersistentResourceDataEnd;

			// Load index for the owning asset
			size_t ownerLoadIndex;

			/// Cached owner reference.
			AssetPtr spOwner;

			/// Load flags.
			uint32_t flags;

			bool forceReload;
		};

		/// Cache from which objects will be loaded.
		Cache* m_pCache;
		/// True if we've synced the cache TOC load process.
		bool m_bFinishedCacheTocLoad;

		/// Pending load requests.
		SparseArray< LoadRequest* > m_loadRequests;
		/// Load request pool.
		ObjectPool< LoadRequest > m_loadRequestPool;

		/// @name Load Ticking Functions
		//@{
		bool TickCacheLoad( LoadRequest* pRequest );
		bool TickDeserialize( LoadRequest* pRequest );
		//@}

		/// @name Static Private Utility Functions
		//@{
		static void ResolvePackage( AssetPtr& spPackage, AssetPath packagePath );
		static bool ReadCacheData( LoadRequest* pRequest );
		//@}
	};
}

#include "Engine/CachePackageLoader.inl"
