//----------------------------------------------------------------------------------------------------------------------
// AssetLoader.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_ENGINE_OBJECT_LOADER_H
#define HELIUM_ENGINE_OBJECT_LOADER_H

#include "Engine/Engine.h"

#include "Foundation/ConcurrentHashMap.h"
#include "Foundation/ObjectPool.h"
#include "Engine/AssetPath.h"
#include "Engine/Asset.h"
#include "Engine/Serializer.h"

namespace Helium
{
    class PackageLoader;

    /// Asynchronous object loading interface
    class HELIUM_ENGINE_API AssetLoader : NonCopyable
    {
    public:
        /// Number of request objects to allocate in each block of the request pool.
        static const size_t LOAD_REQUEST_POOL_BLOCK_SIZE = 64;

        /// Asset link table entry.
        struct LinkEntry
        {
            /// Load request ID.
            size_t loadId;
            /// Cached object reference.
            AssetPtr spObject;
        };

        /// @name Construction/Destruction
        //@{
        AssetLoader();
        virtual ~AssetLoader() = 0;
        //@}

        /// @name Loading Interface
        //@{
        virtual size_t BeginLoadObject( AssetPath path );
        virtual bool TryFinishLoad( size_t id, AssetPtr& rspObject );
        void FinishLoad( size_t id, AssetPtr& rspObject );

        bool LoadObject( AssetPath path, AssetPtr& rspObject );

#if HELIUM_TOOLS
        virtual bool CacheObject( Asset* pObject, bool bEvictPlatformPreprocessedResourceData = true );
#endif

        virtual void Tick();
        //@}

        /// @name Data Access
        //@{
        inline Name GetCacheName() const;
        //@}

        /// @name Static Access
        //@{
        static AssetLoader* GetStaticInstance();
        static void DestroyStaticInstance();
        //@}

        static void HandleLinkDependency(Asset &_outer, Helium::StrongPtr<Asset> &_asset_pointer, AssetPath &_path);

        static void FinalizeLink(Asset *_asset);

    protected:
        /// Load status flags.
        enum ELoadFlag
        {
            /// Set once object preloading has completed.
            LOAD_FLAG_PRELOADED = 1 << 0,
            /// Set once object linking has completed.
            LOAD_FLAG_LINKED    = 1 << 1,
            /// Set once resource data has been precached.
            LOAD_FLAG_PRECACHED = 1 << 2,
            /// Set once the object load has been finalized.
            LOAD_FLAG_LOADED    = 1 << 3,

            /// All load progress flags.
            LOAD_FLAG_FULLY_LOADED = LOAD_FLAG_PRELOADED | LOAD_FLAG_LINKED | LOAD_FLAG_PRECACHED | LOAD_FLAG_LOADED,

            /// Set when an error has occurred in the load process.
            LOAD_FLAG_ERROR = 1 << 4,

            /// Set if resource precaching has been started.
            LOAD_FLAG_PRECACHE_STARTED = 1 << 5,

            /// Set if ticking is in progress.
            LOAD_FLAG_IN_TICK = 1 << 6
        };

        /// Asset load request information.
        struct LoadRequest
        {
            /// Asset path.
            AssetPath path;
            /// Cached object reference.
            AssetPtr spObject;

            /// Package loader.
            PackageLoader* pPackageLoader;
            /// Asset preload request ID.
            size_t packageLoadRequestId;

            /// Link table.
            DynamicArray< LinkEntry > linkTable;

            /// Loading status flags.
            volatile int32_t stateFlags;

            /// Number of load requests for this specific object.
            volatile int32_t requestCount;
        };
// 
//         /// Serializer for linking object references.
//         class HELIUM_ENGINE_API Linker : public Serializer
//         {
//         public:
//             /// @name Construction/Destruction
//             //@{
//             Linker();
//             virtual ~Linker();
//             //@}
// 
//             /// @name Serialization Control
//             //@{
//             void Prepare( const LinkEntry* pLinkEntries, uint32_t linkEntryCount );
//             //@}
// 
//             /// @name Serialization Interface
//             //@{
//             virtual bool Serialize( Asset* pObject );
//             virtual EMode GetMode() const;
// 
//             virtual void SerializeTag( const Tag& rTag );
//             virtual bool CanResolveTags() const;
// 
//             virtual void SerializeBool( bool& rValue );
//             virtual void SerializeInt8( int8_t& rValue );
//             virtual void SerializeUint8( uint8_t& rValue );
//             virtual void SerializeInt16( int16_t& rValue );
//             virtual void SerializeUint16( uint16_t& rValue );
//             virtual void SerializeInt32( int32_t& rValue );
//             virtual void SerializeUint32( uint32_t& rValue );
//             virtual void SerializeInt64( int64_t& rValue );
//             virtual void SerializeUint64( uint64_t& rValue );
//             virtual void SerializeFloat32( float32_t& rValue );
//             virtual void SerializeFloat64( float64_t& rValue );
//             virtual void SerializeBuffer( void* pBuffer, size_t elementSize, size_t count );
//             virtual void SerializeEnum( int32_t& rValue, uint32_t nameCount, const tchar_t* const* ppNames );
//             virtual void SerializeEnum( int32_t& rValue, const Helium::Reflect::Enumeration* pEnumeration );
//             virtual void SerializeName( Name& rValue );
//             virtual void SerializeString( String& rValue );
//             virtual void SerializeObjectReference( const AssetType* pType, AssetPtr& rspObject );
//             //@}
// 
//         private:
//             /// Link table.
//             const LinkEntry* m_pLinkEntries;
//             /// Number of link table entries.
//             uint32_t m_linkEntryCount;
// 
//             /// True if an invalid link table index was found.
//             bool m_bError;
//         };

        /// Information for deferred freeing of load requests.
        struct DeferredLoadRequestFree
        {
            /// FilePath of the object entry.
            AssetPath path;
            /// Load request instance.
            LoadRequest* pRequest;
        };

        /// Load request hash map.
        ConcurrentHashMap< AssetPath, LoadRequest* > m_loadRequestMap;
        /// Load request pool.
        ObjectPool< LoadRequest > m_loadRequestPool;

        /// Singleton instance.
        static AssetLoader* sm_pInstance;

        /// @name Loading Implementation
        //@{
        virtual PackageLoader* GetPackageLoader( AssetPath path ) = 0;
        virtual void TickPackageLoaders() = 0;

        virtual void OnPrecacheReady( Asset* pObject, PackageLoader* pPackageLoader );
        virtual void OnLoadComplete( AssetPath path, Asset* pObject, PackageLoader* pPackageLoader );
        //@}

    private:
        /// Asset cache name.
        Name m_cacheName;

        /// @name Load Process Updating
        //@{
        bool TickLoadRequest( LoadRequest* pRequest );
        bool TickPreload( LoadRequest* pRequest );
        bool TickLink( LoadRequest* pRequest );
        bool TickPrecache( LoadRequest* pRequest );
        bool TickFinalizeLoad( LoadRequest* pRequest );
        //@}
    };
}

#include "Engine/AssetLoader.inl"

#endif  // HELIUM_ENGINE_OBJECT_LOADER_H
