#pragma once

#include "API.h"
#include "GameCamera.h"
#include "CreateTool.h"

#include "TUID/TUID.h"

namespace Luna
{
  class GameCameraCreateTool : public Luna::CreateTool
  {
    //
    // Members
    //

    //
    // RTTI
    //

    LUNA_DECLARE_TYPE(Luna::GameCameraCreateTool, Luna::CreateTool);
    static void InitializeType();
    static void CleanupType();

  public:
    GameCameraCreateTool(Luna::Scene* scene, Enumerator* enumerator);
    virtual ~GameCameraCreateTool();

    virtual Luna::TransformPtr CreateNode() NOC_OVERRIDE;
  };
}
