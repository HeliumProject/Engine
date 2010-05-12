#include "Precompile.h"

#include "AnimationEventsManager.h"
#include "AnimationEventsEditor.h"
#include "AnimationEventsUndoCommands.h"
#include "RemoteAnimationEvents.h"
#include "AnimationEventsEditorIDs.h"

#include "Editor/SessionManager.h"
#include "Task/Build.h"
#include "Live/RuntimeConnection.h"

#include "Asset/AssetClass.h"
#include "Common/CommandLine.h"
#include "Attribute/AttributeHandle.h"
#include "Asset/ArtFileAttribute.h"
#include "Asset/UpdateClassAttribute.h"
#include "Symbol/Inheritance.h"
#include "Symbol/SymbolBuilder.h"
#include "File/Manager.h"
#include "FileSystem/FileSystem.h"
#include "Console/Console.h"
#include "Windows/Process.h"
#include "Finder/Finder.h"
#include "Finder/AssetSpecs.h"
#include "Finder/AnimationSpecs.h"
#include "Finder/ContentSpecs.h"
#include "Content/Scene.h"

#include "Conduit/EventResponseCollection.h"
#include "Conduit/EventResponse.h"
#include "vtgen/vtgen.h"

#include "Common/Checksum/CRC32.h"

using namespace Luna;
using namespace Asset;
using namespace Symbol;
using namespace Attribute;

BEGIN_EVENT_TABLE( AnimationEventsManager, wxEvtHandler )
END_EVENT_TABLE()

// stolen from the engine
const u8 u16_to_rgb_map[] = { 15, 23, 31, 14, 22, 30, 13, 21, 29, 12, 20, 28, 11, 19, 27, 10 };

AnimationEventsManager::AnimationEventsManager( AnimationEventsEditor* editor )
: DocumentManager( editor )
, m_Editor( editor )
, m_FilterEvents( false )
, m_CurrentFrame( 0 )
{
  // Load the effect event enum from the symbol table
  m_AnimationEventTypesEnum = Symbol::SymbolBuilder::GetInstance()->FindEnum("EventType::EventTypes");
  if ( !m_AnimationEventTypesEnum.ReferencesObject() )
  {
    wxMessageBox( "Could not find the effect event types enum in the symbol table!  This is bad, contact tools.", "No Event Types!", wxOK | wxICON_ERROR, m_Editor );
  }

  // Load the gameplay event enum from the symbol table
  m_GameplayEventTypesEnum = Symbol::SymbolBuilder::GetInstance()->FindEnum("ANIM::MetaDataEnum");
  if ( !m_GameplayEventTypesEnum.ReferencesObject() )
  {
    m_GameplayEventTypesEnum = Symbol::SymbolBuilder::GetInstance()->FindEnum("Anim::GameplayEvent::Type");
    if ( !m_GameplayEventTypesEnum.ReferencesObject() )
    {
      wxMessageBox( "Could not find the gameplay event types enum in the symbol table!  This is bad, contact tools.", "No Event Types!", wxOK | wxICON_ERROR, m_Editor );
    }
  }

  Connect( AnimationEventsEditorIDs::COMMAND_BUILD_DONE, wxCommandEventHandler( AnimationEventsManager::OnBuildFinished ) );

  RuntimeConnection::AddRuntimeConnectionStatusListener( RuntimeConnectionStatusSignature::Delegate ( this, &AnimationEventsManager::RuntimeConnectionStatus ) );

  RemoteAnimationEvents::Initialize( *this );
}

AnimationEventsManager::~AnimationEventsManager()
{
  Disconnect( AnimationEventsEditorIDs::COMMAND_BUILD_DONE, wxCommandEventHandler( AnimationEventsManager::OnBuildFinished ) );

  RuntimeConnection::RemoveRuntimeConnectionStatusListener( RuntimeConnectionStatusSignature::Delegate ( this, &AnimationEventsManager::RuntimeConnectionStatus ) );

  RemoteAnimationEvents::Cleanup();
}

void AnimationEventsManager::RuntimeConnectionStatus( const RuntimeConnectionStatusArgs& args )
{
  if ( args.m_Connected && m_File.ReferencesObject() )
  {
    RemoteAnimationEvents::InitRemote();
  }
  else
  {
    RemoteAnimationEvents::SetValidMoby( false );
  }
}

