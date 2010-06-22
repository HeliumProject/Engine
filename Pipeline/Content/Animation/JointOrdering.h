#pragma once

#include "Pipeline/API.h"

#include "Pipeline/Content/Nodes/SceneNode.h"

namespace Content
{
  class PIPELINE_API JointOrdering : public SceneNode
  {
  public:

    JointOrdering() : SceneNode() {}
    JointOrdering( u32 numRequiredJoints );

    Nocturnal::V_TUID m_JointOrdering;
    Nocturnal::HM_TUID m_MasterToLocalMap;
    Nocturnal::HM_TUID m_LocalToMasterMap;

    REFLECT_DECLARE_CLASS( JointOrdering, SceneNode );

    static void EnumerateClass( Reflect::Compositor<JointOrdering>& comp );

    void SetRequiredJointCount( u32 requiredJointCount );
    u32  GetRequiredJointCount();

    void AddJoint( const Nocturnal::TUID& jointId );
    void Clear();

    Nocturnal::TUID GetMasterJoint( const Nocturnal::TUID& localJoint );
    Nocturnal::TUID GetLocalJoint( const Nocturnal::TUID& masterJoint );

    bool IsRequired( const Nocturnal::TUID& jointId );
  };

  typedef Nocturnal::SmartPtr< JointOrdering > JointOrderingPtr;
  typedef std::vector< JointOrderingPtr > V_JointOrdering;
}