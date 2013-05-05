//----------------------------------------------------------------------------------------------------------------------
// AssetPreprocessor.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_PC_SUPPORT_OBJECT_PREPROCESSOR_H
#define HELIUM_PC_SUPPORT_OBJECT_PREPROCESSOR_H

#include "PcSupport/PcSupport.h"

#include "Engine/Cache.h"

namespace Helium
{
    class Asset;
    class Resource;
    class PlatformPreprocessor;

    /// Asset caching and resource preprocessing interface.
    class HELIUM_PC_SUPPORT_API AssetPreprocessor : NonCopyable
    {
    public:
        /// @name Platform Preprocessor Registration
        //@{
        void SetPlatformPreprocessor( Cache::EPlatform platform, PlatformPreprocessor* pPreprocessor );
        inline PlatformPreprocessor* GetPlatformPreprocessor( Cache::EPlatform platform ) const;
        //@}

        /// @name Asset Caching
        //@{
        bool CacheObject( Asset* pObject, int64_t timestamp, bool bEvictPlatformPreprocessedResourceData = true );
        //@}

        /// @name Resource Preprocessing
        //@{
        void LoadResourceData( Resource* pResource, int64_t objectTimestamp );
        //@}

        /// @name Static Access
        //@{
        static AssetPreprocessor* CreateStaticInstance();
        static void DestroyStaticInstance();

        static AssetPreprocessor* GetStaticInstance();
       //@}

    private:
        /// Platform-specific preprocessing support.
        PlatformPreprocessor* m_pPlatformPreprocessors[ Cache::PLATFORM_MAX ];

        /// Singleton instance.
        static AssetPreprocessor* sm_pInstance;

        /// @name Construction/Destruction
        //@{
        AssetPreprocessor();
        ~AssetPreprocessor();
        //@}

        /// @name Private Utility Functions
        //@{
#if HELIUM_TOOLS
        bool LoadCachedResourceData( Resource* pResource, Cache::EPlatform platform );
        bool PreprocessResource( Resource* pResource, const String& rSourceFilePath );

        uint32_t LoadPersistentResourceData(
            AssetPath resourcePath, Cache::EPlatform platform, DynamicArray< uint8_t >& rPersistentDataBuffer );
#endif
        //@}
    };
}

#include "PcSupport/AssetPreprocessor.inl"

#endif  // HELIUM_PC_SUPPORT_OBJECT_PREPROCESSOR_H
