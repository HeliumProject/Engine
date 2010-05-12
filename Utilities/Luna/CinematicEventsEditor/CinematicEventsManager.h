#pragma once

#include "CinematicEventsDocument.h"

#include "Editor/DocumentManager.h"
#include "Core/Selectable.h"
#include "Undo/Queue.h"
#include "Task/Build.h"
#include "Content/Scene.h" 
#include "Asset/CinematicAsset.h"
#include "Symbol/Enum.h"

namespace RPC
{
  struct CinematicInfo;
}

namespace Luna
{
  class CinematicEventsEditor;

  struct CinematicFileChangeArgs
  {
    CinematicEventsDocumentPtr& m_File;

    CinematicFileChangeArgs( CinematicEventsDocumentPtr& file )
      : m_File( file )
    {
    }
  };
  typedef Nocturnal::Signature< void, const CinematicFileChangeArgs& > CinematicFileChangeSignature;

  struct CinematicChangeArgs
  {
    Asset::CinematicAssetPtr& m_Clip;

    CinematicChangeArgs( Asset::CinematicAssetPtr& clip )
      : m_Clip( clip )
    {
    }
  };
  typedef Nocturnal::Signature< void, const CinematicChangeArgs& > CinematicChangeSignature;

  struct FrameChangedArgs
  {
    int m_Frame;

    FrameChangedArgs( int frame )
      : m_Frame( frame )
    {
    }
  };
  typedef Nocturnal::Signature< void, const FrameChangedArgs& > FrameChangedSignature;

  struct EventExistenceArgs
  {
    Asset::CinematicEventPtr m_Event;
    bool                          m_Created;

    EventExistenceArgs( const Asset::CinematicEventPtr& event, bool created )
      : m_Event( event )
      , m_Created( created )
    {

    }
  };
  typedef Nocturnal::Signature< void, const EventExistenceArgs& > EventExistenceSignature;

  struct EventSelectionChangedArgs
  {
    const Asset::OS_CinematicEvent m_Selection;

    EventSelectionChangedArgs( const Asset::OS_CinematicEvent& selection )
      : m_Selection( selection )
    {

    }
  };
  typedef Nocturnal::Signature< void, const EventSelectionChangedArgs& > EventSelectionChangedSignature;

  struct EventsChangedArgs
  {
    const Asset::OS_CinematicEvent& m_Events;

    EventsChangedArgs( const Asset::OS_CinematicEvent& events )
      : m_Events( events )
    {

    }
  };
  typedef Nocturnal::Signature< void, const EventsChangedArgs& > EventsChangedSignature;

  struct CinematicLoadedArgs
  {
    CinematicLoadedArgs( )
    {

    }
  };
  typedef Nocturnal::Signature< void, const CinematicLoadedArgs& > CinematicLoadedSignature;

  struct EventTypesChangedArgs
  {
    int pad;
  };
  typedef Nocturnal::Signature< void, const EventTypesChangedArgs& > EventTypesChangedSignature;


  typedef std::map< Asset::CinematicAssetPtr, int > M_ClipToFrames;
  typedef std::map< std::string, S_u32 >            M_EventToValues;
  typedef std::map< std::string, u32 >              M_SubsystemToIndex;
  typedef std::multimap< std::string, std::string > M_ActorToJoint;
  typedef std::multimap< std::string, std::string > M_SubsystemToSpec;

  struct RuntimeConnectionStatusArgs;

  class CinematicEventsManager : public wxEvtHandler, public DocumentManager
  {
  public:
    CinematicEventsManager( CinematicEventsEditor* editor );
    virtual ~CinematicEventsManager();

    const Symbol::EnumPtr& GetEventTypesEnum( bool gameplay ) { return (gameplay ? m_GameplayEventTypesEnum : m_CinematicEventTypesEnum); }
    const V_string& GetEffectSpecs();
    const V_string& GetEffectSubsystems();
    const int GetEffectSpecPackageID( u32 subsystemIndex, std::string effectPackageName );
    void SetEventTypeFilter( bool set );
    bool GetEventTypeFilter();
    const S_u32& GetAppliedValues( const std::string& eventType );

