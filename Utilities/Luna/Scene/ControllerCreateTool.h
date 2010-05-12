#pragma once

#include "API.h"
#include "Controller.h"
#include "CreateTool.h"

#include "TUID/TUID.h"

namespace Luna
{
  class ControllerCreateTool : public Luna::CreateTool
  {
    //
    // Members
    //

    //
    // RTTI
    //

    LUNA_DECLARE_TYPE(Luna::ControllerCreateTool, Luna::CreateTool);
    static void InitializeType();
    static void CleanupType();

  public:
    ControllerCreateTool(Luna::Scene* scene, Enumerator* enumerator);
    virtual ~ControllerCreateTool();

    virtual Luna::TransformPtr CreateNode() NOC_OVERRIDE;
  };
}
