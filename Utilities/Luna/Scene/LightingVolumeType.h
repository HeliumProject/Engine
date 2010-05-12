#pragma once

#include "InstanceType.h"

namespace Luna
{
  class LightingVolume;
  class Primitive;
  class PrimitivePointer;
  class PrimitiveCube;
  
  class LightingVolumeType : public Luna::InstanceType
  {
    //
    // Members
    //

  private:
    // volume shapes
    Luna::PrimitiveCube* m_Cube;
   
    //
    // Runtime Type Info
    //

  public:
    LUNA_DECLARE_TYPE( Luna::LightingVolumeType, Luna::InstanceType );
    static void InitializeType();
    static void CleanupType();


    //
    // Implementation
    //

  public:
    LightingVolumeType( Luna::Scene* scene, i32 instanceType );

    virtual ~LightingVolumeType();

    virtual void Create() NOC_OVERRIDE;

    virtual void Delete() NOC_OVERRIDE;

    const Luna::PrimitiveCube* GetCube() const
    {
      return m_Cube;
    }
  };
}