    //void SetCinematicSettings( );
    const std::string& GetCinematic();
    tuid& GetCinematicID();
    const M_ActorToJoint& GetActorJoints();
    const V_string& GetActors();

    void UpdateEventTypes();

    const CinematicEventsDocumentPtr& GetFile() { return m_File; }
    CinematicEventsEditor* GetEditor() { return m_Editor; }


    virtual DocumentPtr OpenPath( const std::string& path, std::string& error ) NOC_OVERRIDE;
    bool Save( std::string& error );
    virtual bool Save( DocumentPtr document, std::string& error ) NOC_OVERRIDE;
    bool Close();
    virtual bool CloseDocument( DocumentPtr document, bool prompt = true ) NOC_OVERRIDE;
    void OnDocumentClosed( const DocumentChangedArgs& args );
    void Build();
    void Sync();

    void Cut();
    void Copy();
    void Paste();

    bool GatherActorJoints( Asset::AssetClassPtr &assetClass );
    bool GatherFrameData( Asset::AssetClassPtr &assetClass );

    bool ViewerConnected( RPC::CinematicInfo* cineInfo );
    void RuntimeConnectionStatus( const RuntimeConnectionStatusArgs& args );
    void BuildFinished( const TaskFinishedArgs& args );

    void SetClip( Asset::CinematicAssetPtr& clip, bool undo = true );
    void SetClip( int clipNum, bool undo = true );
    const Asset::CinematicAssetPtr& GetCurrentClip();
    i32 GetCurrentClipIndex();
    void PopulateClipFrames();

    void  SetFrame( int frame );
    int   GetStartFrame();
    int   GetEndFrame();
    int   GetZoomStartFrame();
    int   GetZoomEndFrame();
    void  SetStartFrame( int frame );
    void  SetEndFrame( int frame );
    void  SetZoomStartFrame( int frame );
    void  SetZoomEndFrame( int frame );
    int   GetNumFrames();
    void  SetNumFrames( int frame );
    int   GetCurrentFrame();
    void  CinematicLoaded();
    void  SetUpdateProperties( bool update );
    bool  GetUpdateProperties();

    void  SetPlayRate( float rate );

    const Asset::CinematicEventListPtr& GetEventList();
    bool  SetOverrideEvents( bool overrideEvents );

    void PlayEvent( const std::string& eventType, const float value, bool untrigger, bool infinite, bool dominantOnly, bool gameplay );
    void StopAll();
    void AddEvent( const Asset::CinematicEventPtr& event );
    void AddEvents( const Asset::OS_CinematicEvent& events );
    void RemoveEvent( const Asset::CinematicEventPtr& event );
    void RemoveEvents( const Asset::OS_CinematicEvent& events );
    void EventChanged( const Asset::CinematicEventPtr& event );
    void EventsChanged( const Asset::OS_CinematicEvent& events );

    wxColour GetEventColor( const std::string& eventType );

    Asset::OS_CinematicEvent& GetSelection();
    void SetSelection( const Asset::OS_CinematicEvent& selection );
    void AddSelection( const Asset::CinematicEventPtr& event );
    void RemoveSelection( const Asset::CinematicEventPtr& event );
    void ClearSelection();
    void SelectAll();

    void DeleteSelected();

    const Asset::OS_CinematicEvent& GetEventsAtCurrentFrame();

    Undo::Queue& GetUndoQueue()
    {
      return m_UndoQueue;
    }

    void OnBuildFinished( wxCommandEvent& event );

