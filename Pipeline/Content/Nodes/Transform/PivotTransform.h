#pragma once

#include "Pipeline/Content/Nodes/Transform/Transform.h"

namespace Content
{
  //
  // Pivot transform node adds:
  //  - compound pivot transforms
  //  - shear
  //
  // [T] = [Sp]-1x[S]x[Sh]x[Sp]x[Spt]x[Rp]-1x[R]x[Rp]x[Rpt]x[T]
  //

  class PIPELINE_API PivotTransform : public Transform
  {
  public:
    // The shear values
    Math::Shear m_Shear;

    // The scale pivot translation vector
    Math::Vector3 m_ScalePivot;

    // The compensation vector for preserving the transform when the scale pivot it moved
    Math::Vector3 m_ScalePivotTranslate;

    // The rotation pivot translation vector
    Math::Vector3 m_RotatePivot;

    // The compensation vector for preserving the transform when the rotation pivot it moved
    Math::Vector3 m_RotatePivotTranslate;

    // The translate pivot translation vector
    Math::Vector3 m_TranslatePivot;

    // If this is true, use m_RotatePivot
    bool m_SnapPivots;

    PivotTransform ()
      : m_SnapPivots (true)
    {

    }

    PivotTransform (const Nocturnal::TUID& id)
      : Transform (id)
      , m_SnapPivots (true)
    {

    }

    REFLECT_DECLARE_CLASS(PivotTransform, Transform);

    static void EnumerateClass( Reflect::Compositor<PivotTransform>& comp );

    virtual bool ProcessComponent(Reflect::ElementPtr element, const tstring& memberName) NOC_OVERRIDE;
    virtual void ResetTransform() NOC_OVERRIDE;
  };

  typedef Nocturnal::SmartPtr<PivotTransform> PivotTransformPtr;
  typedef std::vector<PivotTransformPtr> V_PivotTransform;
}