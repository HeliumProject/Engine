#pragma once

#include "API.h"
#include "Control.h"

namespace Inspect
{
  ///////////////////////////////////////////////////////////////////////////
  // 
  // 
  class INSPECT_API ColorPicker : public Reflect::ConcreteInheritor<ColorPicker, Control>
  {
  private:
    Math::Color3 m_Color3;
    Math::Color4 m_Color4;

    bool m_Alpha;

  public:
    ColorPicker();

    virtual void Realize( Container* parent ) NOC_OVERRIDE;
    virtual void Read() NOC_OVERRIDE;
    virtual bool Write() NOC_OVERRIDE;

    void UseAlpha( bool alpha );

    void SetColor3( const Math::Color3& color );
    const Math::Color3& GetColor3() const;

    void SetColor4( const Math::Color4& color );
    const Math::Color4& GetColor4() const;
  };

  typedef Nocturnal::SmartPtr<ColorPicker> ColorPickerPtr;
}