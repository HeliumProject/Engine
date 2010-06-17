#pragma once

#include "ExpensiveMapAttribute.h"
#include "Foundation/Math/Color3.h"
#include "Foundation/Math/HDRColor3.h"

namespace Asset
{
  // GlossIncanPara format
  namespace GlossParaIncanTexFormats
  {
    enum GlossParaIncanTexFormat
    {
      GPI_DXT = TextureColorFormats::DXT1,
      GPI_RGB565 = TextureColorFormats::RGB565,
      GPI_ARGB4444 = TextureColorFormats::ARGB4444,
      GPI_ARGB8888 = TextureColorFormats::ARGB8888,
    };
    static void GlossParaIncanTexFormatEnumerateEnumeration( Reflect::Enumeration* info )
    {
      info->AddElement(GPI_DXT, "GPI_DXT", "DXT");
      info->AddElement(GPI_RGB565, "GPI_RGB565", "RGB565");
      info->AddElement(GPI_ARGB4444, "GPI_ARGB4444", "ARGB4444");
      info->AddElement(GPI_ARGB8888, "GPI_ARGB8888", "ARGB8888");
    }
  }
  typedef GlossParaIncanTexFormats::GlossParaIncanTexFormat GlossParaIncanTexFormat;

  // CubeSpecType format
  namespace CubeSpecTypeFormats
  {
    enum CubeSpecTypeFormat
    {
      CST_VERY_BROAD = 0,
      CST_MEDIUM_BROAD = 1,
      CST_MEDIUM_TIGHT = 2,
      CST_VERY_TIGHT = 3,
    };
    static void CubeSpecTypeFormatEnumerateEnumeration( Reflect::Enumeration* info )
    {
      info->AddElement(CST_VERY_BROAD, "CST_VERY_BROAD", "VERY_BROAD");
      info->AddElement(CST_MEDIUM_BROAD, "CST_MEDIUM_BROAD", "MEDIUM_BROAD");
      info->AddElement(CST_MEDIUM_TIGHT, "CST_MEDIUM_TIGHT", "MEDIUM_TIGHT");
      info->AddElement(CST_VERY_TIGHT, "CST_VERY_TIGHT", "VERY_TIGHT");
    }
  }
  typedef CubeSpecTypeFormats::CubeSpecTypeFormat CubeSpecTypeFormat;

  class PIPELINE_API StandardExpensiveMapAttribute : public ExpensiveMapAttribute
  {
  public:

    GlossParaIncanTexFormat m_TexFormat;

    // gloss attributes
    bool m_GlossMapEnabled;
    float m_GlossScale;
    float m_GlossDirty;
    Math::Color3 m_GlossTint;
    CubeSpecTypeFormat m_CubeSpecType;
    float m_CubeBias;
    float m_RealTimeSpecPower;
    float m_RealTimeSpecIntensity;

    bool m_ParallaxMapEnabled;
    float m_ParallaxScale;
    float m_ParallaxBias;

    bool m_IncanMapEnabled;

    bool m_DetailMapMaskEnabled;

    float m_RimStrength;
    float m_RimTightness;

    StandardExpensiveMapAttribute()
      : m_TexFormat( GlossParaIncanTexFormats::GPI_DXT )
      , m_GlossMapEnabled( true )
      , m_GlossScale( 1.5f )
      , m_GlossDirty( 0.9f )
      , m_GlossTint( Math::Color3( 128, 128, 128 ) )
      , m_CubeSpecType( CubeSpecTypeFormats::CST_MEDIUM_BROAD )
      , m_CubeBias( -1.0f )
      , m_RealTimeSpecPower( 0.0f )
      , m_RealTimeSpecIntensity( 1.0f )
      , m_ParallaxMapEnabled( false )
      , m_ParallaxScale( 0.02f )
      , m_ParallaxBias( 0.5f )
      , m_IncanMapEnabled( false )
      , m_DetailMapMaskEnabled( false )
      , m_RimStrength( 2.f )
      , m_RimTightness( 3.f )
    {
      m_TexFilter = RunTimeFilters::RTF_BILINEAR;
    }

    virtual ~StandardExpensiveMapAttribute() {}

    virtual bool ProcessComponent(Reflect::ElementPtr element, const std::string& fieldName) NOC_OVERRIDE;

    virtual bool ShouldRebuildTexture( const TextureMapAttribute* oldAttrib ) const NOC_OVERRIDE;

    REFLECT_DECLARE_CLASS( StandardExpensiveMapAttribute, ExpensiveMapAttribute );

    static void EnumerateClass( Reflect::Compositor<StandardExpensiveMapAttribute>& comp );
  };

  typedef Nocturnal::SmartPtr< StandardExpensiveMapAttribute > StandardExpensiveMapAttributePtr;
}