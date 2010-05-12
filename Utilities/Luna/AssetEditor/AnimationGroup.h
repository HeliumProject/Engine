#pragma once

#include "PersistentData.h"
#include "Asset/AnimationGroup.h"

namespace Luna
{

  /////////////////////////////////////////////////////////////////////////////
  // Wrapper for Asset::AnimationGroup.
  // 
  class AnimationGroup : public Luna::PersistentData
  {
  public:
    // Runtime Type Info
    LUNA_DECLARE_TYPE( Luna::AnimationGroup, Luna::PersistentData );
    static void InitializeType();
    static void CleanupType();
    static Luna::PersistentDataPtr CreateAnimationGroup( Reflect::Element* animGroup, Luna::AssetManager* assetManager );

  protected:
    AnimationGroup( Asset::AnimationGroup* animGroup, Luna::AssetManager* assetManager );
  public:
    virtual ~AnimationGroup();

    const std::string& GetCategory() const;
    void SetCategory( const std::string& category );
    
    const M_string& GetModifiers() const;
    void SetModifiers( const M_string& modifiers );

    virtual bool CanHandleClipboardData( const Inspect::ReflectClipboardDataPtr& data ) NOC_OVERRIDE;
    virtual bool HandleClipboardData( const Inspect::ReflectClipboardDataPtr& data, ClipboardOperation op, Undo::BatchCommand* batch = NULL ) NOC_OVERRIDE;
  };
}
