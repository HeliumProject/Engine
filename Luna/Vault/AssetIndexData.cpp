#include "AssetIndexData.h"

using namespace Asset;

REFLECT_DEFINE_CLASS( AssetIndexData );

void AssetIndexData::EnumerateClass( Reflect::Compositor< AssetIndexData >& comp )
{
    comp.AddField( &AssetIndexData::m_IndexData, "m_IndexData" );
}
