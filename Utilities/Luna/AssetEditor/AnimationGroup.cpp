#include "Precompile.h"
#include "AnimationGroup.h"

#include "PersistentDataFactory.h"

#include "Asset/AnimationChain.h"
#include "Asset/AnimationGroup.h"
#include "File/Manager.h"
#include "FileSystem/FileSystem.h"
#include "Inspect/ClipboardElementArray.h"
#include "Inspect/ClipboardFileList.h"

using namespace Luna;

LUNA_DEFINE_TYPE( Luna::AnimationGroup );

///////////////////////////////////////////////////////////////////////////////
// Static initialization.
// 
void AnimationGroup::InitializeType()
{
  Reflect::RegisterClass<Luna::AnimationGroup>( "Luna::AnimationGroup" );
  PersistentDataFactory::GetInstance()->Register( Reflect::GetType< Asset::AnimationGroup >(), &Luna::AnimationGroup::CreateAnimationGroup );
}

///////////////////////////////////////////////////////////////////////////////
// Static cleanup.
// 
void AnimationGroup::CleanupType()
{
  Reflect::UnregisterClass<Luna::AnimationGroup>();
}

///////////////////////////////////////////////////////////////////////////////
// Static creator.
// 
Luna::PersistentDataPtr AnimationGroup::CreateAnimationGroup( Reflect::Element* animGroup, Luna::AssetManager* assetManager )
{
  return new Luna::AnimationGroup( Reflect::AssertCast< Asset::AnimationGroup >( animGroup ), assetManager );
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
AnimationGroup::AnimationGroup( Asset::AnimationGroup* animGroup, Luna::AssetManager* assetManager )
: Luna::PersistentData( animGroup, assetManager )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
AnimationGroup::~AnimationGroup()
{
}

///////////////////////////////////////////////////////////////////////////////
// Returns the category of this animation group.
// 
const std::string& AnimationGroup::GetCategory() const
{
  return GetPackage< Asset::AnimationGroup >()->m_Category;
}

///////////////////////////////////////////////////////////////////////////////
// Sets the category of this animation group and raises a change event.
// 
void AnimationGroup::SetCategory( const std::string& category )
{
  Asset::AnimationGroup* animGroup = GetPackage< Asset::AnimationGroup >();
  if ( category != animGroup->m_Category )
  {
    animGroup->m_Category = category;

    // Raise change event
    animGroup->RaiseChanged( animGroup->GetClass()->FindField( &Asset::AnimationGroup::m_Category ) );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Returns the modifier settings for this animation group.
// 
const M_string& AnimationGroup::GetModifiers() const
{
  return GetPackage< Asset::AnimationGroup >()->m_Modifiers;
}

///////////////////////////////////////////////////////////////////////////////
// Sets the modifier settings for this animation group and raises a change
// event.
// 
void AnimationGroup::SetModifiers( const M_string& modifiers )
{
  Asset::AnimationGroup* animGroup = GetPackage< Asset::AnimationGroup >();

  if ( modifiers != animGroup->m_Modifiers )
  {
    animGroup->m_Modifiers = modifiers;

    // Raise change event
    animGroup->RaiseChanged( animGroup->GetClass()->FindField( &Asset::AnimationGroup::m_Modifiers ) );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the clipboard data can be consumed by this class.  This class
// can handle data that represents arrays of Asset::AnimationChain.
// 
bool AnimationGroup::CanHandleClipboardData( const Inspect::ReflectClipboardDataPtr& data )
{
  if ( data->HasType( Reflect::GetType< Inspect::ClipboardElementArray >() ) )
  {
    Inspect::ClipboardElementArray* elementArray = Reflect::DangerousCast< Inspect::ClipboardElementArray >( data );
    if ( elementArray->GetCommonBaseTypeID() == Reflect::GetType< Asset::AnimationChain >() )
    {
      return true;
    }
  }
  else if ( data->HasType( Reflect::GetType< Inspect::ClipboardFileList >() ) )
  {
    return true;
  }

  return __super::CanHandleClipboardData( data );
}

///////////////////////////////////////////////////////////////////////////////
// Copies data from the clipboard into this object.
// 
bool AnimationGroup::HandleClipboardData( const Inspect::ReflectClipboardDataPtr& data, ClipboardOperation op, Undo::BatchCommand* batch )
{
  if ( CanHandleClipboardData( data ) )
  {
    if ( data->HasType( Reflect::GetType< Inspect::ClipboardElementArray >() ) )
    {
      Inspect::ClipboardElementArray* elementArray = Reflect::DangerousCast< Inspect::ClipboardElementArray >( data );
      if ( elementArray->GetCommonBaseTypeID() == Reflect::GetType< Asset::AnimationChain >() )
      {
        Asset::AnimationGroup* animGroup = GetPackage< Asset::AnimationGroup >();
        size_t origChainCount = animGroup->m_AnimationChains.size();
        for each ( const Reflect::ElementPtr& element in elementArray->m_Elements )
        {
          Asset::AnimationChain* animChain = Reflect::AssertCast< Asset::AnimationChain >( element );
          animGroup->m_AnimationChains.push_back( Reflect::AssertCast< Asset::AnimationChain >( animChain->Clone() ) );
        }

        if ( origChainCount != animGroup->m_AnimationChains.size() )
        {
          animGroup->RaiseChanged( animGroup->GetClass()->FindField( &Asset::AnimationGroup::m_AnimationChains ) );
          return true;
        }
      }
      else
      {
        return __super::HandleClipboardData( data, op, batch );
      }
    }
    else if ( data->HasType( Reflect::GetType< Inspect::ClipboardFileList >() ) )
    {
      Asset::AnimationGroup* animGroup = GetPackage< Asset::AnimationGroup >();

      // See if they are dragging and dropping Maya files, and if so, create a chain for each one
      bool fireEvent = false;
      Inspect::ClipboardFileList* fileList = Reflect::DangerousCast< Inspect::ClipboardFileList >( data );
      for each ( const std::string& file in fileList->GetFilePaths() )
      {
        tuid fileID = File::GlobalManager().GetID( file );
        if ( fileID == TUID::Null )
        {
          try
          {
            fileID = File::GlobalManager().Open( file );
          }
          catch ( const File::Exception& e )
          {
            Console::Warning( "Unable to create a file resolver ID for file '%s'... skipping.\nReason: %s\n", file.c_str(), e.what() );
          }
        }

        if ( fileID != TUID::Null && FileSystem::HasExtension( file, FinderSpecs::Extension::MAYA_BINARY.GetExtension() ) ) 
        {
          Asset::AnimationChainPtr animChain = new Asset::AnimationChain();
          Asset::AnimationClipDataPtr animClip = new Asset::AnimationClipData();
          animClip->m_ArtFile = fileID;
          animChain->m_AnimationClips.push_back( animClip );
          animGroup->m_AnimationChains.push_back( animChain );
          fireEvent = true;
        }
      }

      if ( fireEvent )
      {
        animGroup->RaiseChanged( animGroup->GetClass()->FindField( &Asset::AnimationGroup::m_AnimationChains ) );
        return true;
      }
    }
  }

  return false;
}
