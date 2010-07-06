#include "Pipeline/Content/Nodes/Skin.h"

using namespace Reflect;
using namespace Content;

REFLECT_DEFINE_CLASS(Skin)

void Skin::EnumerateClass( Reflect::Compositor<Skin>& comp )
{
  Reflect::Field* fieldMesh = comp.AddField( &Skin::m_Mesh, "m_Mesh" );
  Reflect::Field* fieldInfluences = comp.AddField( &Skin::m_Influences, "m_Influences" );
  Reflect::Field* fieldInfluenceObjectIDs = comp.AddField( &Skin::m_InfluenceObjectIDs, "m_InfluenceObjectIDs" );
  Reflect::Field* fieldInfluenceIndices = comp.AddField( &Skin::m_InfluenceIndices, "m_InfluenceIndices" );
}