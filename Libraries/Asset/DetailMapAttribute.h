#pragma once

#include "TextureMapAttribute.h"

namespace Asset
{
  class ASSET_API DetailMapAttribute NOC_ABSTRACT : public TextureMapAttribute
  {
  public:

    DetailMapAttribute() {}
    virtual ~DetailMapAttribute() {}

    virtual i32 GetSlot() const NOC_OVERRIDE;

    REFLECT_DECLARE_ABSTRACT( DetailMapAttribute, TextureMapAttribute );

    static void EnumerateClass( Reflect::Compositor<DetailMapAttribute>& comp );
  };

  typedef Nocturnal::SmartPtr< DetailMapAttribute > DetailMapAttributePtr;
}