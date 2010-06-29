#pragma once

#include "Primitive.h"

namespace Luna
{
  class PrimitiveCube : public Luna::PrimitiveTemplate<Position>
  {
  public:
    PrimitiveCube(ResourceTracker* tracker);

    void SetRadius( float radius )
    {
      m_Bounds.minimum = Math::Vector3 (-radius, -radius, -radius);
      m_Bounds.maximum = Math::Vector3 (radius, radius, radius);
    }

    void ScaleRadius( float scale )
    {
      m_Bounds.minimum *= scale;
      m_Bounds.maximum *= scale;
    }

    void SetBounds( const Math::AlignedBox& box )
    {
      m_Bounds = box;
    }

    void SetBounds( const Math::Vector3& min, const Math::Vector3& max )
    {
      m_Bounds.minimum = min;
      m_Bounds.maximum = max;
    }

    virtual void Update() NOC_OVERRIDE;
    virtual void Draw( DrawArgs* args, const bool* solid = NULL, const bool* transparent = NULL ) const NOC_OVERRIDE;
    virtual bool Pick( PickVisitor* pick, const bool* solid = NULL ) const NOC_OVERRIDE;
  };
}
