#include "EditorPch.h"

#include "ProjectViewModel.h"

#include "Editor/App.h"
#include "Editor/FileIconsTable.h"
#include "Editor/Settings/EditorSettings.h"

#include "Foundation/Flags.h"
#include "Foundation/Units.h"

#include "Engine/FileLocations.h"
#include "Engine/AsyncLoader.h"
#include "Engine/PackageLoader.h"
#include "Engine/AssetLoader.h"
#include "Engine/CacheManager.h"
#include "Engine/Config.h"
#include "Engine/Asset.h"

#include "EngineJobs/EngineJobs.h"

#include "GraphicsJobs/GraphicsJobs.h"

#include "Framework/WorldManager.h"
#include "Framework/TaskScheduler.h"
#include "Framework/SystemDefinition.h"

#include "PcSupport/AssetPreprocessor.h"
#include "PcSupport/ConfigPc.h"
#include "PcSupport/LooseAssetLoader.h"
#include "PcSupport/PlatformPreprocessor.h"

#include "PreprocessingPc/PcPreprocessor.h"

using namespace Helium;
using namespace Helium::Editor;

AssetPath g_EditorSystemDefinitionPath( "/Editor:System" );
SystemDefinitionPtr g_EditorSystemDefinition;

void InitializeEditorSystem()
{
	HELIUM_ASSERT( AssetLoader::GetInstance() );
	AssetLoader::GetInstance()->LoadObject<SystemDefinition>( g_EditorSystemDefinitionPath, g_EditorSystemDefinition );
	if ( !g_EditorSystemDefinition )
	{
		HELIUM_TRACE( TraceLevels::Error, TXT( "InitializeEditorSystem(): Could not find SystemDefinition. LoadObject on '%s' failed.\n" ), *g_EditorSystemDefinitionPath.ToString() );
	}
	else
	{
		g_EditorSystemDefinition->Initialize();
	}
}

void DestroyEditorSystem()
{
	if ( HELIUM_VERIFY( g_EditorSystemDefinition ))
	if ( g_EditorSystemDefinition )
	{
		g_EditorSystemDefinition->Cleanup();
		g_EditorSystemDefinition = 0;
	}
}

const wxArtID ProjectViewModel::DefaultFileIcon = ArtIDs::FileSystem::File;
//
//std::string ProjectViewModelNode::GetName() const
//{
//	return std::string( *m_Path.GetName() );
//
//
//    //if ( m_Path.IsDirectory() )
//    //{
//    //    return m_Path.Get();
//    //}
//    //else
//    //{
//    //    if ( wxGetApp().GetSettingsManager()->GetSettings< EditorSettings >()->GetShowFileExtensionsInProjectView() )
//    //    {
//    //        return m_Path.Filename();
//    //    }
//    //    else
//    //    {
//    //        return m_Path.Basename();
//    //    }
//    //}
//}
//
//std::string ProjectViewModelNode::GetFileSize() const
//{
//  //  if ( m_Path.IsDirectory() )
//  //  {
//  //      return std::string( TXT( "" ) );
//  //  }
//  //  else if ( m_Path.IsFile() )
//  //  {
//		//Status status;
//		//status.Read( m_Path.Get().c_str() );
//		//uint64_t size = status.m_Size;
//  //      return BytesToString( size );
//  //  }
//
//    return std::string( TXT( "" ) );
//}
//

