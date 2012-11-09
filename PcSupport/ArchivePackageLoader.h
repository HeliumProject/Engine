//----------------------------------------------------------------------------------------------------------------------
// ArchivePackageLoader.h
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
//#include "Engine/Serializer.h"

#include "Foundation/FilePath.h"

/// XML package file extension string.
#define HELIUM_ARCHIVE_PACKAGE_OBJECT_FILE_EXTENSION TXT( ".object" )
/// Directory-based XML package file name string.
//#define HELIUM_ARCHIVE_PACKAGE_TOC_FILENAME TXT( "!toc.xml" )

namespace Helium
{
    struct HELIUM_PC_SUPPORT_API ObjectDescriptor : public Reflect::Object
    {
        REFLECT_DECLARE_OBJECT( ObjectDescriptor, Reflect::Object );

        tstring m_Name;
        tstring m_TypeName;
        tstring m_TemplatePath;

        static void PopulateComposite( Reflect::Composite& comp );
    };

    class BuildLinkTableFromObjectVisitor;
    class ClearLinkIndicesFromObject;
    
    class HELIUM_PC_SUPPORT_API ArchivePackageLoader : public PackageLoader
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
            /// GameObject path.
            GameObjectPath objectPath;

            /// Type name.
            Name typeName;
            /// Template path.
            GameObjectPath templatePath;

            /// Serialized properties.
            //ConcurrentHashMap< String, String > properties;
            /// Cached array sizes.
            //ConcurrentHashMap< String, uint32_t > arraySizes;
        };

        /// @name Construction/Destruction
        //@{
        ArchivePackageLoader();
        virtual ~ArchivePackageLoader();
        //@}

        /// @name Initialization
        //@{
        bool Initialize( GameObjectPath packagePath );
        void Shutdown();
        //@}

        /// @name Loading
        //@{
        bool BeginPreload();
        virtual bool TryFinishPreload();

        virtual size_t BeginLoadObject( GameObjectPath path );
        virtual bool TryFinishLoadObject(
            size_t requestId, GameObjectPtr& rspObject, DynArray< GameObjectLoader::LinkEntry >& rLinkTable );

        virtual void Tick();
        //@}

        /// @name Data Access
        //@{
        virtual size_t GetObjectCount() const;
        virtual GameObjectPath GetObjectPath( size_t index ) const;

        Package* GetPackage() const;
        GameObjectPath GetPackagePath() const;

        inline const FilePath& GetPackageFileSystemPath() const;
        //@}

        /// @name Package File Information
        //@{
        virtual bool IsSourcePackageFile() const;
        virtual int64_t GetFileTimestamp() const;
        //@}

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
            /// GameObject path.
            //GameObjectPath path;
            /// Load request ID.
            size_t loadRequestId;
        };
        friend class Helium::BuildLinkTableFromObjectVisitor;
        friend class Helium::ClearLinkIndicesFromObject;

        /// GameObject load request data.
        struct LoadRequest
        {
            /// Temporary object reference (hold while loading is in progress).
            GameObjectPtr spObject;
            /// GameObject index.
            size_t index;

            /// Link table.
            DynArray< LinkEntry > linkTable;

            /// Cached type reference.
            GameObjectTypePtr spType;
            /// Cached template reference.
            GameObjectPtr spTemplate;
            /// Cached owner reference.
            GameObjectPtr spOwner;
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
        GameObjectPath m_packagePath;

        /// Non-zero if the preload process has started.
        volatile int32_t m_startPreloadCounter;
        /// Non-zero if the package has been preloaded.
        volatile int32_t m_preloadedCounter;

        /// Serialized object data parsed from the XML package.
        DynArray< SerializedObjectData > m_objects;

        /// Pending load requests.
        SparseArray< LoadRequest* > m_loadRequests;
        /// Load request pool.
        ObjectPool< LoadRequest > m_loadRequestPool;

        /// Package file path name.
        FilePath m_packageDirPath;
        //FilePath m_packageTocFilePath;
        /// Size of the package data file.
        //size_t m_packageTocFileSize;

        /// Destination buffer for async loading.
        //void* m_pTocLoadBuffer;
        /// Async loading ID.
        //size_t m_tocAsyncLoadId;
        
        struct FileReadRequest
        {
            Helium::FilePath filePath; // Used only to give good error messages
            void* pLoadBuffer;
            size_t asyncLoadId;
            uint64_t expectedSize;
        };
        DynArray<FileReadRequest> m_fileReadRequests;


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
    };
}

#include "PcSupport/ArchivePackageLoader.inl"

#endif  // HELIUM_ENGINE_ARCHIVE_PACKAGE_LOADER_H
