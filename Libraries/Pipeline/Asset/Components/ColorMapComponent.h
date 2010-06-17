#pragma once

#include "TextureMapComponent.h"
#include "Foundation/Math/Color3.h"



namespace Asset
{
  class PIPELINE_API ColorMapComponent NOC_ABSTRACT : public TextureMapComponent
  {
  public:

    MipGenFilterType m_AlphaMipGenFilter;
    PostMipFilterType m_AlphaPostMipFilter;
    V_i32 m_AlphaMipFilterPasses;

    ColorMapComponent() 
      : m_AlphaMipGenFilter( MipGenFilterTypes::MIP_SINC )
      , m_AlphaPostMipFilter( PostMipFilterTypes::POST_HIGH_PASS )
    { 
    }
    virtual ~ColorMapComponent() {}

    virtual i32 GetSlot() const NOC_OVERRIDE;
    virtual bool ShouldRebuildTexture( const TextureMapComponent* oldAttrib ) const NOC_OVERRIDE;

    REFLECT_DECLARE_ABSTRACT( ColorMapComponent, TextureMapComponent );

    static void EnumerateClass( Reflect::Compositor<ColorMapComponent>& comp );
  };

  typedef Nocturnal::SmartPtr< ColorMapComponent > ColorMapComponentPtr;
}