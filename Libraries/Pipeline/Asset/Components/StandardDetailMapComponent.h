#pragma once

#include "DetailMapComponent.h"



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

  class PIPELINE_API StandardDetailMapComponent : public DetailMapComponent
  {
  public:

    StandardDetailMapComponent()
      : m_TexFormat( DetailTexFormats::D_ARGB8888 )
      , m_ResolutionScale( 3.0f )
      , m_DetailStrength( 2.0f )
    {
      m_TexFilter = RunTimeFilters::RTF_BILINEAR;
      m_MipGenFilter = MipGenFilterTypes::MIP_SINC;
      m_PostMipFilter = PostMipFilterTypes::POST_HIGH_PASS;
    }

    virtual ~StandardDetailMapComponent() {}

    DetailTexFormat m_TexFormat;

    float m_ResolutionScale;
    float m_DetailStrength;

    virtual bool ShouldRebuildTexture( const TextureMapComponent* oldAttrib ) const NOC_OVERRIDE;

    REFLECT_DECLARE_CLASS( StandardDetailMapComponent, DetailMapComponent );

    static void EnumerateClass( Reflect::Compositor<StandardDetailMapComponent>& comp );
  };

  typedef Nocturnal::SmartPtr< StandardDetailMapComponent > StandardDetailMapComponentPtr;
}