#pragma once

#include "Pipeline/API.h"
#include "Pipeline/Asset/AssetClass.h"

#include "Pipeline/Asset/TextureEnums.h"

#include "Foundation/Reflect/Field.h"
#include "Foundation/TUID.h"

#include "FileBackedComponent.h"

namespace Asset
{
  // Alpha Type
  namespace AlphaTypes
  {
    enum AlphaType
    {
      ALPHA_OPAQUE,
      ALPHA_OVERLAY,
      ALPHA_ADDITIVE,
      ALPHA_SCUNGE,
      ALPHA_CUTOUT,
      ALPHA_SOFT_EDGE,
      ALPHA_BLENDED,
    };
    static void AlphaTypeEnumerateEnumeration( Reflect::Enumeration* info )
    {
      info->AddElement(ALPHA_OPAQUE, TXT( "ALPHA_OPAQUE" ), TXT( "OPAQUE" ) );
      info->AddElement(ALPHA_OVERLAY, TXT( "ALPHA_OVERLAY" ), TXT( "OVERLAY" ) );
      info->AddElement(ALPHA_ADDITIVE, TXT( "ALPHA_ADDITIVE" ), TXT( "ADDITIVE" ) );
      info->AddElement(ALPHA_SCUNGE, TXT( "ALPHA_SCUNGE" ), TXT( "SCUNGE" ) );
      info->AddElement(ALPHA_CUTOUT, TXT( "ALPHA_CUTOUT" ), TXT( "CUTOUT" ) );
      info->AddElement(ALPHA_SOFT_EDGE, TXT( "ALPHA_SOFT_EDGE" ), TXT( "SOFT_EDGE" ) );
      info->AddElement(ALPHA_BLENDED, TXT( "ALPHA_BLENDED" ), TXT( "BLENDED" ) );
    }
  }
  typedef AlphaTypes::AlphaType AlphaType;

  class PIPELINE_API TextureMapComponent NOC_ABSTRACT : public FileBackedComponent
  {
  private:
      static const tstring s_FileFilter;

  public:
    ReductionRatio    m_ReductionRatio;
    MipGenFilterType  m_MipGenFilter;
    PostMipFilterType m_PostMipFilter;
    RunTimeFilter     m_TexFilter;
    float             m_MipBias;
    std::vector< i32 >             m_MipFilterPasses;
    bool              m_IsTextureDirty; // Not serialized - indicates if texture needs to be resent for real-time update

    TextureMapComponent( tuid fileID = Nocturnal::TUID::Null )
      : m_ReductionRatio( ReductionRatios::ONE_ONE )
      , m_MipGenFilter( MipGenFilterTypes::MIP_SINC )
      , m_PostMipFilter( PostMipFilterTypes::POST_HIGH_PASS )
      , m_MipBias( 0.0f )
      , m_TexFilter( RunTimeFilters::RTF_TRILINEAR )
      , m_IsTextureDirty( false )
    {
    }

    virtual ~TextureMapComponent() {}
    virtual Component::ComponentUsage GetComponentUsage() const NOC_OVERRIDE { return Component::ComponentUsages::Class; }
    virtual Component::ComponentCategoryType GetCategoryType() const NOC_OVERRIDE;
    virtual bool ShouldRebuildTexture( const TextureMapComponent* oldAttrib ) const;

    virtual const tstring& GetFileFilter() const NOC_OVERRIDE;

    bool IsTextureDirty() const;
    void SetTextureDirty( bool dirty = true );

    REFLECT_DECLARE_ABSTRACT( TextureMapComponent, FileBackedComponent );

    static void EnumerateClass( Reflect::Compositor<TextureMapComponent>& comp );
  };

  typedef Nocturnal::SmartPtr< TextureMapComponent > TextureMapComponentPtr;
}