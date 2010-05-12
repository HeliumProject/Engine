#pragma once

#include "Primitive.h"

namespace Luna
{
  class PrimitiveFrame : public Luna::PrimitiveTemplate<TransformedColored>
  {
  public:
    Math::Point m_Start;
    Math::Point m_End;

    u32 m_InnerColor;
    u32 m_BorderColor;

  public:
    PrimitiveFrame(ResourceTracker* tracker);

    virtual void Update() NOC_OVERRIDE;
    virtual void Draw( DrawArgs* args, const bool* solid = NULL, const bool* transparent = NULL ) const NOC_OVERRIDE;
    virtual bool Pick( PickVisitor* pick, const bool* solid = NULL ) const NOC_OVERRIDE;
  };
}
