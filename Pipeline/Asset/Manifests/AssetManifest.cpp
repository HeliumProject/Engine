#include "AssetManifest.h"

using namespace Helium;
using namespace Helium::Asset;

REFLECT_DEFINE_ABSTRACT(AssetManifest);

void AssetManifest::PopulateComposite( Reflect::Composite& comp )
{
    comp.AddField( &AssetManifest::m_BoundingBoxMin, TXT( "m_BoundingBoxMin" ) );
    comp.AddField( &AssetManifest::m_BoundingBoxMax, TXT( "m_BoundingBoxMax" ) );
}