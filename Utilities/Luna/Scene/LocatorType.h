#pragma once

#include "InstanceType.h"

#include "Content/Locator.h"

namespace Luna
{
  class Locator;
  class Primitive;
  class PrimitiveLocator;
  class PrimitiveCube;

  class LocatorType : public Luna::InstanceType
  {
    //
    // Members
    //

  private:
    // locator shapes
    Luna::PrimitiveLocator* m_Locator;
    Luna::PrimitiveCube* m_Cube;


    //
    // Runtime Type Info
    //

  public:
    LUNA_DECLARE_TYPE( Luna::LocatorType, Luna::InstanceType );
    static void InitializeType();
    static void CleanupType();


    //
    // Implementation
    //

  public:
    LocatorType( Luna::Scene* scene, i32 instanceType );

    virtual ~LocatorType();

    virtual void Create() NOC_OVERRIDE;
    virtual void Delete() NOC_OVERRIDE;

    virtual void SetConfiguration(const TypeConfigPtr& t) NOC_OVERRIDE;

    const Luna::Primitive* GetShape( Content::LocatorShape shape ) const;
  };
}