///////////////////////////////////////////////////////////////////////////////
ProjectViewModel::ProjectViewModel( DocumentManager* documentManager )
{
	m_FileIconExtensionLookup.insert( M_FileIconExtensionLookup::value_type( TXT( "bin" ), ArtIDs::MimeTypes::Binary ) );
	m_FileIconExtensionLookup.insert( M_FileIconExtensionLookup::value_type( TXT( "dat" ),ArtIDs::MimeTypes::Binary ) );
	m_FileIconExtensionLookup.insert( M_FileIconExtensionLookup::value_type( TXT( "hrb" ), ArtIDs::MimeTypes::ReflectBinary ) );
	m_FileIconExtensionLookup.insert( M_FileIconExtensionLookup::value_type( TXT( "HeliumEntity" ), ArtIDs::MimeTypes::Entity ) );
	m_FileIconExtensionLookup.insert( M_FileIconExtensionLookup::value_type( TXT( "HeliumProject" ), ArtIDs::MimeTypes::Project ) );
	m_FileIconExtensionLookup.insert( M_FileIconExtensionLookup::value_type( TXT( "HeliumScene" ), ArtIDs::MimeTypes::Scene ) );
	m_FileIconExtensionLookup.insert( M_FileIconExtensionLookup::value_type( TXT( "txt" ), ArtIDs::MimeTypes::Text ) );
	
	ThreadSafeAssetTrackerListener::GetInstance()->e_AssetLoaded.AddMethod( this, &ProjectViewModel::OnAssetLoaded );
	ThreadSafeAssetTrackerListener::GetInstance()->e_AssetChanged.AddMethod( this, &ProjectViewModel::OnAssetChanged );
}

ProjectViewModel::~ProjectViewModel()
{
	ThreadSafeAssetTrackerListener::GetInstance()->e_AssetLoaded.RemoveMethod( this, &ProjectViewModel::OnAssetLoaded );
	ThreadSafeAssetTrackerListener::GetInstance()->e_AssetChanged.AddMethod( this, &ProjectViewModel::OnAssetChanged );

	m_FileIconExtensionLookup.clear();
}

wxDataViewColumn* ProjectViewModel::CreateColumn( uint32_t id )
{
	switch( id )
	{
	default:
		{
			return NULL;
		}
		break;

	case ProjectModelColumns::Name:
		{
			wxDataViewIconTextRenderer *render = new wxDataViewIconTextRenderer();

			wxDataViewColumn *column = new wxDataViewColumn(
				ProjectModelColumns::Label( ProjectModelColumns::Name ),
				render,
				m_ColumnLookupTable.size(),
				ProjectModelColumns::Width( ProjectModelColumns::Name ),
				wxALIGN_LEFT,
				wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE );

			m_ColumnLookupTable.push_back( id );

			return column;
		}
		break;

	//case ProjectModelColumns::Icon:
	//    {
	//        wxDataViewBitmapRenderer *render = new wxDataViewBitmapRenderer();

	//        wxDataViewColumn *column = new wxDataViewColumn(
	//            ProjectModelColumns::Label( ProjectModelColumns::Icon ),
	//            render,
	//            m_ColumnLookupTable.size(),
	//            ProjectModelColumns::Width( ProjectModelColumns::Icon ),
	//            wxALIGN_RIGHT,
	//            0 );

	//        column->IS

	//        m_ColumnLookupTable.push_back( id );

	//        return column;
	//    }
	//    break;

	case ProjectModelColumns::Details:
		{
			wxDataViewTextRenderer *render = new wxDataViewTextRenderer( "string", wxDATAVIEW_CELL_INERT );

			wxDataViewColumn *column = new wxDataViewColumn(
				ProjectModelColumns::Label( ProjectModelColumns::Details ),
				render,
				m_ColumnLookupTable.size(),
				ProjectModelColumns::Width( ProjectModelColumns::Details ),
				wxALIGN_LEFT,
				wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE | wxDATAVIEW_COL_RESIZABLE );

			m_ColumnLookupTable.push_back( id );

			return column;
		}
		break;

	case ProjectModelColumns::FileSize:
		{
			wxDataViewTextRenderer *render = new wxDataViewTextRenderer( "string", wxDATAVIEW_CELL_INERT );

			wxDataViewColumn *column = new wxDataViewColumn(
				ProjectModelColumns::Label( ProjectModelColumns::FileSize ),
				render,
				m_ColumnLookupTable.size(),
				ProjectModelColumns::Width( ProjectModelColumns::FileSize ),
				wxALIGN_LEFT,
				wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE | wxDATAVIEW_COL_RESIZABLE );

			m_ColumnLookupTable.push_back( id );

			return column;
		}
		break;

	case ProjectModelColumns::Type:
		{
			wxDataViewTextRenderer *render = new wxDataViewTextRenderer( "string", wxDATAVIEW_CELL_INERT );

			wxDataViewColumn *column = new wxDataViewColumn(
				ProjectModelColumns::Label( ProjectModelColumns::Type ),
				render,
				m_ColumnLookupTable.size(),
				ProjectModelColumns::Width( ProjectModelColumns::Type ),
				wxALIGN_LEFT,
				wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE | wxDATAVIEW_COL_RESIZABLE );

			m_ColumnLookupTable.push_back( id );

			return column;
		}
		break;
	}

	return NULL;
}

