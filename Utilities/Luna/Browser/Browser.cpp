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
#include "Editor/SessionManager.h"
#include "File/Manager.h"
#include "FileSystem/FileSystem.h"
#include "Finder/Finder.h"
#include "Finder/ProjectSpecs.h"
#include "Perforce/Perforce.h"
#include "Scene/SceneManager.h"

using namespace Luna;

int Browser::s_InitCount = 0;
Browser* Browser::s_GlobalBrowser = NULL;

///////////////////////////////////////////////////////////////////////////////
void Browser::Initialize()
{
  if ( ++s_InitCount > 1 )
  {
    return;
  }

  s_InitializerStack.Push( Perforce::Initialize, Perforce::Cleanup );

  s_InitializerStack.Push( Reflect::RegisterClass<AssetCollection>( "AssetCollection" ) );
  s_InitializerStack.Push( Reflect::RegisterClass<DependencyCollection>( "DependencyCollection" ) );
  s_InitializerStack.Push( Reflect::RegisterClass<CollectionManager>( "CollectionManager" ) );

  s_InitializerStack.Push( Reflect::RegisterEnumeration<Luna::SearchTypes::SearchType>( &Luna::SearchTypes::SearchTypesEnumerateEnumeration, "SearchType" ) );
  s_InitializerStack.Push( Reflect::RegisterClass<SearchQuery>( "SearchQuery" ) );

  s_InitializerStack.Push( Reflect::RegisterClass<SearchHistory>( "SearchHistory" ) );

  s_InitializerStack.Push( Reflect::RegisterEnumeration<ViewOptionIDs::ViewOptionID>( &ViewOptionIDs::ViewOptionIDEnumerateEnumeration, "ViewOptionID" ) );

  s_InitializerStack.Push( Reflect::RegisterClass<BrowserPreferences>( "BrowserPreferences" ) );
  
  s_InitializerStack.Push( Luna::BrowserSearchDatabase::Initialize, Luna::BrowserSearchDatabase::Cleanup );

  s_GlobalBrowser = new Browser();
  s_GlobalBrowser->InitializePreferences();
}

///////////////////////////////////////////////////////////////////////////////
void Browser::Cleanup()
{
  if ( --s_InitCount > 0 )
    return;

  NOC_ASSERT( s_InitCount == 0 );
  s_InitCount = 0;

  delete s_GlobalBrowser;
  s_GlobalBrowser = NULL;

  s_InitializerStack.Cleanup();
}

///////////////////////////////////////////////////////////////////////////////
Browser& Luna::GlobalBrowser()
{
  if ( !Browser::GlobalBrowser() )
  {
    throw Nocturnal::Exception( "GlobalBrowser is not initialized, must call Browser::Initialize() first." );
  }

  return *Browser::GlobalBrowser();
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
