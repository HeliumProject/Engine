//----------------------------------------------------------------------------------------------------------------------
// ObjectPreprocessor.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_PC_SUPPORT_OBJECT_PREPROCESSOR_H
#define LUNAR_PC_SUPPORT_OBJECT_PREPROCESSOR_H

#include "PcSupport/PcSupport.h"

#include "Engine/Cache.h"

namespace Lunar
{
    class GameObject;
    class Resource;
    class PlatformPreprocessor;

    /// GameObject caching and resource preprocessing interface.
    class LUNAR_PC_SUPPORT_API ObjectPreprocessor : NonCopyable
    {
    public:
        /// @name Platform Preprocessor Registration
        //@{
        void SetPlatformPreprocessor( Cache::EPlatform platform, PlatformPreprocessor* pPreprocessor );
        inline PlatformPreprocessor* GetPlatformPreprocessor( Cache::EPlatform platform ) const;
        //@}

        /// @name GameObject Caching
        //@{
        bool CacheObject( GameObject* pObject, int64_t timestamp, bool bEvictPlatformPreprocessedResourceData = true );
        //@}

        /// @name Resource Preprocessing
        //@{
        void LoadResourceData( Resource* pResource, int64_t objectTimestamp );

        uint32_t LoadPersistentResourceData(
            GameObjectPath resourcePath, Cache::EPlatform platform, DynArray< uint8_t >& rPersistentDataBuffer );
        //@}

        /// @name Static Access
        //@{
        static ObjectPreprocessor* CreateStaticInstance();
        static void DestroyStaticInstance();

        static ObjectPreprocessor* GetStaticInstance();
       //@}

    private:
        /// Platform-specific preprocessing support.
        PlatformPreprocessor* m_pPlatformPreprocessors[ Cache::PLATFORM_MAX ];

        /// Singleton instance.
        static ObjectPreprocessor* sm_pInstance;

        /// @name Construction/Destruction
        //@{
        ObjectPreprocessor();
        ~ObjectPreprocessor();
        //@}

        /// @name Private Utility Functions
        //@{
#if L_EDITOR
        bool LoadCachedResourceData( Resource* pResource, Cache::EPlatform platform );
        bool PreprocessResource( Resource* pResource, const String& rSourceFilePath );
#endif
        //@}
    };
}

#include "PcSupport/ObjectPreprocessor.inl"

#endif  // LUNAR_PC_SUPPORT_OBJECT_PREPROCESSOR_H