void AnimationEventsManager::BuildMoby( tuid& assetId )
{
  Luna::AddTaskFinishedListener( Luna::TaskFinishedSignature::Delegate ( this, &AnimationEventsManager::BuildFinished ) );

  AssetBuilder::MobyBuilderOptionsPtr options = new AssetBuilder::MobyBuilderOptions;
  options->m_Conduit = true;

  SessionManager::GetInstance()->SaveAllOpenDocuments();
  Luna::BuildAsset( assetId, m_Editor, options );
}

DocumentPtr AnimationEventsManager::OpenPath( const std::string& path, std::string& error )
{
  if ( m_File.ReferencesObject() )
  {
    if ( !Close() )
    {
      // No error
      return NULL;
    }
  }

  m_AssetTuid = File::GlobalManager().Open( path );
  if ( m_AssetTuid == TUID::Null )
  {
    error = "The file path (" + path +") does not exist in the resolver and the system failed to open it.";
    return NULL;
  }

  std::string fileName( FileSystem::GetLeaf( path ) );
  FileSystem::StripExtension( fileName );

  AssetClassPtr assetClass = NULL;
  try
  {
    assetClass = AssetClass::FindAssetClass( m_AssetTuid, false );
  }
  catch ( Nocturnal::Exception& e )
  {
    error = "Could not load AssetClass " + fileName + ": " + e.Get();
    return NULL;
  }

  if ( assetClass->GetEngineType() == EngineTypes::Moby )
  {
    AttributeViewer< UpdateClassAttribute > updateClass( assetClass );

    if ( !updateClass.Valid() || updateClass->GetClassNames().empty() )
    {
      error = fileName + " has no valid update classes!  Add one through the Asset Editor.";
      return false;
    }

    try
    {
      m_File = new AnimationEventsDocument( this, assetClass );
    }
    catch ( Nocturnal::Exception& e )
    {
      error = "Error: " + e.Get();
      return NULL;
    }

    m_File->AddDocumentClosedListener( DocumentChangedSignature::Delegate( this, &AnimationEventsManager::OnDocumentClosed ) );
  }
  else
  {
    error = fileName + " is not a Moby!";
    return NULL;
  }

  PopulateClipFrames();

  if ( !RuntimeConnection::IsConnected() || wxMessageBox( "Build moby and start viewer?", "Build/View", wxYES_NO, m_Editor ) == wxYES )
  {
    BuildMoby( m_AssetTuid );
  }
  else
  {
    // fake the build done message
    ProcessEvent( wxCommandEvent( AnimationEventsEditorIDs::COMMAND_BUILD_DONE ) );
  }

  bool added = AddDocument( m_File );
  NOC_ASSERT( added );
  m_Editor->GetMRU()->Insert( path );
  return m_File;
}

bool AnimationEventsManager::Save( std::string& error )
{
  return Save( m_File, error );
}

bool AnimationEventsManager::Save( DocumentPtr document, std::string& error )
{
  if ( document.Ptr() != m_File.Ptr() )
  {
    error = "Tried to save a file that we don't have open!";
    NOC_BREAK();
    return false;
  }


  if ( !m_File->Save() )
  {
    error = "Error while saving file. Refer to the output window for more details.";
    return false;
  }

  return __super::Save( m_File, error );
}

bool AnimationEventsManager::Close()
{
  if ( m_File.ReferencesObject() )
  {
    return CloseDocument( m_File );
  }
  return true;
}

bool AnimationEventsManager::CloseDocument( DocumentPtr document, bool prompt )
{
  bool wasClosed = false;
  bool shouldClose = true;

  if ( document->IsModified() )
  {
    int result = wxMessageBox( "Would you like to save your changes before closing?", "Save Changes?", wxCENTER | wxYES_NO | wxCANCEL | wxICON_QUESTION, m_Editor );
    if ( result == wxYES )
    {
      std::string error;
      if ( !Save( document, error ) )
      {
        wxMessageBox( error.c_str(), "Error", wxCENTER | wxICON_ERROR | wxOK );
        shouldClose = false;
      }
    }
    else if ( result == wxCANCEL )
    {
      shouldClose = false;
    }
  }

  if ( shouldClose )
  {
    document->RaiseClosed();
    wasClosed = true;
  }
  return wasClosed;
}

void AnimationEventsManager::OnDocumentClosed( const DocumentChangedArgs& args )
{
  m_Closed.Raise( MobyFileChangeArgs( m_File ) );
  m_File->RemoveDocumentClosedListener( DocumentChangedSignature::Delegate( this, &AnimationEventsManager::OnDocumentClosed ) );

  m_File = NULL;
  m_CurrentClip = NULL;
  m_CurrentFrame = 0;

  m_UndoQueue.Reset();

  m_Selection.Clear();
  m_CopyBuffer.Clear();
  m_EventsAtCurrentFrame.Clear();
}

