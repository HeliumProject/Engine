#pragma once

#include "Primitive.h"

namespace Luna
{
  class PrimitivePointer : public Luna::PrimitiveTemplate<Position>
  {
  public:
    PrimitivePointer(ResourceTracker* tracker);

    virtual void Update() NOC_OVERRIDE;
    virtual void Draw( DrawArgs* args, const bool* solid = NULL, const bool* transparent = NULL ) const NOC_OVERRIDE;
    virtual bool Pick( PickVisitor* pick, const bool* solid = NULL ) const NOC_OVERRIDE;
  };
}
