#pragma once

#include "InstanceType.h"

#include "Pipeline/Content/Nodes/Instance/Volume.h"

namespace Luna
{
  class Volume;
  class Primitive;
  class PrimitivePointer;
  class PrimitiveCube;
  class PrimitiveCylinder;
  class PrimitiveSphere;
  class PrimitiveCapsule;

  class VolumeType : public Luna::InstanceType
  {
    //
    // Members
    //

  private:
    // volume shapes
    Luna::PrimitiveCube* m_Cube;
    Luna::PrimitiveCylinder* m_Cylinder;
    Luna::PrimitiveSphere* m_Sphere;
    Luna::PrimitiveCapsule* m_Capsule;


    //
    // Runtime Type Info
    //

  public:
    LUNA_DECLARE_TYPE( Luna::VolumeType, Luna::InstanceType );
    static void InitializeType();
    static void CleanupType();


    //
    // Implementation
    //

  public:
    VolumeType( Luna::Scene* scene, i32 instanceType );

    virtual ~VolumeType();

    virtual void Create() NOC_OVERRIDE;
    virtual void Delete() NOC_OVERRIDE;

    virtual void SetConfiguration(const TypeConfigPtr& t) NOC_OVERRIDE;

    const Luna::Primitive* GetShape( Content::VolumeShape shape ) const;
  };
}