void AnimationEventsManager::UpdateEventTypes()
{
  m_AllEventTypes.clear();
  m_GameplayEventTypes.clear();

  // find all effect event types
  if ( m_AnimationEventTypesEnum.ReferencesObject() )
  {
    m_AnimationEventTypesEnum->GetElemNames( m_AllEventTypes );
    std::sort( m_AllEventTypes.begin(), m_AllEventTypes.end() );
  }

  // find all gameplay event types
  if ( m_GameplayEventTypesEnum.ReferencesObject() )
  {
    m_GameplayEventTypesEnum->GetElemNames( m_GameplayEventTypes );
    std::sort( m_GameplayEventTypes.begin(), m_GameplayEventTypes.end() );
  }

  // find all the applied event types for this update class
  m_AppliedEventTypes.clear();
  m_EventValues.clear();

  S_string eventSet;
  Symbol::EnumPtr subsystems = Symbol::SymbolBuilder::GetInstance()->FindEnum("EffectsConduit::Subsystem::Subsystems");
  V_string subsystemNames;
  subsystems->GetElemNames( subsystemNames );

  S_string updateClasses;
  updateClasses.insert( m_UpdateClass );
  std::string updateClass = m_UpdateClass;
  std::string baseClass;
  while ( Symbol::Inheritance::GetBaseClass( updateClass, baseClass ) )
  {
    updateClasses.insert( baseClass );
    updateClass = baseClass;
  }

  V_string::const_iterator subsystemItr = subsystemNames.begin();
  V_string::const_iterator subsystemEnd = subsystemNames.end();
  for ( ; subsystemItr != subsystemEnd; ++subsystemItr )
  {
    const std::string& subsystem = *subsystemItr;
    S_string::const_iterator updateClassItr = updateClasses.begin();
    S_string::const_iterator updateClassEnd = updateClasses.end();
    for ( ; updateClassItr != updateClassEnd; ++updateClassItr )
    {
      const std::string& updateClass = *updateClassItr;
      std::string friendlyName;
      VTGen::CreateVTGenFriendlyEnum( updateClass, friendlyName, false );
      std::string path = FinderSpecs::Asset::CONDUIT_FOLDER.GetFolder() + subsystem + "effectssubsystem/" + "eventresponses/" + friendlyName + ".irb";

      try
      {
        Conduit::EventResponseCollectionPtr collection = Reflect::Archive::FromFile< Conduit::EventResponseCollection >( path );

        if ( collection.ReferencesObject() )
        {
          Conduit::V_EventResponse::const_iterator responseItr = collection->m_event_responses.begin();
          Conduit::V_EventResponse::const_iterator responseEnd = collection->m_event_responses.end();
          for ( ; responseItr != responseEnd; ++responseItr )
          {
            const Conduit::EventResponsePtr& response = *responseItr;
            eventSet.insert( response->m_type );

            for ( int i = response->m_value_min; i <= response->m_value_max; ++i )
            {
              m_EventValues[ response->m_type ].insert( i );
            }
          }
        }
      }
      catch ( Reflect::Exception& )
      {
        Console::Debug( "Could not open file: %s\n", path.c_str() );
      }
    }
  }

  S_string::const_iterator eventTypeItr = eventSet.begin();
  S_string::const_iterator eventTypeEnd = eventSet.end();
  for ( ; eventTypeItr != eventTypeEnd; ++eventTypeItr )
  {
    m_AppliedEventTypes.push_back( *eventTypeItr );
  }

  std::sort( m_AppliedEventTypes.begin(), m_AppliedEventTypes.end() );

  m_EventTypesChanged.Raise( EventTypesChangedArgs() );
}

void AnimationEventsManager::SetEventTypeFilter( bool set )
{
  m_FilterEvents = set;

  m_EventTypesChanged.Raise( EventTypesChangedArgs() );
}

bool AnimationEventsManager::GetEventTypeFilter()
{
  return m_FilterEvents;
}

const S_u32& AnimationEventsManager::GetAppliedValues( const std::string& eventType )
{
  return m_EventValues[ eventType ];
}

const V_string& AnimationEventsManager::GetEventTypes( const Asset::AnimationEventPtr& evt )
{
  return GetEventTypes( evt->m_GameplayEvent );
}

const V_string& AnimationEventsManager::GetEventTypes( bool gameplay )
{
  if ( gameplay )
  {
    return m_GameplayEventTypes;
  }
  else if ( m_FilterEvents )
  {
    return m_AppliedEventTypes;
  }

  return m_AllEventTypes;
}