void ProjectViewModel::ResetColumns()
{
	m_ColumnLookupTable.clear();
}

bool ProjectViewModel::OpenProject( const FilePath& project )
{
	CloseProject();

	FileLocations::SetBaseDirectory( project );

	// Make sure various module-specific heaps are initialized from the main thread before use.
	InitEngineJobsDefaultHeap();
	InitGraphicsJobsDefaultHeap();

	// Register shutdown for general systems.
	m_InitializerStack.Push( FileLocations::Shutdown );
	m_InitializerStack.Push( Name::Shutdown );
	m_InitializerStack.Push( AssetPath::Shutdown );

	// Async I/O.
	AsyncLoader* pAsyncLoader = AsyncLoader::GetInstance();
	HELIUM_VERIFY( pAsyncLoader->Initialize() );
	m_InitializerStack.Push( AsyncLoader::DestroyStaticInstance );

	// Asset cache management.
	m_InitializerStack.Push( CacheManager::Startup, CacheManager::Shutdown );

	// libs
	m_InitializerStack.Push( Reflect::ObjectRefCountSupport::Shutdown );
	m_InitializerStack.Push( Asset::Shutdown );
	m_InitializerStack.Push( AssetType::Shutdown );
	m_InitializerStack.Push( Reflect::Startup, Reflect::Shutdown);

	// Asset loader and preprocessor.
	HELIUM_VERIFY( LooseAssetLoader::InitializeStaticInstance() );
	m_InitializerStack.Push( LooseAssetLoader::DestroyStaticInstance );

	AssetLoader* pAssetLoader = AssetLoader::GetInstance();
	HELIUM_ASSERT( pAssetLoader );

	AssetPreprocessor* pAssetPreprocessor = AssetPreprocessor::CreateStaticInstance();
	HELIUM_ASSERT( pAssetPreprocessor );
	PlatformPreprocessor* pPlatformPreprocessor = new PcPreprocessor;
	HELIUM_ASSERT( pPlatformPreprocessor );
	pAssetPreprocessor->SetPlatformPreprocessor( Cache::PLATFORM_PC, pPlatformPreprocessor );

	m_InitializerStack.Push( AssetPreprocessor::DestroyStaticInstance );
	m_InitializerStack.Push( ThreadSafeAssetTrackerListener::DestroyStaticInstance );
	m_InitializerStack.Push( AssetTracker::DestroyStaticInstance );

	m_InitializerStack.Push( InitializeEditorSystem, DestroyEditorSystem );

	HELIUM_ASSERT( g_EditorSystemDefinition.Get() ); // TODO: Figure out why this sometimes doesn't load
	Helium::Components::Initialize( g_EditorSystemDefinition.Get() );
	m_InitializerStack.Push( Components::Cleanup );

	// Engine configuration.
	Config* pConfig = Config::GetInstance();
	HELIUM_ASSERT( pConfig );

	pConfig->BeginLoad();
	while( !pConfig->TryFinishLoad() )
	{
		pAssetLoader->Tick();
	}

	m_InitializerStack.Push( Config::DestroyStaticInstance );

	ConfigPc::SaveUserConfig();

#if HELIUM_OS_WIN
	m_Engine.Initialize( &wxGetApp().GetFrame()->GetSceneManager(), GetHwndOf( wxGetApp().GetFrame() ) );
#else
	m_Engine.Initialize( &wxGetApp().GetFrame()->GetSceneManager(), NULL );
#endif

	ForciblyFullyLoadedPackageManager::GetInstance()->e_AssetForciblyLoadedEvent.AddMethod( this, &ProjectViewModel::OnAssetEditable );

	return true;
}

