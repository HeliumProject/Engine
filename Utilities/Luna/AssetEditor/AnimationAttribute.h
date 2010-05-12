#pragma once

#include "AttributeWrapper.h"
#include "Asset/AnimationAttribute.h"

namespace Luna
{
  /////////////////////////////////////////////////////////////////////////////
  // Wrapper around Asset::AnimationAttribute.
  // 
  class AnimationAttribute : public Luna::AttributeWrapper
  {
  public:
    // Runtime Type Info
    LUNA_DECLARE_TYPE( Luna::AnimationAttribute, Luna::AttributeWrapper );
    static void InitializeType();
    static void CleanupType();
    static Luna::PersistentDataPtr CreateAnimationAttribute( Reflect::Element* attribute, Luna::AssetManager* assetManager );

  protected:
    AnimationAttribute( Asset::AnimationAttribute* attribute, Luna::AssetManager* assetManager );
  public:
    virtual ~AnimationAttribute();

    tuid GetAnimationSetID() const;
    void SetAnimationSetID( const tuid& fileID );
  };
}