void AnimationEventsManager::SetUpdateClass( const int index )
{
  AttributeViewer<UpdateClassAttribute> updateClass( m_File->m_MobyClass );

  if ( updateClass.Valid() && !updateClass->GetClassNames().empty() )
  {
    m_UpdateClass = updateClass->GetClassNames()[index];

    std::string friendlyName;
    VTGen::CreateVTGenFriendlyEnum( m_UpdateClass, friendlyName, true );

    UpdateEventTypes();

    EnumPtr vt_gen_enums = SymbolBuilder::GetInstance()->FindEnum("VTGenEnums");
    NOC_ASSERT(vt_gen_enums != NULL);

    bool result = vt_gen_enums->GetElemValue(friendlyName.c_str(), m_UpdateClassIndex);
    NOC_ASSERT(result);

    m_UpdateClassChanged.Raise( UpdateClassChangedArgs( m_UpdateClass, m_UpdateClassIndex ) );
  }
}

const std::string& AnimationEventsManager::GetUpdateClass()
{
  return m_UpdateClass;
}

int AnimationEventsManager::GetUpdateClassVTGenIndex()
{
  return m_UpdateClassIndex;
}

int AnimationEventsManager::GetNumFrames( const Asset::AnimationClipDataPtr& clip )
{
  M_ClipToFrames::iterator it = m_ClipFrames.find( clip );
  if ( it != m_ClipFrames.end() )
  {
    return it->second;
  }
  else
  {
    return 100;
  }
}

int AnimationEventsManager::GetNumFrames()
{
  return GetNumFrames( m_CurrentClip );
}

int AnimationEventsManager::GetCurrentFrame()
{
  return m_CurrentFrame;
}

tuid AnimationEventsManager::GetTuid()
{
  return m_AssetTuid;
}

const AnimationClipDataPtr& AnimationEventsManager::GetCurrentClip()
{
  return m_CurrentClip;
}

const AnimationEventListPtr& AnimationEventsManager::GetEventList()
{
  return m_CurrentEvents;
}

bool AnimationEventsManager::SetOverrideEvents( bool overrideEvents )
{
  if ( !m_File->EditAnimSet() )
  {
    return false;
  }

  m_UndoQueue.Push( new AnimationEventsChangedCommand( *this ) );

  m_CurrentClip->m_UseOverrideEvents = overrideEvents;

  m_CurrentEvents = m_File->GetClipEventList( m_CurrentClip );

  OS_AnimationEvent events;
  V_AnimationEvent::const_iterator evtItr = m_CurrentEvents->m_Events.begin();
  V_AnimationEvent::const_iterator evtEnd = m_CurrentEvents->m_Events.end();
  for ( ; evtItr != evtEnd; ++evtItr )
  {
    events.Append( *evtItr );
  }

  EventsChanged( events );

  return true;
}

i32 AnimationEventsManager::GetCurrentClipIndex()
{
  int i = 0;
  V_AnimationClipData::const_iterator clipItr = m_File->m_ClipData.begin();
  V_AnimationClipData::const_iterator clipEnd = m_File->m_ClipData.end();
  for ( ; clipItr != clipEnd; ++clipItr )
  {
    const AnimationClipDataPtr& clip = *clipItr;
    if ( clip == m_CurrentClip )
    {
      return i;
    }

    ++i;
  }

  return -1;
}

void AnimationEventsManager::SetFrame( int frame )
{
  int numFrames = GetNumFrames();
  if ( frame > numFrames - 1 )
  {
    frame = numFrames - 1;
  }
  else if ( frame < 0 )
  {
    frame = 0;
  }

  m_CurrentFrame = frame;

  m_EventsAtCurrentFrame.Clear();

  if ( m_CurrentEvents )
  {
    V_AnimationEvent::const_iterator evtItr = m_CurrentEvents->m_Events.begin();
    V_AnimationEvent::const_iterator evtEnd = m_CurrentEvents->m_Events.end();
    for ( ; evtItr != evtEnd; ++evtItr )
    {
      const AnimationEventPtr& evt = *evtItr;

      if ( evt->m_Time == m_CurrentFrame )
      {
        m_EventsAtCurrentFrame.Append( evt );
      }
    }
  }
  m_FrameChanged.Raise( FrameChangedArgs( frame ) );
}

