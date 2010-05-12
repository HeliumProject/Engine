#pragma once
#include "JointAttribute.h" 
#include "TUID/tuid.h"

namespace Content
{
  class CONTENT_API LooseAttachCollisionAttribute : public JointAttribute
  {
  public: 
    LooseAttachCollisionAttribute(); 
    ~LooseAttachCollisionAttribute(); 

    const V_tuid & GetCollisionTuids( )           { return m_CollisionTuids;  }
    void SetCollisionTuids( const V_tuid& tuids ) { m_CollisionTuids = tuids; }

  private: 
    V_tuid m_CollisionTuids;
    
    REFLECT_DECLARE_CLASS(LooseAttachCollisionAttribute, JointAttribute); 
    static void EnumerateClass( Reflect::Compositor<LooseAttachCollisionAttribute>& comp );
  }; 

  typedef Nocturnal::SmartPtr<LooseAttachCollisionAttribute> LooseAttachCollisionAttributePtr;
}
