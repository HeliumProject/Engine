#include "Precompile.h"
#include "Vault.h"

#include "VaultFrame.h"
#include "VaultSearch.h"
#include "VaultPreferences.h"
#include "CollectionManager.h"
#include "SearchHistory.h"
#include "SearchQuery.h"
#include "AssetCacheEntry.h"

#include "Platform/Exception.h"
#include "Application/RCS/Providers/Perforce/Perforce.h"
#include "Scene/SceneManager.h"

using namespace Luna;

int Vault::s_InitCount = 0;
Nocturnal::InitializerStack Vault::s_InitializerStack;

Vault::Vault()
: m_VaultSearch( NULL ) 
, m_VaultFrame( NULL )
, m_HasFrame( false )
, m_VaultPreferences( new VaultPreferences() )
{
    if ( ++s_InitCount == 1 )
    {
        s_InitializerStack.Push( Perforce::Initialize, Perforce::Cleanup );

        s_InitializerStack.Push( Reflect::RegisterClass< AssetIndexData >( TXT( "AssetIndexData" ) ) );
        //s_InitializerStack.Push( Reflect::RegisterClass< AssetIndexData >( TXT( "AssetCacheEntry" ) ) );

        s_InitializerStack.Push( Reflect::RegisterClass<AssetCollection>( TXT( "AssetCollection" ) ) );
        s_InitializerStack.Push( Reflect::RegisterClass<DependencyCollection>( TXT( "DependencyCollection" ) ) );
        s_InitializerStack.Push( Reflect::RegisterClass<CollectionManager>( TXT( "CollectionManager" ) ) );

        s_InitializerStack.Push( Reflect::RegisterEnumeration<Luna::SearchTypes::SearchType>( &Luna::SearchTypes::SearchTypesEnumerateEnumeration, TXT( "SearchType" ) ) );
        s_InitializerStack.Push( Reflect::RegisterClass<SearchQuery>( TXT( "SearchQuery" ) ) );

        s_InitializerStack.Push( Reflect::RegisterClass<SearchHistory>( TXT( "SearchHistory" ) ) );

        s_InitializerStack.Push( Reflect::RegisterEnumeration<ViewOptionIDs::ViewOptionID>( &ViewOptionIDs::ViewOptionIDEnumerateEnumeration, TXT( "ViewOptionID" ) ) );

        s_InitializerStack.Push( Reflect::RegisterClass<VaultPreferences>( TXT( "VaultPreferences" ) ) );
    }

    InitializePreferences();

    // Create the one and only VaultSearch
    m_VaultSearch = new VaultSearch();

    ////////////////////////////////////////////////////////////////
    // Connect Listeners                                          //
    ////////////////////////////////////////////////////////////////
    DocumentManager::AddDocumentChangeListener( DocumentChangeSignature::Delegate( this, &Vault::OnDocumentChange ) );
}

Vault::~Vault()
{
    ////////////////////////////////////////////////////////////////
    // Disconnect Listeners                                       //
    ////////////////////////////////////////////////////////////////
    DocumentManager::RemoveDocumentChangeListener( DocumentChangeSignature::Delegate( this, &Vault::OnDocumentChange ) );

    m_HasFrame = false;
    m_VaultSearch = NULL;
    m_SearchHistory = NULL;

    if ( --s_InitCount == 0 )
    {
      s_InitializerStack.Cleanup();
    }

    NOC_ASSERT( s_InitCount >= 0 );
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
            m_VaultFrame->Search( m_VaultPreferences->GetDefaultFolderPath() );
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
    m_VaultPreferences->SavePreferences();
    m_VaultFrame = NULL;
    m_VaultSearch->RequestStop();

    m_HasFrame = false;
}

///////////////////////////////////////////////////////////////////////////////
// TODO: this needs to go somewhere else!!!
void Vault::OnDocumentChange( const DocumentManagerChangeArgs& args )
{
    // if there aren't any documents this was a remove so empty the tree and move on
    if ( !args.m_Manager->GetDocuments().Empty() )
    {
        SceneManager* manager;
        if ( manager = dynamic_cast<SceneManager*>(args.m_Manager) )
        {
            if ( manager->GetCurrentLevel() )
            {
                tstring name = manager->GetCurrentLevel()->GetShortName();

                DependencyCollection* collection = Reflect::ObjectCast<DependencyCollection>( m_CollectionManager->FindCollection( name ) );
                if ( collection && collection->GetPath().Hash() == manager->GetCurrentLevel()->GetPath().Hash() )
                {
                    collection->LoadDependencies();
                }
                else
                {
                    m_CollectionManager->GetUniqueName( name, name.c_str() );

                    DependencyCollectionPtr collection = new DependencyCollection( name, AssetCollectionFlags::Temporary );
                    collection->SetRoot( manager->GetCurrentLevel()->GetPath() );

                    if ( m_CollectionManager->AddCollection( collection ) )
                    {
                        collection->LoadDependencies();
                    }
                }        
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
VaultPreferences* Vault::GetVaultPreferences()
{
    return m_VaultPreferences;
}

///////////////////////////////////////////////////////////////////////////////
void Vault::InitializePreferences()
{
    // Load Preferences
    m_VaultPreferences->LoadPreferences();

    m_CollectionManager = m_VaultPreferences->GetCollectionManager();
    m_SearchHistory = m_VaultPreferences->GetSearchHistory();
    m_SearchHistory->SetVaultSearch( m_VaultSearch );
}
