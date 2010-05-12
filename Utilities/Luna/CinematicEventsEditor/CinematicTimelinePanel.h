#pragma once

#include "CinematicPanelBase.h"
#include "CinematicEventsManager.h"
#include "CinematicEventsUndoCommands.h"

#include "Asset/AnimationClip.h"

class CinematicTimelinePanel;

namespace Luna
{
  struct Marker
  {
    Marker()
      : m_Selected( false )
    {}

    Asset::CinematicEventPtr     m_Event;
    wxRect                            m_Rect;
    wxColour                          m_Color;
    bool                              m_Selected;
  };
  typedef std::vector< Marker > V_Marker;

  class CinematicEventsTimelinePanel : public CinematicPanelBase< CinematicTimelinePanel >
  {
  public:
    CinematicEventsTimelinePanel( CinematicEventsManager& manager, CinematicTimelinePanel* panel );
    ~CinematicEventsTimelinePanel();

    void DrawEventMarkers( wxDC& dc );
    void UpdateMarkers();

    void EnableAll();
    void DisableAll();

    const Asset::CinematicEventPtr FindEvent( int x, int y );

    void ClipChanged( const CinematicChangeArgs& args );
    void CinematicLoaded( const CinematicLoadedArgs& args );
    void FrameChanged( const FrameChangedArgs& args );
    void EventExistence( const EventExistenceArgs& args );
    void SelectionChanged( const EventSelectionChangedArgs& args );
    void EventsChanged( const EventsChangedArgs& args );
    void AssetOpened( const CinematicFileChangeArgs& args );
    void AssetClosed( const CinematicFileChangeArgs& args );

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
    void OnMoveSelectedFrames( wxCommandEvent& e );

    void OnSetZoomStart( wxCommandEvent& e );
    void OnSetZoomEnd( wxCommandEvent& e );
    void OnZoom( wxMouseEvent& e );

  private:
    Asset::CinematicAssetPtr  m_CurrentClip;
    V_Marker                      m_Markers;

    bool                          m_MarkerMouseDown;
    bool                          m_MovedEvents;
    bool                          m_SliderMouseDown;
    bool                          m_MouseWasDown;
    int                           m_PreMoveMouseTicLocation;
    int                           m_LastMouseTicLocation;
    int                           m_LastSliderValue;
    Undo::CommandPtr              m_UndoCommand;

    bool                          m_MultiSelect;
    Asset::CinematicEventPtr m_LastSelected;

    int                           m_LastWidth;
    int                           m_LastHeight;

    DECLARE_EVENT_TABLE();
  };
}