#pragma once

#include "Pipeline/API.h"
#include "Pipeline/Content/Nodes/Transform/PivotTransform.h"

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
      info->AddElement(Cube, TXT( "Cube" ) );
      info->AddElement(Cylinder, TXT( "Cylinder" ) );
      info->AddElement(Sphere, TXT( "Sphere" ) );
      info->AddElement(Capsule, TXT( "Capsule" ) );
      info->AddElement(CapsuleChild, TXT( "CapsuleChild" ) );
      info->AddElement(CylinderChild, TXT( "CylinderChild" ) );
    }
  }

  typedef CollisionShapes::CollisionShape CollisionShape;

  class PIPELINE_API CollisionPrimitive : public PivotTransform
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