#pragma once

#include "Engine/Engine.h"
#include "Engine/Asset.h"
#include "Engine/PackageLoader.h"
#include "Engine/PipelineMessages.h"

#include "Foundation/FilePath.h"

namespace Helium
{
	class HELIUM_ENGINE_API PipelinePackageLoader : public PackageLoader
	{
	public:
		/// Load request pool block size.
		static const size_t LOAD_REQUEST_POOL_BLOCK_SIZE = 4;

		/// @name Construction/Destruction
		//@{
		PipelinePackageLoader();
		virtual ~PipelinePackageLoader();
		//@}

		/// @name Initialization
		//@{
		bool Initialize( AssetPath packagePath );
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
		virtual size_t GetAssetCount() const;
		virtual AssetPath GetAssetPath( size_t index ) const;

		Package* GetPackage() const;
		AssetPath GetPackagePath() const;
		//@}

#if HELIUM_TOOLS
		/// @name Package File Information
		//@{
		virtual bool HasAssetFileState() const;
		virtual size_t GetAssetIndex( const AssetPath &path ) const;
		virtual const FilePath &GetAssetFileSystemPath( size_t index ) const;
		virtual int64_t GetAssetFileSystemTimestamp( size_t index ) const;
		virtual Name GetAssetTypeName( size_t index ) const;
		virtual AssetPath GetAssetTemplatePath( size_t index ) const;
		//@}
		
		virtual void EnumerateChildren( DynamicArray< AssetPath > &children ) const;

		virtual bool SaveAsset( Asset *pAsset ) const;
#endif

		void UpdateChildPackages(const DynamicArray< String > &childPackages);
		void UpdateAssetInfo(const Pipeline::PipelineAssetInfo &info);
		void UpdateAssetInfoComplete();

	private:
		AssetPath m_PackagePath;
		PackagePtr m_Package;

		struct AssetMetadata
		{
			AssetPath m_Path;
			AssetPath m_TemplatePath;
			Name m_TypeName;
			uint64_t m_Timestamp;
			uint64_t m_Size;
		};
		DynamicArray< AssetMetadata > m_Metadata;

		DynamicArray< AssetPath > m_ChildPackagePaths;

		bool m_AssetInfoReceived;

		size_t FindObjectByPath( const AssetPath &path ) const;
		size_t FindObjectByName( const Name &name ) const;

		/// Load request flags.
		enum ELoadFlag
		{
			/// Set once property preloading has completed.
			LOAD_FLAG_PROPERTY_PRELOADED            = 1 << 0,
			/// Set once persistent resource data loading has completed.
			LOAD_FLAG_PERSISTENT_RESOURCE_PRELOADED = 1 << 1,

			/// Set once all preloading has completed.
			LOAD_FLAG_PRELOADED = LOAD_FLAG_PROPERTY_PRELOADED | LOAD_FLAG_PERSISTENT_RESOURCE_PRELOADED,

			/// Set when an error has occurred in the load process.
			LOAD_FLAG_ERROR = 1 << 2
		};

		struct LoadRequest
		{
			/// Temporary object reference (hold while loading is in progress).
			AssetPtr m_Asset;
			/// Asset index.
			size_t m_Index;
			/// Resolver from top-level request
			Reflect::ObjectResolver *m_pResolver;

			/// Template
			AssetPtr m_Template;
			size_t m_TemplateLoadId;

			/// Owner
			AssetPtr m_Owner;
			size_t m_OwnerLoadId;

			/// Type
			AssetTypePtr m_Type;

			/// Load flags.
			uint32_t m_Flags;

			/// Pipeline request ID
			size_t m_DataRequestId;

			bool IsPreloaded() { return (m_Flags & LOAD_FLAG_PRELOADED ) == LOAD_FLAG_PRELOADED; }
			bool HasError() { return (m_Flags & LOAD_FLAG_ERROR ) == LOAD_FLAG_ERROR; }
		};

		/// Pending load requests.
		SparseArray< LoadRequest* > m_loadRequests;
		/// Load request pool.
		ObjectPool< LoadRequest > m_loadRequestPool;
		
		LoadRequest *AllocateLoadRequest();

		void TickPreload();
		void TickLoadRequests();
		bool TickDeserialize( LoadRequest* pRequest );

		/// Mutex for synchronizing access between threads.
		mutable Mutex m_accessLock;

		/// Parent package load request ID.
		size_t m_parentPackageLoadId;

		/// Non-zero if the preload process has started.
		volatile int32_t m_startPreloadCounter;
		/// Non-zero if the package has been preloaded.
		volatile int32_t m_preloadedCounter;
	};
}
