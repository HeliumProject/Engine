#include "DropSource.h"

#include <wx/timer.h>

using namespace Inspect;

static const int s_RaiseTimeMilliseconds = 2000; // 2 seconds
class DragTimer : public wxTimer
{
public:
  DragTimer()
  {
  }

  virtual void Notify() NOC_OVERRIDE
  {
    wxPoint point;
    wxWindow* window = ::wxFindWindowAtPointer( point );
    while ( window && window->GetParent() )
    {
      window = window->GetParent();
    }
    
    if ( window )
    {
      window->Raise();
    }
  }
};

DropSource::DropSource( wxWindow* win, const wxCursor& copy, const wxCursor& move, const wxCursor& none )
: wxDropSource( win, copy, move, none )
, m_AutoRaise( false )
{
}

DropSource::DropSource( wxDataObject& data, wxWindow* win, const wxCursor& copy, const wxCursor& move, const wxCursor& none )
: wxDropSource( data, win, copy, move, none )
, m_AutoRaise( false )
{
}

void DropSource::SetAutoRaise( bool autoRaise )
{
  m_AutoRaise = autoRaise;
}

wxDragResult DropSource::DoDragDrop( int flags )
{
  DragTimer timer;
  if ( m_AutoRaise )
  {
    timer.Start( s_RaiseTimeMilliseconds, wxTIMER_CONTINUOUS );
  }

  wxDragResult result = __super::DoDragDrop( flags );
  timer.Stop();
  return result;
}