void AnimationEventsManager::SetClip( Asset::AnimationClipDataPtr& clip, bool undo )
{
  if ( undo )
  {
    m_UndoQueue.Push( new AnimationEventsChangedCommand( *this ) );
  }

  m_CurrentClip = clip;
  m_CurrentEvents = m_File->GetClipEventList( m_CurrentClip );

  m_ClipChanged.Raise( AnimClipChangeArgs( clip ) );
  SetFrame( 0 );
}

void AnimationEventsManager::SetClip( int clipNum, bool undo )
{
  if ( clipNum < (int)m_File->m_ClipData.size() && clipNum >= 0 )
  {
    SetClip( m_File->m_ClipData[ clipNum ], undo );
  }
  else
  {
    Console::Error( "Tried to set an invalid animation clip index: %d\n", clipNum );
  }
}

const Asset::OS_AnimationEvent& AnimationEventsManager::GetEventsAtCurrentFrame()
{
  return m_EventsAtCurrentFrame;
}

wxColour AnimationEventsManager::GetEventColor( const std::string& eventType )
{
  u32 hash = Nocturnal::Crc32( eventType.data(), (u32)eventType.length() );

  u32 color = 0xff;
  for (u32 i = 0; i < 16; ++i)
  {
    u32 bit = (hash >> i) & 0x1;
    color |= bit << u16_to_rgb_map[i];
  }
  return wxColour( (color >> 24) & 0xFF, (color >> 16) & 0xFF, (color >> 8)  & 0xFF );
}

void AnimationEventsManager::PlayEvent( const std::string& eventType, const float value, const u32 value2, bool untrigger, bool infinite, bool dominantOnly, bool gameplay )
{
  RemoteAnimationEvents::PlayEvent( eventType, value, value2, untrigger, infinite, dominantOnly, gameplay );
}

void AnimationEventsManager::StopAll()
{
  RemoteAnimationEvents::StopAll();
}

void AnimationEventsManager::AddEvent( const Asset::AnimationEventPtr& event )
{
  event->m_Time = m_CurrentFrame;

  OS_AnimationEvent events;
  events.Append( event );
  AddEvents( events );
}

void AnimationEventsManager::AddEvents( const Asset::OS_AnimationEvent& events )
{
  if ( m_File->Edit( m_CurrentClip ) )
  {
    m_UndoQueue.Push( new AnimationEventsChangedCommand( *this ) );

    m_Editor->Freeze();

    OS_AnimationEvent::Iterator eventIt = events.Begin();
    OS_AnimationEvent::Iterator eventEnd = events.End();
    for ( ; eventIt != eventEnd; ++eventIt )
    {
      const AnimationEventPtr& event = *eventIt;

      m_CurrentEvents->m_Events.push_back( event );
      m_EventExistence.Raise( EventExistenceArgs( event, true ) );
    }

    m_Editor->Thaw();
  }
}

void AnimationEventsManager::RemoveEvent( const AnimationEventPtr& event )
{
  OS_AnimationEvent events;
  events.Append( event );
  RemoveEvents( events );
}

void AnimationEventsManager::RemoveEvents( const Asset::OS_AnimationEvent& events )
{
  if ( m_File->Edit( m_CurrentClip ) )
  {
    m_UndoQueue.Push( new AnimationEventsChangedCommand( *this ) );

    m_Editor->Freeze();

    OS_AnimationEvent::Iterator eventIt = events.Begin();
    OS_AnimationEvent::Iterator eventEnd = events.End();
    for ( ; eventIt != eventEnd; ++eventIt )
    {
      const AnimationEventPtr& event = *eventIt;

      V_AnimationEvent::iterator it = std::find( m_CurrentEvents->m_Events.begin(), m_CurrentEvents->m_Events.end(), event );

      if ( it != m_CurrentEvents->m_Events.end() )
      {
        m_CurrentEvents->m_Events.erase( it );

        m_EventExistence.Raise( EventExistenceArgs( event, false ) );      
      }
    }

    m_Editor->Thaw();
  }
}

void AnimationEventsManager::EventChanged( const Asset::AnimationEventPtr& event )
{
  OS_AnimationEvent events;
  events.Append( event );
  EventsChanged( events );
}

void AnimationEventsManager::EventsChanged( const Asset::OS_AnimationEvent& events )
{
  if ( m_File->Edit( m_CurrentClip ) )
  {
    m_Editor->Freeze();
    m_EventsChanged.Raise( EventsChangedArgs( events ) );
    m_Editor->Thaw();
  }
}

OS_AnimationEvent& AnimationEventsManager::GetSelection()
{
  return m_Selection;
}

