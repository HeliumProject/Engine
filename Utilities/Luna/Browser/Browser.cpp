#include "Precompile.h"
#include "Browser.h"

#include "BrowserFrame.h"
#include "BrowserSearch.h"
#include "BrowserSearchDatabase.h"
#include "BrowserPreferences.h"
#include "CollectionManager.h"
#include "SearchHistory.h"
#include "SearchQuery.h"

#include "Asset/AssetFolder.h"
#include "Asset/AssetFile.h"
#include "Asset/CacheDB.h"
#include "Common/Environment.h"
#include "Common/Exception.h"
#include "Common/InitializerStack.h"
#include "Editor/SessionManager.h"
#include "File/Manager.h"
#include "FileSystem/FileSystem.h"
#include "Finder/Finder.h"
#include "Finder/ProjectSpecs.h"
#include "Perforce/Perforce.h"
#include "Scene/SceneManager.h"

using namespace Luna;


///////////////////////////////////////////////////////////////////////////////
Nocturnal::InitializerStack g_InitializerStack;
int g_InitRef = 0;
static Browser* g_GlobalBrowser = NULL;
void Browser::Initialize()
{
  if ( ++g_InitRef > 1 )
    return;

  g_InitializerStack.Push( Perforce::Initialize, Perforce::Cleanup );

  g_InitializerStack.Push( Reflect::RegisterClass<AssetCollection>( "AssetCollection" ) );
  g_InitializerStack.Push( Reflect::RegisterClass<DependencyCollection>( "DependencyCollection" ) );
  g_InitializerStack.Push( Reflect::RegisterClass<CollectionManager>( "CollectionManager" ) );

  g_InitializerStack.Push( Reflect::RegisterEnumeration<Luna::SearchTypes::SearchType>( &Luna::SearchTypes::SearchTypesEnumerateEnumeration, "SearchType" ) );
  g_InitializerStack.Push( Reflect::RegisterClass<SearchQuery>( "SearchQuery" ) );

  g_InitializerStack.Push( Reflect::RegisterClass<SearchHistory>( "SearchHistory" ) );

  g_InitializerStack.Push( Reflect::RegisterEnumeration<ViewOptionIDs::ViewOptionID>( &ViewOptionIDs::ViewOptionIDEnumerateEnumeration, "ViewOptionID" ) );

  g_InitializerStack.Push( Reflect::RegisterClass<BrowserPreferences>( "BrowserPreferences" ) );
  
  g_InitializerStack.Push( Luna::BrowserSearchDatabase::Initialize, Luna::BrowserSearchDatabase::Cleanup );

  g_GlobalBrowser = new Browser();
  g_GlobalBrowser->InitializePreferences();
}

///////////////////////////////////////////////////////////////////////////////
void Browser::Cleanup()
{
  if ( --g_InitRef > 0 )
    return;

  NOC_ASSERT( g_InitRef == 0 );
  g_InitRef = 0;

  delete g_GlobalBrowser;
  g_GlobalBrowser = NULL;

  g_InitializerStack.Cleanup();
}

///////////////////////////////////////////////////////////////////////////////
Browser& Luna::GlobalBrowser()
{
  if ( !g_GlobalBrowser )
  {
    throw Nocturnal::Exception( "GlobalBrowser is not initialized, must call Browser::Initialize() first." );
  }

  return *g_GlobalBrowser;
}

///////////////////////////////////////////////////////////////////////////////
Browser::Browser()
: m_CacheDB( new Asset::CacheDB( "LunaBrowser-AssetCacheDB" ) )
, m_BrowserSearch( NULL ) 
, m_BrowserFrame( NULL )
, m_HasFrame( false )
, m_BrowserPreferences( new BrowserPreferences() )
{
  // Connect the DB
  std::string rootDir = Finder::ProjectAssets() + FinderSpecs::Project::ASSET_TRACKER_FOLDER.GetRelativeFolder();
  FileSystem::GuaranteeSlash( rootDir );
  FileSystem::MakePath( rootDir );
  m_CacheDB->Open( FinderSpecs::Project::ASSET_TRACKER_DB.GetFile( rootDir ),
    FinderSpecs::Project::ASSET_TRACKER_CONFIGS.GetFolder(),
    m_CacheDB->s_TrackerDBVersion,
    SQLITE_OPEN_READONLY );

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
  m_CacheDB = NULL;
}

///////////////////////////////////////////////////////////////////////////////
void Browser::ShowBrowser( const std::string& queryString )
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
        std::string name = manager->GetCurrentLevel()->GetShortName();
        const tuid rootID = manager->GetCurrentLevel()->GetFileID();

        DependencyCollection* collection = Reflect::ObjectCast<DependencyCollection>( m_CollectionManager->FindCollection( name ) );
        if ( collection && collection->GetRootID() == rootID )
        {
          collection->LoadDependencies();
        }
        else
        {
          m_CollectionManager->GetUniqueName( name, name.c_str() );

          DependencyCollectionPtr collection = new DependencyCollection( name, AssetCollectionFlags::Temporary );
          collection->SetRootID( rootID );
            
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
