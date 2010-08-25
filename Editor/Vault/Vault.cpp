#include "Precompile.h"
#include "Vault.h"

#include "Editor/Vault/VaultPanel.h"
#include "VaultSearch.h"
#include "VaultSettings.h"
#include "CollectionManager.h"
#include "SearchHistory.h"
#include "SearchQuery.h"

#include "Platform/Exception.h"
#include "Application/RCS/Providers/Perforce/Perforce.h"
#include "Core/Scene/SceneManager.h"
#include "Editor/App.h"

using namespace Helium;
using namespace Helium::Editor;

Vault::Vault()
: m_VaultSearch( NULL ) 
, m_VaultPanel( NULL )
, m_HasFrame( false )
{
    // Create the one and only VaultSearch
    m_VaultSearch = new VaultSearch();

    m_CollectionManager = wxGetApp().GetSettingsManager()->GetSettings< VaultSettings >()->GetCollectionManager();
    m_SearchHistory = wxGetApp().GetSettingsManager()->GetSettings< VaultSettings >()->GetSearchHistory();
    m_SearchHistory->SetVaultSearch( m_VaultSearch );
}

Vault::~Vault()
{
    m_HasFrame = false;
    m_VaultSearch = NULL;
    m_SearchHistory = NULL;
}

///////////////////////////////////////////////////////////////////////////////
void Vault::ShowVault( const tstring& queryString )
{
    if ( !m_VaultPanel )
    {
        m_VaultPanel = new VaultPanel( this, m_VaultSearch, m_SearchHistory );
        m_HasFrame = true;
    }

    m_VaultPanel->Show();
    //if ( m_VaultPanel->IsIconized() )
    //{
    //    m_VaultPanel->Restore();
    //}
    m_VaultPanel->SetFocus();

    if ( !queryString.empty() )
    {
        m_VaultPanel->Search( queryString );
    }
    else
    {
        if ( !m_SearchHistory->RunCurrentQuery() )
        {
            m_VaultPanel->Search( wxGetApp().GetSettingsManager()->GetSettings< VaultSettings >()->GetDefaultFolderPath() );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
bool Vault::HasFrame()
{
    return ( m_VaultPanel != NULL || m_HasFrame );
}

///////////////////////////////////////////////////////////////////////////////
void Vault::OnCloseVault()
{
    wxGetApp().SaveSettings();
    m_VaultPanel = NULL;
    m_VaultSearch->RequestStop();

    m_HasFrame = false;
}