    // Listeners
    void AddCinematicOpenedListener( const CinematicFileChangeSignature::Delegate& listener );
    void RemoveCinematicOpenedListener( const CinematicFileChangeSignature::Delegate& listener );
    void AddCinematicClosedListener( const CinematicFileChangeSignature::Delegate& listener );
    void RemoveCinematicClosedListener( const CinematicFileChangeSignature::Delegate& listener );
    void AddCinematicChangeListener( const CinematicChangeSignature::Delegate& listener );
    void RemoveCinematicChangeListener( const CinematicChangeSignature::Delegate& listener );
    void AddFrameChangedListener( const FrameChangedSignature::Delegate& listener );
    void RemoveFrameChangedListener( const FrameChangedSignature::Delegate& listener );
    void AddEventExistenceListener( const EventExistenceSignature::Delegate& listener );
    void RemoveEventExistenceListener( const EventExistenceSignature::Delegate& listener );
    void AddSelectionChangedListener( const EventSelectionChangedSignature::Delegate& listener );
    void RemoveSelectionChangedListener( const EventSelectionChangedSignature::Delegate& listener );
    void AddEventsChangedListener( const EventsChangedSignature::Delegate& listener );
    void RemoveEventsChangedListener( const EventsChangedSignature::Delegate& listener );
    void AddCinematicLoadedListener( const CinematicLoadedSignature::Delegate& listener );
    void RemoveCinematicLoadedListener( const CinematicLoadedSignature::Delegate& listener );
    void AddEventTypesChangedListener( const EventTypesChangedSignature::Delegate& listener );
    void RemoveEventTypesChangedListener( const EventTypesChangedSignature::Delegate& listener );

  private:
    Symbol::EnumPtr                   m_CinematicEventTypesEnum;
    Symbol::EnumPtr                   m_GameplayEventTypesEnum;
    Symbol::EnumPtr                   m_CinematicEffectSubsystemsEnum;
    std::vector<Symbol::EnumPtr>      m_CinematicEffectSpecPlayerPackageID;
    V_string                          m_CinematicEffectSubsytemNames;
    V_string                          m_AllCinematicEffectSpecNames;
    M_SubsystemToIndex                m_AllCinematicEffectSpecSubsystems;
    V_string                          m_AllCinematicEffectSpecNamesSimple;
    V_string                          m_AllEventTypes;
    V_string                          m_AppliedEventTypes;
    V_string                          m_GameplayEventTypes;
    M_EventToValues                   m_EventValues;
    
    std::string                       m_CinematicName;
    tuid                              m_CinematicID;
    CinematicEventsDocumentPtr       m_File;
    CinematicEventsEditor*           m_Editor;
    bool                              m_FilterEvents;
    bool                              m_UpdateProperties;

    int                               m_CurrentFrame;

    Content::ScenePtr                 m_CinematicScene; 
    int                               m_FrameCount;
    int                               m_StartFrame;
    int                               m_EndFrame;
    int                               m_ZoomStartFrame;
    int                               m_ZoomEndFrame;
    M_ActorToJoint                    m_ActorJoints;
    V_string                          m_Actors;

    M_ClipToFrames                    m_ClipFrames;
    Asset::CinematicAssetPtr          m_CurrentClip;
    Asset::CinematicEventListPtr      m_CurrentEvents;

    Asset::OS_CinematicEvent          m_Selection;
    Asset::OS_CinematicEvent          m_CopyBuffer;
    Asset::OS_CinematicEvent          m_EventsAtCurrentFrame;

    Undo::Queue                      m_UndoQueue;

    // Events
    CinematicFileChangeSignature::Event           m_Opened;
    CinematicFileChangeSignature::Event           m_Closed;
    CinematicChangeSignature::Event               m_ClipChanged;
    FrameChangedSignature::Event                  m_FrameChanged;
    EventExistenceSignature::Event                m_EventExistence;
    EventSelectionChangedSignature::Event         m_SelectionChanged;
    EventsChangedSignature::Event                 m_EventsChanged;
    CinematicLoadedSignature::Event               m_CinematicLoaded;
    EventTypesChangedSignature::Event             m_EventTypesChanged;

    DECLARE_EVENT_TABLE();
  };
}
