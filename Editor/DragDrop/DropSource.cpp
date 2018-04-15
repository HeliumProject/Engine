#include "Precompile.h"
#include "DropSource.h"

#include <wx/window.h>
#include <wx/timer.h>

using namespace Helium;
using namespace Helium::Editor;

static const int s_RaiseTimeMilliseconds = 2000; // 2 seconds
class DragTimer : public wxTimer
{
public:
  DragTimer()
  {
  }

  virtual void Notify() override
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

DropSource::DropSource( wxWindow* win, const DropSourceCursor& copy, const DropSourceCursor& move, const DropSourceCursor& none )
: wxDropSource( win, copy, move, none )
, m_AutoRaise( false )
{
}

DropSource::DropSource( wxDataObject& data, wxWindow* win, const DropSourceCursor& copy, const DropSourceCursor& move, const DropSourceCursor& none )
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

  wxDragResult result = wxDropSource::DoDragDrop( flags );
  timer.Stop();
  return result;
}
