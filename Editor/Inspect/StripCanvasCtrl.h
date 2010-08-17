#pragma once

#include "Application/API.h"
#include "Application/Inspect/Controls/InspectCanvas.h"

namespace Helium
{
    namespace Inspect
    {
        class APPLICATION_API StripCanvasCtrl : public wxPanel
        {
        public:
            Canvas* m_Canvas;

            StripCanvasCtrl( wxWindow *parent,
                wxWindowID winid = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTAB_TRAVERSAL | wxNO_BORDER,
                const wxString& name = wxT( "StripCanvasCtrl" ) );

            Canvas* GetCanvas()
            {
                return m_Canvas;
            }

            void SetCanvas(Canvas* canvas)
            {
                m_Canvas = canvas;
            }

        protected:
            void OnShow(wxShowEvent&);
            void OnSize(wxSizeEvent&);
            void OnClick(wxMouseEvent&);
            void OnToggle(wxTreeEvent&);

            DECLARE_EVENT_TABLE();
        };
    }
}