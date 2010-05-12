#pragma once

#include "PersistentData.h"
#include "Asset/AnimationChain.h"

namespace Luna
{

  /////////////////////////////////////////////////////////////////////////////
  // Wrapper for Asset::AnimationChain.
  // 
  class AnimationChain : public Luna::PersistentData
  {
  public:
    // Runtime Type Info
    LUNA_DECLARE_TYPE( Luna::AnimationChain, Luna::PersistentData );
    static void InitializeType();
    static void CleanupType();
    static Luna::PersistentDataPtr CreateAnimationChain( Reflect::Element* animChain, Luna::AssetManager* assetManager );

  protected:
    AnimationChain( Asset::AnimationChain* animChain, Luna::AssetManager* assetManager );
  public:
    virtual ~AnimationChain();

    virtual bool CanHandleClipboardData( const Inspect::ReflectClipboardDataPtr& data ) NOC_OVERRIDE;
    virtual bool HandleClipboardData( const Inspect::ReflectClipboardDataPtr& data, ClipboardOperation op, Undo::BatchCommand* batch ) NOC_OVERRIDE;
  };
}
