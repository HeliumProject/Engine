#pragma once

#include "NormalMapAttribute.h"

namespace Asset
{
  // Normal map format
  namespace NormalTexFormats
  {
    enum NormalTexFormat
    {
      NM_DXT5 = TextureColorFormats::DXT5,
      NM_AL88 = TextureColorFormats::AL88,
    };
    static void NormalTexFormatEnumerateEnumeration( Reflect::Enumeration* info )
    {
      info->AddElement(NM_DXT5, "NM_DXT5", "DXT5");
      info->AddElement(NM_AL88, "NM_AL88", "AL88");
    }
  }
  typedef NormalTexFormats::NormalTexFormat NormalTexFormat;

  class ASSET_API StandardNormalMapAttribute : public NormalMapAttribute
  {
  public:

    StandardNormalMapAttribute() 
      : m_TexFormat( NormalTexFormats::NM_DXT5 )

      , m_NormalMapScale( 1.0f )
    {
      m_MipGenFilter = MipGenFilterTypes::MIP_SINC; // base-class member
      m_TexFilter = RunTimeFilters::RTF_ANISO2_TRI;
    }

    ~StandardNormalMapAttribute() {}

    NormalTexFormat m_TexFormat;

    float m_NormalMapScale;

    virtual bool ShouldRebuildTexture( const TextureMapAttribute* oldAttrib ) const NOC_OVERRIDE;

    REFLECT_DECLARE_CLASS( StandardNormalMapAttribute, NormalMapAttribute );

    static void EnumerateClass( Reflect::Compositor<StandardNormalMapAttribute>& comp );
  };

  typedef Nocturnal::SmartPtr< StandardNormalMapAttribute > StandardNormalMapAttributePtr;
}
