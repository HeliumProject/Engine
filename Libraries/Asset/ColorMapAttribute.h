#pragma once

#include "TextureMapAttribute.h"
#include "math/Color3.h"



namespace Asset
{
  class ASSET_API ColorMapAttribute NOC_ABSTRACT : public TextureMapAttribute
  {
  public:

    MipGenFilterType m_AlphaMipGenFilter;
    PostMipFilterType m_AlphaPostMipFilter;
    V_i32 m_AlphaMipFilterPasses;

    ColorMapAttribute() 
      : m_AlphaMipGenFilter( MipGenFilterTypes::MIP_SINC )
      , m_AlphaPostMipFilter( PostMipFilterTypes::POST_HIGH_PASS )
    { 
    }
    virtual ~ColorMapAttribute() {}

    virtual i32 GetSlot() const NOC_OVERRIDE;
    virtual bool ShouldRebuildTexture( const TextureMapAttribute* oldAttrib ) const NOC_OVERRIDE;

    REFLECT_DECLARE_ABSTRACT( ColorMapAttribute, TextureMapAttribute );

    static void EnumerateClass( Reflect::Compositor<ColorMapAttribute>& comp );
  };

  typedef Nocturnal::SmartPtr< ColorMapAttribute > ColorMapAttributePtr;
}