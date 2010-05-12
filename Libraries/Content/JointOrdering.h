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

    UniqueID::V_TUID m_JointOrdering;
    UniqueID::HM_TUID m_MasterToLocalMap;
    UniqueID::HM_TUID m_LocalToMasterMap;

    REFLECT_DECLARE_CLASS( JointOrdering, SceneNode );

    static void EnumerateClass( Reflect::Compositor<JointOrdering>& comp );

    void SetRequiredJointCount( u32 requiredJointCount );
    u32  GetRequiredJointCount();

    void AddJoint( const UniqueID::TUID& jointId );
    void Clear();

    UniqueID::TUID GetMasterJoint( const UniqueID::TUID& localJoint );
    UniqueID::TUID GetLocalJoint( const UniqueID::TUID& masterJoint );

    bool IsRequired( const UniqueID::TUID& jointId );
  };

  typedef Nocturnal::SmartPtr< JointOrdering > JointOrderingPtr;
  typedef std::vector< JointOrderingPtr > V_JointOrdering;
}