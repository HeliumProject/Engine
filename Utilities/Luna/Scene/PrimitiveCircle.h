#pragma once

#include "PrimitiveRadius.h"

namespace Luna
{
  class PrimitiveCircle : public Luna::PrimitiveRadius
  {
 
  public:

    bool m_HackyRotateFlag;

    PrimitiveCircle(ResourceTracker* tracker);

    virtual void Update() NOC_OVERRIDE;
    virtual void Draw( DrawArgs* args, const bool* solid = NULL, const bool* transparent = NULL ) const NOC_OVERRIDE;
    virtual void DrawFill( DrawArgs* args ) const;
    virtual void DrawHiddenBack( DrawArgs* args, const Luna::Camera* camera, const Math::Matrix4& m ) const;
    virtual bool Pick( PickVisitor* pick, const bool* solid = NULL ) const NOC_OVERRIDE;
  };
}
