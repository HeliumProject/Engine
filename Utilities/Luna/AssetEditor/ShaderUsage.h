#pragma once

#include "PersistentData.h"
#include "Asset/ShaderUsagesAttribute.h"

namespace Luna
{
  /////////////////////////////////////////////////////////////////////////////
  // Wrapper for Asset::ShaderUsage.
  // 
  class ShaderUsage : public Luna::PersistentData
  {
  public:
    // Runtime Type Info
    LUNA_DECLARE_TYPE( Luna::ShaderUsage, Luna::PersistentData );
    static void InitializeType();
    static void CleanupType();
    static Luna::PersistentDataPtr CreateShaderUsage( Reflect::Element* shaderUsage, Luna::AssetManager* assetManager );

  protected:
    ShaderUsage( Asset::ShaderUsage* shaderUsage, Luna::AssetManager* assetManager );
  public:
    virtual ~ShaderUsage();

    virtual Undo::CommandPtr CopyFrom( Luna::PersistentData* src ) NOC_OVERRIDE;
  };
  typedef Nocturnal::SmartPtr< Luna::ShaderUsage > ShaderUsagePtr;
}
