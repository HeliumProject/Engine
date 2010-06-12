#pragma once

#include "API.h"
#include "PivotTransform.h"

namespace Content
{
  namespace CollisionShapes
  {
    enum CollisionShape
    {
      Cube,
      Cylinder,
      Sphere,
      Capsule,
      CapsuleChild,
      CylinderChild,
    };
    static void CollisionShapeEnumerateEnumeration( Reflect::Enumeration* info )
    {
      info->AddElement(Cube, "Cube");
      info->AddElement(Cylinder, "Cylinder");
      info->AddElement(Sphere, "Sphere");
      info->AddElement(Capsule, "Capsule");
      info->AddElement(CapsuleChild, "CapsuleChild");
      info->AddElement(CylinderChild, "CylinderChild");
    }
  }

  typedef CollisionShapes::CollisionShape CollisionShape;

  class CONTENT_API CollisionPrimitive : public PivotTransform
  {
    //
    // Members
    //

  public:
    CollisionShape m_Shape;
    Nocturnal::TUID m_ChildID;      // this is used to match the child sphere of a capsule to the parent sphere

    //
    // RTTI
    //

  public:
    REFLECT_DECLARE_CLASS(CollisionPrimitive, PivotTransform);

    static void EnumerateClass( Reflect::Compositor<CollisionPrimitive>& comp );

    //
    // Implementation
    //

  public:
    CollisionPrimitive()
      : m_Shape (CollisionShapes::Cube)
    {

    }

    CollisionPrimitive (const Nocturnal::TUID& id)
      : PivotTransform (id)
      , m_Shape (CollisionShapes::Cube)
    {

    }
  };

  typedef Nocturnal::SmartPtr<CollisionPrimitive> CollisionPrimitivePtr;
  typedef std::vector<CollisionPrimitivePtr> V_CollisionPrimitive;
}