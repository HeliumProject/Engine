#pragma once

#include "Primitive.h"

namespace Luna
{
  class PrimitiveCone : public Luna::PrimitiveTemplate<Position>
  {
  private:
    mutable i32 m_WireVertCount;
    mutable i32 m_PolyVertCount;

  public:
    f32 m_Length;
    f32 m_Radius;
    i32 m_Steps;

  public:
    PrimitiveCone(ResourceTracker* tracker);

    u32 GetWireVertCount() const;
    u32 GetPolyVertCount() const;

    virtual void Update() NOC_OVERRIDE;
    virtual void Draw( DrawArgs* args, const bool* solid = NULL, const bool* transparent = NULL ) const NOC_OVERRIDE;
    virtual bool Pick( PickVisitor* pick, const bool* solid = NULL ) const NOC_OVERRIDE;
  };
}
