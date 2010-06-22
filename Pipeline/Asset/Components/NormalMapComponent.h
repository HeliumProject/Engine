#pragma once

#include "TextureMapComponent.h"

namespace Asset
{
  class PIPELINE_API NormalMapComponent NOC_ABSTRACT : public TextureMapComponent
  {
  public:

    NormalMapComponent() {}
    virtual ~NormalMapComponent() {}

    virtual i32 GetSlot() const NOC_OVERRIDE;

    REFLECT_DECLARE_ABSTRACT( NormalMapComponent, TextureMapComponent );

    static void EnumerateClass( Reflect::Compositor<NormalMapComponent>& comp );
  };

  typedef Nocturnal::SmartPtr< NormalMapComponent > NormalMapComponentPtr;
}