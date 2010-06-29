#pragma once

#include "PrimitiveRadius.h"

namespace Luna
{
  class PrimitiveSphere : public Luna::PrimitiveRadius
  {
  private:
    mutable i32 m_WireVertCount;
    mutable i32 m_PolyVertCount;

  public:
    int m_LengthSteps;

  public:
    PrimitiveSphere(ResourceTracker* tracker);

    int GetWireVertCount() const;
    int GetPolyVertCount() const;

    virtual void Update() NOC_OVERRIDE;
    virtual void Draw( DrawArgs* args, const bool* solid = NULL, const bool* transparent = NULL ) const NOC_OVERRIDE;
    virtual bool Pick( PickVisitor* pick, const bool* solid = NULL ) const NOC_OVERRIDE;
  };
}
