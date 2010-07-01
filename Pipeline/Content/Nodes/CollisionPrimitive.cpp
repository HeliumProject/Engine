#include "Pipeline/Content/Nodes/CollisionPrimitive.h"

using namespace Content;

REFLECT_DEFINE_CLASS(CollisionPrimitive);

void CollisionPrimitive::EnumerateClass( Reflect::Compositor<CollisionPrimitive>& comp )
{
  Reflect::EnumerationField* enumShape = comp.AddEnumerationField( &CollisionPrimitive::m_Shape, "m_Shape" );
  Reflect::Field* fieldChildID = comp.AddField( &CollisionPrimitive::m_ChildID, "m_ChildID" );
}