#pragma once

#include "Primitive.h"

namespace Math
{
  class Matrix4;
}

namespace Luna
{
  class PrimitiveAxes : public Luna::PrimitiveTemplate<PositionColored>
  {
  private:
    u32 m_ColorX;
    u32 m_ColorY;
    u32 m_ColorZ;

  public:
    float m_Length;

  public:
    Luna::PrimitiveAxes(ResourceTracker* tracker);

    void SetColor(Math::AxesFlags axis, u32 c);
    void SetColor(u32 c);
    void SetRGB();

    virtual void Update() NOC_OVERRIDE;
    virtual void Draw( DrawArgs* args, const bool* solid = NULL, const bool* transparent = NULL ) const NOC_OVERRIDE;
    virtual void DrawAxes(DrawArgs* args, Math::AxesFlags axes) const;
    virtual void DrawViewport(DrawArgs* args, const Luna::Camera* camera) const;
    virtual bool Pick( PickVisitor* pick, const bool* solid = NULL ) const NOC_OVERRIDE;
    Math::AxesFlags PickAxis(const Math::Matrix4& transform, Math::Line pick, float err);
  };
}
