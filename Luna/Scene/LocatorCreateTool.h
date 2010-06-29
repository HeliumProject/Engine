#pragma once

#include "Luna/API.h"
#include "Locator.h"
#include "CreateTool.h"

#include "Foundation/TUID.h"

namespace Luna
{
  class LocatorCreateTool : public Luna::CreateTool
  {
    //
    // Members
    //

  public:
    static Content::LocatorShape s_Shape;

    //
    // RTTI
    //

    LUNA_DECLARE_TYPE(Luna::LocatorCreateTool, Luna::CreateTool);
    static void InitializeType();
    static void CleanupType();

  public:
    LocatorCreateTool(Luna::Scene* scene, Enumerator* enumerator);

    virtual ~LocatorCreateTool();

    virtual Luna::TransformPtr CreateNode() NOC_OVERRIDE;

    virtual void CreateProperties() NOC_OVERRIDE;

    int GetLocatorShape() const;

    void SetLocatorShape(int value);
  };
}
