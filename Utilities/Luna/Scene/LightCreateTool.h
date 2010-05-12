#pragma once

#include "API.h"
#include "Light.h"
#include "CreateTool.h"

#include "TUID/TUID.h"

namespace Luna
{
  class LightCreateTool : public Luna::CreateTool
  {
    //
    // Members
    //

  public:
    static Content::LightType s_Type;

    //
    // RTTI
    //

    LUNA_DECLARE_TYPE(Luna::LightCreateTool, Luna::CreateTool);
    static void InitializeType();
    static void CleanupType();

  public:
    LightCreateTool(Luna::Scene* scene, Enumerator* enumerator);
    virtual ~LightCreateTool();

    virtual Luna::TransformPtr CreateNode() NOC_OVERRIDE;
    virtual void CreateProperties() NOC_OVERRIDE;

    int GetLightType() const;
    void SetLightType(int value);
  };
}
