#pragma once

#include "Pipeline/Content/Nodes/Transform.h"

namespace Content
{
  class PIPELINE_API JointTransform : public Transform
  {
  public:
    bool m_SegmentScaleCompensate;

    JointTransform ()
      : m_SegmentScaleCompensate( false )
    {

    }

    JointTransform (const Nocturnal::TUID& id)
      : Transform (id)
      , m_SegmentScaleCompensate( false )
    {

    }

    REFLECT_DECLARE_CLASS(JointTransform, Transform);

    static void EnumerateClass( Reflect::Compositor<JointTransform>& comp );

    virtual void ResetTransform() NOC_OVERRIDE;
  };

  typedef Nocturnal::SmartPtr<JointTransform> JointTransformPtr;
  typedef std::vector<JointTransformPtr> V_JointTransform;
  typedef std::vector<V_JointTransform> VV_JointTransform;
  typedef std::set<JointTransformPtr> S_JointTransform;
}