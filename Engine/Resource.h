//----------------------------------------------------------------------------------------------------------------------
// Resource.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_ENGINE_RESOURCE_H
#define HELIUM_ENGINE_RESOURCE_H

#include "Engine/Asset.h"

#include "Engine/Cache.h"

namespace Helium
{
    /// Base interface for object types that correspond to a resource file.
    ///
    /// When dealing with object and resource data that has not been preprocessed (still stored in text packages and
    /// source asset files), objects of this type will correspond to a single resource file on disk.  Note that objects
    /// of this type may not actually hold onto the final processed resource data at runtime.
    class HELIUM_ENGINE_API Resource : public Asset
    {
        HELIUM_DECLARE_OBJECT( Resource, Asset );

    public:
#if HELIUM_TOOLS
        /// In-memory preprocessed resource data for a specific platform cache.
        ///
        /// For resources modified in-memory in the editor, this is used to keep track of preprocessed resource data
        /// that should be committed to the appropriate cache when the resource object is saved.  This makes it so that
        /// if the user decides not to save their changes, the existing cached data for the saved file is left intact.
        ///
        /// Each buffer represents the binary cached data in the exact format as it would be stored in the cache.
        struct PreprocessedData
        {
            /// Persistent resource data.
            DynamicArray< uint8_t > persistentDataBuffer;
            /// Non-persistent sub-resource data.
            /// pmd: Not sure that this is non-persitent anymore. See pResourceCache->CacheEntry call in ObjectPreprocessor::CacheObject
            DynamicArray< DynamicArray< uint8_t > > subDataBuffers;
            /// True if this data is loaded (even if the buffers are empty).
            bool bLoaded;
        };
#endif

        /// @name Construction/Destruction
        //@{
        Resource();
        virtual ~Resource();
        //@}

        /// @name Resource Serialization
        //@{
        //PMDTODO: Remove SerializePersistentResourceData and make LoadPersistentResourceObject pure virtual
        virtual void SerializePersistentResourceData( Serializer& s );
        virtual bool LoadPersistentResourceObject(Reflect::ObjectPtr &_object) { return false; }
        //@}

        /// @name Resource Caching Support
        //@{
        virtual Name GetCacheName() const;
        //@}

#if HELIUM_TOOLS
        /// @name Editor Support
        //@{
        inline PreprocessedData& GetPreprocessedData( Cache::EPlatform platform );
        inline const PreprocessedData& GetPreprocessedData( Cache::EPlatform platform ) const;
        //@}
#endif

    protected:
        /// @name Resource Loading Utility Functions
        //@{
        size_t GetSubDataSize( uint32_t subDataIndex ) const;
        size_t BeginLoadSubData( void* pBuffer, uint32_t subDataIndex, size_t loadSizeMax = Invalid< size_t >() );
        bool TryFinishLoadSubData( size_t loadId );
        //@}

    private:
#if HELIUM_TOOLS
        /// In-memory preprocessed resource data for each platform.
        PreprocessedData m_preprocessedData[ Cache::PLATFORM_MAX ];
#endif
    };
}

#include "Engine/Resource.inl"

#endif  // HELIUM_ENGINE_RESOURCE_H
