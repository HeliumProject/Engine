#pragma once

#include "LunaGenerated.h"

namespace Luna
{
    class VaultPanel : public VaultPanelGenerated
    {
    public:
   	    VaultPanel( wxWindow* parent = NULL, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
    };
}