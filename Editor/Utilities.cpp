#include "EditorPch.h"

#include "Editor/Utilities.h"

bool Helium::IsMouseInWindow( wxWindow *win )
{
   wxPoint pt = ::wxGetMousePosition();
   pt = win->ScreenToClient( pt );
   wxRect rec = win->GetRect();

   return rec.Contains( pt );
}