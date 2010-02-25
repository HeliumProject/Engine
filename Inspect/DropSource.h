#pragma once

#include "API.h"
#include "Common/Compiler.h"
#include <wx/dnd.h>

namespace Inspect
{
  /////////////////////////////////////////////////////////////////////////////
  // By default, this class behaves exactly like wxDropSource.  However, if you
  // use this class, you can call the SetAutoRaise function (passing in true)
  // before you start a drag-and-drop operation.  This feature will poll the
  // mouse position during the drag-and-drop operation and if your mouse moves
  // over another top level window within the same application, that window 
  // will be brought to the front.
  // 
  class INSPECT_API DropSource : public wxDropSource
  {
  public:
    DropSource( wxWindow* win = NULL, const wxCursor& copy = wxNullCursor, const wxCursor& move = wxNullCursor, const wxCursor& none = wxNullCursor );
    DropSource( wxDataObject& data, wxWindow* win = NULL, const wxCursor& copy = wxNullCursor, const wxCursor& move = wxNullCursor, const wxCursor& none = wxNullCursor );

    void SetAutoRaise( bool autoRaise );

    virtual wxDragResult DoDragDrop( int flags = wxDrag_CopyOnly ) NOC_OVERRIDE;

  private:
    bool m_AutoRaise;
  };
}
