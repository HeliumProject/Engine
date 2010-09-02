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
            VaultPanel( wxWindow* parent = NULL, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );

            void SetDirectory( const Helium::Path& directory );
            const Helium::Path& GetDirectory() const;

            void Search( const tstring& queryString );

        protected:
            // Virtual event handlers, overide them in your derived class
            virtual void OnSearchCancelButtonClick( wxCommandEvent& event ) HELIUM_OVERRIDE;
            virtual void OnSearchGoButtonClick( wxCommandEvent& event ) HELIUM_OVERRIDE;
            virtual void OnSearchTextEnter( wxCommandEvent& event ) HELIUM_OVERRIDE;
            virtual void OnVaultSettingsButtonClick( wxCommandEvent& event ) HELIUM_OVERRIDE;

        private:
            VaultSearch m_VaultSearch;
        };
    }
}