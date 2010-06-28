#pragma once

#include "Pipeline/API.h"
#include "ShaderAsset.h"
#include "Foundation/TUID.h"

namespace Asset
{
  class PIPELINE_API StandardShaderAsset : public ShaderAsset
  {
  public:
    bool m_CinematicShader;


    StandardShaderAsset()
      : m_CinematicShader( false )
    {
    }

    void MakeDefault();

    virtual void PostDeserialize() NOC_OVERRIDE;

    virtual bool ValidateCompatible( const Component::ComponentPtr& attr, tstring& error ) const NOC_OVERRIDE;

    virtual bool IsCinematicShader() const NOC_OVERRIDE;

    REFLECT_DECLARE_CLASS( StandardShaderAsset, ShaderAsset );

    static void EnumerateClass( Reflect::Compositor<StandardShaderAsset>& comp );
  };

  typedef Nocturnal::SmartPtr< StandardShaderAsset > StandardShaderAssetPtr;
}