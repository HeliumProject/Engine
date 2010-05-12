#pragma once

#include "wx/slider.h"

namespace Luna
{
  class CinematicTimelineSlider : public wxSlider
  {
  public:
    CinematicTimelineSlider( wxWindow* parent, wxWindowID id, int value , int minValue, int maxValue, const wxPoint& point = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxSL_HORIZONTAL, const wxValidator& validator = wxDefaultValidator, const wxString& name = "slider" );
    ~CinematicTimelineSlider();

    void OnMouseDown(wxMouseEvent& e);
    void OnMouseUp(wxMouseEvent& e);
    void OnMouseMove(wxMouseEvent& e);

    int PixelToThumbPos( int x );
    int ThumbPosToPixel( int val ) const;
    bool IsMouseOnChannel( int x, int y );
    bool IsMouseOnThumb( int x, int y );

  private:

    bool m_MouseDown;
    
    DECLARE_EVENT_TABLE();
  };
}