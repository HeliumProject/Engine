#pragma once

#include "TextureMapComponent.h"



namespace Asset
{
  class PIPELINE_API ExpensiveMapComponent NOC_ABSTRACT : public TextureMapComponent
  {
  public:

    MipGenFilterType m_IncanMipGenFilter;
    PostMipFilterType m_IncanPostMipFilter;
    MipGenFilterType m_ParaMipGenFilter;
    PostMipFilterType m_ParaPostMipFilter;
    MipGenFilterType m_DetailMaskMipGenFilter;
    PostMipFilterType m_DetailMaskPostMipFilter;

    V_i32 m_IncanMipFilterPasses;
    V_i32 m_ParaMipFilterPasses;
    V_i32 m_DetailMaskMipFilterPasses;

    ExpensiveMapComponent()
      : m_IncanMipGenFilter( MipGenFilterTypes::MIP_SINC )
      , m_IncanPostMipFilter( PostMipFilterTypes::POST_HIGH_PASS )
      , m_ParaMipGenFilter( MipGenFilterTypes::MIP_SINC )
      , m_ParaPostMipFilter( PostMipFilterTypes::POST_HIGH_PASS )
      , m_DetailMaskMipGenFilter( MipGenFilterTypes::MIP_SINC )
      , m_DetailMaskPostMipFilter( PostMipFilterTypes::POST_HIGH_PASS )
    {
    }

    virtual ~ExpensiveMapComponent() {}

    virtual i32 GetSlot() const NOC_OVERRIDE;
    virtual bool ShouldRebuildTexture( const TextureMapComponent* oldAttrib ) const NOC_OVERRIDE;

    REFLECT_DECLARE_ABSTRACT( ExpensiveMapComponent, TextureMapComponent );

    static void EnumerateClass( Reflect::Compositor<ExpensiveMapComponent>& comp );
  };

  typedef Nocturnal::SmartPtr< ExpensiveMapComponent > ExpensiveMapComponentPtr;
}