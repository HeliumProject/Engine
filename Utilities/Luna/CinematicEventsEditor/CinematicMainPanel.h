#pragma once

#include "CinematicPanelBase.h"
#include "CinematicEventsDocument.h"

#include "CinematicEventsManager.h"

#include "Asset/AnimationClip.h"

class CinematicPanel;

namespace Luna
{
  class CinematicMainPanel : public CinematicPanelBase< CinematicPanel >
  {
  public:
    CinematicMainPanel( CinematicEventsManager& manager, CinematicPanel* panel );
    ~CinematicMainPanel();

    void Clear();
    void DisableAll();
    void EnableAll();
    
    void Populate( const CinematicEventsDocumentPtr& file );
    void PopulateActorList();
    void PopulateEventTypes();
    void PopulateEffectSubsystems();

    void PopulateAppliedValues();
    void PopulateJointList();
    void PopulateEffects();

    void AssetOpened( const CinematicFileChangeArgs& args );
    void AssetClosed( const CinematicFileChangeArgs& args );
    void ClipChanged( const CinematicChangeArgs& args );
    void CinematicLoaded( const CinematicLoadedArgs& args );
    void EventTypesChanged( const EventTypesChangedArgs& args );

    void AddEvent();

    // wx Handlers
    void OnClipChanged( wxCommandEvent& evt );
    void OnAddEvent( wxCommandEvent& evt );
    void OnChar( wxKeyEvent& e );
    void OnDeleteSelection( wxCommandEvent& evt );
    void OnFilterEvents( wxCommandEvent& evt );
    void OnCinematic( wxCommandEvent& evt );
    void OnRebuild( wxCommandEvent& evt );
    void OnSync( wxCommandEvent& evt );
    void OnStopAll( wxCommandEvent& evt );
    void OnEventType( wxCommandEvent& evt );
    void OnActor( wxCommandEvent& evt );
    void OnSubsystem( wxCommandEvent& evt );
    void OnRefreshEvents( wxCommandEvent& evt );
    void OnGameplayEvent( wxCommandEvent& evt );
    void OnOverrideEvents( wxCommandEvent& evt );

  private:
    Asset::CinematicAssetPtr m_CurrentClip;

    DECLARE_EVENT_TABLE();
  };
}