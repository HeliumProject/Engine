#pragma once

#include "Editor/EditorGeneratedWrapper.h"

namespace Helium
{
    namespace Editor
    {
        class HelpPanel : public HelpPanelGenerated
        {
        public:
            HelpPanel( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL );

            void SetText( const char* text );
        };
    }
}