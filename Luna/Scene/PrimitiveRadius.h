#pragma once

#include "Primitive.h"

namespace Luna
{
  class PrimitiveRadius : public Luna::PrimitiveTemplate<Position>
  {
  public:
    float m_Radius;
    int m_RadiusSteps;

  public:
    PrimitiveRadius(ResourceTracker* tracker);

    virtual void Update() NOC_OVERRIDE;
    virtual void Draw( DrawArgs* args, const bool* solid = NULL, const bool* transparent = NULL ) const NOC_OVERRIDE = 0;
    virtual bool Pick( PickVisitor* pick, const bool* solid = NULL ) const NOC_OVERRIDE = 0;
  };
}
