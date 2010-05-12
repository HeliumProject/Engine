#pragma once

#include "PrimitiveRadius.h"

namespace Luna
{
  class PrimitiveCylinder : public Luna::PrimitiveRadius
  {
  public:
    float m_Length;
    int m_LengthSteps;

    bool m_VerticalOrientation;

  public:
    PrimitiveCylinder(ResourceTracker* tracker);

    int GetWireVertCount() const;
    int GetPolyVertCount() const;

    virtual void Update() NOC_OVERRIDE;
    virtual void Draw( DrawArgs* args, const bool* solid = NULL, const bool* transparent = NULL ) const NOC_OVERRIDE;
    virtual bool Pick( PickVisitor* pick, const bool* solid = NULL ) const NOC_OVERRIDE;
  };
}
