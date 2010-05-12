#include "Precompile.h"
#include "AnimationEventsDocument.h"

#include "AnimationEventsManager.h"
#include "AnimationEventsEditor.h"

#include "Attribute/AttributeHandle.h"
#include "Asset/AnimationAttribute.h"
#include "Asset/AnimationSetAsset.h"

#include "File/Manager.h"
#include "FileSystem/FileSystem.h"
#include "Finder/AnimationSpecs.h"

#include "Console/Console.h"
#include "AnimationEventsInit.h"

#include "RCS/RCS.h"

using namespace Luna;
using namespace Asset;
using namespace Attribute;


// 
// RTTI
// 
LUNA_DEFINE_TYPE( AnimationEventsDocument );

void AnimationEventsDocument::InitializeType()
{
  Luna::LunaAnimationEvents::g_RegisteredTypes.Push( Reflect::RegisterClass<AnimationEventsDocument>( "AnimationEventsDocument" ) );
}

void AnimationEventsDocument::CleanupType()
{
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
AnimationEventsDocument::AnimationEventsDocument( AnimationEventsManager* manager, const Asset::AssetClassPtr& mobyAsset )
: Document( mobyAsset->GetFilePath() )
, m_Manager( manager )
, m_MobyClass( mobyAsset )
{
  Initialize();
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
AnimationEventsDocument::~AnimationEventsDocument()
{
}

void AnimationEventsDocument::Initialize()
{
  m_ClipData.clear();

  AttributeViewer< AnimationAttribute > animation( m_MobyClass );
  if ( animation.Valid() && animation->m_AnimationSetId != TUID::Null )
  {
    m_AnimationSetAsset = AssetClass::GetAssetClass<AnimationSetAsset>( animation->m_AnimationSetId, false );
    if ( !m_AnimationSetAsset.ReferencesObject() )
      return;

    M_ClipToIndex clipMapping;
    m_AnimationSetAsset->GatherUniqueClips( m_ClipData, clipMapping );

    DocumentPtr animSetFile = new Document( m_AnimationSetAsset->GetFilePath() );
    m_Files[ m_AnimationSetAsset->GetFilePath() ] = animSetFile;

    for each ( const AnimationClipDataPtr& clip in m_ClipData )
    {
      std::string eventsPath = GetClipEventsFile( clip );

      if ( m_Files.find( eventsPath ) == m_Files.end() )
      {
        m_Files[ eventsPath ] = new Document( eventsPath );

        AnimationEventListPtr eventList;
        if ( FileSystem::Exists( eventsPath ) )
        {
          try
          {
            eventList = Reflect::Archive::FromFile< AnimationEventList >( eventsPath );
          }
          catch( Reflect::StreamException )
          {
            Console::Warning( "An anim event file is empty. Did you expect it to have any events? (%s)\n", eventsPath.c_str() );
            eventList = new AnimationEventList;
          }
        }

        if ( !eventList.ReferencesObject() )
        {
          eventList = new AnimationEventList;
        }

        m_EventLists[ eventsPath ] = eventList;
      }
    }
  }
  else
  {
    wxMessageBox( "Moby has no animation clips!", "No Animations", wxOK | wxCENTRE, m_Manager->GetEditor() );
  }
}

std::string AnimationEventsDocument::GetClipEventsFile( const AnimationClipDataPtr& clip )
{
  std::string artFilePath = File::GlobalManager().GetPath( clip->m_ArtFile );

  return FinderSpecs::Animation::EVENTS_DECORATION.GetMetaDataFile( artFilePath );

}

AnimationEventListPtr AnimationEventsDocument::GetClipEventList( const AnimationClipDataPtr& clip )
{
  if ( clip->m_UseOverrideEvents )
  {
    return clip->m_OverrideEvents;
  }

  std::string eventsFile = GetClipEventsFile( clip );

  return m_EventLists[ eventsFile ];
}

bool AnimationEventsDocument::Save()
{
  bool success = true;

  // save the animset file if necessary
  std::string fileName = m_AnimationSetAsset->GetFilePath();

  M_StringToDocument::iterator it = m_Files.find( fileName );
  if ( it == m_Files.end() )
  {
    NOC_BREAK();
    Console::Error( "%s is not in the file list!\n", fileName.c_str() );

    return false;
  }

  DocumentPtr animSetDocument = it->second;
  if ( animSetDocument->IsModified() )
  {
    try
    {
      m_AnimationSetAsset->Serialize();
    }
    catch( Nocturnal::Exception& e )
    {
      std::string errorString = std::string( "Failed to save file: " ) + e.what();
      wxMessageBox( errorString.c_str(), "Error", wxOK|wxCENTRE|wxICON_ERROR, m_Manager->GetEditor() );

      success = false;
    }
  }

  for each ( const AnimationClipDataPtr& clip in m_DirtyClips )
  {
    try
    {
      if ( !clip->m_UseOverrideEvents )
      {
        std::string eventsFile = GetClipEventsFile( clip );

        FileSystem::MakePath( eventsFile, true );

        AnimationEventListPtr eventList = m_EventLists[ eventsFile ];
        
        try
        {
          Reflect::Archive::ToFile( eventList, eventsFile );
        }
        catch ( Nocturnal::Exception& ex )
        {
          std::ostringstream str;
          str << "Failed to write file " << eventsFile << ": " << ex.what();
          wxMessageBox( str.str().c_str(), "Error", wxOK|wxCENTRE|wxICON_ERROR, m_Manager->GetEditor() );
        }

        RCS::File rcsFile( eventsFile );
        rcsFile.GetInfo();

        if ( !rcsFile.ExistsInDepot() || rcsFile.HeadDeleted() )
        {
          try
          {
            rcsFile.Add();
          }
          catch ( Nocturnal::Exception& ex )
          {
            std::stringstream str;
            str << "Unable to get info for '" << eventsFile << "': " << ex.what();
            wxMessageBox( str.str().c_str(), "Error", wxCENTER | wxICON_ERROR | wxOK, m_Manager->GetEditor() );
          }
        }
      }
    }
    catch( Nocturnal::Exception& e )
    {
      std::string errorString = std::string( "Failed to save file: " ) + e.what();
      wxMessageBox( errorString.c_str(), "Error", wxOK|wxCENTRE|wxICON_ERROR, m_Manager->GetEditor() );
      
      success = false;
    }
  }
  
  if ( success )
  {
    SetModified( false );

    m_DirtyClips.clear();
  }

  return success;
}

bool AnimationEventsDocument::EditAnimSet()
{
  bool success = false;

  std::string fileName = m_AnimationSetAsset->GetFilePath();

  M_StringToDocument::iterator it = m_Files.find( fileName );
  if ( it == m_Files.end() )
  {
    NOC_BREAK();
    Console::Error( "%s is not in the file list!\n", fileName.c_str() );

    return false;
  }

  DocumentPtr editorFile = it->second;

  if ( m_Manager->AttemptChanges( editorFile ) )
  {
    SetModified( true );
    editorFile->SetModified( true );

    success = true;
  }

  return success;
}

bool AnimationEventsDocument::Edit( const AnimationClipDataPtr& clip )
{
  bool success = false;

  std::string fileName;
  if ( clip->m_UseOverrideEvents )
  {
    fileName = m_AnimationSetAsset->GetFilePath();
  }
  else
  {
    fileName = GetClipEventsFile( clip );
  }

  M_StringToDocument::iterator it = m_Files.find( fileName );
  if ( it == m_Files.end() )
  {
    NOC_BREAK();
    Console::Error( "%s is not in the file list!\n", fileName.c_str() );

    return false;
  }

  DocumentPtr editorFile = it->second;
 
  if ( m_Manager->AttemptChanges( editorFile ) )
  {
    SetModified( true );
    editorFile->SetModified( true );

    m_DirtyClips.insert( clip );

    success = true;
  }

  return success;
}

