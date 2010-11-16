//----------------------------------------------------------------------------------------------------------------------
// CachePackageLoader.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_ENGINE_CACHE_PACKAGE_LOADER_H
#define LUNAR_ENGINE_CACHE_PACKAGE_LOADER_H

#include "Engine/PackageLoader.h"

#include "Engine/Cache.h"

namespace Lunar
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

        virtual size_t BeginLoadObject( ObjectPath path );
        virtual bool TryFinishLoadObject(
            size_t requestId, ObjectPtr& rspObject, DynArray< ObjectLoader::LinkEntry >& rLinkTable );

        virtual void Tick();
        //@}

        /// @name Data Access
        //@{
        virtual size_t GetObjectCount() const;
        virtual ObjectPath GetObjectPath( size_t index ) const;

        inline Cache* GetCache() const;
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
            /// Set once object preloading has completed.
            LOAD_FLAG_PRELOADED = 1 << 0,
            /// Set when an error has occurred in the load process.
            LOAD_FLAG_ERROR = 1 << 1
        };

        /// Object load request data.
        struct LoadRequest
        {
            /// Cache entry.
            const Cache::Entry* pEntry;
            /// Temporary object reference (hold while loading is in progress).
            ObjectPtr spObject;

            /// Async load ID.
            size_t asyncLoadId;
            /// Async load buffer.
            uint8_t* pAsyncLoadBuffer;
            /// Binary serialized object property data (immediately past the link table).
            uint8_t* pSerializedData;
            /// End of the serialized property data.
            uint8_t* pPropertyStreamEnd;
            /// End of the serialized persistent resource data.
            uint8_t* pPersistentResourceStreamEnd;

            /// Link table (table stores load request IDs for objects to link).
            DynArray< size_t > linkTable;

            /// Cached type reference.
            ObjectPtr spType;
            /// Cached template reference.
            ObjectPtr spTemplate;
            /// Cached owner reference.
            ObjectPtr spOwner;
            /// Type link table index.
            uint32_t typeLinkIndex;
            /// Template link table index.
            uint32_t templateLinkIndex;
            /// Owner link reference.
            uint32_t ownerLinkIndex;

            /// Load flags.
            uint32_t flags;
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
        static void ResolvePackage( ObjectPtr& spPackage, ObjectPath packagePath );
        static bool DeserializeLinkTable( LoadRequest* pRequest );
        //@}
    };
}

#include "Engine/CachePackageLoader.inl"

#endif  // LUNAR_ENGINE_CACHE_PACKAGE_LOADER_H