void AnimationEventsManager::SetSelection( const OS_AnimationEvent& selection )
{
  m_Selection = selection;

  m_Editor->Freeze();
  m_SelectionChanged.Raise( EventSelectionChangedArgs( m_Selection ) );
  m_Editor->Thaw();
}

void AnimationEventsManager::AddSelection( const AnimationEventPtr& event )
{
  m_Selection.Append( event );

  m_Editor->Freeze();
  m_SelectionChanged.Raise( EventSelectionChangedArgs( m_Selection ) );
  m_Editor->Thaw();
}

void AnimationEventsManager::RemoveSelection( const AnimationEventPtr& event )
{
  m_Selection.Remove( event );

  m_Editor->Freeze();
  m_SelectionChanged.Raise( EventSelectionChangedArgs( m_Selection ) );
  m_Editor->Thaw();
}

void AnimationEventsManager::ClearSelection()
{
  m_Selection.Clear();

  m_Editor->Freeze();
  m_SelectionChanged.Raise( EventSelectionChangedArgs( m_Selection ) );

  m_Editor->Thaw();
}

void AnimationEventsManager::SelectAll()
{
  OS_AnimationEvent events;
  V_AnimationEvent::const_iterator evtItr = m_CurrentEvents->m_Events.begin();
  V_AnimationEvent::const_iterator evtEnd = m_CurrentEvents->m_Events.end();
  for ( ; evtItr != evtEnd; ++evtItr )
  {
    events.Append( *evtItr );
  }

  SetSelection( events );
}

void AnimationEventsManager::DeleteSelected()
{
  if ( !m_Selection.Empty() )
  {
    RemoveEvents( m_Selection );

    m_Selection.Clear();

    m_Editor->Freeze();
    m_SelectionChanged.Raise( EventSelectionChangedArgs( m_Selection ) );
    m_Editor->Thaw();
  }
}

void AnimationEventsManager::Cut()
{
  Copy();

  DeleteSelected();
}

void AnimationEventsManager::Copy()
{
  if ( !m_Selection.Empty() )
  {
    m_CopyBuffer.Clear();

    OS_AnimationEvent::Iterator eventIt = m_Selection.Begin();
    OS_AnimationEvent::Iterator eventEnd = m_Selection.End();
    for ( ; eventIt != eventEnd; ++eventIt )
    {
      const AnimationEventPtr& event = *eventIt;

      m_CopyBuffer.Append( Reflect::ObjectCast< AnimationEvent >( event->Clone() ) );
    }
  }
}

void AnimationEventsManager::Paste()
{
  // there's some strange paste logic here
  // if everything in the copy buffer came from the same frame, paste into the current frame
  // otherwise, assume we're pasting from one clip to another, and paste into the frame they came from

  if ( !m_CopyBuffer.Empty() )
  {
    int frame = -1;
    bool allSameFrame = true;
    // figure out if we all came from the same frame
    OS_AnimationEvent::Iterator eventIt = m_CopyBuffer.Begin();
    OS_AnimationEvent::Iterator eventEnd = m_CopyBuffer.End();
    for ( ; eventIt != eventEnd; ++eventIt )
    {
      const AnimationEventPtr& event = *eventIt;

      if ( frame == -1 )
      {
        frame = event->m_Time;
      }

      if ( event->m_Time != frame )
      {
        allSameFrame = false;
        break;
      }
    }


    int numFrames = GetNumFrames( m_CurrentClip );
    eventIt = m_CopyBuffer.Begin();
    eventEnd = m_CopyBuffer.End();
    for ( ; eventIt != eventEnd; ++eventIt )
    {
      const AnimationEventPtr& event = *eventIt;

      // if we all came from the same frame, set our frames to the new frame
      // otherwise, clamp to the new frame's length
      if ( allSameFrame )
      {
        event->m_Time = m_CurrentFrame;
      }
      else if ( event->m_Time > numFrames - 1 )
      {
        event->m_Time = numFrames - 1;
      }
    }

    OS_AnimationEvent newEvents;
    // we need to clone the events again here, in case we paste the same thing multiple times
    eventIt = m_CopyBuffer.Begin();
    eventEnd = m_CopyBuffer.End();
    for ( ; eventIt != eventEnd; ++eventIt )
    {
      const AnimationEventPtr& event = *eventIt;

      newEvents.Append( Reflect::ObjectCast< AnimationEvent >( event->Clone() ) );
    }

    // now add all the events
    AddEvents( newEvents );

    SetSelection( newEvents );
  }
}

