#pragma once

#include "DetailMapAttribute.h"



namespace Asset
{
  // Detail Map format
  namespace DetailTexFormats
  {
    enum DetailTexFormat
    {
      D_DXT5 = TextureColorFormats::DXT5,
      D_ARGB4444 = TextureColorFormats::ARGB4444,
      D_ARGB8888 = TextureColorFormats::ARGB8888,
    };
    static void DetailTexFormatEnumerateEnumeration( Reflect::Enumeration* info )
    {
      info->AddElement(D_DXT5, "D_DXT5", "DXT5");
      info->AddElement(D_ARGB4444, "D_ARGB4444", "ARGB4444");
      info->AddElement(D_ARGB8888, "D_ARGB8888", "ARGB8888");
    }
  }
  typedef DetailTexFormats::DetailTexFormat DetailTexFormat;

  class PIPELINE_API StandardDetailMapAttribute : public DetailMapAttribute
  {
  public:

    StandardDetailMapAttribute()
      : m_TexFormat( DetailTexFormats::D_ARGB8888 )
      , m_ResolutionScale( 3.0f )
      , m_DetailStrength( 2.0f )
    {
      m_TexFilter = RunTimeFilters::RTF_BILINEAR;
      m_MipGenFilter = MipGenFilterTypes::MIP_SINC;
      m_PostMipFilter = PostMipFilterTypes::POST_HIGH_PASS;
    }

    virtual ~StandardDetailMapAttribute() {}

    DetailTexFormat m_TexFormat;

    float m_ResolutionScale;
    float m_DetailStrength;

    virtual bool ShouldRebuildTexture( const TextureMapAttribute* oldAttrib ) const NOC_OVERRIDE;

    REFLECT_DECLARE_CLASS( StandardDetailMapAttribute, DetailMapAttribute );

    static void EnumerateClass( Reflect::Compositor<StandardDetailMapAttribute>& comp );
  };

  typedef Nocturnal::SmartPtr< StandardDetailMapAttribute > StandardDetailMapAttributePtr;
}