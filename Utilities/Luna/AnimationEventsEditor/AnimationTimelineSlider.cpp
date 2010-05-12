#include "Precompile.h"

#include "AnimationTimelineSlider.h"

#include <CommCtrl.h>

using namespace Luna;

BEGIN_EVENT_TABLE( AnimationTimelineSlider, wxSlider )
EVT_LEFT_DOWN( OnMouseDown )
EVT_LEFT_UP( OnMouseUp )
EVT_MOTION( OnMouseMove )
END_EVENT_TABLE()

AnimationTimelineSlider::AnimationTimelineSlider(wxWindow* parent, wxWindowID id, int value , int minValue, int maxValue, const wxPoint& point /* = wxDefaultPosition */, const wxSize& size /* = wxDefaultSize */, long style /* = wxSL_HORIZONTAL */, const wxValidator& validator /* = wxDefaultValidator */, const wxString& name /* =  */)
: wxSlider( parent, id, value, minValue, maxValue, point, size, style, validator, name )
, m_MouseDown( false )
{
  SetPageSize( 0 );
}

AnimationTimelineSlider::~AnimationTimelineSlider()
{

}

void AnimationTimelineSlider::OnMouseDown( wxMouseEvent& e )
{
  e.Skip();

  int x = e.GetX();
  int y = e.GetY();

  if ( !IsMouseOnThumb( x, y ) )
  {
    m_MouseDown = true;

    SetValue( PixelToThumbPos( e.GetPosition().x ) );
  }
}

void AnimationTimelineSlider::OnMouseUp( wxMouseEvent& e )
{
  e.Skip();

  m_MouseDown = false;
}

void AnimationTimelineSlider::OnMouseMove( wxMouseEvent& e )
{
  e.Skip();

  if ( m_MouseDown )
  {
    SetValue( PixelToThumbPos( e.GetPosition().x ) );
  }
}

bool AnimationTimelineSlider::IsMouseOnChannel( int x, int y )
{
  RECT channelRect;
  SendMessage( GetHwnd(), TBM_GETCHANNELRECT, 0, (LPARAM)&channelRect );

  if ( x >= channelRect.left && x <= channelRect.right
    && y >= channelRect.top && y <=  channelRect.bottom )
    return true;

  return false;
}

bool AnimationTimelineSlider::IsMouseOnThumb( int x, int y )
{
  RECT thumbRect;
  SendMessage( GetHwnd(), TBM_GETTHUMBRECT, 0, (LPARAM)&thumbRect );

  if ( x >= thumbRect.left && x <= thumbRect.right
    && y >= thumbRect.top && y <= thumbRect.bottom )
    return true;

  return false;
}

int AnimationTimelineSlider::ThumbPosToPixel( int val ) const
{
  wxRect rect = GetRect();

  RECT thumbRect;
  SendMessage( GetHwnd(), TBM_GETTHUMBRECT, 0, (LPARAM)&thumbRect );

  int thumbWidth = thumbRect.right - thumbRect.left;

  int x;
  
  --val;
  if ( val < 0 )
  {
    x = thumbWidth;
  }
  else if ( val > GetMax() - 2 )
  {
    x = rect.GetWidth() - thumbWidth;
  }
  else 
  {
    x = SendMessage( GetHwnd(), TBM_GETTICPOS, val, 0 ) - rect.GetLeft() + thumbWidth / 2;
  }

  return x;
}

int AnimationTimelineSlider::PixelToThumbPos( int x )
{
  RECT channelRect;
  RECT thumbRect;
  SendMessage( GetHwnd(), TBM_GETCHANNELRECT, 0, (LPARAM)&channelRect );
  SendMessage( GetHwnd(), TBM_GETTHUMBRECT, 0, (LPARAM)&thumbRect );
  
  int channelWidth = channelRect.right - channelRect.left;
  int thumbWidth = thumbRect.right - thumbRect.left;

  wxCoord x0 = channelRect.left;
  wxCoord len = channelWidth - thumbWidth;

  int min = GetMin();
  int max = GetMax();
  int pos = min;
  if ( len > 0 )
  {
    if ( x > x0 )
    {
      pos += ((x - x0) * (max - min)) / len;
      if ( pos > max )
        pos = max;
    }
    //else: x <= x0, leave pos = min
  }

  return pos;
}