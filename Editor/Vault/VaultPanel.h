#pragma once

#include "VaultMenuIDs.h"
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
            virtual ~VaultPanel();

            void SetDirectory( const Helium::Path& directory );
            const Helium::Path& GetDirectory() const;

            void Search( const tstring& queryString );

        public:
            void SetViewMode( VaultViewMode view );
            VaultViewMode GetViewMode() const;

            void SetResults( VaultSearchResults* results );
            void ClearResults();

            void SelectPath( const Helium::Path& path );
            u32 GetSelectedPaths( std::set< Helium::Path >& paths );

            void OnSearchResultsAvailable( const Editor::SearchResultsAvailableArgs& args );

        protected:
            // Virtual event handlers, overide them in your derived class
            virtual void OnSearchCancelButtonClick( wxCommandEvent& event ) HELIUM_OVERRIDE;
            virtual void OnSearchGoButtonClick( wxCommandEvent& event ) HELIUM_OVERRIDE;
            virtual void OnSearchTextEnter( wxCommandEvent& event ) HELIUM_OVERRIDE;
            virtual void OnVaultSettingsButtonClick( wxCommandEvent& event ) HELIUM_OVERRIDE;

        private:
            VaultSearch m_VaultSearch;

            VaultViewMode m_CurrentMode;
            wxWindow* m_CurrentView;
        };
    }
}