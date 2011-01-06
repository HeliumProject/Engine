#include "AssetManifest.h"

using namespace Helium;
using namespace Helium::Asset;

REFLECT_DEFINE_ABSTRACT(AssetManifest);

void AssetManifest::AcceptCompositeVisitor( Reflect::Composite& comp )
{
  Reflect::Field* fieldBoundingBoxMin = comp.AddField( &AssetManifest::m_BoundingBoxMin, "m_BoundingBoxMin" );
  Reflect::Field* fieldBoundingBoxMax = comp.AddField( &AssetManifest::m_BoundingBoxMax, "m_BoundingBoxMax" );
}