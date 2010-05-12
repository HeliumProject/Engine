#pragma once

#include "AssetClass.h"
#include "Asset/LooseGraphShaderAsset.h"

namespace Inspect
{
  class Control;
}

namespace Luna
{
  /////////////////////////////////////////////////////////////////////////////
  // Overridden to provide real-time updating of loose graph shaders.
  // 
  class LooseGraphShaderAsset : public Luna::AssetClass
  {
  public:
    // Runtime Type Info
    LUNA_DECLARE_TYPE( Luna::LooseGraphShaderAsset, Luna::AssetClass );
    static void InitializeType();
    static void CleanupType();
    static Luna::PersistentDataPtr Create( Reflect::Element* lgsClass, Luna::AssetManager* manager );

    protected:
      LooseGraphShaderAsset( Asset::LooseGraphShaderAsset* lgsClass, Luna::AssetManager* manager): Luna::AssetClass( lgsClass, manager ){};
    public:
      virtual ~LooseGraphShaderAsset(){};
      virtual void Changed( Inspect::Control* control ) NOC_OVERRIDE;
  };
}
