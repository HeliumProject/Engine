#pragma once

#include "AssetClass.h"

#include "Asset/AnimationGroup.h"
#include "Undo/ExistenceCommand.h"

namespace Asset
{
  class AnimationSetAsset;
}

namespace Inspect
{
  struct ChoiceArgs;
  class Button;
}

namespace Luna
{
  // Forwards
  struct CreatePanelArgs;

  /////////////////////////////////////////////////////////////////////////////
  // Wrapper for Asset::AnimationSetAsset.
  // 
  class AnimationSetAsset : public Luna::AssetClass
  {
  private:

  public:
    // Runtime Type Info
    LUNA_DECLARE_TYPE( Luna::AnimationSetAsset, Luna::AssetClass );
    static void InitializeType();
    static void CleanupType();
    static Luna::PersistentDataPtr Create( Reflect::Element* animationSet, Luna::AssetManager* manager );

  protected:
    AnimationSetAsset( Asset::AnimationSetAsset* animationSet, Luna::AssetManager* manager );
  public:
    virtual ~AnimationSetAsset();

    void AddAnimationGroup( const Asset::AnimationGroupPtr& group );
    void RemoveAnimationGroup( const Asset::AnimationGroupPtr& group );
    bool ContainsMatchingGroup( const Asset::AnimationGroupPtr& group );

    void PopulateContextMenu( ContextMenuItemSet& menu );
    void CopyAnimClipArtFiles( const ContextMenuArgsPtr& args );
  };

  typedef std::set< Luna::AnimationSetAsset* > S_AnimationSetAssetDumbPtr;

  /////////////////////////////////////////////////////////////////////////////
  // Command for adding and removing Animation Groups from an Animation Set.
  // 
  class AnimGroupExistenceCommand : public Undo::ExistenceCommand
  {
  public:
    AnimGroupExistenceCommand( Undo::ExistenceAction action, Luna::AnimationSetAsset* animSet, const Asset::AnimationGroupPtr& animGroup, bool redo = true )
      : Undo::ExistenceCommand( action, new Undo::MemberFunctionConstRef< Luna::AnimationSetAsset, Asset::AnimationGroupPtr >( animSet, animGroup, &Luna::AnimationSetAsset::AddAnimationGroup ), new Undo::MemberFunctionConstRef< Luna::AnimationSetAsset, Asset::AnimationGroupPtr >( animSet, animGroup, &Luna::AnimationSetAsset::RemoveAnimationGroup ), redo )
    {
    }
  };
}
