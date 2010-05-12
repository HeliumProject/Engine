#pragma once

#include "InstanceType.h"

namespace Luna
{
  class GameCamera;
  class Primitive;
  class PrimitiveCube;

  class GameCameraType : public Luna::InstanceType
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
    LUNA_DECLARE_TYPE( Luna::GameCameraType, Luna::InstanceType );
    static void InitializeType();
    static void CleanupType();


    //
    // Implementation
    //

  public:
    GameCameraType( Luna::Scene* scene, i32 instanceType );

    virtual ~GameCameraType();

    virtual void Create() NOC_OVERRIDE;
    virtual void Delete() NOC_OVERRIDE;

    virtual void SetConfiguration(const TypeConfigPtr& t) NOC_OVERRIDE;

    const Luna::PrimitiveCube* GetBounds() const
    {
      return m_Bounds;
    }
  };
}
