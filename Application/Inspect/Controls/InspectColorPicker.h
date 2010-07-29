#pragma once

#include "Application/API.h"
#include "Application/Inspect/Controls/Control.h"

#include "Foundation/Math/Color3.h"
#include "Foundation/Math/Color4.h"
#include "Foundation/Reflect/Class.h"

namespace Inspect
{
  ///////////////////////////////////////////////////////////////////////////
  // 
  // 
  class APPLICATION_API ColorPicker : public Reflect::ConcreteInheritor<ColorPicker, Control>
  {
  private:
    Math::Color3 m_Color3;
    Math::Color4 m_Color4;

    bool m_Alpha;

  public:
    ColorPicker();
    virtual ~ColorPicker()
    {
    }

    virtual void Realize( Container* parent ) HELIUM_OVERRIDE;
    virtual void Read() HELIUM_OVERRIDE;
    virtual bool Write() HELIUM_OVERRIDE;

    void UseAlpha( bool alpha );

    void SetColor3( const Math::Color3& color );
    const Math::Color3& GetColor3() const;

    void SetColor4( const Math::Color4& color );
    const Math::Color4& GetColor4() const;
  };

  typedef Helium::SmartPtr<ColorPicker> ColorPickerPtr;
}