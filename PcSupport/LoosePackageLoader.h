//----------------------------------------------------------------------------------------------------------------------
// LoosePackageLoader.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_PC_SUPPORT_ARCHIVE_PACKAGE_LOADER_H
#define HELIUM_PC_SUPPORT_ARCHIVE_PACKAGE_LOADER_H

#include "Engine/Engine.h"
#include "Engine/Package.h"
#include "Engine/PackageLoader.h"

#include "Foundation/FilePath.h"

// XML package file extension string.
#define HELIUM_ARCHIVE_PACKAGE_OBJECT_FILE_EXTENSION TXT( ".object" )

namespace Helium
{
	// TODO: Use real types instead of strings and completely replace SerializedObjectData
	struct HELIUM_PC_SUPPORT_API ObjectDescriptor : public Reflect::Object
	{
		REFLECT_DECLARE_OBJECT( ObjectDescriptor, Reflect::Object );

		tstring m_Name;
		tstring m_TypeName;
		tstring m_TemplatePath;

		static void PopulateStructure( Reflect::Structure& comp );
	};
	
	class HELIUM_PC_SUPPORT_API LoosePackageLoader : public PackageLoader
	{
		struct LinkEntry;

	public:
		/// Load request pool block size.
		static const size_t LOAD_REQUEST_POOL_BLOCK_SIZE = 4;

		/// Maximum number of bytes to parse at a time.
		static const size_t PARSE_CHUNK_SIZE = 4 * 1024;

		/// Serialized object data.
		struct SerializedObjectData
		{
			/// Asset path.
			AssetPath objectPath;
			/// File path
			FilePath filePath;
			/// File time stamp
			int64_t fileTimeStamp;
			/// Type name.
			Name typeName;
			/// Template path.
			AssetPath templatePath;
		};

		/// @name Construction/Destruction
		//@{
		LoosePackageLoader();
		virtual ~LoosePackageLoader();
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

		virtual size_t BeginLoadObject( AssetPath path );
		virtual bool TryFinishLoadObject(
			size_t requestId, AssetPtr& rspObject, DynamicArray< AssetLoader::LinkEntry >& rLinkTable );

		virtual void Tick();
		//@}

		/// @name Data Access
		//@{
		virtual size_t GetObjectCount() const;
		virtual AssetPath GetAssetPath( size_t index ) const;

		Package* GetPackage() const;
		AssetPath GetPackagePath() const;
		//@}

#if HELIUM_TOOLS
		/// @name Package File Information
		//@{
		virtual bool CanResolveLooseAssetFilePaths() const;
		virtual const FilePath &GetLooseAssetFileSystemPath( const AssetPath &path ) const;
		virtual int64_t GetLooseAssetFileSystemTimestamp( const AssetPath &path ) const;
		//@}
#endif

	private:
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

		/// Link table entry.
		struct LinkEntry
		{
			/// Asset path.
			//AssetPath path;
			/// Load request ID.
			size_t loadRequestId;
		};

		/// Asset load request data.
		struct LoadRequest
		{
			/// Temporary object reference (hold while loading is in progress).
			AssetPtr spObject;
			/// Asset index.
			size_t index;

			/// Link table.
			DynamicArray< LinkEntry > linkTable;

			/// Cached type reference.
			AssetTypePtr spType;
			/// Cached template reference.
			AssetPtr spTemplate;
			/// Cached owner reference.
			AssetPtr spOwner;
			/// Template object load request ID.
			size_t templateLoadId;
			/// Owner object load request ID.
			size_t ownerLoadId;

			/// Async load ID for persistent resource data.
			size_t persistentResourceDataLoadId;
			/// Buffer for loading cached object data (for pre-loading the persistent resource data).
			uint8_t* pCachedObjectDataBuffer;
			/// Size of the cached object data buffer.
			uint32_t cachedObjectDataBufferSize;

			/// Async load for object file
			size_t asyncFileLoadId;
			void* pAsyncFileLoadBuffer;
			size_t asyncFileLoadBufferSize;

			/// Load flags.
			uint32_t flags;
		};

		/// Package reference.
		PackagePtr m_spPackage;
		/// Package path.
		AssetPath m_packagePath;

		/// Non-zero if the preload process has started.
		volatile int32_t m_startPreloadCounter;
		/// Non-zero if the package has been preloaded.
		volatile int32_t m_preloadedCounter;

		/// Serialized object data parsed from the XML package.
		DynamicArray< SerializedObjectData > m_objects;

		/// Pending load requests.
		SparseArray< LoadRequest* > m_loadRequests;
		/// Load request pool.
		ObjectPool< LoadRequest > m_loadRequestPool;

		/// Package file path name.
		FilePath m_packageDirPath;
		
		struct FileReadRequest
		{
			Helium::FilePath filePath;
			void* pLoadBuffer;
			size_t asyncLoadId;
			uint64_t expectedSize;

			uint64_t fileTimestamp;
		};
		DynamicArray<FileReadRequest> m_fileReadRequests;

		/// Parent package load request ID.
		size_t m_parentPackageLoadId;

		/// Mutex for synchronizing access between threads.
		Mutex m_accessLock;

		/// @name Private Utility Functions
		//@{
		void TickPreload();

		void TickLoadRequests();
		bool TickDeserialize( LoadRequest* pRequest );
		bool TickPersistentResourcePreload( LoadRequest* pRequest );
		//@}

		size_t FindObjectByPath( const AssetPath &path ) const;
	};
}

#include "PcSupport/LoosePackageLoader.inl"

#endif  // HELIUM_ENGINE_ARCHIVE_PACKAGE_LOADER_H
