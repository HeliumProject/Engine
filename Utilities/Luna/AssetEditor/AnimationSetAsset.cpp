#include "Precompile.h"
#include "AnimationSetAsset.h"

#include "AssetEditor.h"
#include "AssetManager.h"
#include "PersistentDataFactory.h"

#include "Asset/AnimationSetAsset.h"
#include "File/Manager.h"
#include "Core/Enumerator.h"
#include "Windows/Clipboard.h"

// Using
using namespace Luna;

// Definition
LUNA_DEFINE_TYPE( Luna::AnimationSetAsset );


///////////////////////////////////////////////////////////////////////////////
// Static initialization for all Luna::AnimationSetAsset types.
// 
void AnimationSetAsset::InitializeType()
{
  Reflect::RegisterClass<Luna::AnimationSetAsset>( "Luna::AnimationSetAsset" );
  PersistentDataFactory::GetInstance()->Register( Reflect::GetType< Asset::AnimationSetAsset >(), &Luna::AnimationSetAsset::Create );
}

///////////////////////////////////////////////////////////////////////////////
// Static cleanup of all Luna::AnimationSetAsset types.
// 
void AnimationSetAsset::CleanupType()
{
  Reflect::UnregisterClass<Luna::AnimationSetAsset>();
}

///////////////////////////////////////////////////////////////////////////////
// Static creator function.
// 
Luna::PersistentDataPtr AnimationSetAsset::Create( Reflect::Element* animationSet, Luna::AssetManager* manager )
{
  return new Luna::AnimationSetAsset( Reflect::AssertCast< Asset::AnimationSetAsset >( animationSet ), manager );
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
AnimationSetAsset::AnimationSetAsset( Asset::AnimationSetAsset* animationSet, Luna::AssetManager* manager )
: Luna::AssetClass( animationSet, manager )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
AnimationSetAsset::~AnimationSetAsset()
{
}

///////////////////////////////////////////////////////////////////////////////
// Adds a new group to this set.
// 
void AnimationSetAsset::AddAnimationGroup( const Asset::AnimationGroupPtr& group )
{
  Asset::AnimationSetAsset* pkg = GetPackage< Asset::AnimationSetAsset >();
  pkg->m_AnimationGroups.push_back( group );

  pkg->RaiseChanged( pkg->GetClass()->FindField( &Asset::AnimationSetAsset::m_AnimationGroups ) );
}

///////////////////////////////////////////////////////////////////////////////
// Removes the animation with the specified ID from this group.
// 
void AnimationSetAsset::RemoveAnimationGroup( const Asset::AnimationGroupPtr& group )
{
  Asset::AnimationSetAsset* pkg = GetPackage< Asset::AnimationSetAsset >();
  Asset::V_AnimationGroup::iterator itr = pkg->m_AnimationGroups.begin();
  Asset::V_AnimationGroup::iterator end = pkg->m_AnimationGroups.end();
  for ( ; itr != end; ++itr )
  {
    if ( *itr == group )
    {
      pkg->m_AnimationGroups.erase( itr );

      pkg->RaiseChanged( pkg->GetClass()->FindField( &Asset::AnimationSetAsset::m_AnimationGroups ) );

      break;
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if there is already an animation group within this set that
// has the exact same category and modifier settings.  If true, this indicates
// that the specified group should not be added to this set.
// 
bool AnimationSetAsset::ContainsMatchingGroup( const Asset::AnimationGroupPtr& group )
{
  Asset::AnimationSetAsset* pkg = GetPackage< Asset::AnimationSetAsset >();
  Asset::V_AnimationGroup::const_iterator groupItr = pkg->m_AnimationGroups.begin();
  Asset::V_AnimationGroup::const_iterator groupEnd = pkg->m_AnimationGroups.end();
  for ( ; groupItr != groupEnd; ++groupItr )
  {
    const Asset::AnimationGroupPtr& currentGroup = *groupItr;
    if ( currentGroup->m_Category != group->m_Category )
    {
      continue;
    }

    if ( currentGroup->m_Modifiers.size() == 0 && group->m_Modifiers.size() == 0 )
    {
      return true;
    }

    bool modifiersMatch = true;
    const M_string::const_iterator currentModEnd = currentGroup->m_Modifiers.end();
    M_string::const_iterator modItr = group->m_Modifiers.begin();
    M_string::const_iterator modEnd = group->m_Modifiers.end();
    for ( ; modItr != modEnd; ++modItr )
    {
      M_string::const_iterator found = currentGroup->m_Modifiers.find( modItr->first );
      if ( found != currentModEnd )
      {
        if ( found->second != modItr->second )
        {
          modifiersMatch = false;
          break;
        }
      }
    }

    if ( modifiersMatch )
    {
      return true;
    }
  }

  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Adds menu item for dumping the list of all animation clip art files.
// 
void AnimationSetAsset::PopulateContextMenu( ContextMenuItemSet& menu )
{
  __super::PopulateContextMenu( menu );

  menu.AppendSeparator();

  ContextMenuItemPtr menuItem = new ContextMenuItem( "Copy Art File Paths to Clipboard" );
  menuItem->AddCallback( ContextMenuSignature::Delegate( this, &AnimationSetAsset::CopyAnimClipArtFiles ) );
  menu.AppendItem( menuItem );
}

///////////////////////////////////////////////////////////////////////////////
// Iterates over the animation chains and finds all animation clips, building
// up a unique list of art files and saves them to the clipboard (newline delimited).
// This functionality is provided so that batch exporttool scripts can be made
// from the list of files since exporttool crashes when trying to operate on a 
// bunch of files continuously.  If we ever fix exporttool, this can be removed.
// 
void AnimationSetAsset::CopyAnimClipArtFiles( const ContextMenuArgsPtr& args )
{
  typedef Nocturnal::OrderedSet< std::string > OS_string;
  OS_string paths;

  const Asset::AnimationSetAsset* animSet = GetPackage< Asset::AnimationSetAsset >();
  Asset::V_AnimationGroup::const_iterator groupItr = animSet->m_AnimationGroups.begin();
  Asset::V_AnimationGroup::const_iterator groupEnd = animSet->m_AnimationGroups.end();
  for ( ; groupItr != groupEnd; ++groupItr )
  {
    const Asset::AnimationGroup* animGroup = *groupItr;
    Asset::V_AnimationChain::const_iterator chainItr = animGroup->m_AnimationChains.begin();
    Asset::V_AnimationChain::const_iterator chainEnd = animGroup->m_AnimationChains.end();
    for ( ; chainItr != chainEnd; ++chainItr )
    {
      const Asset::AnimationChain* animChain = *chainItr;
      Asset::V_AnimationClipData::const_iterator clipItr = animChain->m_AnimationClips.begin();
      Asset::V_AnimationClipData::const_iterator clipEnd = animChain->m_AnimationClips.end();
      for ( ; clipItr != clipEnd; ++clipItr )
      {
        const Asset::AnimationClipData* clip = *clipItr;
        std::string path;
        try
        {
          path = File::GlobalManager().GetPath( clip->m_ArtFile );
        }
        catch ( const Nocturnal::Exception& )
        {
          // do nothing
        }

        if ( !path.empty() )
        {
          paths.Append( path );
        }
      }
    }
  }
  
  std::string pathList;
  OS_string::Iterator pathItr = paths.Begin();
  OS_string::Iterator pathEnd = paths.End();
  for ( ; pathItr != pathEnd; ++pathItr )
  {
    if ( !pathList.empty() )
    {
      pathList += "\n";
    }
    pathList += *pathItr;
  }

  std::string error;
  Windows::CopyToClipboard( GetHwndOf( m_AssetManager->GetAssetEditor() ), pathList, error );
}