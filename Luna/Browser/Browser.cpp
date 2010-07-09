#include "Precompile.h"
#include "Browser.h"

#include "BrowserFrame.h"
#include "BrowserSearch.h"
#include "BrowserPreferences.h"
#include "CollectionManager.h"
#include "SearchHistory.h"
#include "SearchQuery.h"

#include "Platform/Exception.h"
#include "Application/RCS/Providers/Perforce/Perforce.h"
#include "Scene/SceneManager.h"

using namespace Luna;

int Browser::s_InitCount = 0;
Nocturnal::InitializerStack Browser::s_InitializerStack;

///////////////////////////////////////////////////////////////////////////////
void Browser::Initialize()
{
    if ( ++s_InitCount > 1 )
    {
        return;
    }

    s_InitializerStack.Push( Perforce::Initialize, Perforce::Cleanup );

    s_InitializerStack.Push( Reflect::RegisterClass<AssetCollection>( TXT( "AssetCollection" ) ) );
    s_InitializerStack.Push( Reflect::RegisterClass<DependencyCollection>( TXT( "DependencyCollection" ) ) );
    s_InitializerStack.Push( Reflect::RegisterClass<CollectionManager>( TXT( "CollectionManager" ) ) );

    s_InitializerStack.Push( Reflect::RegisterEnumeration<Luna::SearchTypes::SearchType>( &Luna::SearchTypes::SearchTypesEnumerateEnumeration, TXT( "SearchType" ) ) );
    s_InitializerStack.Push( Reflect::RegisterClass<SearchQuery>( TXT( "SearchQuery" ) ) );

    s_InitializerStack.Push( Reflect::RegisterClass<SearchHistory>( TXT( "SearchHistory" ) ) );

    s_InitializerStack.Push( Reflect::RegisterEnumeration<ViewOptionIDs::ViewOptionID>( &ViewOptionIDs::ViewOptionIDEnumerateEnumeration, TXT( "ViewOptionID" ) ) );

    s_InitializerStack.Push( Reflect::RegisterClass<BrowserPreferences>( TXT( "BrowserPreferences" ) ) );

}

///////////////////////////////////////////////////////////////////////////////
void Browser::Cleanup()
{
    if ( --s_InitCount > 0 )
        return;

    NOC_ASSERT( s_InitCount == 0 );
    s_InitCount = 0;

    s_InitializerStack.Cleanup();
}

///////////////////////////////////////////////////////////////////////////////
Browser::Browser()
: m_BrowserSearch( NULL ) 
, m_BrowserFrame( NULL )
, m_HasFrame( false )
, m_BrowserPreferences( new BrowserPreferences() )
{
    InitializePreferences();

    // Create the one and only BrowserSearch
    m_BrowserSearch = new BrowserSearch();

    ////////////////////////////////////////////////////////////////
    // Connect Listeners                                          //
    ////////////////////////////////////////////////////////////////
    DocumentManager::AddDocumentChangeListener( DocumentChangeSignature::Delegate( this, &Browser::OnDocumentChange ) );
}

Browser::~Browser()
{
    ////////////////////////////////////////////////////////////////
    // Disconnect Listeners                                       //
    ////////////////////////////////////////////////////////////////
    DocumentManager::RemoveDocumentChangeListener( DocumentChangeSignature::Delegate( this, &Browser::OnDocumentChange ) );

    m_HasFrame = false;
    m_BrowserSearch = NULL;
    m_SearchHistory = NULL;
}

///////////////////////////////////////////////////////////////////////////////
void Browser::ShowBrowser( const tstring& queryString )
{
    if ( !m_BrowserFrame )
    {
        m_BrowserFrame = new BrowserFrame( this, m_BrowserSearch, m_SearchHistory );
        m_HasFrame = true;
    }

    m_BrowserFrame->Show();
    if ( m_BrowserFrame->IsIconized() )
    {
        m_BrowserFrame->Restore();
    }
    m_BrowserFrame->SetFocus();

    if ( !queryString.empty() )
    {
        m_BrowserFrame->Search( queryString );
    }
    else
    {
        if ( !m_SearchHistory->RunCurrentQuery() )
        {
            m_BrowserFrame->Search( m_BrowserPreferences->GetDefaultFolderPath() );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
bool Browser::HasFrame()
{
    return ( m_BrowserFrame != NULL || m_HasFrame );
}

///////////////////////////////////////////////////////////////////////////////
void Browser::OnCloseBrowser()
{
    m_BrowserPreferences->SavePreferences();
    m_BrowserFrame = NULL;
    m_BrowserSearch->RequestStop();

    m_HasFrame = false;
}

///////////////////////////////////////////////////////////////////////////////
// TODO: this needs to go somewhere else!!!
void Browser::OnDocumentChange( const DocumentManagerChangeArgs& args )
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
BrowserPreferences* Browser::GetBrowserPreferences()
{
    return m_BrowserPreferences;
}

///////////////////////////////////////////////////////////////////////////////
void Browser::InitializePreferences()
{
    // Load Preferences
    m_BrowserPreferences->LoadPreferences();

    m_CollectionManager = m_BrowserPreferences->GetCollectionManager();
    m_SearchHistory = m_BrowserPreferences->GetSearchHistory();
    m_SearchHistory->SetBrowserSearch( m_BrowserSearch );
}
