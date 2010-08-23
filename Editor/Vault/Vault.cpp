#include "Precompile.h"
#include "Vault.h"

#include "VaultFrame.h"
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
, m_VaultFrame( NULL )
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
    if ( !m_VaultFrame )
    {
        m_VaultFrame = new VaultFrame( this, m_VaultSearch, m_SearchHistory );
        m_HasFrame = true;
    }

    m_VaultFrame->Show();
    if ( m_VaultFrame->IsIconized() )
    {
        m_VaultFrame->Restore();
    }
    m_VaultFrame->SetFocus();

    if ( !queryString.empty() )
    {
        m_VaultFrame->Search( queryString );
    }
    else
    {
        if ( !m_SearchHistory->RunCurrentQuery() )
        {
            m_VaultFrame->Search( wxGetApp().GetSettingsManager()->GetSettings< VaultSettings >()->GetDefaultFolderPath() );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
bool Vault::HasFrame()
{
    return ( m_VaultFrame != NULL || m_HasFrame );
}

///////////////////////////////////////////////////////////////////////////////
void Vault::OnCloseVault()
{
    wxGetApp().SaveSettings();
    m_VaultFrame = NULL;
    m_VaultSearch->RequestStop();

    m_HasFrame = false;
}
