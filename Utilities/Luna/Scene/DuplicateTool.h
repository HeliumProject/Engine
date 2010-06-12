#pragma once

#include "API.h"
#include "Volume.h"
#include "CreateTool.h"

#include "Foundation/TUID.h"

namespace Luna
{
  class DuplicateTool : public Luna::CreateTool
  {
    //
    // Members
    //

  private:
    Luna::Transform* m_Source;

    //
    // RTTI
    //

  public:
    LUNA_DECLARE_TYPE(Luna::DuplicateTool, Luna::CreateTool);
    static void InitializeType();
    static void CleanupType();

  public:
    DuplicateTool(Luna::Scene* scene, Enumerator* enumerator);

    virtual ~DuplicateTool();

    virtual Luna::TransformPtr CreateNode() NOC_OVERRIDE;

    virtual void CreateProperties() NOC_OVERRIDE;

    int GetVolumeShape() const;

    void SetVolumeShape(int value);
  };
}
