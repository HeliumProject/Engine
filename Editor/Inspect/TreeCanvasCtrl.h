#pragma once

#include "Foundation/Inspect/Controls/CanvasControl.h"
#include "Editor/Controls/Tree/TreeWndCtrl.h"

namespace Helium
{
    namespace Inspect
    {
        class TreeCanvasCtrl : public Helium::TreeWndCtrl
        {
        public:
            Canvas* m_Canvas;

            TreeCanvasCtrl(wxWindow *parent,
                wxWindowID winid = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxScrolledWindowStyle,
                const wxString& name = wxT( "TreeCanvasCtrl" ),
                int treeStyle = ( wxTR_ALL_LINES | wxTR_HIDE_ROOT ),
                unsigned int columnSize = WXTWC_DEFAULT_COLUMN_SIZE,
                wxBitmap expandedBitmap = Helium::TreeWndCtrlDefaultExpand,
                wxBitmap collapsedBitmap = Helium::TreeWndCtrlDefaultCollapse,
                wxPen pen = Helium::TreeWndCtrlDefaultPen,
                unsigned int clickTolerance = WXTWC_DEFAULT_CLICK_TOLERANCE);

            Canvas* GetCanvas();
            void SetCanvas(Canvas* canvas);

        protected:
            void OnShow(wxShowEvent&);
            void OnSize(wxSizeEvent&);
            void OnClick(wxMouseEvent&);
            void OnToggle(wxTreeEvent&);

            DECLARE_EVENT_TABLE();
        };
    }
}