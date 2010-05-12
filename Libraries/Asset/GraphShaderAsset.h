#pragma once

#include "API.h"

#include "ShaderAsset.h"

#include "TUID/TUID.h"

namespace Asset
{
  class ASSET_API GraphShaderAsset : public ShaderAsset
  {
    public:
      tuid m_GraphFile;

      GraphShaderAsset()
        : m_GraphFile( TUID::Null )
        , m_CaptureFlags(0)
      {
      }

      virtual ~GraphShaderAsset() {}

      virtual void MakeDefault() NOC_OVERRIDE;
      virtual void GetAllowableEngineTypes( S_EngineType& engineTypes ) const NOC_OVERRIDE;
      virtual bool ValidateCompatible( const Attribute::AttributePtr& attr, std::string& error ) const NOC_OVERRIDE;
      virtual const Finder::FileSpec& GetBuiltFileSpec() const NOC_OVERRIDE;

      REFLECT_DECLARE_CLASS( GraphShaderAsset, ShaderAsset );
      static void EnumerateClass( Reflect::Compositor<GraphShaderAsset>& comp );
    
      const inline u32  GetCaptureFlags()       { return m_CaptureFlags;  }
      void              SetCaptureFlags(u32 v)  { m_CaptureFlags = v;     }
      
    private:
      u32  m_CaptureFlags;
  };

  typedef Nocturnal::SmartPtr< GraphShaderAsset > GraphShaderAssetPtr;
}