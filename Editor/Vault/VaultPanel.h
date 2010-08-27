#pragma once

#include "Editor/EditorGenerated.h"
#include "Editor/Vault/VaultSearch.h"

namespace Helium
{
    namespace Editor
    {
        class VaultPanel : public VaultPanelGenerated
        {
        public:
            VaultPanel( VaultSearch* vaultSearch, wxWindow* parent = NULL, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );

            void Search( const tstring& queryString );

        private:
            VaultSearch*            m_VaultSearch;
        };
    }
}