void ProjectViewModel::CloseProject()
{
	if ( m_Engine.IsInitialized() )
	{
		ForciblyFullyLoadedPackageManager::GetInstance()->e_AssetForciblyLoadedEvent.RemoveMethod( this, &ProjectViewModel::OnAssetEditable );
		m_Engine.Shutdown();
	}

	m_InitializerStack.Cleanup();
}

bool ProjectViewModel::IsDropPossible( const wxDataViewItem& item )
{
	//ProjectViewModelNode *node = static_cast< ProjectViewModelNode* >( item.GetID() );

	//if ( !node )
	//{
	//    return true;
	//}

	return false;
}

void ProjectViewModel::SetActive( const AssetPath& path, bool active )
{
	////AssetPath relativePath = path.GetRelativePath( m_Project->m_Path );
	//for ( MM_ProjectViewModelNodesByPath::iterator lower = m_MM_ProjectViewModelNodesByPath.lower_bound( path ),
	//    upper = m_MM_ProjectViewModelNodesByPath.upper_bound( path );
	//    lower != upper && lower != m_MM_ProjectViewModelNodesByPath.end();
	//++lower )
	//{
	//    ProjectViewModelNode *node = lower->second;
	//    node->m_IsActive = active;
	//}
}

unsigned int ProjectViewModel::GetColumnCount() const
{
	return ProjectModelColumns::COUNT;
}

wxString ProjectViewModel::GetColumnType( unsigned int type ) const
{
	if ( type < ProjectModelColumns::COUNT )
	{
		return ProjectModelColumns::Label( type );
	}
	else
	{
		return ProjectModelColumns::s_Labels[ProjectModelColumns::COUNT];
	}
}

void ProjectViewModel::GetValue( wxVariant& variant, const wxDataViewItem& item, unsigned int column ) const
{
	if ( !item.IsOk() || ( column >= m_ColumnLookupTable.size() ) )
	{
		return;
	}

	Asset *node = static_cast< Asset* >( item.GetID() );
	if ( !node )
	{
		return;
	}

	switch( m_ColumnLookupTable.at( column ) )
	{
	default:
		break;

	case ProjectModelColumns::Name:
		{            
			uint32_t docStatus = DocumentStatus::Default; //  node->GetDocumentStatus();

			String assetString( *node->GetName() );
			

			wxString name = *assetString;
			if ( HasFlags<uint32_t>( docStatus, DocumentStatus::Changed ) )
			{
				name = wxString( TXT( '*' ) ) + name; 
			}

			wxBitmap bitmap = wxArtProvider::GetBitmap( GetArtIDFromPath( node->GetPath() ), wxART_OTHER, wxSize(16, 16) );
			if ( docStatus > 0 )
			{
				wxImage image = bitmap.ConvertToImage();
				HELIUM_ASSERT( image.Ok() );

				int overlayWidth = image.GetWidth() / 2;
				int overlayHeight = image.GetHeight() / 2;

				wxImage overlayImage;

				if ( HasFlags<uint32_t>( docStatus, DocumentStatus::Saving ) )
				{
					overlayImage = wxArtProvider::GetBitmap( ArtIDs::Status::Busy, wxART_OTHER, wxSize( overlayWidth, overlayHeight ) ).ConvertToImage();
					HELIUM_ASSERT( overlayImage.Ok() );
				}
				else if ( HasFlags<uint32_t>( docStatus, DocumentStatus::Loading ) )
				{
					overlayImage = wxArtProvider::GetBitmap( ArtIDs::Status::Busy, wxART_OTHER, wxSize( overlayWidth, overlayHeight ) ).ConvertToImage();
					HELIUM_ASSERT( overlayImage.Ok() );
				}
				else if ( HasFlags<uint32_t>( docStatus, DocumentStatus::Changed ) )
				{
					overlayImage = wxArtProvider::GetBitmap( ArtIDs::Actions::Edit, wxART_OTHER, wxSize( overlayWidth, overlayHeight ) ).ConvertToImage();
					HELIUM_ASSERT( overlayImage.Ok() );
				}

				if ( overlayImage.Ok() )
				{
					if ( overlayImage.GetWidth() != overlayWidth || overlayImage.GetHeight() != overlayHeight )
					{
						overlayImage.Rescale( overlayWidth, overlayHeight );
					}

					int x = 0;
					int y = 0;
					IconArtFile::CalculatePlacement( image, overlayImage, OverlayQuadrants::BottomRight, x, y );
					image.Paste( overlayImage, x, y );
				}

				bitmap = wxBitmap( image );
			}

			wxIcon icon;
			icon.CopyFromBitmap( bitmap );

			variant << wxDataViewIconText( name, icon );

		}
		break;

	//case ProjectModelColumns::Icon:
	//    {
	//        int32_t imageID = GlobalFileIconsTable().GetIconIDFromPath( node->GetPath() );
	//        wxVariant bitmapVariant;
	//        bitmapVariant.
	//        variant = GlobalFileIconsTable().GetSmallImageList()->GetBitmap( imageID );
	//    }
	//    break;

	case ProjectModelColumns::Details:
		{
			variant = std::string( TXT( "" ) );
		}
		break;

	case ProjectModelColumns::FileSize:
		{
			variant = std::string( TXT( "" ) );
		}
		break;
	case ProjectModelColumns::Type:
		{
			const AssetType *pType = node->GetAssetType();
			HELIUM_ASSERT( pType );
			variant = std::string( *pType->GetName() );
		}
		break;
	}
}

