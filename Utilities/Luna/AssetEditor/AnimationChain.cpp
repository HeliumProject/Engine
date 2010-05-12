#include "Precompile.h"
#include "AnimationChain.h"

#include "PersistentDataFactory.h"

#include "Asset/AnimationChain.h"
#include "File/Manager.h"
#include "FileSystem/FileSystem.h"
#include "Inspect/ClipboardElementArray.h"
#include "Inspect/ClipboardFileList.h"

using namespace Luna;

LUNA_DEFINE_TYPE( Luna::AnimationChain );

///////////////////////////////////////////////////////////////////////////////
// Static initialization.
// 
void AnimationChain::InitializeType()
{
  Reflect::RegisterClass<Luna::AnimationChain>( "Luna::AnimationChain" );
  PersistentDataFactory::GetInstance()->Register( Reflect::GetType< Asset::AnimationChain >(), &AnimationChain::CreateAnimationChain );
}

///////////////////////////////////////////////////////////////////////////////
// Static cleanup.
// 
void AnimationChain::CleanupType()
{
  Reflect::UnregisterClass<Luna::AnimationChain>();
}

///////////////////////////////////////////////////////////////////////////////
// Static creator.
// 
Luna::PersistentDataPtr AnimationChain::CreateAnimationChain( Reflect::Element* animChain, Luna::AssetManager* assetManager )
{
  return new AnimationChain( Reflect::AssertCast< Asset::AnimationChain >( animChain ), assetManager );
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
AnimationChain::AnimationChain( Asset::AnimationChain* animChain, Luna::AssetManager* assetManager )
: Luna::PersistentData( animChain, assetManager )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
AnimationChain::~AnimationChain()
{
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the clipboard data can be consumed by this class.  This class
// can handle data that represents arrays of Asset::AnimationChain.
// 
bool AnimationChain::CanHandleClipboardData( const Inspect::ReflectClipboardDataPtr& data )
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
bool AnimationChain::HandleClipboardData( const Inspect::ReflectClipboardDataPtr& data, ClipboardOperation op, Undo::BatchCommand* batch )
{
  if ( CanHandleClipboardData( data ) )
  {
    if ( data->HasType( Reflect::GetType< Inspect::ClipboardElementArray >() ) )
    {
      Inspect::ClipboardElementArray* elementArray = Reflect::DangerousCast< Inspect::ClipboardElementArray >( data );
      if ( elementArray->GetCommonBaseTypeID() == Reflect::GetType< Asset::AnimationChain >() )
      {
        // Dragging animation chains...
//        Asset::AnimationChain* animGroup = GetPackage< Asset::AnimationChain >();
//        size_t origChainCount = animGroup->m_AnimationChains.size();
//        for each ( const Reflect::ElementPtr& element in elementArray->m_Elements )
//        {
//          Asset::AnimationChain* animChain = Reflect::AssertCast< Asset::AnimationChain >( element );
//          animGroup->m_AnimationChains.push_back( Reflect::AssertCast< Asset::AnimationChain >( animChain->Clone() ) );
//        }
//
//        if ( origChainCount != animGroup->m_AnimationChains.size() )
//        {
//          animGroup->RaiseChanged( animGroup->GetClass()->FindField( &Asset::AnimationGroup::m_AnimationChains ) );
//          return true;
//        }
      }
      else
      {
        return __super::HandleClipboardData( data, op, batch );
      }
    }
    else if ( data->HasType( Reflect::GetType< Inspect::ClipboardFileList >() ) ) 
    {
      bool fireEvent = false;
      Asset::AnimationChain* animChain = GetPackage< Asset::AnimationChain >();
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
          Asset::AnimationClipDataPtr clip = new Asset::AnimationClipData();
          clip->m_ArtFile = fileID;
          animChain->m_AnimationClips.push_back( clip );
          fireEvent = true;
        }
      }

      if ( fireEvent )
      {
        animChain->RaiseChanged( animChain->GetClass()->FindField( &Asset::AnimationChain::m_AnimationClips ) );
        return true;
      }
    }
  }

  return false;
}
