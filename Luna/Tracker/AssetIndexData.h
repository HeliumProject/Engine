#pragma once

#include "Luna/API.h"

#include "Foundation/Reflect/Serializers.h"

namespace Luna
{
    class AssetIndexData : public Reflect::Element
    {
    public:
        AssetIndexData();
        virtual ~AssetIndexData();

        void SetIndexData( const std::multimap< tstring, tstring >& indexData );
        const std::multimap< tstring, tstring > GetIndexData() const;

        void AddDataItem( const tstring& key, const tstring& value );
        bool GetDataItem( const tstring& key, tstring& value );
        bool GetDataItem( const tstring& key, std::set< tstring >& value );
        void RemoveDataItem( const tstring& key );
        
        bool HasData();
        
    private:
        std::multimap< tstring, tstring > m_IndexData;

    public:
        REFLECT_DECLARE_ABSTRACT( AssetIndexData, Reflect::Element );
        static void EnumerateClass( Reflect::Compositor< AssetIndexData >& comp );
    };
}
