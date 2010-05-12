#pragma once

#include "CinematicPanelBase.h"
#include "CinematicEventsManager.h"

#include "Asset/CinematicAsset.h"

class CinematicActivePanel;

namespace Luna
{
  class CinematicActiveEventsPanel : public CinematicPanelBase< CinematicActivePanel >
  {
  public:
    CinematicActiveEventsPanel( CinematicEventsManager& manager, CinematicActivePanel* panel );
    ~CinematicActiveEventsPanel();

    void EnableAll();
    void DisableAll();

    void ClipChanged( const CinematicChangeArgs& args );
    void FrameChanged( const FrameChangedArgs& args );
    void EventExistence( const EventExistenceArgs& args );
    void SelectionChanged( const EventSelectionChangedArgs& args );
    void EventsChanged( const EventsChangedArgs& args );
    void CinematicLoaded( const CinematicLoadedArgs& args );
    void EventTypesChanged( const EventTypesChangedArgs& args );
    void AssetOpened( const CinematicFileChangeArgs& args );
    void AssetClosed( const CinematicFileChangeArgs& args );

    void UpdateEventList();
    void UpdateSelection();

    void ResizeColumns();

    void OnSize( wxSizeEvent& event );
    void OnSelect( wxCommandEvent& event );
    void OnRangeSelect( wxGridRangeSelectEvent& event );
    void OnCellChange( wxGridEvent& event );
    void OnChar( wxKeyEvent& e );
    void OnUpdateEventList( wxCommandEvent& event );

  private:
    Asset::CinematicAssetPtr m_CurrentClip;
    int                          m_CurrentFrame;

    Asset::V_CinematicEvent      m_CurrentEvents;

    bool                         m_Freeze;

    DECLARE_EVENT_TABLE();
  };
}