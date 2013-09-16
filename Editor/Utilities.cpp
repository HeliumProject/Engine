#include "Utilities.h"

bool IsMouseInWindow( wxWindow *win )
{
   wxPoint pt = ::wxGetMousePosition();
   pt = win->ScreenToClient( pt );
   wxRect rec = win->GetRect();

   return rec.Inside( pt );
}