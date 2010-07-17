#pragma once

#include "Luna/API.h"

#include "Scene.h"

#include "Tool.h"

namespace Luna
{
  class Scene;

  class SceneTool NOC_ABSTRACT : public Luna::Tool
  {
    //
    // Members
    //

  protected:
    // The scene to edit
    Luna::Scene* m_Scene;


    //
    // RTTI
    //

    LUNA_DECLARE_TYPE(Luna::SceneTool, Luna::Tool);
    static void InitializeType();
    static void CleanupType();


    //
    // Implementation
    //

  public:
    SceneTool( Luna::Scene* scene, PropertiesGenerator* generator )
      : Tool ( scene->GetViewport(), generator )
      , m_Scene ( scene )
    {

    }

  };
}
