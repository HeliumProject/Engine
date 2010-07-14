#include "Precompile.h"
#include "AssetIndexData.h"

using namespace Luna;

REFLECT_DEFINE_CLASS( AssetIndexData );
void AssetIndexData::EnumerateClass( Reflect::Compositor< AssetIndexData >& comp )
{
    comp.AddField( &AssetIndexData::m_IndexData, "m_IndexData" );
}

AssetIndexData::AssetIndexData()
{
}

AssetIndexData::~AssetIndexData()
{
}

void AssetIndexData::SetIndexData( const std::multimap< tstring, tstring >& indexData )
{
    m_IndexData = indexData;
}

const std::multimap< tstring, tstring > AssetIndexData::GetIndexData() const
{
    return m_IndexData;
}

void AssetIndexData::AddDataItem( const tstring& key, const tstring& value )
{
    m_IndexData.insert( std::multimap< tstring, tstring >::value_type( key, value ) );
}

bool AssetIndexData::GetDataItem( const tstring& key, tstring& value )
{
    bool result = false;

    std::multimap< tstring, tstring >::const_iterator lower = m_IndexData.lower_bound( key );
    std::multimap< tstring, tstring >::const_iterator upper = m_IndexData.upper_bound( key );

    if ( lower != upper && lower != m_IndexData.end() )
    {
        value = (*lower).second;
        result = true;
    }

    return result;
}

bool AssetIndexData::GetDataItem( const tstring& key, std::set< tstring >& value )
{
    bool result = false;

    std::multimap< tstring, tstring >::const_iterator lower = m_IndexData.lower_bound( key );
    std::multimap< tstring, tstring >::const_iterator upper = m_IndexData.upper_bound( key );

    while ( lower != upper && lower != m_IndexData.end() )
    {
        value.insert( (*lower).second );
        result = true;
        ++lower;
    }

    return result;
}

void AssetIndexData::RemoveDataItem( const tstring& key )
{
    m_IndexData.erase( key );
}

bool AssetIndexData::HasData()
{
    return m_IndexData.size() > 0;
}

