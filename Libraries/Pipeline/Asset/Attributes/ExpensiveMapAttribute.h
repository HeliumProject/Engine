#pragma once

#include "TextureMapAttribute.h"



namespace Asset
{
  class PIPELINE_API ExpensiveMapAttribute NOC_ABSTRACT : public TextureMapAttribute
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

    ExpensiveMapAttribute()
      : m_IncanMipGenFilter( MipGenFilterTypes::MIP_SINC )
      , m_IncanPostMipFilter( PostMipFilterTypes::POST_HIGH_PASS )
      , m_ParaMipGenFilter( MipGenFilterTypes::MIP_SINC )
      , m_ParaPostMipFilter( PostMipFilterTypes::POST_HIGH_PASS )
      , m_DetailMaskMipGenFilter( MipGenFilterTypes::MIP_SINC )
      , m_DetailMaskPostMipFilter( PostMipFilterTypes::POST_HIGH_PASS )
    {
    }

    virtual ~ExpensiveMapAttribute() {}

    virtual i32 GetSlot() const NOC_OVERRIDE;
    virtual bool ShouldRebuildTexture( const TextureMapAttribute* oldAttrib ) const NOC_OVERRIDE;

    REFLECT_DECLARE_ABSTRACT( ExpensiveMapAttribute, TextureMapAttribute );

    static void EnumerateClass( Reflect::Compositor<ExpensiveMapAttribute>& comp );
  };

  typedef Nocturnal::SmartPtr< ExpensiveMapAttribute > ExpensiveMapAttributePtr;
}