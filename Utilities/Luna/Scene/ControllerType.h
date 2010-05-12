#pragma once

#include "InstanceType.h"

namespace Luna
{
  class Controller;
  class Primitive;
  class PrimitiveCube;

  class ControllerType : public Luna::InstanceType
  {
    //
    // Members
    //

  private:
    // volume shapes
    Luna::PrimitiveCube* m_Bounds;


    //
    // Runtime Type Info
    //

  public:
    LUNA_DECLARE_TYPE( Luna::ControllerType, Luna::InstanceType );
    static void InitializeType();
    static void CleanupType();


    //
    // Implementation
    //

  public:
    ControllerType( Luna::Scene* scene, i32 instanceType );

    virtual ~ControllerType();

    virtual void Create() NOC_OVERRIDE;
    virtual void Delete() NOC_OVERRIDE;

    virtual void SetConfiguration(const TypeConfigPtr& t) NOC_OVERRIDE;

    const Luna::PrimitiveCube* GetBounds() const
    {
      return m_Bounds;
    }
  };
}
