#pragma once

#include "Luna/API.h"

#include "Foundation/Reflect/Serializers.h"
#include "AssetIndexData.h"

namespace Luna
{
    class AssetCacheEntry : public Reflect::Element
    {
    public:
        Nocturnal::Path m_Path;
        AssetIndexData m_IndexData;
        std::set< Nocturnal::Path > m_Dependencies;

    public:
        AssetCacheEntry();
        virtual ~AssetCacheEntry();

    public:
        REFLECT_DECLARE_ABSTRACT( AssetCacheEntry, Reflect::Element );
        static void EnumerateClass( Reflect::Compositor< AssetCacheEntry >& comp );
    };
}