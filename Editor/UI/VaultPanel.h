#pragma once

#include "Editor/UI/EditorGenerated.h"

namespace Helium
{
    namespace Editor
    {
        class VaultPanel : public VaultPanelGenerated
        {
        public:
            VaultPanel( wxWindow* parent = NULL, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
        };
    }
}