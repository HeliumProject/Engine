#include "Precompile.h"

#include "Editor/Utilities.h"

bool Helium::IsMouseInWindow( wxWindow *win )
{
   wxPoint pt = ::wxGetMousePosition();
   pt = win->ScreenToClient( pt );
   wxRect rec = win->GetClientRect();

   return rec.Contains( pt );
}