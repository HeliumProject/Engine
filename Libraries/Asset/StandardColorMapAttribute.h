#pragma once

#include "ColorMapAttribute.h"

namespace Asset
{
  // Color map format
  namespace ColorTexFormats
  {
    enum ColorTexFormat
    {
      CA_DXT5 = TextureColorFormats::DXT5,
      CA_4444 = TextureColorFormats::ARGB4444,
      CA_8888 = TextureColorFormats::ARGB8888,
    };
    static void ColorTexFormatEnumerateEnumeration( Reflect::Enumeration* info )
    {
      info->AddElement(CA_DXT5, "CA_DXT5", "DXT5");
      info->AddElement(CA_4444, "CA_4444", "ARGB4444");
      info->AddElement(CA_8888, "CA_8888", "ARGB8888");
    }
  }
  typedef ColorTexFormats::ColorTexFormat ColorTexFormat;

  class ASSET_API StandardColorMapAttribute : public ColorMapAttribute
  {
  private:
    // Legacy.  This member has been moved to StandardShaderAsset, but we need
    // to restore any existing data.  See StandardShaderAsset::PostDeserialize.
    AlphaType m_AlphaMode; 
    friend class StandardShaderAsset;

  public:
    ColorTexFormat m_TexFormat;

    float m_AmbOccScale;
    Math::Color3 m_BaseMapTint;
    bool m_DisableBaseTint;

    StandardColorMapAttribute() 
      : m_AlphaMode( static_cast< AlphaType >( -1 ) )
      , m_TexFormat( ColorTexFormats::CA_DXT5 )
      , m_AmbOccScale( 1.0f )
      , m_BaseMapTint( 255, 255, 255 )
      , m_DisableBaseTint( false )
    { 
      m_TexFilter = RunTimeFilters::RTF_BILINEAR;
    }

    virtual ~StandardColorMapAttribute() {}

    virtual bool ShouldRebuildTexture( const TextureMapAttribute* oldAttrib ) const NOC_OVERRIDE;

    REFLECT_DECLARE_CLASS( StandardColorMapAttribute, ColorMapAttribute );

    static void EnumerateClass( Reflect::Compositor<StandardColorMapAttribute>& comp );
  };

  typedef Nocturnal::SmartPtr< StandardColorMapAttribute > StandardColorMapAttributePtr;
}