#pragma once

#include "HierarchyNode.h"

namespace Content
{
  //
  // A general affine transformation node
  //

  class CONTENT_API Transform NOC_ABSTRACT : public HierarchyNode
  {
  public:
    // Scale factors
    Math::Vector3 m_Scale;

    // Rotation euler angles, in radians and XYZs
    Math::Vector3 m_Rotate;

    // Translation vector
    Math::Vector3 m_Translate;

    // Object matrix
    Math::Matrix4 m_ObjectTransform;

    // Global matrix
    Math::Matrix4 m_GlobalTransform;

    // Do we transform with our parent?
    bool m_InheritTransform;

    Transform ()
      : m_Scale (1.0f, 1.0f, 1.0f)
      , m_InheritTransform (true)
    {

    }

    Transform (const Nocturnal::TUID& id)
      : HierarchyNode (id)
      , m_Scale (1.0f, 1.0f, 1.0f)
      , m_InheritTransform (true)
    {

    }

    REFLECT_DECLARE_ABSTRACT(Transform, HierarchyNode);

    static void EnumerateClass( Reflect::Compositor<Transform>& comp );

    virtual void ResetTransform();
  };

  typedef Nocturnal::SmartPtr<Transform> TransformPtr;
}