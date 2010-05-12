#pragma once

#include "AnimationEventsDocument.h"

#include "Undo/Queue.h"
#include "Editor/DocumentManager.h"
#include "Task/Build.h"

#include "Asset/AnimationClip.h"
#include "Symbol/Enum.h"

namespace RPC
{
  struct AnimClipInfo;
}

namespace Luna
{
  class AnimationEventsEditor;

  struct MobyFileChangeArgs
  {
    AnimationEventsDocumentPtr& m_File;

    MobyFileChangeArgs( AnimationEventsDocumentPtr& file )
      : m_File( file )
    {
    }
  };
  typedef Nocturnal::Signature< void, const MobyFileChangeArgs& > MobyFileChangeSignature;

  struct AnimClipChangeArgs
  {
    Asset::AnimationClipDataPtr& m_Clip;

    AnimClipChangeArgs( Asset::AnimationClipDataPtr& clip )
      : m_Clip( clip )
    {
    }
  };
  typedef Nocturnal::Signature< void, const AnimClipChangeArgs& > AnimClipChangeSignature;

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
    Asset::AnimationEventPtr m_Event;
    bool                          m_Created;

    EventExistenceArgs( const Asset::AnimationEventPtr& event, bool created )
      : m_Event( event )
      , m_Created( created )
    {

    }
  };
  typedef Nocturnal::Signature< void, const EventExistenceArgs& > EventExistenceSignature;

  struct EventSelectionChangedArgs
  {
    const Asset::OS_AnimationEvent& m_Selection;

    EventSelectionChangedArgs( const Asset::OS_AnimationEvent& selection )
      : m_Selection( selection )
    {

    }
  };
  typedef Nocturnal::Signature< void, const EventSelectionChangedArgs& > EventSelectionChangedSignature;

  struct EventsChangedArgs
  {
    const Asset::OS_AnimationEvent& m_Events;

    EventsChangedArgs( const Asset::OS_AnimationEvent& events )
      : m_Events( events )
    {

    }
  };
  typedef Nocturnal::Signature< void, const EventsChangedArgs& > EventsChangedSignature;

  struct UpdateClassChangedArgs
  {
    const std::string& m_UpdateClass;
    const int m_Index;

    UpdateClassChangedArgs( const std::string& updateClass, const int index )
      : m_UpdateClass( updateClass )
      , m_Index( index )
    {

    }
  };
  typedef Nocturnal::Signature< void, const UpdateClassChangedArgs& > UpdateClassChangedSignature;

  struct EventTypesChangedArgs
  {
    int pad;
  };
  typedef Nocturnal::Signature< void, const EventTypesChangedArgs& > EventTypesChangedSignature;


  typedef std::map< Asset::AnimationClipDataPtr, int > M_ClipToFrames;
  typedef std::map< std::string, S_u32 > M_EventToValues;

  struct RuntimeConnectionStatusArgs;

  class AnimationEventsManager : public wxEvtHandler, public DocumentManager
  {
  public:
    AnimationEventsManager( AnimationEventsEditor* editor );
    virtual ~AnimationEventsManager();

    const Symbol::EnumPtr& GetEventTypesEnum( bool gameplay ) { return (gameplay ? m_GameplayEventTypesEnum : m_AnimationEventTypesEnum); }
    const V_string& GetEventTypes( const Asset::AnimationEventPtr& evt );
    const V_string& GetEventTypes( bool gameplay );
    void SetEventTypeFilter( bool set );
    bool GetEventTypeFilter();
    const S_u32& GetAppliedValues( const std::string& eventType );

    void SetUpdateClass( const int index );
    const std::string& GetUpdateClass();

    void UpdateEventTypes();

    const AnimationEventsDocumentPtr& GetFile() { return m_File; }
    AnimationEventsEditor* GetEditor() { return m_Editor; }

    virtual DocumentPtr OpenPath( const std::string& path, std::string& error ) NOC_OVERRIDE;
    bool Save( std::string& error );
    virtual bool Save( DocumentPtr document, std::string& error ) NOC_OVERRIDE;
    bool Close();
    virtual bool CloseDocument( DocumentPtr document, bool prompt = true ) NOC_OVERRIDE;

    void Cut();
    void Copy();
    void Paste();

    bool ViewerConnected( int numClips, RPC::AnimClipInfo* clipInfo );
    void RuntimeConnectionStatus( const RuntimeConnectionStatusArgs& args );
    void BuildMoby( tuid& assetId );
    void BuildFinished( const TaskFinishedArgs& args );

    void SetClip( Asset::AnimationClipDataPtr& clip, bool undo = true );
    void SetClip( int clipNum, bool undo = true );
    const Asset::AnimationClipDataPtr& GetCurrentClip();
    i32 GetCurrentClipIndex();
    void PopulateClipFrames();
    int GetUpdateClassVTGenIndex();

    void SetFrame( int frame );
    int GetNumFrames( const Asset::AnimationClipDataPtr& clip );
    int GetNumFrames();
    int GetCurrentFrame();
    tuid GetTuid();

    void SetPlayRate( float rate );

    const Asset::AnimationEventListPtr& GetEventList();
    bool SetOverrideEvents( bool overrideEvents );

    void PlayEvent( const std::string& eventType, const float value, const u32 value2, bool untrigger, bool infinite, bool dominantOnly, bool gameplay );
    void StopAll();
    void AddEvent( const Asset::AnimationEventPtr& event );
    void AddEvents( const Asset::OS_AnimationEvent& events );
    void RemoveEvent( const Asset::AnimationEventPtr& event );
    void RemoveEvents( const Asset::OS_AnimationEvent& events );
    void EventChanged( const Asset::AnimationEventPtr& event );
    void EventsChanged( const Asset::OS_AnimationEvent& events );

    wxColour GetEventColor( const std::string& eventType );

    Asset::OS_AnimationEvent& GetSelection();
    void SetSelection( const Asset::OS_AnimationEvent& selection );
    void AddSelection( const Asset::AnimationEventPtr& event );
    void RemoveSelection( const Asset::AnimationEventPtr& event );
    void ClearSelection();
    void SelectAll();

    void DeleteSelected();

    const Asset::OS_AnimationEvent& GetEventsAtCurrentFrame();

    Undo::Queue& GetUndoQueue()
    {
      return m_UndoQueue;
    }

    void OnDocumentClosed( const DocumentChangedArgs& args );
    void OnBuildFinished( wxCommandEvent& event );

    // Listeners
    void AddMobyOpenedListener( const MobyFileChangeSignature::Delegate& listener );
    void RemoveMobyOpenedListener( const MobyFileChangeSignature::Delegate& listener );
    void AddMobyClosedListener( const MobyFileChangeSignature::Delegate& listener );
    void RemoveMobyClosedListener( const MobyFileChangeSignature::Delegate& listener );
    void AddAnimClipChangeListener( const AnimClipChangeSignature::Delegate& listener );
    void RemoveAnimClipChangeListener( const AnimClipChangeSignature::Delegate& listener );
    void AddFrameChangedListener( const FrameChangedSignature::Delegate& listener );
    void RemoveFrameChangedListener( const FrameChangedSignature::Delegate& listener );
    void AddEventExistenceListener( const EventExistenceSignature::Delegate& listener );
    void RemoveEventExistenceListener( const EventExistenceSignature::Delegate& listener );
    void AddSelectionChangedListener( const EventSelectionChangedSignature::Delegate& listener );
    void RemoveSelectionChangedListener( const EventSelectionChangedSignature::Delegate& listener );
    void AddEventsChangedListener( const EventsChangedSignature::Delegate& listener );
    void RemoveEventsChangedListener( const EventsChangedSignature::Delegate& listener );
    void AddUpdateClassChangedListener( const UpdateClassChangedSignature::Delegate& listener );
    void RemoveUpdateClassChangedListener( const UpdateClassChangedSignature::Delegate& listener );
    void AddEventTypesChangedListener( const EventTypesChangedSignature::Delegate& listener );
    void RemoveEventTypesChangedListener( const EventTypesChangedSignature::Delegate& listener );

  private:
    Symbol::EnumPtr                   m_AnimationEventTypesEnum;
    Symbol::EnumPtr                   m_GameplayEventTypesEnum;
    V_string                          m_AllEventTypes;
    V_string                          m_AppliedEventTypes;
    V_string                          m_GameplayEventTypes;
    M_EventToValues                   m_EventValues;
    
    std::string                       m_UpdateClass;
    int                               m_UpdateClassIndex;
    AnimationEventsDocumentPtr          m_File;
    AnimationEventsEditor*              m_Editor;
    bool                              m_FilterEvents;
    tuid                              m_AssetTuid;

    int                               m_CurrentFrame;
    M_ClipToFrames                    m_ClipFrames;
    Asset::AnimationClipDataPtr       m_CurrentClip;
    Asset::AnimationEventListPtr               m_CurrentEvents;

    Asset::OS_AnimationEvent     m_Selection;
    Asset::OS_AnimationEvent     m_CopyBuffer;
    Asset::OS_AnimationEvent     m_EventsAtCurrentFrame;

    Undo::Queue                      m_UndoQueue;

    // Events
    MobyFileChangeSignature::Event                m_Opened;
    MobyFileChangeSignature::Event                m_Closed;
    AnimClipChangeSignature::Event                m_ClipChanged;
    FrameChangedSignature::Event                  m_FrameChanged;
    EventExistenceSignature::Event                m_EventExistence;
    EventSelectionChangedSignature::Event         m_SelectionChanged;
    EventsChangedSignature::Event                 m_EventsChanged;
    UpdateClassChangedSignature::Event            m_UpdateClassChanged;
    EventTypesChangedSignature::Event             m_EventTypesChanged;

    DECLARE_EVENT_TABLE();
  };
}