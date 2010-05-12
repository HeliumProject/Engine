#include "Precompile.h"

#include "CinematicEventsManager.h"
#include "CinematicEventsEditor.h"
#include "CinematicEventsUndoCommands.h"
#include "RemoteCinematicEvents.h"
#include "CinematicEventsEditorIDs.h"

#include "Editor/SessionManager.h"
#include "Task/Build.h"
#include "Live/RuntimeConnection.h"

#include "Asset/AssetClass.h"
#include "Asset/EntityAsset.h"
#include "Attribute/AttributeHandle.h"
#include "Asset/ArtFileAttribute.h"
#include "Symbol/Inheritance.h"
#include "Symbol/SymbolBuilder.h"
#include "File/Manager.h"
#include "Console/Console.h"
#include "Windows/Process.h"
#include "Finder/Finder.h"
#include "Finder/AssetSpecs.h"
#include "Finder/AnimationSpecs.h"
#include "Finder/ContentSpecs.h"
#include "FileSystem/FileSystem.h" 
#include "Content/Scene.h"

#include "Conduit/EventResponseCollection.h"
#include "Conduit/EventResponse.h"
#include "vtgen/vtgen.h"

#include "common/checksum/crc32.h"

using namespace Luna;
using namespace Asset;
using namespace Symbol;

BEGIN_EVENT_TABLE( CinematicEventsManager, wxEvtHandler )
END_EVENT_TABLE()

// stolen from the engine
const u8 u16_to_rgb_map[] = { 15, 23, 31, 14, 22, 30, 13, 21, 29, 12, 20, 28, 11, 19, 27, 10 };

