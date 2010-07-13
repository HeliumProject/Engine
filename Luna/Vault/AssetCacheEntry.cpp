#include "Precompile.h"
#include "AssetCacheEntry.h"

using namespace Luna;

REFLECT_DEFINE_CLASS( AssetCacheEntry );

void AssetCacheEntry::EnumerateClass( Reflect::Compositor< AssetCacheEntry >& comp )
{
    comp.AddField( &AssetCacheEntry::m_Path, "m_Path" );
    comp.AddField( &AssetCacheEntry::m_IndexData, "m_IndexData" );
    comp.AddField( &AssetCacheEntry::m_Dependencies, "m_Dependencies" );
}

AssetCacheEntry::AssetCacheEntry()
{
}

AssetCacheEntry::~AssetCacheEntry()
{
}