void AnimationEventsManager::PopulateClipFrames()
{
  // load the rigged data from the moby
  AttributeViewer<ArtFileAttribute> content( m_File->m_MobyClass );
  std::string mobyContentPath;
  File::GlobalManager().GetPath( content->m_FileID, mobyContentPath );
  std::string riggedPath = FinderSpecs::Content::RIGGED_DECORATION.GetExportFile( mobyContentPath, content->m_FragmentNode );

  Content::Scene scene;

  try
  {
    scene.Load( riggedPath );
  }
  catch ( Nocturnal::Exception& e )
  {
    Console::Error( "Could not load file %s: %s\n", riggedPath.c_str(), e.what() );
    return;
  }

  V_AnimationClipData::const_iterator clipItr = m_File->m_ClipData.begin();
  V_AnimationClipData::const_iterator clipEnd = m_File->m_ClipData.end();
  for ( ; clipItr != clipEnd; ++clipItr )
  { 
    const AnimationClipDataPtr& clipAsset = *clipItr; 

    std::string contentPath;
    File::GlobalManager().GetPath( clipAsset->m_ArtFile, contentPath );
    if ( contentPath.empty() )
    {
      Console::Error( "Could not locate an art file with ID: "TUID_HEX_FORMAT"\n", clipAsset->m_ArtFile );
      continue;
    }

    std::string animationPath = FinderSpecs::Content::ANIMATION_DECORATION.GetExportFile( contentPath, content->m_FragmentNode );

    // Find the animation clip that matches this skeleton
    Reflect::V_Element animationClips;
    try
    {
      Reflect::Archive::FromFile( animationPath, animationClips );
    }
    catch ( Nocturnal::Exception& ex )
    {
      Console::Error( "%s\n", ex.what() );
    }

    Reflect::V_Element::const_iterator animItr = animationClips.begin();
    Reflect::V_Element::const_iterator animEnd = animationClips.end();

    for ( ; animItr != animEnd; ++animItr )
    {
      Content::AnimationClipPtr clip = Reflect::ObjectCast<Content::AnimationClip>( *animItr );
      if ( clip && scene.Get< Content::Descriptor >( clip->m_SkeletonID ) )
      {
        f32 samplingFrequency = (f32) clip->m_Rate / (f32) clip->m_DataRate;
        u32 numFrames = (u32)(clip->WindowSamples() * samplingFrequency);

        if ( numFrames > 0 )
        {
          m_ClipFrames[ clipAsset ] = numFrames;
        }
      }
    }
  }
}

bool AnimationEventsManager::ViewerConnected( int numClips, RPC::AnimClipInfo* clipInfo )
{
  if (numClips != m_File->m_ClipData.size())
  {
    wxMessageBox( "Viewer # of clips doesn't match local # of clips!  Did the moby fail to build?", "Error", wxOK | wxICON_ERROR, m_Editor );

    return false;
  }

  // validate the clips themselves
  int i = 0;
  V_AnimationClipData::const_iterator clipItr = m_File->m_ClipData.begin();
  V_AnimationClipData::const_iterator clipEnd = m_File->m_ClipData.end();
  for ( ; clipItr != clipEnd; ++clipItr )
  { 
    const AnimationClipDataPtr& clip = *clipItr;
    //get the anim info from the view
    RPC::AnimClipInfo& info = clipInfo[ i ];

    //validate the info from the view
    std::string clip_name = info.m_name;
    if ( strstr(clip->GetName().c_str(), clip_name.c_str()) == 0 )
    {
      wxMessageBox( "Viewer clip '" + clip->GetName() + "' does not match local clip '" + clip_name + "'!  Did the moby fail to build?", "Error", wxOK | wxICON_ERROR, m_Editor );

      return false;
    }
    else
    {
      M_ClipToFrames::iterator it = m_ClipFrames.find( clip );
      if ( it != m_ClipFrames.end() )
      {
        if ( it->second != info.m_frames )
        {
          char buf[ 1024 ];
          _snprintf( buf, sizeof(buf), "Viewer clip '%s' with %d frames does not match local clip '%s' with %d frames!", info.m_name, info.m_frames, clip->GetName(), it->second );
          buf[ sizeof(buf) - 1] = 0; 
          wxMessageBox( buf, "Error", wxOK | wxICON_ERROR, m_Editor );

          return false;
        }
      }
      else
      {
        m_ClipFrames[ clip ] = info.m_frames;
      }
    }

    ++i;
  }

  return true;
}

