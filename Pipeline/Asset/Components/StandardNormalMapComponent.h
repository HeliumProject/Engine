#pragma once

#include "NormalMapComponent.h"

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
      info->AddElement(NM_DXT5, TXT( "NM_DXT5" ), TXT( "DXT5" ) );
      info->AddElement(NM_AL88, TXT( "NM_AL88" ), TXT( "AL88" ) );
    }
  }
  typedef NormalTexFormats::NormalTexFormat NormalTexFormat;

  class PIPELINE_API StandardNormalMapComponent : public NormalMapComponent
  {
  public:

    StandardNormalMapComponent() 
      : m_TexFormat( NormalTexFormats::NM_DXT5 )

      , m_NormalMapScale( 1.0f )
    {
      m_MipGenFilter = MipGenFilterTypes::MIP_SINC; // base-class member
      m_TexFilter = RunTimeFilters::RTF_ANISO2_TRI;
    }

    ~StandardNormalMapComponent() {}

    NormalTexFormat m_TexFormat;

    float m_NormalMapScale;

    virtual bool ShouldRebuildTexture( const TextureMapComponent* oldAttrib ) const NOC_OVERRIDE;

    REFLECT_DECLARE_CLASS( StandardNormalMapComponent, NormalMapComponent );

    static void EnumerateClass( Reflect::Compositor<StandardNormalMapComponent>& comp );
  };

  typedef Nocturnal::SmartPtr< StandardNormalMapComponent > StandardNormalMapComponentPtr;
}
