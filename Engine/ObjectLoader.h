//----------------------------------------------------------------------------------------------------------------------
// ObjectLoader.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_ENGINE_OBJECT_LOADER_H
#define LUNAR_ENGINE_OBJECT_LOADER_H

#include "Engine/Engine.h"

#include "Foundation/Container/ConcurrentHashMap.h"
#include "Foundation/Container/ObjectPool.h"
#include "Engine/ObjectPath.h"
#include "Engine/Serializer.h"

namespace Lunar
{
    class PackageLoader;

    /// Asynchronous object loading interface
    class LUNAR_ENGINE_API ObjectLoader : NonCopyable
    {
    public:
        /// Number of request objects to allocate in each block of the request pool.
        static const size_t LOAD_REQUEST_POOL_BLOCK_SIZE = 64;

        /// Object link table entry.
        struct LinkEntry
        {
            /// Load request ID.
            size_t loadId;
            /// Cached object reference.
            ObjectPtr spObject;
        };

        /// @name Construction/Destruction
        //@{
        ObjectLoader();
        virtual ~ObjectLoader() = 0;
        //@}

        /// @name Loading Interface
        //@{
        virtual size_t BeginLoadObject( ObjectPath path );
        virtual bool TryFinishLoad( size_t id, ObjectPtr& rspObject );
        void FinishLoad( size_t id, ObjectPtr& rspObject );

        bool LoadObject( ObjectPath path, ObjectPtr& rspObject );

#if L_EDITOR
        virtual bool CacheObject( Object* pObject, bool bEvictPlatformPreprocessedResourceData = true );
#endif

        virtual void Tick();
        //@}

        /// @name Data Access
        //@{
        inline Name GetCacheName() const;
        //@}

        /// @name Static Access
        //@{
        static ObjectLoader* GetStaticInstance();
        static void DestroyStaticInstance();
        //@}

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

        /// Object load request information.
        struct LoadRequest
        {
            /// Object path.
            ObjectPath path;
            /// Cached object reference.
            ObjectPtr spObject;

            /// Package loader.
            PackageLoader* pPackageLoader;
            /// Object preload request ID.
            size_t packageLoadRequestId;

            /// Link table.
            DynArray< LinkEntry > linkTable;

            /// Loading status flags.
            volatile int32_t stateFlags;

            /// Number of load requests for this specific object.
            volatile int32_t requestCount;
        };

        /// Serializer for linking object references.
        class LUNAR_ENGINE_API Linker : public Serializer
        {
        public:
            /// @name Construction/Destruction
            //@{
            Linker();
            virtual ~Linker();
            //@}

            /// @name Serialization Control
            //@{
            void Prepare( const LinkEntry* pLinkEntries, uint32_t linkEntryCount );
            //@}

            /// @name Serialization Interface
            //@{
            virtual bool Serialize( Object* pObject );
            virtual EMode GetMode() const;

            virtual void SerializeTag( const Tag& rTag );
            virtual bool CanResolveTags() const;

            virtual void SerializeBool( bool& rValue );
            virtual void SerializeInt8( int8_t& rValue );
            virtual void SerializeUint8( uint8_t& rValue );
            virtual void SerializeInt16( int16_t& rValue );
            virtual void SerializeUint16( uint16_t& rValue );
            virtual void SerializeInt32( int32_t& rValue );
            virtual void SerializeUint32( uint32_t& rValue );
            virtual void SerializeInt64( int64_t& rValue );
            virtual void SerializeUint64( uint64_t& rValue );
            virtual void SerializeFloat32( float32_t& rValue );
            virtual void SerializeFloat64( float64_t& rValue );
            virtual void SerializeBuffer( void* pBuffer, size_t elementSize, size_t count );
            virtual void SerializeEnum( int32_t& rValue, uint32_t nameCount, const tchar_t* const* ppNames );
            virtual void SerializeCharName( CharName& rValue );
            virtual void SerializeWideName( WideName& rValue );
            virtual void SerializeCharString( CharString& rValue );
            virtual void SerializeWideString( WideString& rValue );
            virtual void SerializeObjectReference( Type* pType, ObjectPtr& rspObject );
            //@}

        private:
            /// Link table.
            const LinkEntry* m_pLinkEntries;
            /// Number of link table entries.
            uint32_t m_linkEntryCount;

            /// True if an invalid link table index was found.
            bool m_bError;
        };

        /// Information for deferred freeing of load requests.
        struct DeferredLoadRequestFree
        {
            /// Path of the object entry.
            ObjectPath path;
            /// Load request instance.
            LoadRequest* pRequest;
        };

        /// Load request hash map.
        ConcurrentHashMap< ObjectPath, LoadRequest* > m_loadRequestMap;
        /// Load request pool.
        ObjectPool< LoadRequest > m_loadRequestPool;
        /// List of load requests to update in the current tick.
        DynArray< LoadRequest* > m_loadRequestTickArray;

        /// Singleton instance.
        static ObjectLoader* sm_pInstance;

        /// @name Loading Implementation
        //@{
        virtual PackageLoader* GetPackageLoader( ObjectPath path ) = 0;
        virtual void TickPackageLoaders() = 0;

        virtual void OnPrecacheReady( Object* pObject, PackageLoader* pPackageLoader );
        virtual void OnLoadComplete( ObjectPath path, Object* pObject, PackageLoader* pPackageLoader );
        //@}

    private:
        /// Object cache name.
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

#include "Engine/ObjectLoader.inl"

#endif  // LUNAR_ENGINE_OBJECT_LOADER_H
