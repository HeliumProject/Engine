#include "AssetManifest.h"

using namespace Reflect;
using namespace Asset;

REFLECT_DEFINE_ABSTRACT(AssetManifest);

void AssetManifest::EnumerateClass( Reflect::Compositor<AssetManifest>& comp )
{
  Reflect::Field* fieldBoundingBoxMin = comp.AddField( &AssetManifest::m_BoundingBoxMin, "m_BoundingBoxMin" );
  Reflect::Field* fieldBoundingBoxMax = comp.AddField( &AssetManifest::m_BoundingBoxMax, "m_BoundingBoxMax" );
}