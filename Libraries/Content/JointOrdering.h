#pragma once

#include "API.h"

#include "SceneNode.h"

namespace Content
{
  class CONTENT_API JointOrdering : public SceneNode
  {
  public:

    JointOrdering() : SceneNode() {}
    JointOrdering( u32 numRequiredJoints );

    Nocturnal::UID::V_TUID m_JointOrdering;
    Nocturnal::UID::HM_TUID m_MasterToLocalMap;
    Nocturnal::UID::HM_TUID m_LocalToMasterMap;

    REFLECT_DECLARE_CLASS( JointOrdering, SceneNode );

    static void EnumerateClass( Reflect::Compositor<JointOrdering>& comp );

    void SetRequiredJointCount( u32 requiredJointCount );
    u32  GetRequiredJointCount();

    void AddJoint( const Nocturnal::UID::TUID& jointId );
    void Clear();

    Nocturnal::UID::TUID GetMasterJoint( const Nocturnal::UID::TUID& localJoint );
    Nocturnal::UID::TUID GetLocalJoint( const Nocturnal::UID::TUID& masterJoint );

    bool IsRequired( const Nocturnal::UID::TUID& jointId );
  };

  typedef Nocturnal::SmartPtr< JointOrdering > JointOrderingPtr;
  typedef std::vector< JointOrderingPtr > V_JointOrdering;
}