CinematicEventsManager::CinematicEventsManager( CinematicEventsEditor* editor )
: DocumentManager( editor )
, m_Editor( editor )
, m_FilterEvents( false )
, m_CurrentFrame( 0 )
{
  m_CinematicEffectSubsystemsEnum = Symbol::SymbolBuilder::GetInstance()->FindEnum("EffectsConduit::Subsystem::Subsystems");
  if ( m_CinematicEffectSubsystemsEnum.ReferencesObject() )
  {
    m_CinematicEffectSubsystemsEnum->GetElemNames( m_CinematicEffectSubsytemNames );
    //sort(m_CinematicEffectSubsytemNames.begin(), m_CinematicEffectSubsytemNames.end());

    m_CinematicEffectSpecPlayerPackageID.resize( m_CinematicEffectSubsytemNames.size() );

    {
      V_string::const_iterator effectItr = m_CinematicEffectSubsytemNames.begin();
      V_string::const_iterator effectEnd = m_CinematicEffectSubsytemNames.end();
      for ( int index = 0; effectItr != effectEnd; ++effectItr, ++index )
    {
      //gather the package data for this subsystem
        std::string enumName = "EffectsConduit::Explicit";
        enumName += (*effectItr);
        enumName += "EffectSpecification";

        m_CinematicEffectSpecPlayerPackageID[ index ] = SymbolBuilder::GetInstance()->FindEnum( enumName );
      V_string specNames;
      if (m_CinematicEffectSpecPlayerPackageID[ index ].ReferencesObject())
      {
        m_CinematicEffectSpecPlayerPackageID[ index ]->GetElemNames(specNames);
      }

      V_string::iterator specNameItr = specNames.begin();
      V_string::iterator specNameEnd = specNames.end();
      for(; specNameItr != specNameEnd; ++specNameItr )
      {
          std::string temp = *specNameItr;
          m_AllCinematicEffectSpecNames.push_back( temp );
          m_AllCinematicEffectSpecSubsystems.insert( std::make_pair( temp, index ) );
        }
      }
    }

    sort(m_AllCinematicEffectSpecNames.begin(), m_AllCinematicEffectSpecNames.end());

    V_string::iterator packitr = m_AllCinematicEffectSpecNames.begin();
    V_string::iterator packend = m_AllCinematicEffectSpecNames.end(); 
    for ( int index = 0; packitr != packend; ++packitr, ++index )
    {
      if ( (*packitr).find("_") == -1 )
        continue;
      std::string subsystemName = (*packitr).substr(0,(*packitr).find("_"));
      if ( (*packitr).find("SPECIFICATION_") == -1 )
        continue;
      std::string specName = (*packitr).substr((*packitr).find("SPECIFICATION_")+14);
      i32 packageID;
      m_CinematicEffectSpecPlayerPackageID[0]->GetElemValue( (*packitr), packageID );
      char combinedName[100] = "";
      sprintf( combinedName, "%s::%s", subsystemName.c_str(), specName.c_str());
      m_AllCinematicEffectSpecNamesSimple.push_back( combinedName );
    }
  }



  // Load the cinematic event enum from the symbol table
  m_CinematicEventTypesEnum = Symbol::SymbolBuilder::GetInstance()->FindEnum("EventType::EventTypes");
  if ( !m_CinematicEventTypesEnum.ReferencesObject() )
  {
    wxMessageBox( "Could not find the cinematic event types enum in the symbol table!  This is bad, contact tools.", "No Event Types!", wxOK | wxICON_ERROR, m_Editor );
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

  Connect( CinematicEventsEditorIDs::COMMAND_BUILD_DONE, wxCommandEventHandler( CinematicEventsManager::OnBuildFinished ) );

  RuntimeConnection::AddRuntimeConnectionStatusListener( RuntimeConnectionStatusSignature::Delegate ( this, &CinematicEventsManager::RuntimeConnectionStatus ) );

  RemoteCinematicEvents::Initialize( *this );
}

CinematicEventsManager::~CinematicEventsManager()
{
  Disconnect( CinematicEventsEditorIDs::COMMAND_BUILD_DONE, wxCommandEventHandler( CinematicEventsManager::OnBuildFinished ) );

  RuntimeConnection::RemoveRuntimeConnectionStatusListener( RuntimeConnectionStatusSignature::Delegate ( this, &CinematicEventsManager::RuntimeConnectionStatus ) );

  RemoteCinematicEvents::Cleanup();
}

void CinematicEventsManager::RuntimeConnectionStatus( const RuntimeConnectionStatusArgs& args )
{
  if ( args.m_Connected && m_File.ReferencesObject() )
  {
    RemoteCinematicEvents::InitRemote();
  }
  else
  {
    RemoteCinematicEvents::SetValidCinematic( false );
  }
}

bool CinematicEventsManager::GatherActorJoints( AssetClassPtr &assetClass )
{
  Attribute::AttributeViewer<ArtFileAttribute> modelFile( assetClass );
  std::string modelPath   = modelFile->GetFilePath(); 
  std::string riggedPath  = FinderSpecs::Content::RIGGED_DECORATION.GetExportFile( modelPath );
  std::string animationPath  = FinderSpecs::Content::ANIMATION_DECORATION.GetExportFile( modelPath );
  if( !FileSystem::Exists( riggedPath ) )
  {
    wxMessageBox("Rigged file " + riggedPath + " does not exist. Please export it.", 
                 "No Rigged File"); 
    return false; 
  }
  m_CinematicScene = new Content::Scene;
  m_CinematicScene->Load( riggedPath );
  m_CinematicScene->Load( animationPath );
  m_ActorJoints.clear();
  m_Actors.clear();
  std::string lastActorName = "";
  if ( m_CinematicScene->m_AnimationClips.size() > 20 )
  {
    char actorCount[8];
    sprintf( actorCount, "%d", m_CinematicScene->m_AnimationClips.size() );
    wxMessageBox("Hold up there, chief! You've got WAY too many actors ( " + (std::string)actorCount + " nodes with exporting skeletons ) in this cinematic.  Please clean that shit up in the maya file and re-export!", 
                   "Feck off with your crazy actor count"); 
  }

  bool foundActorWithoutTuid = false;
  for ( u32 i = 0; i< m_CinematicScene->m_AnimationClips.size(); ++i )
  {
    const Content::AnimationClipPtr clip = m_CinematicScene->m_AnimationClips[ i ];
    if ( !clip->m_OptionalEntityID )
    {
      if ( !foundActorWithoutTuid )
      {
        wxMessageBox("Atleast one of the actors has no TUID?!?  Skipping him and the rest!", 
                   "Actor Sans Tuid"); 
      }
      foundActorWithoutTuid = true;
      continue; 
    }

    m_Actors.push_back( clip->m_ActorName );

    EntityAssetPtr actorAsset = AssetClass::GetAssetClass< EntityAsset >( clip->m_OptionalEntityID );

    Attribute::AttributeViewer<ArtFileAttribute> actorModelFile( actorAsset );
    std::string actorModelPath   = actorModelFile->GetFilePath(); 
    std::string actorRiggedPath  = FinderSpecs::Content::RIGGED_DECORATION.GetExportFile( actorModelPath );
    if( !FileSystem::Exists( actorRiggedPath ) )
    {
      wxMessageBox("Rigged file " + riggedPath + " does not exist. Please export it.", 
                   "No Rigged File"); 
      return false; 
    }
    Content::Scene* actorScene = new Content::Scene( actorRiggedPath ); 
    for ( u32 i = 0 ; i < actorScene->m_Joints.size() ; ++i )
    {
      m_ActorJoints.insert( make_pair( clip->m_ActorName, actorScene->m_Joints[ i ]->GetName() ) );
      //allJoints.push_back( actorScene->m_Joints[ i ]->m_Name );
    }
  }
  return true;
}

bool CinematicEventsManager::GatherFrameData( AssetClassPtr &assetClass )
{
  Attribute::AttributeViewer<ArtFileAttribute> modelFile( assetClass );
  std::string modelPath   = modelFile->GetFilePath(); 
  std::string cineScenePath  = FinderSpecs::Content::CINESCENE_DECORATION.GetExportFile( modelPath );
  if( !FileSystem::Exists( cineScenePath ) )
  {
    wxMessageBox("CineScene file " + cineScenePath + " does not exist. Please export it.", 
                 "No CineScene File"); 
    return false; 
  }
  m_CinematicScene = new Content::Scene( cineScenePath ); 

  Content::V_CameraClip shots = m_CinematicScene->m_CineScenes[0]->m_CameraController->m_Shots;
  size_t shotCount = shots.size();
  int startFrame = 0;//(int) shots[0]->m_StartFrame - 1;
  int endFrame = (int) shots[ shotCount - 1 ]->m_EndFrame - ((int) shots[0]->m_StartFrame - 1);//(int) shots[ shotCount - 1 ]->m_EndFrame;
  SetNumFrames( endFrame - startFrame );
  SetStartFrame( startFrame );
  SetEndFrame( endFrame );
  SetZoomStartFrame( startFrame );
  SetZoomEndFrame( endFrame );
  return true;
}

DocumentPtr CinematicEventsManager::OpenPath( const std::string& path, std::string& error )
{
  if ( m_File.ReferencesObject() )
  {
    if ( !Close() )
    {
      return false;
    }
  }

  m_CinematicID = File::GlobalManager().Open( path );
  AssetClassPtr assetClass = NULL;
  try
  {
    assetClass = AssetClass::FindAssetClass( m_CinematicID, false );
  }
  catch ( Nocturnal::Exception& e )
  {
    error = "Could not load AssetClass " + path + ": " + e.Get();
    return NULL;
  }

  if ( assetClass->GetEngineType() == EngineTypes::Cinematic )
  {
    m_CinematicName = assetClass->GetShortName();

    try
    {
      m_File = new CinematicEventsDocument( this, assetClass );
    }
    catch (Nocturnal::Exception& e)
    {
      error = "Error: " + e.Get();
      return NULL;
    }

    m_File->AddDocumentClosedListener( DocumentChangedSignature::Delegate( this, &CinematicEventsManager::OnDocumentClosed ) );
  }
  else
  {
    error = path + " is not a Cinematic!";
    return NULL;
  }

  //Gather cinematic actor joints
  if ( !GatherActorJoints( assetClass ) )
  {
    return NULL;
  }
  if ( !GatherFrameData( assetClass ) )
  {
    return NULL;
  }

  m_CurrentEvents = m_File->GetCinematicEventList();
  SetFrame( GetStartFrame() );

  if ( !RuntimeConnection::IsConnected() || wxMessageBox( "Build cinematic and start viewer?", "Build/View", wxYES_NO, m_Editor ) == wxYES )
  {
    Build();
  }
  else
  {
    // fake the build done message
    ProcessEvent( wxCommandEvent( CinematicEventsEditorIDs::COMMAND_BUILD_DONE ) );
  }

  bool added = AddDocument( m_File );
  NOC_ASSERT( added );
  m_Editor->GetMRU()->Insert( path );
  return m_File;
}

void CinematicEventsManager::Build()
{
  SessionManager::GetInstance()->SaveAllOpenDocuments();

  Luna::AddTaskFinishedListener( Luna::TaskFinishedSignature::Delegate ( this, &CinematicEventsManager::BuildFinished ) );

  AssetBuilder::CinematicBuilderOptionsPtr options = new AssetBuilder::CinematicBuilderOptions;

  Luna::BuildAsset( m_CinematicID, m_Editor, options );
}

void CinematicEventsManager::Sync()
{
  RemoteCinematicEvents::UpdateEvents();
}

bool CinematicEventsManager::Save( std::string& error )
{
  return Save( m_File, error );
}

bool CinematicEventsManager::Save( DocumentPtr document, std::string& error )
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

bool CinematicEventsManager::Close()
{
  if ( m_File.ReferencesObject() )
  {
    return CloseDocument( m_File );
  }
  return true;
}

bool CinematicEventsManager::CloseDocument( DocumentPtr document, bool prompt )
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

void CinematicEventsManager::OnDocumentClosed( const DocumentChangedArgs& args )
{
  NOC_ASSERT( args.m_Document == m_File.Ptr() );

  m_Closed.Raise( CinematicFileChangeArgs( m_File ) );
  m_File->RemoveDocumentClosedListener( DocumentChangedSignature::Delegate( this, &CinematicEventsManager::OnDocumentClosed ) );

  m_File = NULL;
  m_CurrentClip = NULL;
  m_CurrentFrame = 0;

  m_UndoQueue.Reset();

  m_Selection.Clear();
  m_CopyBuffer.Clear();
  m_EventsAtCurrentFrame.Clear();
}

void CinematicEventsManager::UpdateEventTypes()
{
  m_AllEventTypes.clear();
  m_GameplayEventTypes.clear();

  // find all cinematic event types
  if ( m_CinematicEventTypesEnum.ReferencesObject() )
  {
    m_CinematicEventTypesEnum->GetElemNames( m_AllEventTypes );
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

  //S_string updateClasses;
  //updateClasses.insert( m_UpdateClass );
  //std::string updateClass = m_UpdateClass;
  //std::string baseClass;
  //while ( Symbol::Inheritance::GetBaseClass( updateClass, baseClass ) )
  //{
  //  updateClasses.insert( baseClass );
  //  updateClass = baseClass;
  //}

  /*for each ( const std::string& subsystem in subsystemNames )
  {

    std::string friendlyName;

    VTGen::CreateVTGenFriendlyEnum( m_CinematicName, friendlyName, false );
    std::string path = FinderSpecs::Asset::CONDUIT_FOLDER.GetFolder() + subsystem + "effectssubsystem/" + "eventresponses/" + "expliciteffectspec.irb";

    try
    {
      Conduit::EventResponseCollectionPtr collection = Reflect::Archive::FromFile< Conduit::EventResponseCollection >( path );

      if ( collection.ReferencesObject() )
      {
        for each ( const Conduit::EventResponsePtr& response in collection->m_event_responses )
        {
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

  }*/

  for each ( const std::string& eventType in eventSet )
  {
    m_AppliedEventTypes.push_back( eventType );
  }

  std::sort( m_AppliedEventTypes.begin(), m_AppliedEventTypes.end() );

  m_EventTypesChanged.Raise( EventTypesChangedArgs() );
}

void CinematicEventsManager::SetEventTypeFilter( bool set )
{
  m_FilterEvents = set;

  m_EventTypesChanged.Raise( EventTypesChangedArgs() );
}

bool CinematicEventsManager::GetEventTypeFilter()
{
  return m_FilterEvents;
}

const S_u32& CinematicEventsManager::GetAppliedValues( const std::string& eventType )
{
  return m_EventValues[ eventType ];
}

const V_string& CinematicEventsManager::GetEffectSpecs()
{
  return m_AllCinematicEffectSpecNamesSimple;
}

const int CinematicEventsManager::GetEffectSpecPackageID( u32 subsystemIndex, std::string effectPackageName )
{
  i32 packageID;
  if ( !m_CinematicEffectSpecPlayerPackageID[ subsystemIndex ]->GetElemValue( effectPackageName, packageID ) )
  {
    return 0;
  }
  return packageID;
}

const V_string& CinematicEventsManager::GetEffectSubsystems()
{
  return m_CinematicEffectSubsytemNames;
}

/*void CinematicEventsManager::SetCinematicSettings( )
{
  std::string blah = "";
  AttributeViewer<UpdateClassAttribute> runtimeClass( m_File->m_MobyClass );

  if ( runtimeClass.Valid() && !runtimeClass->m_ValidClasses.empty() )
  {
    m_UpdateClass = runtimeClass->m_ValidClasses[ index ];

    std::string friendlyName;
    VTGen::CreateVTGenFriendlyEnum( m_UpdateClass, friendlyName, true );

    UpdateEventTypes();

    EnumPtr vt_gen_enums = SymbolBuilder::GetInstance()->FindEnum("VTGenEnums");
    NOC_ASSERT(vt_gen_enums != NULL);

    bool result = vt_gen_enums->GetElemValue(friendlyName.c_str(), m_UpdateClassIndex);
    NOC_ASSERT(result);

    m_UpdateClassChanged.Raise( UpdateClassChangedArgs( m_UpdateClass, m_UpdateClassIndex ) );
  }
}*/

const std::string& CinematicEventsManager::GetCinematic()
{
  return m_CinematicName;
}

tuid& CinematicEventsManager::GetCinematicID()
{
  return m_CinematicID;
}

const M_ActorToJoint& CinematicEventsManager::GetActorJoints()
{
  return m_ActorJoints;
}

const V_string& CinematicEventsManager::GetActors()
{
  return m_Actors;
}

/*int CinematicEventsManager::GetCinematicVTGenIndex()
{
  return m_CinematicIndex;
}*/

int CinematicEventsManager::GetNumFrames()
{
  return m_FrameCount;
}

int CinematicEventsManager::GetStartFrame()
{
  return m_StartFrame;
}

int CinematicEventsManager::GetZoomStartFrame()
{
  return m_ZoomStartFrame;
}

int CinematicEventsManager::GetEndFrame()
{
  return m_EndFrame;
}

int CinematicEventsManager::GetZoomEndFrame()
{
  return m_ZoomEndFrame;
}

int CinematicEventsManager::GetCurrentFrame()
{
  return m_CurrentFrame;
}

void CinematicEventsManager::SetStartFrame( int frame )
{
  m_StartFrame = frame;
}

void CinematicEventsManager::SetEndFrame( int frame )
{
  m_EndFrame = frame;
}

void CinematicEventsManager::SetZoomStartFrame( int frame )
{
  m_ZoomStartFrame = frame;
}

void CinematicEventsManager::SetZoomEndFrame( int frame )
{
  m_ZoomEndFrame = frame;
}


void CinematicEventsManager::SetNumFrames( int frame )
{
  m_FrameCount = frame;
}

void CinematicEventsManager::CinematicLoaded( )
{
  m_CinematicLoaded.Raise( CinematicLoadedArgs() );
}

const CinematicAssetPtr& CinematicEventsManager::GetCurrentClip()
{
  return m_CurrentClip;
}

const CinematicEventListPtr& CinematicEventsManager::GetEventList()
{
  return m_CurrentEvents;
}

bool CinematicEventsManager::SetOverrideEvents( bool overrideEvents )
{
  /*if ( !m_File->EditCinematic() )
  {
    return false;
  }

  m_UndoQueue.Push( new CinematicEventsChangedCommand( *this ) );

  m_CurrentClip->m_UseOverrideEvents = overrideEvents;

  m_CurrentEvents = m_File->GetCinematicEventList();

  OS_CinematicEvent events;
  for each ( const CinematicEventPtr& event in m_CurrentEvents->m_Events )
  {
    events.Append( event );
  }

  EventsChanged( events );
  */
  return true;
}

i32 CinematicEventsManager::GetCurrentClipIndex()
{
  /*int i = 0;
  for each ( const CinematicAssetPtr& clip in m_File->m_ClipData )
  {
    if ( clip == m_CurrentClip )
    {
      return i;
    }

    ++i;
  }
  */
  return -1;
} 

void CinematicEventsManager::SetFrame( int frame )
{
  if ( frame > GetEndFrame() - 1 )
  {
    frame = GetEndFrame() - 1;
  }
  else if ( frame < GetStartFrame() )
  {
    frame = GetStartFrame();
  }

  m_CurrentFrame = frame;

  m_EventsAtCurrentFrame.Clear();

  //for each ( const CinematicEventPtr& evt in m_CurrentEvents->m_Events )
  //{
  //  if ( evt->m_Time == m_CurrentFrame )
  //  {
  //    m_EventsAtCurrentFrame.Append( evt );
  //  }
  //}

  m_FrameChanged.Raise( FrameChangedArgs( frame ) );
}

void CinematicEventsManager::SetClip( Asset::CinematicAssetPtr& clip, bool undo )
{
  if ( undo )
  {
    m_UndoQueue.Push( new CinematicEventsChangedCommand( *this ) );
  }

  m_CurrentClip = clip;
  m_CurrentEvents = m_File->GetCinematicEventList();

  m_ClipChanged.Raise( CinematicChangeArgs( clip ) );
  SetFrame( 0 );
}

void CinematicEventsManager::SetClip( int clipNum, bool undo )
{
  /*if ( clipNum < (int)m_File->m_ClipData.size() && clipNum >= 0 )
  {
    SetClip( m_File->m_ClipData[ clipNum ], undo );
  }
  else
  {
    Console::Error( "Tried to set an invalid animation clip index: %d\n", clipNum );
  }*/
}

const OS_CinematicEvent& CinematicEventsManager::GetEventsAtCurrentFrame()
{
  return m_EventsAtCurrentFrame;
}

wxColour CinematicEventsManager::GetEventColor( const std::string& eventType )
{
  u32 hash = Nocturnal::Crc32( eventType.data(), (u32)eventType.length() );
  
  u32 color = 0xff;
  for (u32 i = 0; i < 16; ++i)
  {
    u32 bit = (hash >> i) & 0x1;
    color |= bit << u16_to_rgb_map[i];
  }
  return wxColour( (color >> 24) | 0x80, (color >> 16) | 0x80, (color >> 8) | 0x80 );
}

void CinematicEventsManager::PlayEvent( const std::string& eventType, const float value, bool untrigger, bool infinite, bool dominantOnly, bool gameplay )
{
  RemoteCinematicEvents::PlayEvent( eventType, value, untrigger, infinite, dominantOnly, gameplay );
}

void CinematicEventsManager::StopAll()
{
  RemoteCinematicEvents::StopAll();
}

void CinematicEventsManager::AddEvent( const CinematicEventPtr& event )
{
  event->m_Time = m_CurrentFrame;
  //CinematicEventPtr cinematicEvent = new CinematicEvent( event );
  OS_CinematicEvent events;
  events.Append( event );
  AddEvents( events );
}

void CinematicEventsManager::AddEvents( const OS_CinematicEvent& events )
{
  if ( m_File->Edit() )
  {
    m_UndoQueue.Push( new CinematicEventsChangedCommand( *this ) );

    m_Editor->Freeze();

    OS_CinematicEvent::Iterator eventIt = events.Begin();
    OS_CinematicEvent::Iterator eventEnd = events.End();
    for ( ; eventIt != eventEnd; ++eventIt )
    {
      const CinematicEventPtr& event = *eventIt;

      m_CurrentEvents->m_Events.push_back( event );
      m_EventExistence.Raise( EventExistenceArgs( event, true ) );
    }

    m_Editor->Thaw();
  }
}

void CinematicEventsManager::RemoveEvent( const CinematicEventPtr& event )
{
  OS_CinematicEvent events;
  events.Append( event );
  RemoveEvents( events );
}

void CinematicEventsManager::RemoveEvents( const OS_CinematicEvent& events )
{
  //if ( m_File->Edit( m_CurrentClip ) )
  //{
    m_UndoQueue.Push( new CinematicEventsChangedCommand( *this ) );

    m_Editor->Freeze();

    OS_CinematicEvent::Iterator eventIt = events.Begin();
    OS_CinematicEvent::Iterator eventEnd = events.End();
    for ( ; eventIt != eventEnd; ++eventIt )
    {
      const CinematicEventPtr& event = *eventIt;

      V_CinematicEvent::iterator it = std::find( m_CurrentEvents->m_Events.begin(), m_CurrentEvents->m_Events.end(), event );

      if ( it != m_CurrentEvents->m_Events.end() )
      {
        m_CurrentEvents->m_Events.erase( it );

        m_EventExistence.Raise( EventExistenceArgs( event, false ) );      
      }
    }

    m_Editor->Thaw();
  //}
}

void CinematicEventsManager::EventChanged( const Asset::CinematicEventPtr& event )
{
  OS_CinematicEvent events;
  events.Append( event );
  EventsChanged( events );
}

void CinematicEventsManager::EventsChanged( const OS_CinematicEvent& events )
{
  if ( m_File->Edit() )
  {
    m_Editor->Freeze();
    m_EventsChanged.Raise( EventsChangedArgs( events ) );
    m_Editor->Thaw();
  }
}

void CinematicEventsManager::SetUpdateProperties( bool update )
{
  m_UpdateProperties = update;
}

bool CinematicEventsManager::GetUpdateProperties()
{
  return m_UpdateProperties;
}

OS_CinematicEvent& CinematicEventsManager::GetSelection()
{
  return m_Selection;
}

void CinematicEventsManager::SetSelection( const OS_CinematicEvent& selection )
{
  m_Selection = selection;
  m_Editor->Freeze();
  m_SelectionChanged.Raise( EventSelectionChangedArgs( m_Selection ) );
  m_Editor->Thaw();
}

void CinematicEventsManager::AddSelection( const CinematicEventPtr& event )
{
  m_Selection.Append( event );

  m_Editor->Freeze();
  m_SelectionChanged.Raise( EventSelectionChangedArgs( m_Selection ) );
  m_Editor->Thaw();
}

void CinematicEventsManager::RemoveSelection( const CinematicEventPtr& event )
{
  m_Selection.Remove( event );

  m_Editor->Freeze();
  m_SelectionChanged.Raise( EventSelectionChangedArgs( m_Selection ) );
  m_Editor->Thaw();
}

void CinematicEventsManager::ClearSelection()
{
  m_Selection.Clear();

  m_Editor->Freeze();
  m_SelectionChanged.Raise( EventSelectionChangedArgs( m_Selection ) );
  m_Editor->Thaw();
}

void CinematicEventsManager::SelectAll()
{
  /*OS_CinematicEvent events;
  for each ( const CinematicEventPtr& event in m_CurrentEvents->m_Events )
  {
    events.Append( event );
  }

  SetSelection( events );*/
}

void CinematicEventsManager::DeleteSelected()
{
  if ( !m_Selection.Empty() )
  {
    if ( m_File->Edit() )
    {
    RemoveEvents( m_Selection );

    m_Selection.Clear();

    m_Editor->Freeze();
    m_SelectionChanged.Raise( EventSelectionChangedArgs( m_Selection ) );
    m_Editor->Thaw();
  }
}
}

void CinematicEventsManager::Cut()
{
  Copy();

  DeleteSelected();
}

void CinematicEventsManager::Copy()
{
  if ( !m_Selection.Empty() )
  {
    m_CopyBuffer.Clear();

    OS_CinematicEvent::Iterator eventIt = m_Selection.Begin();
    OS_CinematicEvent::Iterator eventEnd = m_Selection.End();
    for ( ; eventIt != eventEnd; ++eventIt )
    {
      const CinematicEventPtr& event = *eventIt;

      m_CopyBuffer.Append( Reflect::ObjectCast< CinematicEvent >( event->Clone() ) );
    }
  }
}

void CinematicEventsManager::Paste()
{
  // there's some strange paste logic here
  // if everything in the copy buffer came from the same frame, paste into the current frame
  // otherwise, assume we're pasting from one clip to another, and paste into the frame they came from

  if ( !m_CopyBuffer.Empty() )
  {
    int frame = -1;
    bool allSameFrame = true;
    // figure out if we all came from the same frame
    OS_CinematicEvent::Iterator eventIt = m_CopyBuffer.Begin();
    OS_CinematicEvent::Iterator eventEnd = m_CopyBuffer.End();
    for ( ; eventIt != eventEnd; ++eventIt )
    {
      const CinematicEventPtr& event = *eventIt;

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

    
    int numFrames = GetNumFrames( );
    eventIt = m_CopyBuffer.Begin();
    eventEnd = m_CopyBuffer.End();
    for ( ; eventIt != eventEnd; ++eventIt )
    {
      const CinematicEventPtr& event = *eventIt;

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

    OS_CinematicEvent newEvents;

    // we need to clone the events again here, in case we paste the same thing multiple times
    eventIt = m_CopyBuffer.Begin();
    eventEnd = m_CopyBuffer.End();
    for ( ; eventIt != eventEnd; ++eventIt )
    {
      const CinematicEventPtr& event = *eventIt;

      newEvents.Append( Reflect::ObjectCast< CinematicEvent >( event->Clone() ) );
    }

    // now add all the events
    AddEvents( newEvents );

    //SetSelection( newEvents );
  }
}

void CinematicEventsManager::PopulateClipFrames()
{
  // load the rigged data from the moby
  /*AttributeViewer<ArtFileAttribute> content( m_File->m_MobyClass );
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

  for each ( const CinematicAssetPtr& clipAsset in m_File->m_ClipData )
  { 
    std::string contentPath;

    try
    {
      File::GlobalManager().GetPath( clipAsset->m_ArtFile, contentPath );
    }
    catch( File::NoFilesFoundException& )
    {
      Console::Error( "Could not locate an art file with ID: "TUID_HEX_FORMAT"\n", clipAsset->m_ArtFile );
      continue;
    }

    std::string animationPath = FinderSpecs::Content::ANIMATION_DECORATION.GetExportFile( contentPath, content->m_FragmentNode );

    // Find the animation clip that matches this skeleton
    Reflect::V_Element animationClips;
    Reflect::Archive::FromFile( animationPath, animationClips );

    Reflect::V_Element::const_iterator animItr = animationClips.begin();
    Reflect::V_Element::const_iterator animEnd = animationClips.end();

    for ( ; animItr != animEnd; ++animItr )
    {
      Content::AnimationClipPtr clip = Reflect::ObjectCast<Content::AnimationClip>( *animItr );
      if ( clip && scene.Get< Content::Descriptor >( clip->m_SkeletonID ) )
      {
        f32 samplingFrequency = (f32) clip->m_Rate / (f32) clip->m_DataRate;
        u32 numFrames = (u32)(clip->NumSamples() * samplingFrequency);
        
        if ( numFrames > 0 )
        {
          m_ClipFrames[ clipAsset ] = numFrames;
        }
      }
    }
  }*/
}

bool CinematicEventsManager::ViewerConnected( RPC::CinematicInfo* cineInfo )
{
  return true;
}

void CinematicEventsManager::BuildFinished( const TaskFinishedArgs& args )
{
  Luna::RemoveTaskFinishedListener( Luna::TaskFinishedSignature::Delegate ( this, &CinematicEventsManager::BuildFinished ) );

  if ( args.m_Result == TaskResults::Success )
  {
    //std::stringstream tuidString;
    //tuidString << std::hex << m_File->m_CinematicAsset->m_AssetClassID;
    

    Windows::Execute( "SceneView.bat \"" + m_CinematicName + "\" -luna", true );
  }
  else
  {
    wxMessageBox( "Cinematic failed to build.  You can still edit the cinematic, but there will be no live-link to SceneView.", "Error", wxOK | wxICON_ERROR, m_Editor );
  }

  // builddone is always called from a different thread.  fire this event so all the UI work happens in the main thread, and our thread finishes
  wxCommandEvent event( CinematicEventsEditorIDs::COMMAND_BUILD_DONE );
  wxPostEvent( this, event );
}

void CinematicEventsManager::OnBuildFinished( wxCommandEvent& event )
{
  m_Opened.Raise( CinematicFileChangeArgs( m_File ) );

  //if ( !m_File->m_ClipData.empty() )
  //{
    //SetClip( m_File->m_ClipData[0], false );
  //}

  UpdateEventTypes();
}

void CinematicEventsManager::SetPlayRate( float rate )
{
  // TODO: make this event based, so we can have a slider and the devkit can change this
  RemoteCinematicEvents::SetPlayRate( rate );
}

void CinematicEventsManager::AddCinematicOpenedListener( const CinematicFileChangeSignature::Delegate& listener )
{
  m_Opened.Add( listener );
}

void CinematicEventsManager::RemoveCinematicOpenedListener( const CinematicFileChangeSignature::Delegate& listener )
{
  m_Opened.Remove( listener );
}

void CinematicEventsManager::AddCinematicClosedListener( const CinematicFileChangeSignature::Delegate& listener )
{
  m_Closed.Add( listener );
}

void CinematicEventsManager::RemoveCinematicClosedListener( const CinematicFileChangeSignature::Delegate& listener )
{
  m_Closed.Remove( listener );
}

void CinematicEventsManager::AddCinematicChangeListener( const CinematicChangeSignature::Delegate& listener )
{
  m_ClipChanged.Add( listener );
}

void CinematicEventsManager::RemoveCinematicChangeListener( const CinematicChangeSignature::Delegate& listener )
{
  m_ClipChanged.Remove( listener );
}

void CinematicEventsManager::AddFrameChangedListener( const FrameChangedSignature::Delegate& listener )
{
  m_FrameChanged.Add( listener );
}

void CinematicEventsManager::RemoveFrameChangedListener( const FrameChangedSignature::Delegate& listener )
{
  m_FrameChanged.Remove( listener );
}

void CinematicEventsManager::AddEventExistenceListener( const EventExistenceSignature::Delegate& listener )
{
  m_EventExistence.Add( listener );
}

void CinematicEventsManager::RemoveEventExistenceListener( const EventExistenceSignature::Delegate& listener )
{
  m_EventExistence.Remove( listener );
}

void CinematicEventsManager::AddSelectionChangedListener( const EventSelectionChangedSignature::Delegate& listener )
{
  m_SelectionChanged.Add( listener );
}

void CinematicEventsManager::RemoveSelectionChangedListener( const EventSelectionChangedSignature::Delegate& listener )
{
  m_SelectionChanged.Remove( listener );
}

void CinematicEventsManager::AddEventsChangedListener( const EventsChangedSignature::Delegate& listener )
{
  m_EventsChanged.Add( listener );
}

void CinematicEventsManager::RemoveEventsChangedListener( const EventsChangedSignature::Delegate& listener )
{
  m_EventsChanged.Add( listener );
}

void CinematicEventsManager::AddCinematicLoadedListener( const CinematicLoadedSignature::Delegate& listener )
{
  m_CinematicLoaded.Add( listener );
}

void CinematicEventsManager::RemoveCinematicLoadedListener( const CinematicLoadedSignature::Delegate& listener )
{
  m_CinematicLoaded.Remove( listener );
}

void CinematicEventsManager::AddEventTypesChangedListener( const EventTypesChangedSignature::Delegate& listener )
{
  m_EventTypesChanged.Add( listener );
}

void CinematicEventsManager::RemoveEventTypesChangedListener( const EventTypesChangedSignature::Delegate& listener )
{
  m_EventTypesChanged.Remove( listener );
}
