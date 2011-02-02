#pragma once

#include "VaultMenuIDs.h"
#include "Editor/EditorGenerated.h"
#include "Editor/Vault/VaultSearch.h"
#include "Editor/Vault/ListResultsView.h"
#include "Editor/Vault/ThumbnailView.h"

namespace Helium
{
    namespace Editor
    {
        class VaultSettings;

        class VaultPanel : public VaultPanelGenerated
        {
        public:
            VaultPanel( wxWindow* parent = NULL, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
            virtual ~VaultPanel();

            void Search( const tstring& queryString );
        
            void SetViewMode( VaultViewMode view );
            VaultViewMode GetViewMode() const;

            void SetResults( VaultSearchResults* results );
            void ClearResults();

            void SelectPath( const Helium::Path& path );
            uint32_t GetSelectedPaths( std::set< Helium::Path >& paths );

            void SaveSettings();
            void OnSearchResultsAvailable( const Editor::SearchResultsAvailableArgs& args );

        protected:
            void StartSearchFromField();

            // Virtual event handlers, overide them in your derived class
            virtual void OnSearchCancelButtonClick( wxCommandEvent& event ) HELIUM_OVERRIDE;
            virtual void OnSearchGoButtonClick( wxCommandEvent& event ) HELIUM_OVERRIDE;
            virtual void OnSearchTextEnter( wxCommandEvent& event ) HELIUM_OVERRIDE;

            void OnOptionsMenuOpen( wxMenuEvent& event );
            void OnOptionsMenuClose( wxMenuEvent& event );
            void OnOptionsMenuSelect( wxCommandEvent& event );

            void OnClose( wxCloseEvent& event );

        private:
            VaultSettings* m_VaultSettings;

            VaultSearch m_VaultSearch;

            VaultViewMode m_CurrentViewMode;
            uint32_t m_CurrentThumbnailSize;

            ListResultsView* m_ListResultsView;
            ThumbnailView* m_ThumbnailView;
            wxWindow* m_CurrentView;

            wxMenu* m_OptionsMenu;
        };
    }
}