#pragma once

#include "Editor/UI/EditorGenerated.h"

#include "Editor/UI/Viewport.h"

namespace Helium
{
    namespace Editor
    {
        class ViewPanel : public ViewPanelGenerated
        {
        public:
            ViewPanel( wxWindow* parent = NULL, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );

            Viewport* GetViewport()
            {
                return m_Viewport;
            }

        private:
            Viewport* m_Viewport;
        };
    }
}