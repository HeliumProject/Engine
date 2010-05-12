#pragma once

#include "API.h"
#include "Volume.h"
#include "CreateTool.h"

#include "TUID/TUID.h"

namespace Luna
{
  class VolumeCreateTool : public Luna::CreateTool
  {
    //
    // Members
    //

  public:
    static Content::VolumeShape s_Shape;

    //
    // RTTI
    //

    LUNA_DECLARE_TYPE(Luna::VolumeCreateTool, Luna::CreateTool);
    static void InitializeType();
    static void CleanupType();

  public:
    VolumeCreateTool(Luna::Scene* scene, Enumerator* enumerator);

    virtual ~VolumeCreateTool();

    virtual Luna::TransformPtr CreateNode() NOC_OVERRIDE;

    virtual void CreateProperties() NOC_OVERRIDE;

    int GetVolumeShape() const;

    void SetVolumeShape(int value);
  };
}
