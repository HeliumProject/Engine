#include "EntityManifest.h"

using namespace Helium;
using namespace Helium::Asset;

REFLECT_DEFINE_CLASS(EntityManifest);

void EntityManifest::AcceptCompositeVisitor( Reflect::Composite& comp )
{
  Reflect::Field* fieldShaders = comp.AddField( &EntityManifest::m_Shaders, "m_Shaders" );
  Reflect::Field* fieldBlendTextures = comp.AddField( &EntityManifest::m_BlendTextures, "m_BlendTextures" );
  Reflect::Field* fieldTriangleCount = comp.AddField( &EntityManifest::m_TriangleCount, "m_TriangleCount" );
  Reflect::Field* fieldMentalRayShaderCount = comp.AddField( &EntityManifest::m_MentalRayShaderCount, "m_MentalRayShaderCount" );
  Reflect::Field* fieldLightMapped = comp.AddField( &EntityManifest::m_LightMapped, "m_LightMapped" );
  Reflect::Field* fieldLooseTextures  = comp.AddField( &EntityManifest::m_LooseTextures, "m_LooseTextures" );
}
