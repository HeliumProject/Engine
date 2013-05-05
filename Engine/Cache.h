//----------------------------------------------------------------------------------------------------------------------
// Cache.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_ENGINE_CACHE_H
#define HELIUM_ENGINE_CACHE_H

#include "Engine/Engine.h"

#include "Foundation/ConcurrentHashMap.h"
#include "Foundation/ObjectPool.h"
#include "Engine/AssetPath.h"
#include "Reflect/Object.h"

namespace Helium
{
    /// Serialization cache interface.
    class HELIUM_ENGINE_API Cache : NonCopyable
    {
    public:
        /// Current cache file format version number.
        static const uint32_t sm_Version = 0;

        /// Default Entry pool block size (for use with modifiable caches on the PC).
        static const size_t ENTRY_POOL_BLOCK_SIZE = 64;

        /// Cache platforms.
        enum EPlatform
        {
            PLATFORM_FIRST   =  0,
            PLATFORM_INVALID = -1,

            /// PC (general).
            PLATFORM_PC,

            PLATFORM_MAX,
            PLATFORM_LAST = PLATFORM_MAX - 1
        };

        /// Cache entry information.  Note that the members of this struct are organized as such so as to reduce memory
        /// overhead from padding each value.
        struct Entry
        {
            /// Entry offset.
            uint64_t offset;
            /// Entry timestamp.
            int64_t timestamp;

            /// Entry path name.
            AssetPath path;
            /// Sub-data index.
            uint32_t subDataIndex;

            /// Entry size.
            uint32_t size;
        };

        /// @name Construction/Destruction
        //@{
        Cache();
        ~Cache();
        //@}

        /// @name Initialization
        //@{
        bool Initialize( Name name, EPlatform platform, const tchar_t* pTocFileName, const tchar_t* pCacheFileName );
        void Shutdown();
        //@}

        /// @name Saving/Loading
        //@{
        bool BeginLoadToc();
        bool TryFinishLoadToc();
        inline bool IsTocLoaded() const;

        void EnforceTocLoad();
        //@}

        /// @name Data Access
        //@{
        inline Name GetName() const;
        inline EPlatform GetPlatform() const;

        inline const String& GetTocFileName() const;
        inline const String& GetCacheFileName() const;

        inline uint32_t GetEntryCount() const;
        inline const Entry& GetEntry( uint32_t index ) const;
        const Entry* FindEntry( AssetPath path, uint32_t subDataIndex ) const;

        bool CacheEntry( AssetPath path, uint32_t subDataIndex, const void* pData, int64_t timestamp, uint32_t size );
        //@}

#if HELIUM_TOOLS
        static void WriteCacheObjectToBuffer( Helium::Reflect::Object &_object, DynamicArray< uint8_t > &_buffer );
#endif
        static Reflect::ObjectPtr ReadCacheObjectFromBuffer( const DynamicArray< uint8_t > &_buffer );
        static Reflect::ObjectPtr ReadCacheObjectFromBuffer( const uint8_t *_buffer, const size_t _offset, const size_t _count );

    private:
        /// Value read callback.
        typedef void ( LOAD_VALUE_CALLBACK )( void* pDestination, const void* pSource, size_t byteCount );

        /// Asset entry key.
        struct EntryKey
        {
            /// Asset path.
            AssetPath path;
            /// Sub-data index.
            uint32_t subDataIndex;

            /// @name Overloaded Operators
            //@{
            bool operator==( const EntryKey& rOther ) const;
            //@}
        };

        /// Asset entry key hasher.
        class EntryKeyHash
        {
        public:
            /// @name Hash Calculation
            //@{
            size_t operator()( const EntryKey& rKey ) const;
            //@}
        };

        /// Cache entry hash map type.
        typedef ConcurrentHashMap< EntryKey, Entry*, EntryKeyHash > EntryMapType;

        /// Cache name.
        Name m_name;
        /// Cache platform.
        EPlatform m_platform;

        /// Table of contents file name.
        String m_tocFileName;
        /// Cache file name.
        String m_cacheFileName;

        /// True if a TOC load request has been fully processed and synced (not indicative of whether the cache files
        /// actually exist, though).
        bool m_bTocLoaded;

        /// Asynchronous TOC load ID.
        size_t m_asyncLoadId;
        /// Allocated buffer for asynchronous TOC loading.
        uint8_t* m_pTocBuffer;
        /// Size of the TOC, in bytes.
        uint32_t m_tocSize;

        /// Cache entry pool.
        ObjectPool< Entry >* m_pEntryPool;
        /// Cache entry information.
        DynamicArray< Entry* > m_entries;
        /// Entry lookup hash map.
        EntryMapType m_entryMap;

        /// @name Loading Utility Functions
        //@{
        bool FinalizeTocLoad();
        //@}

        /// @name Private Static Utility Functions
        //@{
        template< typename T > static bool CheckedTocRead(
            LOAD_VALUE_CALLBACK* pLoadFunction, T& rValue, const tchar_t* pDescription, const uint8_t*& rpTocCurrent,
            const uint8_t* pTocMax );
        //@}
    };
}

#include "Engine/Cache.inl"

#endif  // HELIUM_ENGINE_CACHE_H
