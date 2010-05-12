#pragma once

#include "AnimationPanelBase.h"
#include "AnimationEventsManager.h"
#include "AnimationEventsUndoCommands.h"

#include "Asset/AnimationClip.h"

class AnimationTimelinePanel;

namespace Luna
{
  struct Marker
  {
    Marker()
      : m_Selected( false )
    {}

    Asset::AnimationEventPtr     m_Event;
    wxRect                            m_Rect;
    wxColour                          m_Color;
    bool                              m_Selected;
  };
  typedef std::vector< Marker > V_Marker;

  class AnimationEventsTimelinePanel : public AnimationPanelBase< AnimationTimelinePanel >
  {
  public:
    AnimationEventsTimelinePanel( AnimationEventsManager& manager, AnimationTimelinePanel* panel );
    ~AnimationEventsTimelinePanel();

    void DrawEventMarkers( wxDC& dc );
    void UpdateMarkers();

    void EnableAll();
    void DisableAll();

    const Asset::AnimationEventPtr FindEvent( int x, int y );

    void ClipChanged( const AnimClipChangeArgs& args );
    void FrameChanged( const FrameChangedArgs& args );
    void EventExistence( const EventExistenceArgs& args );
    void SelectionChanged( const EventSelectionChangedArgs& args );
    void EventsChanged( const EventsChangedArgs& args );
    void AssetOpened( const MobyFileChangeArgs& args );
    void AssetClosed( const MobyFileChangeArgs& args );

    void OnSliderValueChanged( wxScrollEvent& e );
    void OnMarkersPaint( wxPaintEvent& e );

    void OnMarkerMouseDown( wxMouseEvent& e );
    void OnMarkerMouseUp( wxMouseEvent& e );
    void OnMarkerMouseMove( wxMouseEvent& e );

    void OnSliderMouseDown( wxMouseEvent& e );
    void OnSliderMouseUp( wxMouseEvent& e );

    void OnSize( wxSizeEvent& event );
    void OnChar( wxKeyEvent& e );

    void OnPause( wxCommandEvent& e );
    void OnPlay( wxCommandEvent& e );
    void OnGotoFrame( wxCommandEvent& e );

  private:
    Asset::AnimationClipDataPtr  m_CurrentClip;
    V_Marker                      m_Markers;

    bool                          m_MarkerMouseDown;
    bool                          m_MovedEvents;
    bool                          m_SliderMouseDown;
    int                           m_LastMouseTicLocation;
    int                           m_LastSliderValue;
    Undo::CommandPtr             m_UndoCommand;

    bool                          m_MultiSelect;
    Asset::AnimationEventPtr m_LastSelected;

    int                           m_LastWidth;
    int                           m_LastHeight;

    DECLARE_EVENT_TABLE();
  };
}