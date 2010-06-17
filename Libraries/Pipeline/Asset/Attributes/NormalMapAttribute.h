#pragma once

#include "TextureMapAttribute.h"

namespace Asset
{
  class PIPELINE_API NormalMapAttribute NOC_ABSTRACT : public TextureMapAttribute
  {
  public:

    NormalMapAttribute() {}
    virtual ~NormalMapAttribute() {}

    virtual i32 GetSlot() const NOC_OVERRIDE;

    REFLECT_DECLARE_ABSTRACT( NormalMapAttribute, TextureMapAttribute );

    static void EnumerateClass( Reflect::Compositor<NormalMapAttribute>& comp );
  };

  typedef Nocturnal::SmartPtr< NormalMapAttribute > NormalMapAttributePtr;
}