#pragma once

#include "Pipeline/API.h"

#include "Foundation/Reflect/Serializers.h"

namespace Asset
{
    class PIPELINE_API AssetIndexData : public Reflect::Element
    {
    private:

        std::map< tstring, tstring > m_IndexData;

    public:
        //
        // RTTI
        //
        REFLECT_DECLARE_ABSTRACT( AssetIndexData, Reflect::Element );

        static void EnumerateClass( Reflect::Compositor< AssetIndexData >& comp );

    public:

        AssetIndexData()
        {
        }

        virtual ~AssetIndexData()
        {
        }

        const std::map< tstring, tstring > GetIndexData() const
        {
            return m_IndexData;
        }
        
        void SetIndexData( const std::map< tstring, tstring >& indexData )
        {
            m_IndexData = indexData;
        }

        void AddDataItem( const tstring& key, const tstring& value )
        {
            m_IndexData[ key ] = value;
        }
        
        void RemoveDataItem( const tstring& key )
        {
            m_IndexData.erase( key );
        }

        bool GetDataItem( const tstring& key, tstring& value )
        {
            std::map< tstring, tstring >::const_iterator itr = m_IndexData.find( key );
            if ( itr != m_IndexData.end() )
            {
                value = (*itr).second;
                return true;
            }

            return false;
        }

        bool HasData()
        {
            return m_IndexData.size() > 0;
        }


    };
}