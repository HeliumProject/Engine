#pragma once

#include "AttributeWrapper.h"
#include "Asset/ShaderUsagesAttribute.h"

namespace Luna
{
  // Forwards
  class AssetNode;

  /////////////////////////////////////////////////////////////////////////////
  // Wrapper around Asset::ShaderUsagesAttribute.
  // 
  class ShaderUsagesAttribute : public Luna::AttributeWrapper
  {
  public:
    // Runtime Type Info
    LUNA_DECLARE_TYPE( Luna::ShaderUsagesAttribute, Luna::AttributeWrapper );
    static void InitializeType();
    static void CleanupType();
    static Luna::PersistentDataPtr CreateShaderUsagesAttribute( Reflect::Element* attribute, Luna::AssetManager* assetManager );

  protected:
    ShaderUsagesAttribute( Asset::ShaderUsagesAttribute* attribute, Luna::AssetManager* assetManager );
  public:
    virtual ~ShaderUsagesAttribute();
    virtual void CreateChildren( Luna::AssetNode* parentNode ) NOC_OVERRIDE;

  protected:
    virtual void PackageChanged( const Reflect::ElementChangeArgs& args ) NOC_OVERRIDE;
  };
}
