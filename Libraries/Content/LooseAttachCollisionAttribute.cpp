#include "LooseAttachCollisionAttribute.h"

namespace Content
{
  REFLECT_DEFINE_CLASS(LooseAttachCollisionAttribute); 

  void LooseAttachCollisionAttribute::EnumerateClass( Reflect::Compositor<LooseAttachCollisionAttribute>& comp )
  {
    comp.GetComposite().m_UIName = "Loose Attach Collision";
    Reflect::Field* elemCollisionTuids = comp.AddField( &LooseAttachCollisionAttribute::m_CollisionTuids, "m_CollisionTuids", Reflect::FieldFlags::NodeID );
    elemCollisionTuids->m_UIName = "Collision Tuids";
  }

  LooseAttachCollisionAttribute::LooseAttachCollisionAttribute()
  {
  }

  LooseAttachCollisionAttribute::~LooseAttachCollisionAttribute()
  {
  }
}
