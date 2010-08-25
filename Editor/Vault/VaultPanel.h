#pragma once

#include "Editor/EditorGenerated.h"

#include "Editor/Vault/VaultSearch.h"
#include "Editor/Vault/Vault.h"

namespace Helium
{
    namespace Editor
    {
        class VaultPanel : public VaultPanelGenerated
        {
        public:
            VaultPanel( Vault* vault, VaultSearch* vaultSearch, SearchHistory* searchHistory, wxWindow* parent = NULL, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );

            void Search( const tstring& queryString );

        private:
            Vault*                  m_Vault;
            VaultSearch*            m_VaultSearch;
            SearchHistory*          m_SearchHistory;
        };
    }
}