bool ProjectViewModel::SetValue( const wxVariant& variant, const wxDataViewItem& item, unsigned int column )
{
	// nothing should be setting column values yet!
	HELIUM_BREAK();
	return false;

	//if ( !item.IsOk()
	//    || ( column < 0 )
	//    || ( column >= m_ColumnLookupTable.size() ) )
	//{
	//    return false;
	//}

	//ProjectViewModelNode *node = static_cast< ProjectViewModelNode* >( item.GetID() );
	//if ( !node )
	//{
	//    return false;
	//}

	//switch( m_ColumnLookupTable.at( column ) )
	//{
	//default:
	//    {
	//        return false;
	//    }
	//    break;

	//case ProjectModelColumns::Name:
	//    {
	//        //wxDataViewIconText iconText;
	//        //iconText << variant;
	//        //node->m_Name = iconText.GetText();
	//    }
	//    break;

	//case ProjectModelColumns::Details:
	//    {
	//        //node->m_Details = variant.GetString();
	//    }
	//    break;

	//case ProjectModelColumns::Size:
	//    {
	//        //node->m_Details = variant.GetString();
	//    }
	//    break;
	//}

	//return true;
}

bool ProjectViewModel::GetAttr( const wxDataViewItem& item, unsigned int column, wxDataViewItemAttr& attr ) const
{
	if ( !item.IsOk() )
	{
		return false;
	}

	Asset *node = static_cast< Asset* >( item.GetID() );
	HELIUM_ASSERT( node );

	// bold the entry if the node is active
	attr.SetBold( node->IsPackage() );

	
	if ( node->GetAllFlagsSet( Asset::FLAG_EDITOR_FORCIBLY_LOADED ) )
	{
		attr.SetColour( *wxBLACK );
	}
	else
	{
		attr.SetColour( *wxLIGHT_GREY );
	}
	

	// italicize the entry if it is modified
	attr.SetItalic( node->GetAllFlagsSet( Asset::FLAG_CHANGED_SINCE_LOADED ) );

	if ( node->GetAllFlagsSet( Asset::FLAG_CHANGED_SINCE_LOADED ) )
	{
		attr.SetColour( *wxRED );
	}

	return true;
}

wxDataViewItem ProjectViewModel::GetParent( const wxDataViewItem& item ) const
{
	Asset *node = static_cast< Asset* >( item.GetID() );

	return wxDataViewItem( node->GetOwner() );
}

