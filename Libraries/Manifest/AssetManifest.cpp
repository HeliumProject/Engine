#include "AssetManifest.h"

using namespace Reflect;
using namespace Manifest;

REFLECT_DEFINE_CLASS(AssetManifest)

void AssetManifest::EnumerateClass( Reflect::Compositor<AssetManifest>& comp )
{
  Reflect::Field* fieldShaders = comp.AddField( &AssetManifest::m_Shaders, "m_Shaders" );
  Reflect::Field* fieldBlendTextures = comp.AddField( &AssetManifest::m_BlendTextures, "m_BlendTextures" );
  Reflect::Field* fieldBoundingBoxMin = comp.AddField( &AssetManifest::m_BoundingBoxMin, "m_BoundingBoxMin" );
  Reflect::Field* fieldBoundingBoxMax = comp.AddField( &AssetManifest::m_BoundingBoxMax, "m_BoundingBoxMax" );
  Reflect::Field* fieldTriangleCount = comp.AddField( &AssetManifest::m_TriangleCount, "m_TriangleCount" );
  Reflect::Field* fieldMentalRayShaderCount = comp.AddField( &AssetManifest::m_MentalRayShaderCount, "m_MentalRayShaderCount" );
  Reflect::Field* fieldLightMapped = comp.AddField( &AssetManifest::m_LightMapped, "m_LightMapped" );
}