void AnimationEventsManager::BuildFinished( const TaskFinishedArgs& args )
{
  Luna::RemoveTaskFinishedListener( Luna::TaskFinishedSignature::Delegate ( this, &AnimationEventsManager::BuildFinished ) );

  if ( args.m_Result == TaskResults::Success )
  {
    std::stringstream tuidString;
    tuidString << std::hex << m_File->m_MobyClass->m_AssetClassID;

    if (Nocturnal::GetCmdLineFlag( "pipe" ))
    {
      Windows::Execute( "EditorView.bat \"" + tuidString.str() + "\" -pipe Luna", true );
    }
    else
    {
      Windows::Execute( "EditorView.bat \"" + tuidString.str() + "\" -tcp -port 31338", true );
    }
  }
  else
  {
    wxMessageBox( "Moby failed to build.  You can still edit the clips, but there will be no live-link to EditorView.", "Error", wxOK | wxICON_ERROR, m_Editor );
  }

  // builddone is always called from a different thread.  fire this event so all the UI work happens in the main thread, and our thread finishes
  wxCommandEvent event( AnimationEventsEditorIDs::COMMAND_BUILD_DONE );
  wxPostEvent( this, event );
}

void AnimationEventsManager::OnBuildFinished( wxCommandEvent& event )
{
  m_Opened.Raise( MobyFileChangeArgs( m_File ) );

  if ( !m_File->m_ClipData.empty() )
  {
    SetClip( m_File->m_ClipData[0], false );
  }

  UpdateEventTypes();
}

void AnimationEventsManager::SetPlayRate( float rate )
{
  // TODO: make this event based, so we can have a slider and the devkit can change this
  RemoteAnimationEvents::SetPlayRate( rate );
}

void AnimationEventsManager::AddMobyOpenedListener( const MobyFileChangeSignature::Delegate& listener )
{
  m_Opened.Add( listener );
}

void AnimationEventsManager::RemoveMobyOpenedListener( const MobyFileChangeSignature::Delegate& listener )
{
  m_Opened.Remove( listener );
}

void AnimationEventsManager::AddMobyClosedListener( const MobyFileChangeSignature::Delegate& listener )
{
  m_Closed.Add( listener );
}

void AnimationEventsManager::RemoveMobyClosedListener( const MobyFileChangeSignature::Delegate& listener )
{
  m_Closed.Remove( listener );
}

void AnimationEventsManager::AddAnimClipChangeListener( const AnimClipChangeSignature::Delegate& listener )
{
  m_ClipChanged.Add( listener );
}

void AnimationEventsManager::RemoveAnimClipChangeListener( const AnimClipChangeSignature::Delegate& listener )
{
  m_ClipChanged.Remove( listener );
}

void AnimationEventsManager::AddFrameChangedListener( const FrameChangedSignature::Delegate& listener )
{
  m_FrameChanged.Add( listener );
}

void AnimationEventsManager::RemoveFrameChangedListener( const FrameChangedSignature::Delegate& listener )
{
  m_FrameChanged.Remove( listener );
}

void AnimationEventsManager::AddEventExistenceListener( const EventExistenceSignature::Delegate& listener )
{
  m_EventExistence.Add( listener );
}

void AnimationEventsManager::RemoveEventExistenceListener( const EventExistenceSignature::Delegate& listener )
{
  m_EventExistence.Remove( listener );
}

void AnimationEventsManager::AddSelectionChangedListener( const EventSelectionChangedSignature::Delegate& listener )
{
  m_SelectionChanged.Add( listener );
}

void AnimationEventsManager::RemoveSelectionChangedListener( const EventSelectionChangedSignature::Delegate& listener )
{
  m_SelectionChanged.Remove( listener );
}

void AnimationEventsManager::AddEventsChangedListener( const EventsChangedSignature::Delegate& listener )
{
  m_EventsChanged.Add( listener );
}

void AnimationEventsManager::RemoveEventsChangedListener( const EventsChangedSignature::Delegate& listener )
{
  m_EventsChanged.Add( listener );
}

void AnimationEventsManager::AddUpdateClassChangedListener( const UpdateClassChangedSignature::Delegate& listener )
{
  m_UpdateClassChanged.Add( listener );
}

void AnimationEventsManager::RemoveUpdateClassChangedListener( const UpdateClassChangedSignature::Delegate& listener )
{
  m_UpdateClassChanged.Remove( listener );
}

void AnimationEventsManager::AddEventTypesChangedListener( const EventTypesChangedSignature::Delegate& listener )
{
  m_EventTypesChanged.Add( listener );
}

void AnimationEventsManager::RemoveEventTypesChangedListener( const EventTypesChangedSignature::Delegate& listener )
{
  m_EventTypesChanged.Remove( listener );
}