unsigned int ProjectViewModel::GetChildren( const wxDataViewItem& item, wxDataViewItemArray& items ) const
{
	int count = 0;
	Asset *pAsset = NULL;

	if (!item.IsOk())
	{
		ForciblyFullyLoadedPackageManager::GetInstance()->ForceFullyLoadRootPackages();
		pAsset = Asset::GetFirstTopLevelAsset();
	}
	else
	{
		Asset* pParentAsset = static_cast< Asset* >( item.GetID() );

		if ( pParentAsset->IsPackage() )
		{
			ForciblyFullyLoadedPackageManager::GetInstance()->ForceFullyLoadPackage( pParentAsset->GetPath() );
		}

		pAsset = pParentAsset->GetFirstChild();
	}

	while (pAsset)
	{
		//if ( m_AssetsInTree.Insert( pAsset ).Second() )
		{
			items.Add( wxDataViewItem( pAsset ) );
			++count;
		}

		pAsset = pAsset->GetNextSibling();
	}

	return count;
}

bool ProjectViewModel::IsContainer( const wxDataViewItem& item ) const
{
	// root node can have children
	if ( !item.IsOk() )
	{
		return true;
	}

	AssetPath *node = static_cast< AssetPath* >( item.GetID() );
	return node ? node->IsPackage() : false;
}

const wxArtID& ProjectViewModel::GetArtIDFromPath( const AssetPath& path ) const
{
	//std::string extension = path.FullExtension();
	//if ( extension.empty() )
	//{
	//    return DefaultFileIcon;
	//}

	//M_FileIconExtensionLookup::const_iterator foundArtID = m_FileIconExtensionLookup.find( extension );
	//if ( foundArtID != m_FileIconExtensionLookup.end() )
	//{
	//    return foundArtID->second;
	//}

	//// try just the end extension
	//if ( extension.find( '.' ) != std::string::npos )
	//{
	//    extension = path.Extension();
	//    foundArtID = m_FileIconExtensionLookup.find( extension );
	//    if ( foundArtID != m_FileIconExtensionLookup.end() )
	//    {
	//        return foundArtID->second;
	//    }
	//}
	
	return DefaultFileIcon;
}

void Helium::Editor::ProjectViewModel::OnAssetLoaded( const AssetEventArgs& args )
{
	HELIUM_TRACE(
		TraceLevels::Info,
		"Asset '%s' now editable\n",
		*args.m_Asset->GetPath().ToString());

	if (args.m_Asset->GetOwner())
	{
		// If we are not in the tree
		if ( m_AssetsInTree.Find(args.m_Asset) == m_AssetsInTree.End() )
		{
			HELIUM_TRACE(
				TraceLevels::Info,
				"Adding item %x '%s'\n", 
				args.m_Asset, 
				*args.m_Asset->GetPath().ToString());

			if ( ForciblyFullyLoadedPackageManager::GetInstance()->IsPackageForcedFullyLoaded( args.m_Asset->GetOwner()->GetPath() ) )
			//if ( m_AssetsInTree.Find( args.m_Asset->GetOwner() ) != m_AssetsInTree.End() )
			{
				//m_AssetsInTree.Insert( args.m_Asset );
				ItemAdded( wxDataViewItem( args.m_Asset->GetOwner() ), wxDataViewItem( args.m_Asset ) );
			}
		}
		else
		{
			HELIUM_TRACE(
				TraceLevels::Info,
				"Changing item %x '%s'\n", 
				args.m_Asset, 
				*args.m_Asset->GetPath().ToString());

			ItemChanged( wxDataViewItem( args.m_Asset ) );
		}
	}
}

void Helium::Editor::ProjectViewModel::OnAssetEditable( const AssetEventArgs& args )
{
	if ( m_AssetsInTree.Find(args.m_Asset) != m_AssetsInTree.End() )
	{
		ItemChanged( wxDataViewItem( args.m_Asset ) );
	}
}

void Helium::Editor::ProjectViewModel::OnAssetChanged( const AssetEventArgs& args )
{
	if ( m_AssetsInTree.Find(args.m_Asset) != m_AssetsInTree.End() )
	{
		ItemChanged( wxDataViewItem( args.m_Asset ) );
	}
}