#pragma once

#include "Editor/EditorGenerated.h"

#include "Editor/ViewCanvas.h"

namespace Helium
{
    namespace Editor
    {
        class ViewPanel : public ViewPanelGenerated
        {
        public:
            ViewPanel( wxWindow* parent = NULL, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );

            ViewCanvas* GetViewCanvas()
            {
                return m_ViewCanvas;
            }

        private:
            ViewCanvas* m_ViewCanvas;
        };
    }
}