#pragma once

#include "AnimationPanelBase.h"
#include "AnimationEventsManager.h"

#include "Asset/AnimationClip.h"

class AnimationActivePanel;

namespace Luna
{
  class AnimationActiveEventsPanel : public AnimationPanelBase< AnimationActivePanel >
  {
  public:
    AnimationActiveEventsPanel( AnimationEventsManager& manager, AnimationActivePanel* panel );
    ~AnimationActiveEventsPanel();

    void EnableAll();
    void DisableAll();

    void ClipChanged( const AnimClipChangeArgs& args );
    void FrameChanged( const FrameChangedArgs& args );
    void EventExistence( const EventExistenceArgs& args );
    void SelectionChanged( const EventSelectionChangedArgs& args );
    void EventsChanged( const EventsChangedArgs& args );
    void UpdateClassChanged( const UpdateClassChangedArgs& args );
    void EventTypesChanged( const EventTypesChangedArgs& args );
    void AssetOpened( const MobyFileChangeArgs& args );
    void AssetClosed( const MobyFileChangeArgs& args );

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
    Asset::AnimationClipDataPtr m_CurrentClip;
    int                          m_CurrentFrame;

    Asset::V_AnimationEvent m_CurrentEvents;

    bool                         m_Freeze;

    DECLARE_EVENT_TABLE();
  };
}