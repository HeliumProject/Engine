#pragma once

#include "Primitive.h"

namespace Luna
{
  class PrimitiveLocator : public Luna::PrimitiveTemplate<Position>
  {
  public:
    float m_Length;

  public:
    PrimitiveLocator(ResourceTracker* tracker);

    virtual void Update() NOC_OVERRIDE;
    virtual void Draw( DrawArgs* args, const bool* solid = NULL, const bool* transparent = NULL ) const NOC_OVERRIDE;
    virtual bool Pick( PickVisitor* pick, const bool* solid = NULL ) const NOC_OVERRIDE;
  };
}
