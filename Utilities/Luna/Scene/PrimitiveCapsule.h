#pragma once

#include "Primitive.h"

namespace Luna
{
  class PrimitiveCapsule : public Luna::PrimitiveTemplate<Position>
  {
  private:
    mutable i32 m_WireVertCount;
    mutable i32 m_PolyVertCount;
    mutable i32 m_CapVertCount;
    mutable i32 m_ShaftVertCount;

  public:
    f32 m_Radius;
    i32 m_RadiusSteps;

    f32 m_Length;
    i32 m_LengthSteps;

  public:
    Luna::PrimitiveCapsule(ResourceTracker* tracker);

    int GetWireVertCount() const;
    int GetPolyVertCount() const;

    virtual void Update() NOC_OVERRIDE;
    virtual void Draw( DrawArgs* args, const bool* solid = NULL, const bool* transparent = NULL ) const NOC_OVERRIDE;
    virtual bool Pick( PickVisitor* pick, const bool* solid = NULL ) const NOC_OVERRIDE;
  };
}
