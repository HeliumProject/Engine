#pragma once

#include "Editor/API.h"
#include "Volume.h"
#include "CreateTool.h"

#include "Foundation/TUID.h"

namespace Editor
{
  class VolumeCreateTool : public Editor::CreateTool
  {
    //
    // Members
    //

  public:
    static Content::VolumeShape s_Shape;

    //
    // RTTI
    //

    LUNA_DECLARE_TYPE(Editor::VolumeCreateTool, Editor::CreateTool);
    static void InitializeType();
    static void CleanupType();

  public:
    VolumeCreateTool(Editor::Scene* scene, PropertiesGenerator* generator);

    virtual ~VolumeCreateTool();

    virtual Editor::TransformPtr CreateNode() HELIUM_OVERRIDE;

    virtual void CreateProperties() HELIUM_OVERRIDE;

    int GetVolumeShape() const;

    void SetVolumeShape(int value);
  };
}
