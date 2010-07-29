#pragma once

#include "Editor/API.h"
#include "Volume.h"
#include "CreateTool.h"

#include "Foundation/TUID.h"

namespace Editor
{
  class DuplicateTool : public Editor::CreateTool
  {
    //
    // Members
    //

  private:
    Editor::Transform* m_Source;

    //
    // RTTI
    //

  public:
    LUNA_DECLARE_TYPE(Editor::DuplicateTool, Editor::CreateTool);
    static void InitializeType();
    static void CleanupType();

  public:
    DuplicateTool(Editor::Scene* scene, PropertiesGenerator* generator);

    virtual ~DuplicateTool();

    virtual Editor::TransformPtr CreateNode() HELIUM_OVERRIDE;

    virtual void CreateProperties() HELIUM_OVERRIDE;

    int GetVolumeShape() const;

    void SetVolumeShape(int value);
  };
}
