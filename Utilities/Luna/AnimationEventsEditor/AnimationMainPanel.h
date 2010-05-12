#pragma once

#include "AnimationPanelBase.h"
#include "AnimationEventsDocument.h"

#include "AnimationEventsManager.h"

#include "Asset/AnimationClip.h"

#include "Inspect/FilteredDropTarget.h"

class AnimationPanel;

namespace Luna
{
  class AnimationEventsMainPanel : public AnimationPanelBase< AnimationPanel >
  {
  public:
    AnimationEventsMainPanel( AnimationEventsManager& manager, AnimationPanel* panel );
    ~AnimationEventsMainPanel();

    void Clear();
    void DisableAll();
    void EnableAll();
    
    void Populate( const AnimationEventsDocumentPtr& file );
    void PopulateEventTypes();
    void PopulateAppliedValues();

    void AssetOpened( const MobyFileChangeArgs& args );
    void AssetClosed( const MobyFileChangeArgs& args );
    void ClipChanged( const AnimClipChangeArgs& args );
    void UpdateClassChanged( const UpdateClassChangedArgs& args );
    void EventTypesChanged( const EventTypesChangedArgs& args );

    void AddEvent();

    // wx Handlers
    void OnClipChanged( wxCommandEvent& evt );
    void OnAddEvent( wxCommandEvent& evt );
    void OnChar( wxKeyEvent& e );
    void OnDeleteSelection( wxCommandEvent& evt );
    void OnRebuildMoby( wxCommandEvent& evt );
    void OnFilterEvents( wxCommandEvent& evt );
    void OnUpdateClass( wxCommandEvent& evt );
    void OnAudition( wxCommandEvent& evt );
    void OnStopAll( wxCommandEvent& evt );
    void OnEventType( wxCommandEvent& evt );
    void OnRefreshEvents( wxCommandEvent& evt );
    void OnGameplayEvent( wxCommandEvent& evt );
    void OnOverrideEvents( wxCommandEvent& evt );
    void OnEntityDropped( const Inspect::FilteredDropTargetArgs& args );

  private:
    Asset::AnimationClipDataPtr m_CurrentClip;

    DECLARE_EVENT_TABLE();
  };
}