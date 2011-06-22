#include "PipelinePch.h"
#include "EntityManifest.h"

using namespace Helium;
using namespace Helium::Asset;

REFLECT_DEFINE_OBJECT(EntityManifest);

void EntityManifest::PopulateComposite( Reflect::Composite& comp )
{
  comp.AddField( &EntityManifest::m_Shaders, TXT( "m_Shaders" ) );
  comp.AddField( &EntityManifest::m_BlendTextures, TXT( "m_BlendTextures" ) );
  comp.AddField( &EntityManifest::m_TriangleCount, TXT( "m_TriangleCount" ) );
  comp.AddField( &EntityManifest::m_MentalRayShaderCount, TXT( "m_MentalRayShaderCount" ) );
  comp.AddField( &EntityManifest::m_LightMapped, TXT( "m_LightMapped" ) );
  comp.AddField( &EntityManifest::m_LooseTextures, TXT( "m_LooseTextures" ) );
}
