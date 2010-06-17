#pragma once

#include "TextureMapComponent.h"

namespace Asset
{
  class PIPELINE_API DetailMapComponent NOC_ABSTRACT : public TextureMapComponent
  {
  public:

    DetailMapComponent() {}
    virtual ~DetailMapComponent() {}

    virtual i32 GetSlot() const NOC_OVERRIDE;

    REFLECT_DECLARE_ABSTRACT( DetailMapComponent, TextureMapComponent );

    static void EnumerateClass( Reflect::Compositor<DetailMapComponent>& comp );
  };

  typedef Nocturnal::SmartPtr< DetailMapComponent > DetailMapComponentPtr;
}