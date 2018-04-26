#pragma once

#include "Platform/System.h"

#include <wx/dnd.h>

#if HELIUM_OS_LINUX
# define DropSourceCursor wxIcon
# define DropSourceCursorDefault wxNullIcon
#else
# define DropSourceCursor wxCursor
# define DropSourceCursorDefault wxNullCursor
#endif

namespace Helium
{
    namespace Editor
    {
        /////////////////////////////////////////////////////////////////////////////
        // By default, this class behaves exactly like wxDropSource.  However, if you
        // use this class, you can call the SetAutoRaise function (passing in true)
        // before you start a drag-and-drop operation.  This feature will poll the
        // mouse position during the drag-and-drop operation and if your mouse moves
        // over another top level window within the same application, that window 
        // will be brought to the front.
        // 
        class DropSource : public wxDropSource
        {
        public:
            DropSource( wxWindow* win = NULL, const DropSourceCursor& copy = DropSourceCursorDefault, const DropSourceCursor& move = DropSourceCursorDefault, const DropSourceCursor& none = DropSourceCursorDefault );
            DropSource( wxDataObject& data, wxWindow* win = NULL, const DropSourceCursor& copy = DropSourceCursorDefault, const DropSourceCursor& move = DropSourceCursorDefault, const DropSourceCursor& none = DropSourceCursorDefault );

            void SetAutoRaise( bool autoRaise );

            virtual wxDragResult DoDragDrop( int flags = wxDrag_CopyOnly ) override;

        private:
            bool m_AutoRaise;
        };
    }
}