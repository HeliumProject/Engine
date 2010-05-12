#pragma once

#include "Primitive.h"

namespace Luna
{
  class PrimitiveRings : public Luna::PrimitiveTemplate<Position>
  {
  public:
    f32 m_Radius;
    u32 m_Steps;

  public:
    PrimitiveRings(ResourceTracker* tracker);

    virtual void Update() NOC_OVERRIDE;
    virtual void Draw( DrawArgs* args, const bool* solid = NULL, const bool* transparent = NULL ) const NOC_OVERRIDE;
    virtual bool Pick( PickVisitor* pick, const bool* solid = NULL ) const NOC_OVERRIDE;
 };
}
