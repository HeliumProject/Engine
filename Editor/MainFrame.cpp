#include "EditorPch.h"
#include "MainFrame.h"

#include "Platform/System.h"

#include "Persist/ArchiveJson.h"

#include "Framework/WorldManager.h"
#include "Framework/SceneDefinition.h"

#include "SceneGraph/Scene.h"
#include "SceneGraph/TransformManipulator.h"
#include "SceneGraph/CurveCreateTool.h"
#include "SceneGraph/CurveEditTool.h"
#include "SceneGraph/DuplicateTool.h"
#include "SceneGraph/LocatorCreateTool.h"
#include "SceneGraph/ScaleManipulator.h"
#include "SceneGraph/RotateManipulator.h"
#include "SceneGraph/TranslateManipulator.h"

#include "Editor/App.h"
#include "Editor/EditorIDs.h"
#include "Editor/Dialogs/FileDialog.h"
#include "Editor/ArtProvider.h"
#include "Editor/Settings/EditorSettings.h"
#include "Editor/Dialogs/SettingsDialog.h"
#include "Editor/Settings/WindowSettings.h"
#include "Editor/Clipboard/ClipboardFileList.h"
#include "Editor/Clipboard/ClipboardDataObject.h"
#include "Editor/Dialogs/ImportOptionsDialog.h"
#include "Editor/Dialogs/ExportOptionsDialog.h"
#include "Editor/Input.h"

using namespace Helium;
using namespace Helium::SceneGraph;
using namespace Helium::Editor;

///////////////////////////////////////////////////////////////////////////////
// Wraps up a pointer to an SceneGraph::Scene so that it can be stored in the combo box that
// is used for selecting the current scene.  Each item in the combo box stores 
// the scene that it refers to.
// 
class SceneSelectData : public wxClientData
{
public:
	SceneGraph::Scene* m_Scene;

	SceneSelectData( SceneGraph::Scene* scene )
		: m_Scene( scene )
	{
	}
};

///////////////////////////////////////////////////////////////////////////////
// Helper template for stashing data inside wx calls
// 
template< class T >
class DataObject : public wxObject
{
public:
	DataObject()
		: m_Data ()
	{

	}

	DataObject(const T& t)
		: m_Data ( t )
	{

	}

	T m_Data;
};

///////////////////////////////////////////////////////////////////////////////
// Stashes data for selection context menu
// 
namespace ContextCallbackTypes
{
	enum ContextCallbackType
	{
		All,
		Item,
		Instance,
		Count
	};
}

class ContextCallbackData: public wxObject
{
public:
	ContextCallbackData()
		: m_ContextCallbackType( ContextCallbackTypes::All )
		, m_Nodes( NULL )
	{

	}

	ContextCallbackTypes::ContextCallbackType m_ContextCallbackType;
	SceneGraph::SceneNode* m_Nodes;
};

MainFrame::MainFrame( SettingsManager* settingsManager, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style )
: MainFrameGenerated( parent, id, title, pos, size, style )
, m_SettingsManager( settingsManager )
, m_MenuMRU( new MenuMRU( 30, this ) )
, m_TreeMonitor( &m_SceneManager )
, m_Project( NULL )
, m_MessageDisplayer( this )
, m_DocumentManager( MessageSignature::Delegate( &m_MessageDisplayer, &MessageDisplayer::DisplayMessage ), FileDialogSignature::Delegate( &m_FileDialogDisplayer, &FileDialogDisplayer::DisplayFileDialog ) )
, m_SceneManager()
, m_VaultPanel( NULL )
{
	wxIcon appIcon;
	appIcon.CopyFromBitmap( wxArtProvider::GetBitmap( ArtIDs::Editor::Helium, wxART_OTHER, wxSize( 32, 32 ) ) );
	SetIcon( appIcon );

	SetLabel( TXT("Helium Editor") );
}

bool MainFrame::Initialize()
{
	//
	// Frame Key events
	//
	Connect( wxEVT_CHAR, wxKeyEventHandler( MainFrame::OnChar ) );

	//
	// Dynamic Menu Generation
	//
	Connect( wxEVT_MENU_OPEN, wxMenuEventHandler( MainFrame::OnMenuOpen ) );

	//
	// General Events
	//
	Connect( wxID_CLOSE, wxEVT_CLOSE_WINDOW, wxCloseEventHandler( MainFrame::OnExiting ) );
	Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( MainFrame::OnExiting ) );
	Connect( wxID_SELECTALL, wxCommandEventHandler( MainFrame::OnSelectAll ) );
	Connect( ID_Close, wxCommandEventHandler( MainFrame::OnClose ), NULL, this );
	
	//EVT_MENU(wxID_HELP_INDEX, MainFrame::OnHelpIndex)
	//EVT_MENU(wxID_HELP_SEARCH, MainFrame::OnHelpSearch)
	
	//
	// Toolbox
	//
	Connect( EventIds::ID_ToolsScalePivot, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnToolSelected ) );
	Connect( EventIds::ID_ToolsRotatePivot, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnToolSelected ) );
	Connect( EventIds::ID_ToolsTranslatePivot, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnToolSelected ) );
	Connect( EventIds::ID_ToolsPivot, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnToolSelected ) );

	// Help menu
	wxString caption = wxT( "About " );
	caption += wxGetApp().AppVerName().c_str();
	caption += wxT( "..." );
	wxMenuItem* aboutMenuItem = m_MenuHelp->FindItem( ID_About );
	aboutMenuItem->SetItemLabel( caption );

	// Tool Bar
	m_ToolbarPanel = new ToolbarPanel( this );
	m_FrameManager.AddPane( m_ToolbarPanel, wxAuiPaneInfo().Name( wxT( "tools" ) ).Top().Layer( 5 ).Position( 1 ).CaptionVisible( false ).PaneBorder( false ).Gripper( false ).CloseButton( false ).MaximizeButton( false ).MinimizeButton( false ).PinButton( false ).Movable( false ).MinSize( m_ToolbarPanel->GetMinSize() ).MaxSize( m_ToolbarPanel->GetMaxSize() ) );
	m_ToolbarPanel->EnableTools( false );

	m_ToolbarPanel->m_SelectButton->Connect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnToolSelected ), NULL, this );
	m_ToolbarPanel->m_TranslateButton->Connect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnToolSelected ), NULL, this );
	m_ToolbarPanel->m_RotateButton->Connect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnToolSelected ), NULL, this );
	m_ToolbarPanel->m_ScaleButton->Connect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnToolSelected ), NULL, this );
	m_ToolbarPanel->m_DuplicateToolButton->Connect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnToolSelected ), NULL, this );
	m_ToolbarPanel->m_LocatorToolButton->Connect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnToolSelected ), NULL, this );
	m_ToolbarPanel->m_VolumeToolButton->Connect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnToolSelected ), NULL, this );
	m_ToolbarPanel->m_CurveToolButton->Connect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnToolSelected ), NULL, this );   
	m_ToolbarPanel->m_CurveEditToolButton->Connect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnToolSelected ), NULL, this );

	m_ToolbarPanel->m_VaultSearchBox->Connect( wxEVT_COMMAND_SEARCHCTRL_SEARCH_BTN, wxCommandEventHandler( MainFrame::OnSearchGoButtonClick ), NULL, this );
	m_ToolbarPanel->m_VaultSearchBox->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( MainFrame::OnSearchTextEnter ), NULL, this );

	// View panel area
	m_ViewPanel = new ViewPanel( m_SettingsManager, this );
	m_ViewPanel->GetViewCanvas()->GetViewport().AddRenderListener( RenderSignature::Delegate ( this, &MainFrame::Render ) );
	m_ViewPanel->GetViewCanvas()->GetViewport().AddSelectListener( SelectSignature::Delegate ( this, &MainFrame::Select ) ); 
	m_ViewPanel->GetViewCanvas()->GetViewport().AddSetHighlightListener( SetHighlightSignature::Delegate ( this, &MainFrame::SetHighlight ) );
	m_ViewPanel->GetViewCanvas()->GetViewport().AddClearHighlightListener( ClearHighlightSignature::Delegate ( this, &MainFrame::ClearHighlight ) );
	m_ViewPanel->GetViewCanvas()->GetViewport().AddToolChangedListener( ToolChangeSignature::Delegate ( this, &MainFrame::ViewToolChanged ) );
	m_FrameManager.AddPane( m_ViewPanel, wxAuiPaneInfo().Name( wxT( "view" ) ).CenterPane() );

	// Project
	m_ProjectPanel = new ProjectPanel( this, &m_DocumentManager );
	wxAuiPaneInfo projectPaneInfo = wxAuiPaneInfo().Name( wxT( "project" ) ).Caption( wxT( "Project" ) ).Left().Layer( 2 ).Position( 1 ).BestSize( 200, 700 );
	projectPaneInfo.dock_proportion = 30000;
	m_FrameManager.AddPane( m_ProjectPanel, projectPaneInfo );

	// Help
	m_HelpPanel = new HelpPanel( this );
	wxAuiPaneInfo helpPaneInfo = wxAuiPaneInfo().Name( wxT( "help" ) ).Caption( wxT( "Help" ) ).Left().Layer( 2 ).Position( 2 ).MinSize( 200, 200 ).BestSize( wxSize( 200, 200 ) );
	helpPaneInfo.dock_proportion = 10000;
	m_FrameManager.AddPane( m_HelpPanel, helpPaneInfo );

	// Directory
	m_DirectoryPanel = new DirectoryPanel( &m_SceneManager, &m_TreeMonitor, this );
	m_FrameManager.AddPane( m_DirectoryPanel, wxAuiPaneInfo().Name( wxT( "directory" ) ).Caption( wxT( "Directory" ) ).Left().Layer( 1 ).Position( 1 ).BestSize( wxSize( 200, 900 ) ) );

	// Properties/Layers/Vault area
	m_PropertiesPanel = new PropertiesPanel( this );
	m_FrameManager.AddPane( m_PropertiesPanel, wxAuiPaneInfo().Name( wxT( "properties" ) ).Caption( wxT( "Properties" ) ).Right().Layer( 1 ).Position( 1 ) );

	m_LayersPanel = new LayersPanel( &m_SceneManager, this );
	wxAuiPaneInfo layersPaneInfo = wxAuiPaneInfo().Name( wxT( "layers" ) ).Caption( wxT( "Layers" ) ).Right().Layer( 1 ).Position( 2 ).MinSize( 200, 200 ).BestSize( wxSize( 400, 200 ) );
	layersPaneInfo.dock_proportion = 10000;
	m_FrameManager.AddPane( m_LayersPanel, layersPaneInfo );

	// Vault (hidden by default)
	m_VaultPanel = new VaultPanel( this );
	wxAuiPaneInfo vaultPanelInfo = wxAuiPaneInfo().Name( wxT( "vault" ) ).Caption( wxT( "Asset Vault" ) ).Right().Layer( 1 ).Position( 4 ).Hide();
	m_FrameManager.AddPane( m_VaultPanel, vaultPanelInfo );

	m_FrameManager.Update();

	CreatePanelsMenu( m_MenuPanels );

	// Restore layout if any
	wxGetApp().GetSettingsManager()->GetSettings< WindowSettings >()->ApplyToWindow( this, &m_FrameManager, true );
	m_ViewPanel->GetViewCanvas()->GetViewport().LoadSettings( wxGetApp().GetSettingsManager()->GetSettings< ViewportSettings >() ); 

	// Disable accelerators, we'll handle them ourselves
	m_MainMenuBar->SetAcceleratorTable( wxAcceleratorTable() );

	// Attach event handlers
	m_SceneManager.e_CurrentSceneChanging.AddMethod( this, &MainFrame::CurrentSceneChanging );
	m_SceneManager.e_CurrentSceneChanged.AddMethod( this, &MainFrame::CurrentSceneChanged );
	m_SceneManager.e_SceneAdded.AddMethod( this, &MainFrame::SceneAdded );
	m_SceneManager.e_SceneRemoving.AddMethod( this, &MainFrame::SceneRemoving );

	m_MenuMRU->AddItemSelectedListener( MRUSignature::Delegate( this, &MainFrame::OnMRUOpen ) );

	const std::vector< std::string >& mruPaths = wxGetApp().GetSettingsManager()->GetSettings<EditorSettings>()->GetMRUProjects();
	m_MenuMRU->FromVector( mruPaths );

	DropTarget* dropTarget = new DropTarget();
	dropTarget->SetDragOverCallback( DragOverCallback::Delegate( this, &MainFrame::DragOver ) );
	dropTarget->SetDropCallback( DropCallback::Delegate( this, &MainFrame::Drop ) );
	m_ViewPanel->GetViewCanvas()->SetDropTarget( dropTarget );

#ifdef EDITOR_DEBUG_RENDER
	class RenderThread : public wxThread
	{
	private:
		Editor::Viewport* m_View;

	public:
		RenderThread(Editor::Viewport* view)
			: m_View (view)
		{

		}

		wxThread::ExitCode Entry()
		{
			while (true)
			{
				m_View->Refresh();
			}

			return NULL;
		}
	};

	RenderThread* thread = new RenderThread (m_View);
	thread->Create();
	thread->Run();
#endif

	return true;
}

MainFrame::~MainFrame()
{
	m_PropertiesPanel->GetPropertiesManager().SyncThreads();

	// Remove any straggling document listeners
	OS_DocumentSmartPtr::Iterator docItr = m_DocumentManager.GetDocuments().Begin();
	OS_DocumentSmartPtr::Iterator docEnd = m_DocumentManager.GetDocuments().End();
	for ( ; docItr != docEnd; ++docItr )
	{
		DisconnectDocument( *docItr );
	}

	// Save preferences and MRU
	wxGetApp().GetSettingsManager()->GetSettings<EditorSettings>()->SetMRUProjects( m_MenuMRU );

	wxGetApp().GetSettingsManager()->GetSettings< WindowSettings >()->SetFromWindow( this, &m_FrameManager );
	m_ViewPanel->GetViewCanvas()->GetViewport().SaveSettings( wxGetApp().GetSettingsManager()->GetSettings< ViewportSettings >() ); 


	CloseProject();

	//
	// Detach event handlers
	//

	m_SceneManager.e_CurrentSceneChanging.RemoveMethod( this, &MainFrame::CurrentSceneChanging );
	m_SceneManager.e_CurrentSceneChanged.RemoveMethod( this, &MainFrame::CurrentSceneChanged );
	m_SceneManager.e_SceneAdded.RemoveMethod( this, &MainFrame::SceneAdded );
	m_SceneManager.e_SceneRemoving.RemoveMethod( this, &MainFrame::SceneRemoving );

	m_MenuMRU->RemoveItemSelectedListener( MRUSignature::Delegate( this, &MainFrame::OnMRUOpen ) );

	m_ViewPanel->GetViewCanvas()->GetViewport().RemoveRenderListener( RenderSignature::Delegate ( this, &MainFrame::Render ) );
	m_ViewPanel->GetViewCanvas()->GetViewport().RemoveSelectListener( SelectSignature::Delegate ( this, &MainFrame::Select ) ); 
	m_ViewPanel->GetViewCanvas()->GetViewport().RemoveSetHighlightListener( SetHighlightSignature::Delegate ( this, &MainFrame::SetHighlight ) );
	m_ViewPanel->GetViewCanvas()->GetViewport().RemoveClearHighlightListener( ClearHighlightSignature::Delegate ( this, &MainFrame::ClearHighlight ) );
	m_ViewPanel->GetViewCanvas()->GetViewport().RemoveToolChangedListener( ToolChangeSignature::Delegate ( this, &MainFrame::ViewToolChanged ) );

#pragma TODO( "We shouldn't really have to do these if we clean up how some of our objects reference each other" )
	m_DirectoryPanel->Destroy();
	m_LayersPanel->Destroy();

	if ( m_VaultPanel )
	{
		m_VaultPanel->SaveSettings();
		m_VaultPanel->Destroy();
		m_VaultPanel=NULL;
	}
}

void MainFrame::SetHelpText( const char* text )
{
	m_HelpPanel->SetText( text );
}

///////////////////////////////////////////////////////////////////////////////
// Helper function for common opening code.
// 
void MainFrame::OpenProject( const Helium::FilePath& path )
{
	HELIUM_ASSERT( !path.empty() );

	CloseProject();

	bool isNewProject = false;
	if ( path.Exists() )
	{
		bool opened = false;

		// this is our default error
		std::string error = TXT( "We could not parse the project file you selected, it has not been loaded." );
		try
		{
			m_Project = Reflect::SafeCast< Project >( Persist::FromArchive( path ) );
		}
		catch ( const Helium::Exception& ex )
		{
			error = ex.What();
		}

		opened = m_Project.ReferencesObject();

		if ( opened )
		{
			m_Project->a_Path.Set( path );

			m_MenuMRU->Insert( path );
			wxGetApp().GetSettingsManager()->GetSettings<EditorSettings>()->SetMRUProjects( m_MenuMRU );
		}
		else
		{
			wxMessageBox( error.c_str(), wxT( "Error" ), wxCENTER | wxICON_ERROR | wxOK, this );

			return;
		}
	}
	else
	{
		m_Project = new Project();
		m_Project->a_Path.Set( path );
		isNewProject = true;
	}

	Document* document = m_DocumentManager.FindDocument( m_Project->a_Path.Get() );
	if ( !document )
	{
		std::string error;
		bool result = m_DocumentManager.OpenDocument( new Document( m_Project->a_Path.Get() ), error );
		HELIUM_ASSERT( result );

		document = m_DocumentManager.FindDocument( m_Project->a_Path.Get() );
	}
	ConnectDocument( document );

	document->HasChanged( isNewProject );
	m_Project->ConnectDocument( document );

	m_DocumentManager.e_DocumentOpened.AddMethod( m_Project.Ptr(), &Project::OnDocumentOpened );
	m_DocumentManager.e_DocumenClosed.AddMethod( m_Project.Ptr(), &Project::OnDocumenClosed );

	m_MenuMRU->Insert( path );
	wxGetApp().GetSettingsManager()->GetSettings<EditorSettings>()->SetMRUProjects( m_MenuMRU );

	m_ProjectPanel->OpenProject( m_Project, document );

	wxGetApp().GetTracker()->SetProject( m_Project );
	if ( !wxGetApp().GetTracker()->IsThreadRunning() )
	{
		wxGetApp().GetTracker()->StartThread();
	}
}

void MainFrame::CloseProject()
{
	if ( m_Project )
	{
		m_PropertiesPanel->GetPropertiesManager().SyncThreads();

		wxGetApp().GetTracker()->StopThread();
		wxGetApp().GetTracker()->SetProject( NULL );

		// this will release all our listeners which may get signalled with state changes during teardown
		m_SceneManager.SetCurrentScene( NULL );

		m_DocumentManager.e_DocumentOpened.RemoveMethod( m_Project.Ptr(), &Project::OnDocumentOpened );
		m_DocumentManager.e_DocumenClosed.RemoveMethod( m_Project.Ptr(), &Project::OnDocumenClosed );

		m_ProjectPanel->CloseProject();

		m_DocumentManager.CloseAll();
		m_Project = NULL;

		m_UndoQueue.Reset();   
	}
}

///////////////////////////////////////////////////////////////////////////////
// Returns a different name each time this function is called so that scenes
// can be uniquely named.
// 
static void GetUniquePathName( const char* root, const char* extension, const std::set< FilePath >& paths, Helium::FilePath& name )
{
	int32_t number = 0;

	do
	{
		std::ostringstream strm;
		strm << root;
		// number will have a value of 1 on first run     
		if ( ++number > 1 )
		{
			strm << TXT( "(" ) << number << TXT( ")" );
		}
		strm << extension;
		name.Set( strm.str() );
	}
	while ( paths.find( name ) != paths.end() );
}

FilePath MainFrame::NewSceneDialog()
{
	FilePath path;
	GetUniquePathName( TXT( "New Scene" ), TXT( ".HeliumScene" ), m_Project->Paths(), path );

	FileDialog newSceneDialog( this, TXT( "Select New Scene Location" ), wxEmptyString, path.c_str(), TXT( "Scene File (*.HeliumScene)|*.HeliumScene|All Files (*)|*" ), FileDialogStyles::DefaultSave );

	if ( newSceneDialog.ShowModal() != wxID_OK )
	{
		path.Set( TXT( "" ) );
	}
	else
	{
		path.Set( std::string( newSceneDialog.GetPath().c_str() ) );

		// the newSceneDialog prompts if they're choosing an existing path, so we should just need to clean up here if it exists
		if ( path.Exists() )
		{
			if ( !path.Delete() )
			{
				wxMessageBox( wxT( "Could not remove the existing scene: FIXME -- add an error" ), wxT( "Error Removing Exising Scene" ), wxOK );
				path.Set( TXT( "" ) );
			}
		}
	}

	return path;
}

void MainFrame::NewProjectDialog()
{
	FileDialog newProjectDialog( this, TXT( "Select New Project Location" ), wxEmptyString, TXT( "New Project" ), TXT( "Project File (*.HeliumProject)|*.HeliumProject|All Files (*)|*" ), FileDialogStyles::DefaultSave );

	if ( newProjectDialog.ShowModal() == wxID_OK )
	{
		FilePath newProjectPath( std::string( newProjectDialog.GetPath().c_str() ) );

		// the newProjectDialog prompts if they're choosing an existing path, so we should just need to clean up here if it exists
		if ( newProjectPath.Exists() )
		{
			if ( !newProjectPath.Delete() )
			{
				wxMessageBox( wxT( "Could not remove the existing project: FIXME -- add an error" ), wxT( "Error Removing Exising Project" ), wxOK );
				return;
			}
		}

		OpenProject( newProjectPath );
	}
}

void MainFrame::OpenProjectDialog()
{
	FileDialog openDlg( this, TXT( "Open Project..." ), wxEmptyString, wxEmptyString, TXT( "Project File (*.HeliumProject)|*.HeliumProject|All Files (*)|*" ), FileDialogStyles::Open );

	if ( openDlg.ShowModal() == wxID_OK )
	{
		FilePath existingProjectPath( std::string( openDlg.GetPath().c_str() ) );

		if ( !existingProjectPath.Exists() )
		{
			wxMessageBox( wxT( "No such project exists on the disk." ), wxT( "No Such Project" ), wxOK );
			return;
		}

		OpenProject( existingProjectPath );
	}
}

void MainFrame::OpenScene( const FilePath& path )
{
	HELIUM_ASSERT( m_Project );

	m_PropertiesPanel->GetPropertiesManager().SyncThreads();

	// Add to the project before opening it
	m_Project->AddPath( path );

	Scene* scene = m_SceneManager.GetScene( path );
	if ( !scene )
	{
		Document* document = m_DocumentManager.FindDocument( path );
		if ( !document )
		{
			std::string error;
			document = new Document( path );
			bool result = m_DocumentManager.OpenDocument( document, error );
			HELIUM_ASSERT( result );
		}

		if ( path.Exists() )
		{
			std::string error;
			scene = m_SceneManager.OpenScene(  &m_ViewPanel->GetViewCanvas()->GetViewport(), document, error );

			if ( !error.empty() )
			{
				wxMessageDialog msgBox( this, error.c_str(), wxT( "Error Loading Scene" ), wxOK | wxICON_EXCLAMATION );
				msgBox.ShowModal();

				return;
			}
		}
		else
		{
			scene = m_SceneManager.NewScene( &m_ViewPanel->GetViewCanvas()->GetViewport(), document );
		}

		HELIUM_ASSERT( scene );

		scene->d_ResolveScene.Set( ResolveSceneSignature::Delegate( this, &MainFrame::AllocateNestedScene ) );
		scene->d_ReleaseScene.Set( ReleaseSceneSignature::Delegate( this, &MainFrame::ReleaseNestedScene ) );
	}

	m_SceneManager.SetCurrentScene( scene );

}

void MainFrame::CloseAllScenes()
{
	std::string error;
	m_SceneManager.SaveAllScenes( error );

	if ( !error.empty() )
	{
		wxMessageBox( error.c_str(), wxT( "Error Saving Scenes" ), wxOK );
		return;
	}

	m_SceneManager.RemoveAllScenes();
}

static void RecurseToggleSelection( SceneGraph::HierarchyNode* node, const OS_ObjectDumbPtr& oldSelection, OS_ObjectDumbPtr& newSelection )
{
	for ( OS_HierarchyNodeDumbPtr::Iterator itr = node->GetChildren().Begin(), end = node->GetChildren().End(); itr != end; ++itr )
	{
		SceneGraph::HierarchyNode* child = *itr;
		RecurseToggleSelection( child, oldSelection, newSelection );
	}

	bool found = false;
	OS_ObjectDumbPtr::Iterator selItr = oldSelection.Begin();
	OS_ObjectDumbPtr::Iterator selEnd = oldSelection.End();
	for ( ; selItr != selEnd && !found; ++selItr )
	{
		SceneGraph::HierarchyNode* current = Reflect::SafeCast< SceneGraph::HierarchyNode >( *selItr );
		if ( current )
		{
			if ( current == node )
			{
				found = true; // breaks out of the loop
			}
		}
	}

	if ( !found )
	{
		newSelection.Append( node );
	}
}

void MainFrame::InvertSelection()
{
	if ( m_SceneManager.HasCurrentScene() )
	{
		const OS_ObjectDumbPtr& selection = m_SceneManager.GetCurrentScene()->GetSelection().GetItems();
		if ( selection.Size() > 0 )
		{
			OS_ObjectDumbPtr newSelection;
			RecurseToggleSelection( m_SceneManager.GetCurrentScene()->GetRoot(), selection, newSelection );
			m_SceneManager.GetCurrentScene()->Push( m_SceneManager.GetCurrentScene()->GetSelection().SetItems( newSelection ) );
		}
	}
}

bool MainFrame::SaveAll( std::string& error )
{
	return m_DocumentManager.SaveAll( error );
}

bool MainFrame::ValidateDrag( const Editor::DragArgs& args )
{
	bool canHandleArgs = false;

#ifdef ASSET_REFACTOR
	std::set< std::string > supportedExtensions;
	Asset::AssetClass::GetExtensions( supportedExtensions ); 

	ClipboardFileListPtr fileList = Reflect::SafeCast< ClipboardFileList >( args.m_ClipboardData->FromBuffer() );
	if ( fileList )
	{
		for ( std::set< std::string >::const_iterator fileItr = fileList->GetFilePaths().begin(), fileEnd = fileList->GetFilePaths().end();
			fileItr != fileEnd && !canHandleArgs;
			++fileItr )
		{
			FilePath path( *fileItr );

			if ( path.Exists() )
			{
				for ( std::set< std::string >::const_iterator extItr = supportedExtensions.begin(), extEnd = supportedExtensions.end(); extItr != extEnd; ++extItr )
				{
					if ( path.HasExtension( (*extItr).c_str() ) )
					{
						canHandleArgs = true;
					}
				}
			}
		}
	}
#endif

	return canHandleArgs;
}

void MainFrame::DragOver( const Editor::DragArgs& args )
{
	wxDragResult result = args.m_Default;

	if ( !ValidateDrag( args ) )
	{
		result = wxDragNone;
	}

	args.m_Result = result;
}

void MainFrame::Drop( const Editor::DragArgs& args )
{
	wxDragResult result = args.m_Default;

	if ( ValidateDrag( args ) )
	{
		ClipboardFileListPtr fileList = Reflect::SafeCast< ClipboardFileList >( args.m_ClipboardData->FromBuffer() );
		if ( fileList )
		{
			for ( std::set< std::string >::const_iterator fileItr = fileList->GetFilePaths().begin(),
				fileEnd = fileList->GetFilePaths().end(); fileItr != fileEnd; ++fileItr )
			{
				FilePath path( *fileItr );

#pragma TODO( "Load the files" )
			}
		}
	}

	args.m_Result = result;
}

void MainFrame::SceneAdded( const SceneChangeArgs& args )
{
	if ( !m_SceneManager.IsNestedScene( args.m_Scene ) )
	{
		// Only listen to zone and world files.
		args.m_Scene->e_StatusChanged.Add( SceneStatusChangeSignature::Delegate( this, &MainFrame::SceneStatusChanged ) );
		args.m_Scene->e_SceneContextChanged.Add( SceneContextChangedSignature::Delegate( this, &MainFrame::SceneContextChanged ) );
		args.m_Scene->e_LoadFinished.Add( LoadSignature::Delegate( this, & MainFrame::SceneLoadFinished ) );
		args.m_Scene->d_UndoCommand.Set( UndoCommandSignature::Delegate( this, &MainFrame::OnSceneUndoCommand ) );
		args.m_Scene->e_Executed.Add( ExecuteSignature::Delegate( this, &MainFrame::SceneExecuted ) );

		m_ViewPanel->GetViewCanvas()->GetViewport().AddRenderListener( RenderSignature::Delegate( args.m_Scene, &Scene::Render ) );

		m_PropertiesPanel->GetPropertiesGenerator().PopulateLink().Add( Inspect::PopulateLinkSignature::Delegate (args.m_Scene, &SceneGraph::Scene::PopulateLink) );

		Document* document = m_DocumentManager.FindDocument( args.m_Scene->GetPath() );
		if ( document )
		{
			ConnectDocument( document );
		}
	}
}

void MainFrame::SceneRemoving( const SceneChangeArgs& args )
{
	args.m_Scene->e_StatusChanged.Remove( SceneStatusChangeSignature::Delegate ( this, &MainFrame::SceneStatusChanged ) );
	args.m_Scene->e_SceneContextChanged.Remove( SceneContextChangedSignature::Delegate ( this, &MainFrame::SceneContextChanged ) );
	args.m_Scene->e_LoadFinished.Remove( LoadSignature::Delegate( this, & MainFrame::SceneLoadFinished ) );
	args.m_Scene->d_UndoCommand.Clear();
	args.m_Scene->e_Executed.Remove( ExecuteSignature::Delegate( this, &MainFrame::SceneExecuted ) );

	m_ViewPanel->GetViewCanvas()->GetViewport().RemoveRenderListener( RenderSignature::Delegate( args.m_Scene, &Scene::Render ) );

	m_PropertiesPanel->GetPropertiesGenerator().PopulateLink().Remove( Inspect::PopulateLinkSignature::Delegate (args.m_Scene, &SceneGraph::Scene::PopulateLink));

	m_ViewPanel->GetViewCanvas()->Refresh();

	m_OutlinerStates.erase( args.m_Scene );
}

void MainFrame::SceneLoadFinished( const LoadArgs& args )
{
	m_ViewPanel->GetViewCanvas()->Refresh();
	Document* document = m_DocumentManager.FindDocument( args.m_Scene->GetPath() );
	DocumentChanged( DocumentEventArgs( document ) );
}

void MainFrame::SceneExecuted( const ExecuteArgs& args )
{
	// invalidate the view
	m_ViewPanel->Refresh();

	if ( args.m_Interactively )
	{
		// paint 3d view
		m_ViewPanel->Update();
	}
}

void MainFrame::OnOpen( wxCommandEvent& event )
{
	OpenProjectDialog();
}

///////////////////////////////////////////////////////////////////////////////
// Overridden from base class.  Called when attempting to open a file from the
// MRU list.  Closes all currently open files before trying to open the new one.
// 
void MainFrame::OnMRUOpen( const MRUArgs& args )
{
	OpenProject( FilePath( args.m_Item ) );
}

void MainFrame::OnChar(wxKeyEvent& event)
{
	Helium::KeyboardInput input;
	Helium::ConvertEvent( event, input );

	if ( input.IsCtrlDown() )
	{
		switch( input.GetKeyCode() )
		{
		case KeyCodes::a: // ctrl-a
			{
				wxCommandEvent evt ( wxEVT_COMMAND_MENU_SELECTED, wxID_SELECTALL );
				GetEventHandler()->ProcessEvent( evt );
				event.Skip( false );
				return;
			}

		case KeyCodes::i: // ctrl-i
			{
				wxCommandEvent evt ( wxEVT_COMMAND_MENU_SELECTED, ID_InvertSelection );
				GetEventHandler()->ProcessEvent( evt );
				event.Skip( false );
				return;
			}

		case KeyCodes::o: // ctrl-o
			{
				wxCommandEvent evt ( wxEVT_COMMAND_MENU_SELECTED, ID_Open );
				GetEventHandler()->ProcessEvent( evt );
				event.Skip( false );
				return;
			}

		case KeyCodes::s: // ctrl-s
			{
				wxCommandEvent evt ( wxEVT_COMMAND_MENU_SELECTED, ID_SaveAll );
				GetEventHandler()->ProcessEvent( evt );
				event.Skip( false );
				return;
			}

		case KeyCodes::v: // ctrl-v
			{
				wxCommandEvent evt ( wxEVT_COMMAND_MENU_SELECTED, wxID_PASTE );
				GetEventHandler()->ProcessEvent( evt );
				event.Skip( false );
				return;
			}

		case KeyCodes::w: // ctrl-w
			{
				wxCommandEvent evt ( wxEVT_COMMAND_MENU_SELECTED, ID_Close );
				GetEventHandler()->ProcessEvent( evt );
				event.Skip( false );
				return;
			}

		case KeyCodes::x: // ctrl-x
			{
				wxCommandEvent evt ( wxEVT_COMMAND_MENU_SELECTED, wxID_CUT );
				GetEventHandler()->ProcessEvent( evt );
				event.Skip( false );
				return;
			}

		default:
			break;
		}
	}

	switch ( input.GetKeyCode() )
	{
	case KeyCodes::Space:
		m_ViewPanel->GetViewCanvas()->GetViewport().NextCameraMode();
		event.Skip( false );
		break;

	case KeyCodes::Up:
		{
			wxCommandEvent evt ( wxEVT_COMMAND_MENU_SELECTED, EventIds::ID_EditWalkUp);
			GetEventHandler()->ProcessEvent( evt );
			event.Skip( false );
			break;
		}

	case KeyCodes::Down:
		{
			wxCommandEvent evt ( wxEVT_COMMAND_MENU_SELECTED, EventIds::ID_EditWalkDown);
			GetEventHandler()->ProcessEvent( evt );
			event.Skip( false );
			break;
		}

	case KeyCodes::Right:
		{
			wxCommandEvent evt ( wxEVT_COMMAND_MENU_SELECTED, EventIds::ID_EditWalkForward);
			GetEventHandler()->ProcessEvent( evt );
			event.Skip( false );
			break;
		}

	case KeyCodes::Left:
		{
			wxCommandEvent evt ( wxEVT_COMMAND_MENU_SELECTED, EventIds::ID_EditWalkBackward);
			GetEventHandler()->ProcessEvent( evt );
			event.Skip( false );
			break;
		}

	case KeyCodes::Insert:
		{
			wxCommandEvent evt ( wxEVT_COMMAND_MENU_SELECTED, EventIds::ID_ToolsPivot);
			GetEventHandler()->ProcessEvent( evt );
			event.Skip( false );
			break;
		}

	case KeyCodes::Delete:
		{
			wxCommandEvent evt ( wxEVT_COMMAND_MENU_SELECTED, wxID_DELETE);
			GetEventHandler()->ProcessEvent( evt );
			event.Skip( false );
			break;
		}

	case KeyCodes::Escape:
		{
			wxCommandEvent evt ( wxEVT_COMMAND_MENU_SELECTED, m_ToolbarPanel->m_SelectButton->GetId() );
			GetEventHandler()->ProcessEvent( evt );
			event.Skip( false );
			break;
		}

	default:
		event.Skip();
		event.ResumePropagation( wxEVENT_PROPAGATE_MAX );
		break;
	}
}

bool CheckMRUPathExists( const std::string& item )
{
	return FilePath( item ).Exists();
}

void MainFrame::OnMenuOpen( wxMenuEvent& event )
{
	const wxMenu* menu = event.GetMenu();

	const bool isProjectOpen = m_Project.ReferencesObject();
	const bool hasCurrentScene = m_SceneManager.HasCurrentScene();
	const bool isAnythingSelected = hasCurrentScene && m_SceneManager.GetCurrentScene()->GetSelection().GetItems().Size() > 0;

	bool hasTextClipboardData = false;
	if ( hasCurrentScene && wxTheClipboard->Open() )
	{
		hasTextClipboardData = wxTheClipboard->IsSupported( wxDF_TEXT );
		wxTheClipboard->Close();
	}

	if ( menu == m_MenuFile )
	{
		// OnMenuOpen is not called for submenus m_MenuFileNew and m_MenuFileOpenRecent
		m_MenuFileNew->Enable( ID_NewEntity, isProjectOpen );
		m_MenuFileNew->Enable( ID_NewScene, isProjectOpen );

		m_MenuMRU->PopulateMenu( m_MenuFileOpenRecent, &CheckMRUPathExists );

		// Item was found using FindItemByPosition 2 because
		// that's the place the "Open Recent" submenu should be
		wxMenuItem* item = m_MenuFile->FindItemByPosition( 2 );
		HELIUM_ASSERT( item->GetSubMenu() == m_MenuFileOpenRecent );

		item->Enable( m_MenuFileOpenRecent->GetMenuItemCount() > 0 );

		// File > Close is enabled if there are documents open in the document manager
		m_MenuFile->Enable( ID_Close, m_DocumentManager.GetDocuments().Size() > 0 );

		// File > SaveAll is enabled if there are any files in the document manager that have changed
		bool doAnyDocsNeedSaving = false;
		OS_DocumentSmartPtr::Iterator docItr = m_DocumentManager.GetDocuments().Begin();
		OS_DocumentSmartPtr::Iterator docEnd = m_DocumentManager.GetDocuments().End();
		for ( ; docItr != docEnd; ++docItr )
		{
			if ( ( *docItr )->HasChanged() || !( *docItr )->GetPath().Exists() )
			{
				doAnyDocsNeedSaving = true;
				break;
			}
		}
		m_MenuFile->Enable( ID_SaveAll, doAnyDocsNeedSaving );

		// File > Import is enabled if there is a current editing scene
		m_MenuFile->Enable( ID_Import, hasCurrentScene );
		m_MenuFile->Enable( ID_ImportFromClipboard, hasTextClipboardData );

		// File > Export is only enabled if there is something selected
		m_MenuFile->Enable( ID_Export, isAnythingSelected );
		m_MenuFile->Enable( ID_ExportToClipboard, isAnythingSelected );
	}
	else if ( menu == m_MenuEdit )
	{
		// Edit > Undo/Redo is only enabled if there are commands in the queue
		m_MenuEdit->Enable( wxID_UNDO, CanUndo() );
		m_MenuEdit->Enable( wxID_REDO, CanRedo() );

		// Edit > Cut/Copy/Paste/Delete
		m_MenuEdit->Enable( wxID_CUT, isAnythingSelected );
		m_MenuEdit->Enable( wxID_COPY, isAnythingSelected );
		m_MenuEdit->Enable( wxID_PASTE, hasTextClipboardData );
		m_MenuEdit->Enable( wxID_DELETE, isAnythingSelected );

		// Edit > Select All
		m_MenuEdit->Enable( ID_SelectAll, hasCurrentScene );
		// Edit > Invert Selection is only enabled if something is selected
		m_MenuEdit->Enable( ID_InvertSelection, isAnythingSelected );

		// Edit > Group options
		m_MenuEdit->Enable( ID_Parent, isAnythingSelected );
		m_MenuEdit->Enable( ID_Unparent, isAnythingSelected );
		m_MenuEdit->Enable( ID_Group, isAnythingSelected );
		m_MenuEdit->Enable( ID_Ungroup, isAnythingSelected );
		m_MenuEdit->Enable( ID_Center, isAnythingSelected );

		// Edit > Duplicate
		m_MenuEdit->Enable( ID_Duplicate, isAnythingSelected );
		m_MenuEdit->Enable( ID_SmartDuplicate, isAnythingSelected );

		m_MenuEdit->Enable( ID_CopyTransform, isAnythingSelected );
		m_MenuEdit->Enable( ID_PasteTransform, hasTextClipboardData );

		m_MenuEdit->Enable( ID_SnapToCamera, isAnythingSelected );
		m_MenuEdit->Enable( ID_SnapCameraTo, isAnythingSelected );

		// Edit > Walk
		m_MenuEdit->Enable( ID_WalkUp, isAnythingSelected );
		m_MenuEdit->Enable( ID_WalkDown, isAnythingSelected );
		m_MenuEdit->Enable( ID_WalkForward, isAnythingSelected );
		m_MenuEdit->Enable( ID_WalkBackward, isAnythingSelected );


	}
	else if ( menu == m_MenuPanels )
	{
		UpdatePanelsMenu( m_MenuPanels );
	}
	else
	{
		event.Skip();
	}
}

void MainFrame::OnNewScene( wxCommandEvent& event )
{
	HELIUM_ASSERT( m_Project );

	m_PropertiesPanel->GetPropertiesManager().SyncThreads();

	FilePath path = NewSceneDialog();

	if ( path.empty() )
	{
		return;
	}

	ScenePtr currentScene = m_SceneManager.GetCurrentScene();
	if ( currentScene.ReferencesObject() )
	{
		currentScene->d_ResolveScene.Clear();
		currentScene->d_ReleaseScene.Clear();
	}

	// Add to the project before opening it
	m_Project->AddPath( path );

	DocumentPtr document = new Document( path );
	document->HasChanged( true );

	std::string error;
	bool result = m_DocumentManager.OpenDocument( document, error );
	HELIUM_ASSERT( result );

	ScenePtr scene = m_SceneManager.NewScene( &m_ViewPanel->GetViewCanvas()->GetViewport(), document );
	HELIUM_ASSERT( scene.ReferencesObject() );

	scene->Serialize();

	scene->d_ResolveScene.Set( ResolveSceneSignature::Delegate( this, &MainFrame::AllocateNestedScene ) );
	scene->d_ReleaseScene.Set( ReleaseSceneSignature::Delegate( this, &MainFrame::ReleaseNestedScene ) );

	m_SceneManager.SetCurrentScene( scene );
}

void MainFrame::OnNewEntity( wxCommandEvent& event )
{
	HELIUM_ASSERT( m_Project );

	wxMessageBox( wxT( "Not supported yet." ), wxT( "Error" ), wxOK|wxICON_ERROR );
}

void MainFrame::OnNewProject( wxCommandEvent& event )
{
	NewProjectDialog();
}

bool MainFrame::DoOpen( const std::string& path )
{
#pragma TODO( "Rachel WIP: "__FUNCTION__" - This should be opening and closing Projects rather than Scenes." )
#pragma TODO( "Rachel WIP: "__FUNCTION__" - We will need to handle opening/loading Scenes from the projectView" )
	//bool opened = false;
	//Helium::FilePath nocPath( path );
	//if ( !path.empty() && nocPath.Exists() )
	//{
	//    m_PropertiesPanel->GetPropertiesManager().SyncThreads();

	//    if ( m_DocumentManager.CloseAll() )
	//    {
	//        std::string error;

	//        try
	//        {
	//            opened = m_SceneManager.OpenScene( &m_ViewPanel->GetViewCanvas()->GetViewport(), path, error ) != NULL;
	//        }
	//        catch ( const Helium::Exception& ex )
	//        {
	//            error = ex.What();
	//        }

	//        if ( opened )
	//        {
	//            m_MenuMRU->Insert( path );
	//        }
	//        else
	//        {
	//            m_MenuMRU->Remove( path );
	//            if ( !error.empty() )
	//            {
	//                wxMessageBox( error.c_str(), wxT( "Error" ), wxCENTER | wxICON_ERROR | wxOK, this );
	//            }
	//        }
	//    }
	//}
	//return opened;
	return false;
}

void MainFrame::OnClose( wxCommandEvent& event )
{
	CloseProject();
}

void MainFrame::OnSaveAll( wxCommandEvent& event )
{
	std::string error;
	if ( !SaveAll( error ) )
	{
		wxMessageBox( error.c_str(), wxT( "Error" ), wxCENTER | wxICON_ERROR | wxOK, this );
	}
}

void MainFrame::OpenVaultPanel()
{
	wxString queryString = m_ToolbarPanel->m_VaultSearchBox->GetLineText(0);
	queryString.Trim(true);  // trim white-space right 
	queryString.Trim(false); // trim white-space left

	if ( !m_VaultPanel )
	{
		m_VaultPanel = new VaultPanel( this );
		wxAuiPaneInfo vaultPanelInfo = wxAuiPaneInfo().Name( wxT( "vault" ) ).Caption( wxT( "Asset Vault" ) ).Right().Layer( 1 ).Position( 4 );
		m_FrameManager.AddPane( m_VaultPanel, vaultPanelInfo );
		//m_ExcludeFromPanelsMenu.insert( vaultPanelInfo.name );

		m_VaultPanel->Show();
		m_FrameManager.Update();
	}

	if ( !m_VaultPanel->IsShown() )
	{
		m_VaultPanel->Show();
	}

	wxAuiPaneInfo& pane = m_FrameManager.GetPane( m_VaultPanel );
	if ( pane.IsOk() )
	{
		pane.Show( !pane.IsShown() );
		m_FrameManager.Update();
	}

	if ( !queryString.empty() )
	{
		wxAuiPaneInfo& pane = m_FrameManager.GetPane( m_VaultPanel );
		pane.caption = wxT( "Vault Search: " );
		pane.caption += queryString;
	}

	m_VaultPanel->Search( std::string( queryString.c_str() ) );
}

void MainFrame::OnSearchGoButtonClick( wxCommandEvent& event )
{
	OpenVaultPanel();
	event.Skip(false);
}

void MainFrame::OnSearchTextEnter( wxCommandEvent& event )
{
	OpenVaultPanel();
	event.Skip(false);
}

void MainFrame::OnViewChange(wxCommandEvent& event)
{
	switch (event.GetId())
	{
	case EventIds::ID_ViewAxes:
		{
			m_ViewPanel->GetViewCanvas()->GetViewport().SetAxesVisible( !m_ViewPanel->GetViewCanvas()->GetViewport().IsAxesVisible() );
			break;
		}

	case EventIds::ID_ViewGrid:
		{
			m_ViewPanel->GetViewCanvas()->GetViewport().SetGridVisible( !m_ViewPanel->GetViewCanvas()->GetViewport().IsGridVisible() );
			break;
		}

	case EventIds::ID_ViewBounds:
		{
			m_ViewPanel->GetViewCanvas()->GetViewport().SetBoundsVisible( !m_ViewPanel->GetViewCanvas()->GetViewport().IsBoundsVisible() );
			break;
		}

	case EventIds::ID_ViewStatistics:
		{
			m_ViewPanel->GetViewCanvas()->GetViewport().SetStatisticsVisible( !m_ViewPanel->GetViewCanvas()->GetViewport().IsStatisticsVisible() );
			break;
		}

	case EventIds::ID_ViewNone:
		{
			m_ViewPanel->GetViewCanvas()->GetViewport().SetGeometryMode( GeometryMode::None );
			break;
		}

	case EventIds::ID_ViewRender:
		{
			m_ViewPanel->GetViewCanvas()->GetViewport().SetGeometryMode( GeometryMode::Render );
			break;
		}

	case EventIds::ID_ViewCollision:
		{
			m_ViewPanel->GetViewCanvas()->GetViewport().SetGeometryMode( GeometryMode::Collision );
			break;
		}

	case EventIds::ID_ViewWireframeOnMesh:
		{
			m_ViewPanel->GetViewCanvas()->GetViewport().GetCamera()->SetWireframeOnMesh( !m_ViewPanel->GetViewCanvas()->GetViewport().GetCamera()->GetWireframeOnMesh() );
			break;
		}

	case EventIds::ID_ViewWireframeOnShaded:
		{
			m_ViewPanel->GetViewCanvas()->GetViewport().GetCamera()->SetWireframeOnShaded( !m_ViewPanel->GetViewCanvas()->GetViewport().GetCamera()->GetWireframeOnShaded() );
			break;
		}

	case EventIds::ID_ViewWireframe:
		{
			m_ViewPanel->GetViewCanvas()->GetViewport().GetCamera()->SetShadingMode( ShadingMode::Wireframe );
			break;
		}

	case EventIds::ID_ViewMaterial:
		{
			m_ViewPanel->GetViewCanvas()->GetViewport().GetCamera()->SetShadingMode( ShadingMode::Material );
			break;
		}

	case EventIds::ID_ViewTexture:
		{
			m_ViewPanel->GetViewCanvas()->GetViewport().GetCamera()->SetShadingMode( ShadingMode::Texture );
			break;
		}

	case EventIds::ID_ViewFrustumCulling:
		{
			m_ViewPanel->GetViewCanvas()->GetViewport().GetCamera()->SetViewFrustumCulling( !m_ViewPanel->GetViewCanvas()->GetViewport().GetCamera()->IsViewFrustumCulling() );
			break;
		}

	case EventIds::ID_ViewBackfaceCulling:
		{
			m_ViewPanel->GetViewCanvas()->GetViewport().GetCamera()->SetBackFaceCulling( !m_ViewPanel->GetViewCanvas()->GetViewport().GetCamera()->IsBackFaceCulling() );
			break;
		}
	}

	m_ViewPanel->GetViewCanvas()->Refresh();
}

void MainFrame::OnViewCameraChange(wxCommandEvent& event)
{
	switch (event.GetId())
	{
	case EventIds::ID_ViewOrbit:
		{
			m_ViewPanel->GetViewCanvas()->GetViewport().SetCameraMode(CameraMode::Orbit);
			break;
		}

	case EventIds::ID_ViewFront:
		{
			m_ViewPanel->GetViewCanvas()->GetViewport().SetCameraMode(CameraMode::Front);
			break;
		}

	case EventIds::ID_ViewSide:
		{
			m_ViewPanel->GetViewCanvas()->GetViewport().SetCameraMode(CameraMode::Side);
			break;
		}

	case EventIds::ID_ViewTop:
		{
			m_ViewPanel->GetViewCanvas()->GetViewport().SetCameraMode(CameraMode::Top);
			break;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// Handles callbacks for menu items dealing with Viewport->Show and Viewport->Hide.
// Changes the visibility of items according to which command was called.
// 
void MainFrame::OnViewVisibleChange(wxCommandEvent& event)
{
	if ( m_SceneManager.HasCurrentScene() )
	{
		BatchUndoCommandPtr batch = new BatchUndoCommand ();

		switch ( event.GetId() )
		{
		case EventIds::ID_ViewShowAll:
			{
				batch->Push( m_SceneManager.GetCurrentScene()->SetHiddenSelected( false ) );
				batch->Push( m_SceneManager.GetCurrentScene()->SetHiddenUnrelated( false ) );
				break;
			}

		case EventIds::ID_ViewShowAllGeometry:
			{
				break;
			}

		case EventIds::ID_ViewShowSelected:
			{
				batch->Push( m_SceneManager.GetCurrentScene()->SetHiddenSelected( false ) );
				break;
			}

		case EventIds::ID_ViewShowSelectedGeometry:
			{
				break;
			}

		case EventIds::ID_ViewShowUnrelated:
			{
				batch->Push( m_SceneManager.GetCurrentScene()->SetHiddenUnrelated( false ) );
				break;
			}

		case EventIds::ID_ViewShowLastHidden:
			{
				batch->Push( m_SceneManager.GetCurrentScene()->ShowLastHidden() );
				break;
			}

		case EventIds::ID_ViewHideAll:
			{
				batch->Push( m_SceneManager.GetCurrentScene()->SetHiddenSelected( true ) );
				batch->Push( m_SceneManager.GetCurrentScene()->SetHiddenUnrelated( true ) );
				break;
			}

		case EventIds::ID_ViewHideAllGeometry:
			{
				break;
			}

		case EventIds::ID_ViewHideSelected:
			{
				batch->Push( m_SceneManager.GetCurrentScene()->SetHiddenSelected( true ) );
				break;
			}

		case EventIds::ID_ViewHideSelectedGeometry:
			{
				break;
			}

		case EventIds::ID_ViewHideUnrelated:
			{
				batch->Push( m_SceneManager.GetCurrentScene()->SetHiddenUnrelated( true ) );
				break;
			}

		default:
			{
				Log::Warning( TXT( "MainFrame::OnViewVisibleChange - Unhandled case\n" ) );
				return;
			}
		}

		m_SceneManager.GetCurrentScene()->Push( batch );

		m_SceneManager.GetCurrentScene()->Execute( false );
	}
}

void MainFrame::OnViewColorModeChange(wxCommandEvent& event)
{
	const ViewColorMode previousColorMode = wxGetApp().GetSettingsManager()->GetSettings< ViewportSettings >()->GetColorMode();

	const M_IDToColorMode::const_iterator newColorModeItr = m_ColorModeLookup.find( event.GetId() );
	if ( newColorModeItr != m_ColorModeLookup.end() )
	{
		wxGetApp().GetSettingsManager()->GetSettings< ViewportSettings >()->SetColorMode( ( ViewColorMode::Enum )( newColorModeItr->second ) );
	}
}

///////////////////////////////////////////////////////////////////////////////
// Called when the user chooses to import a file into the current editing
// scene.  Prompts the user with import options, and carries out the operation
// if the user does not cancel.
// 
void MainFrame::OnImport(wxCommandEvent& event)
{
	static bool update = false;

	if ( m_SceneManager.HasCurrentScene() )
	{
		SceneGraph::Scene* currentScene = m_SceneManager.GetCurrentScene();

		ImportOptionsDialog dlg( this, update );

		if ( dlg.ShowModal() == wxID_OK && currentScene->IsEditable() )
		{
			uint32_t flags = Scene::ImportFlags::Select;
			if ( update )
			{
				flags |= Scene::ImportFlags::Merge;
			}

			switch ( event.GetId() )
			{
			case EventIds::ID_FileImport:
				{
					FileDialog fileDialog( this, TXT( "Import" ) );

					std::set< std::string > filters;
#pragma TODO( "Populate the filters with a list of our supported file types" )
					for ( std::set< std::string >::const_iterator itr = filters.begin(), end = filters.end(); itr != end; ++itr )
					{
						fileDialog.AddFilter( (*itr) );
					}

					if ( fileDialog.ShowModal() != wxID_OK )
					{
						return;
					}

					Helium::FilePath path( std::string( fileDialog.GetPath().c_str() ) );
					currentScene->Push( currentScene->Import( path, Scene::ImportActions::Import, flags, currentScene->GetRoot() ) );
					break;
				}

			case EventIds::ID_FileImportFromClipboard:
				{
					std::string xml;
					if (wxTheClipboard->Open())
					{
						if (wxTheClipboard->IsSupported( wxDF_TEXT ))
						{
							wxTextDataObject data;
							wxTheClipboard->GetData( data );
							xml = data.GetText();
						}  
						wxTheClipboard->Close();
					}

					currentScene->Push( currentScene->ImportXML( xml, flags, currentScene->GetRoot() ) );
					break;
				}
			}

			currentScene->Execute(false);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// Called when the user chooses to export a selection from the current editing
// scene.  Prompts the user with the export options, and performs the operation
// if the user does not cancel.
// 
void MainFrame::OnExport(wxCommandEvent& event)
{
	static bool exportDependencies = true;
	static bool exportHierarchy = true;

	if ( m_SceneManager.HasCurrentScene() )
	{
		ExportOptionsDialog dlg ( this, exportDependencies, exportHierarchy );

		if ( dlg.ShowModal() == wxID_OK )
		{
			Scene::ExportArgs args;

			if ( exportHierarchy )
			{
				args.m_Flags |= Scene::ExportFlags::MaintainHierarchy;
			}

			if ( exportDependencies )
			{
				args.m_Flags |= Scene::ExportFlags::MaintainDependencies;
			}

			args.m_Flags |= Scene::ExportFlags::SelectedNodes;

			uint64_t startTimer = Helium::TimerGetClock();

			SetCursor( wxCursor( wxCURSOR_WAIT ) );

			{
				std::ostringstream str;
				str << "Preparing to export";
				SceneStatusChanged( str.str() );
			}

			BatchUndoCommandPtr changes = new BatchUndoCommand();

			std::vector< Reflect::ObjectPtr > elements;
			bool result = m_SceneManager.GetCurrentScene()->Export( elements, args, changes );
			if ( result && !elements.empty() )
			{
				switch ( event.GetId() )
				{
				case EventIds::ID_FileExport:
					{
						FileDialog fileDialog( this, TXT( "Export Selection" ), TXT( "" ), TXT( "" ), wxFileSelectorDefaultWildcardStr, FileDialogStyles::DefaultSave );

						if ( fileDialog.ShowModal() != wxID_OK )
						{
							return;
						}

						std::string file ( fileDialog.GetPath() );

						try
						{
#if REFLECT_REFACTOR
							Persist::ArchivePtr archive = Reflect::GetArchive( file );
							archive->e_Status.AddMethod( m_SceneManager.GetCurrentScene(), &Scene::ArchiveStatus );
							archive->Put( elements );
							archive->Close();
#endif
						}
						catch ( Helium::Exception& ex )
						{
							std::ostringstream str;
							str << "Failed to generate file '" << file << "': " << ex.What();
							wxMessageBox( str.str(), wxT( "Error" ), wxOK|wxCENTRE|wxICON_ERROR );
							result = false;
						}

						break;
					}

				case EventIds::ID_FileExportToClipboard:
					{
						std::string xml;

						try
						{
#if REFLECT_REFACTOR
							Reflect::ArchiveXML::ToString( elements, xml );
#endif
						}
						catch ( Helium::Exception& ex )
						{
							std::ostringstream str;
							str << "Failed to generate xml: " << ex.What();
							wxMessageBox( str.str(), wxT( "Error" ), wxOK|wxCENTRE|wxICON_ERROR );
							result = false;
						}

						if ( wxTheClipboard->Open() )
						{
							wxTheClipboard->SetData( new wxTextDataObject( xml ) );
							wxTheClipboard->Close();
						}

						break;
					}
				}
			}

			changes->Undo();

			SetCursor( wxCursor( wxCURSOR_ARROW ) );

			{
				std::ostringstream str;
				str.precision( 2 );
				str << "Export Complete: " << std::fixed << Helium::CyclesToMillis( Helium::TimerGetClock() - startTimer ) / 1000.f << " seconds...";
				SceneStatusChanged( str.str() );
			}
		}
	}
}

void MainFrame::CurrentSceneChanged( const SceneChangeArgs& args )
{
	//if ( args.m_PreviousScene )
	//{
	//	m_ProjectPanel->SetActive( args.m_PreviousScene->GetPath(), false );
	//}

	if ( args.m_Scene )
	{
		m_ToolbarPanel->EnableTools();

		// Hook our event handlers
		args.m_Scene->e_StatusChanged.Add( SceneStatusChangeSignature::Delegate ( this, &MainFrame::SceneStatusChanged ) );
		args.m_Scene->e_SceneContextChanged.Add( SceneContextChangedSignature::Delegate ( this, &MainFrame::SceneContextChanged ) );
		args.m_Scene->e_Executed.Add( ExecuteSignature::Delegate ( this, &MainFrame::Executed ) );

		// Selection event handlers
		args.m_Scene->AddSelectionChangedListener( SelectionChangedSignature::Delegate ( this, &MainFrame::SelectionChanged ) );

		// These events are emitted from the attribute editor and cause execution of the scene to occur, and interactive goodness
		m_PropertiesPanel->GetPropertiesGenerator().PropertyChanging().AddMethod( args.m_Scene, &SceneGraph::Scene::PropertyChanging );
		m_PropertiesPanel->GetPropertiesGenerator().PropertyChanged().AddMethod( args.m_Scene, &SceneGraph::Scene::PropertyChanged );
		m_PropertiesPanel->GetPropertiesGenerator().SelectLink().AddMethod( args.m_Scene, &SceneGraph::Scene::SelectLink );
		m_PropertiesPanel->GetPropertiesGenerator().PickLink().AddMethod( args.m_Scene, &SceneGraph::Scene::PickLink );

		// Restore the tree control with the information for the new editing scene
		M_OutlinerStates::iterator foundOutline = m_OutlinerStates.find( args.m_Scene );
		if ( foundOutline != m_OutlinerStates.end() )
		{
			OutlinerStates* stateInfo = &foundOutline->second;
			m_DirectoryPanel->RestoreState( stateInfo->m_Hierarchy, stateInfo->m_Entities, stateInfo->m_Types );
		}

		//Begin batching
		m_LayersPanel->BeginBatch();

		// iterate over all the layer instances and add them to the layer grid UI.
		for ( M_SceneNodeSmartPtr::const_iterator instItr = args.m_Scene->GetNodes().begin(), instEnd = args.m_Scene->GetNodes().end(); instItr != instEnd; ++instItr )
		{
			const SceneNodePtr& dependNode    = instItr->second;
			SceneGraph::Layer*        lunaLayer     = Reflect::AssertCast< SceneGraph::Layer >( dependNode );
			m_LayersPanel->AddLayer( lunaLayer );
		}

		//End batching
		m_LayersPanel->EndBatch();

#pragma TODO( "Change the selection or display changes in the Project view" )

		//m_ProjectPanel->SetActive( args.m_Scene->GetPath(), true );

		// Restore selection-sensitive settings
		args.m_Scene->RefreshSelection();

		// Restore tool
		if (args.m_Scene->GetTool().ReferencesObject())
		{
			// Restore tool to the view from the scene
			m_ViewPanel->GetViewCanvas()->GetViewport().SetTool(args.m_Scene->GetTool());

			// Restore tool attributes
			args.m_Scene->GetTool()->CreateProperties();

			// Layout ui
			m_ToolbarPanel->GetToolsPropertiesPanel()->Layout();

			// Read state
			m_ToolbarPanel->GetCanvas().Read();
		}
	}

	m_ProjectPanel->Refresh();
}

void MainFrame::CurrentSceneChanging( const SceneChangeArgs& args )
{
	if ( args.m_Scene && args.m_Scene->GetType() == Scene::SceneTypes::World )
	{
		World* pWorld = Reflect::AssertCast<World>( args.m_Scene->GetRuntimeObject() );
		m_ViewPanel->GetViewCanvas()->GetViewport().BindToWorld( pWorld );
	}
	else
	{
		m_ViewPanel->GetViewCanvas()->GetViewport().UnbindFromWorld();
	}

	if ( args.m_PreviousScene )
	{
		// Unhook our event handlers
		args.m_PreviousScene->e_StatusChanged.Remove( SceneStatusChangeSignature::Delegate ( this, &MainFrame::SceneStatusChanged ) );
		args.m_PreviousScene->e_SceneContextChanged.Remove( SceneContextChangedSignature::Delegate ( this, &MainFrame::SceneContextChanged ) );
		args.m_PreviousScene->e_Executed.Remove( ExecuteSignature::Delegate ( this, &MainFrame::Executed ) );

		// Selection event handlers
		args.m_PreviousScene->RemoveSelectionChangedListener( SelectionChangedSignature::Delegate ( this, &MainFrame::SelectionChanged ) );

		// Remove attribute listeners
		m_PropertiesPanel->GetPropertiesGenerator().PropertyChanging().RemoveMethod( args.m_PreviousScene, &SceneGraph::Scene::PropertyChanging );
		m_PropertiesPanel->GetPropertiesGenerator().PropertyChanged().RemoveMethod( args.m_PreviousScene, &SceneGraph::Scene::PropertyChanged );
		m_PropertiesPanel->GetPropertiesGenerator().SelectLink().RemoveMethod( args.m_PreviousScene, &SceneGraph::Scene::SelectLink );
		m_PropertiesPanel->GetPropertiesGenerator().PickLink().RemoveMethod( args.m_PreviousScene, &SceneGraph::Scene::PickLink );

		// If we were editing a scene, save the outliner info before changing to the new one.
		OutlinerStates* stateInfo = &m_OutlinerStates.insert( M_OutlinerStates::value_type( args.m_PreviousScene, OutlinerStates() ) ).first->second;
		m_DirectoryPanel->SaveState( stateInfo->m_Hierarchy, stateInfo->m_Entities, stateInfo->m_Types );

		// Clear the selection attribute canvas
		m_PropertiesPanel->GetCanvas().Clear();

		// Clear the tool attribute canavs
		m_ToolbarPanel->GetCanvas().Clear();

		// Release the tool from the VIEW and Scene, saving the tool in the scene isn't a desirable behavior and the way it is currently
		// implimented it will cause a crash under certain scenarios (see trac #1322)
		args.m_PreviousScene->SetTool( NULL );
		m_ViewPanel->GetViewCanvas()->GetViewport().SetTool( NULL );

		m_ToolbarPanel->EnableTools( false );
	}
}

void MainFrame::OnToolSelected( wxCommandEvent& event )
{
	if ( !m_SceneManager.HasCurrentScene() )
	{
		GetStatusBar()->SetStatusText( TXT( "You must create a new scene or open an existing scene to use a tool" ) );
		return;
	}
	
	if ( event.GetId() == 0
		|| event.GetId() == m_ToolbarPanel->m_SelectButton->GetId() )
	{
		m_SceneManager.GetCurrentScene()->SetTool(NULL);
	}
	else if ( event.GetId() == m_ToolbarPanel->m_TranslateButton->GetId() )
	{
		m_SceneManager.GetCurrentScene()->SetTool(new SceneGraph::TranslateManipulator( m_SettingsManager, ManipulatorModes::Translate, m_SceneManager.GetCurrentScene(), &m_ToolbarPanel->GetPropertiesGenerator()));
	}
	else if ( event.GetId() == m_ToolbarPanel->m_RotateButton->GetId() )
	{
		m_SceneManager.GetCurrentScene()->SetTool(new SceneGraph::RotateManipulator( m_SettingsManager, ManipulatorModes::Rotate, m_SceneManager.GetCurrentScene(), &m_ToolbarPanel->GetPropertiesGenerator()));
	}
	else if ( event.GetId() == m_ToolbarPanel->m_ScaleButton->GetId() )
	{
		m_SceneManager.GetCurrentScene()->SetTool(new SceneGraph::ScaleManipulator( m_SettingsManager, ManipulatorModes::Scale, m_SceneManager.GetCurrentScene(), &m_ToolbarPanel->GetPropertiesGenerator()));
	}
	else if ( event.GetId() == m_ToolbarPanel->m_LocatorToolButton->GetId() )
	{
		m_SceneManager.GetCurrentScene()->SetTool(new SceneGraph::LocatorCreateTool( m_SceneManager.GetCurrentScene(), &m_ToolbarPanel->GetPropertiesGenerator()) );
	}
	else if ( event.GetId() == m_ToolbarPanel->m_DuplicateToolButton->GetId() )
	{
		m_SceneManager.GetCurrentScene()->SetTool(new SceneGraph::DuplicateTool( m_SceneManager.GetCurrentScene(), &m_ToolbarPanel->GetPropertiesGenerator()) );
	}
	else if ( event.GetId() == m_ToolbarPanel->m_CurveToolButton->GetId() )
	{
		m_SceneManager.GetCurrentScene()->SetTool( new SceneGraph::CurveCreateTool( m_SceneManager.GetCurrentScene(), &m_ToolbarPanel->GetPropertiesGenerator() ) );
	}
	else if ( event.GetId() == m_ToolbarPanel->m_CurveEditToolButton->GetId() )
	{
		SceneGraph::CurveEditTool* curveEditTool = new SceneGraph::CurveEditTool( m_SettingsManager, m_SceneManager.GetCurrentScene(), &m_ToolbarPanel->GetPropertiesGenerator() );
		m_SceneManager.GetCurrentScene()->SetTool( curveEditTool );
		curveEditTool->StoreSelectedCurves();
	}
	else
	{
		switch ( event.GetId() )
		{
		case EventIds::ID_ToolsScalePivot:
			{
				m_SceneManager.GetCurrentScene()->SetTool(new SceneGraph::TranslateManipulator( m_SettingsManager, ManipulatorModes::ScalePivot, m_SceneManager.GetCurrentScene(), &m_ToolbarPanel->GetPropertiesGenerator()));
				break;
			}

		case EventIds::ID_ToolsRotatePivot:
			{
				m_SceneManager.GetCurrentScene()->SetTool(new SceneGraph::TranslateManipulator( m_SettingsManager, ManipulatorModes::RotatePivot, m_SceneManager.GetCurrentScene(), &m_ToolbarPanel->GetPropertiesGenerator()));
				break;
			}

		case EventIds::ID_ToolsTranslatePivot:
			{
				m_SceneManager.GetCurrentScene()->SetTool(new SceneGraph::TranslateManipulator( m_SettingsManager, ManipulatorModes::TranslatePivot, m_SceneManager.GetCurrentScene(), &m_ToolbarPanel->GetPropertiesGenerator()));
				break;
			}

		case EventIds::ID_ToolsPivot:
			{
				if (m_SceneManager.GetCurrentScene()->GetTool().ReferencesObject())
				{
					if ( m_SceneManager.GetCurrentScene()->GetTool()->GetMetaClass() == Reflect::GetMetaClass< SceneGraph::ScaleManipulator >() )
					{
						m_SceneManager.GetCurrentScene()->SetTool(new SceneGraph::TranslateManipulator( m_SettingsManager, ManipulatorModes::ScalePivot, m_SceneManager.GetCurrentScene(), &m_ToolbarPanel->GetPropertiesGenerator()));
					}
					else if ( m_SceneManager.GetCurrentScene()->GetTool()->GetMetaClass() == Reflect::GetMetaClass< SceneGraph::RotateManipulator >() )
					{
						m_SceneManager.GetCurrentScene()->SetTool(new SceneGraph::TranslateManipulator( m_SettingsManager, ManipulatorModes::RotatePivot, m_SceneManager.GetCurrentScene(), &m_ToolbarPanel->GetPropertiesGenerator()));
					}
					else if ( m_SceneManager.GetCurrentScene()->GetTool()->GetMetaClass() == Reflect::GetMetaClass< SceneGraph::TranslateManipulator >() )
					{
						SceneGraph::TranslateManipulator* manipulator = Reflect::AssertCast< SceneGraph::TranslateManipulator >(m_SceneManager.GetCurrentScene()->GetTool());

						if ( manipulator->GetMode() == ManipulatorModes::Translate)
						{
							m_SceneManager.GetCurrentScene()->SetTool(new SceneGraph::TranslateManipulator( m_SettingsManager, ManipulatorModes::TranslatePivot, m_SceneManager.GetCurrentScene(), &m_ToolbarPanel->GetPropertiesGenerator()));
						}
						else
						{
							switch ( manipulator->GetMode() )
							{
							case ManipulatorModes::ScalePivot:
								m_SceneManager.GetCurrentScene()->SetTool(new SceneGraph::ScaleManipulator( m_SettingsManager, ManipulatorModes::Scale, m_SceneManager.GetCurrentScene(), &m_ToolbarPanel->GetPropertiesGenerator()));
								break;
							case ManipulatorModes::RotatePivot:
								m_SceneManager.GetCurrentScene()->SetTool(new SceneGraph::RotateManipulator( m_SettingsManager, ManipulatorModes::Rotate, m_SceneManager.GetCurrentScene(), &m_ToolbarPanel->GetPropertiesGenerator()));
								break;
							case ManipulatorModes::TranslatePivot:
								m_SceneManager.GetCurrentScene()->SetTool(new SceneGraph::TranslateManipulator( m_SettingsManager, ManipulatorModes::Translate, m_SceneManager.GetCurrentScene(), &m_ToolbarPanel->GetPropertiesGenerator()));
								break;

							default:
								break;
							}
						}
					}
				}
				break;
			}
		}
	}

	m_ToolbarPanel->GetCanvas().Clear();

	Tool* tool = m_SceneManager.GetCurrentScene()->GetTool();

	if (tool)
	{
		tool->PickWorld().Add( PickSignature::Delegate( this, &MainFrame::PickWorld ) );
		tool->CreateProperties();

		Inspect::Canvas* canvas = &m_ToolbarPanel->GetCanvas();
		canvas->Realize( NULL );
	}

	m_ViewPanel->Refresh();
}

void MainFrame::PickWorld( PickArgs& args )
{
#pragma TODO("Pick the project's root scene -Geoff")
}

void MainFrame::ConnectDocument( Document* document )
{
	document->e_Changed.AddMethod( this, &MainFrame::DocumentChanged );
	document->e_Saved.AddMethod( this, &MainFrame::DocumentChanged ) ;
	document->e_Closed.AddMethod( this, &MainFrame::DocumentClosed );
}

void MainFrame::DisconnectDocument( const Document* document )
{
	document->e_Changed.RemoveMethod( this, &MainFrame::DocumentChanged );
	document->e_Saved.RemoveMethod( this, &MainFrame::DocumentChanged ) ;
	document->e_Closed.RemoveMethod( this, &MainFrame::DocumentClosed );
}

void MainFrame::DocumentChanged( const DocumentEventArgs& args )
{
}

void MainFrame::DocumentClosed( const DocumentEventArgs& args )
{
	DocumentChanged( args );
	DisconnectDocument( args.m_Document );
}

void MainFrame::ViewToolChanged( const ToolChangeArgs& args )
{
	int32_t selectedTool = m_ToolbarPanel->m_SelectButton->GetId();
	if ( args.m_NewTool )
	{
		SceneGraph::TransformManipulator* manipulator = Reflect::SafeCast< SceneGraph::TransformManipulator >( args.m_NewTool );
		if ( manipulator )
		{
			switch ( manipulator->GetMode() )
			{
			case ManipulatorModes::Scale:
				selectedTool = m_ToolbarPanel->m_ScaleButton->GetId();
				break;

			case ManipulatorModes::ScalePivot:
				selectedTool = EventIds::ID_ToolsScalePivot;
				break;

			case ManipulatorModes::Rotate:
				selectedTool = m_ToolbarPanel->m_RotateButton->GetId();
				break;

			case ManipulatorModes::RotatePivot:
				selectedTool = EventIds::ID_ToolsRotatePivot;
				break;

			case ManipulatorModes::Translate:
				selectedTool = m_ToolbarPanel->m_TranslateButton->GetId();
				break;

			case ManipulatorModes::TranslatePivot:
				selectedTool = EventIds::ID_ToolsTranslatePivot;
				break;
			}
		}
		else if ( args.m_NewTool->GetMetaClass() == Reflect::GetMetaClass< SceneGraph::LocatorCreateTool >() )
		{
			selectedTool = m_ToolbarPanel->m_LocatorToolButton->GetId();
		}
		else if ( args.m_NewTool->GetMetaClass() == Reflect::GetMetaClass< SceneGraph::DuplicateTool >() )
		{
			selectedTool = m_ToolbarPanel->m_DuplicateToolButton->GetId();
		}
		else if ( args.m_NewTool->GetMetaClass() == Reflect::GetMetaClass< SceneGraph::CurveCreateTool >() )
		{
			selectedTool = m_ToolbarPanel->m_CurveToolButton->GetId();
		}
		else if ( args.m_NewTool->GetMetaClass() == Reflect::GetMetaClass< SceneGraph::CurveEditTool >() )
		{
			selectedTool = m_ToolbarPanel->m_CurveEditToolButton->GetId();
		}
	}

	m_ToolbarPanel->ToggleTool( selectedTool );
}

void MainFrame::OnSceneUndoCommand( const SceneGraph::UndoCommandArgs& args )
{
	if ( args.m_Command->IsSignificant() )
	{
		args.m_Scene->e_HasChanged.Raise( DocumentObjectChangedArgs( true ) );
	}

	m_UndoQueue.Push( args.m_Command );
}

void MainFrame::OnUndo( wxCommandEvent& event )
{
	if ( CanUndo() )
	{
		m_UndoQueue.Undo();
		m_ToolbarPanel->GetCanvas().Read();
		if ( m_SceneManager.HasCurrentScene() )
		{
			m_SceneManager.GetCurrentScene()->Execute(false);
		}
	}
}

void MainFrame::OnRedo( wxCommandEvent& event )
{
	if ( CanRedo() )
	{
		m_UndoQueue.Redo();
		m_ToolbarPanel->GetCanvas().Read();
		if ( m_SceneManager.HasCurrentScene() )
		{
			m_SceneManager.GetCurrentScene()->Execute(false);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// Called when the user chooses to cut the selected item. 
// 
void MainFrame::OnCut( wxCommandEvent& event )
{
	if ( m_SceneManager.HasCurrentScene() )
	{
		if ( Copy( m_SceneManager.GetCurrentScene() ) )
		{
			m_SceneManager.GetCurrentScene()->Push( m_SceneManager.GetCurrentScene()->DeleteSelected() );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// Called when the user chooses to copy the selected item.
// 
void MainFrame::OnCopy( wxCommandEvent& event )
{
	if ( m_SceneManager.HasCurrentScene() && m_SceneManager.GetCurrentScene()->GetSelection().GetItems().Size() > 0 )
	{
		if ( !Copy( m_SceneManager.GetCurrentScene() ) )
		{
			wxMessageDialog msgBox( this, wxT( "There was an error while copying.  Refer to the output window for more details" ), wxT( "Error" ), wxOK | wxICON_EXCLAMATION );
			msgBox.ShowModal();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// Called when the user chooses to paste a previously cut or copied item.
// 
void MainFrame::OnPaste( wxCommandEvent& event )
{
	if ( m_SceneManager.HasCurrentScene() )
	{
		Paste( m_SceneManager.GetCurrentScene() );
	}
}

///////////////////////////////////////////////////////////////////////////////
// Called when the user chooses Delete from the Edit menu. Deletes the current
// selection from the current scene.
// 
void MainFrame::OnDelete( wxCommandEvent& event )
{
	m_PropertiesPanel->GetPropertiesManager().SyncThreads();

	if ( m_SceneManager.HasCurrentScene() )
	{
		m_SceneManager.GetCurrentScene()->Push( m_SceneManager.GetCurrentScene()->DeleteSelected() );
	}
}

void MainFrame::OnSelectAll( wxCommandEvent& event )
{
	if ( !m_ViewPanel->HasFocus() || !m_SceneManager.HasCurrentScene() )
	{
		event.Skip();
		return;
	}

	OS_SceneNodeDumbPtr selection;

	M_SceneNodeSmartPtr::const_iterator itr = m_SceneManager.GetCurrentScene()->GetNodes().begin();
	M_SceneNodeSmartPtr::const_iterator end = m_SceneManager.GetCurrentScene()->GetNodes().end();
	for ( ; itr != end; ++itr )
	{
		SceneGraph::SceneNode* sceneNode = itr->second;
		if ( sceneNode->IsA( Reflect::GetMetaClass< SceneGraph::HierarchyNode >() ) )
		{
			selection.Append( sceneNode );
		}
	}

	m_SceneManager.GetCurrentScene()->Push( m_SceneManager.GetCurrentScene()->GetSelection().SetItems( selection ) );
}

void MainFrame::OnInvertSelection(wxCommandEvent& event)
{
	InvertSelection();
}

void MainFrame::OnParent(wxCommandEvent& event)
{
	if ( m_SceneManager.HasCurrentScene() )
	{
		m_SceneManager.GetCurrentScene()->Push( m_SceneManager.GetCurrentScene()->ParentSelected() );
	}
}

void MainFrame::OnUnparent(wxCommandEvent& event)
{
	if ( m_SceneManager.HasCurrentScene() )
	{
		m_SceneManager.GetCurrentScene()->Push( m_SceneManager.GetCurrentScene()->UnparentSelected() );
	}
}

void MainFrame::OnGroup(wxCommandEvent& event)
{
	if ( m_SceneManager.HasCurrentScene() )
	{
		m_SceneManager.GetCurrentScene()->Push( m_SceneManager.GetCurrentScene()->GroupSelected() );
	}
}

void MainFrame::OnUngroup(wxCommandEvent& event)
{
	if ( m_SceneManager.HasCurrentScene() )
	{
		m_SceneManager.GetCurrentScene()->Push( m_SceneManager.GetCurrentScene()->UngroupSelected() );
	}
}

void MainFrame::OnCenter(wxCommandEvent& event)
{
	if ( m_SceneManager.HasCurrentScene() )
	{
		m_SceneManager.GetCurrentScene()->Push( m_SceneManager.GetCurrentScene()->CenterSelected() );
	}
}

void MainFrame::OnDuplicate(wxCommandEvent& event)
{
	if ( m_SceneManager.HasCurrentScene() )
	{
		m_SceneManager.GetCurrentScene()->Push( m_SceneManager.GetCurrentScene()->DuplicateSelected() );
	}
}

void MainFrame::OnSmartDuplicate(wxCommandEvent& event)
{
	if ( m_SceneManager.HasCurrentScene() )
	{
		m_SceneManager.GetCurrentScene()->Push( m_SceneManager.GetCurrentScene()->SmartDuplicateSelected() );
	}
}

void MainFrame::OnCopyTransform(wxCommandEvent& event)
{
#ifdef REFLECT_REFACTOR
	if ( m_SceneManager.HasCurrentScene() )
	{
		V_Matrix4 transforms;
		m_SceneManager.GetCurrentScene()->GetSelectedTransforms(transforms);

		Helium::StrongPtr<Reflect::Matrix4StlVectorData> data = new Reflect::Matrix4StlVectorData();
		(*data->m_Data) = transforms;

		std::string xml;
		data->ToXML( xml );

		if ( wxTheClipboard->Open() )
		{
			wxTheClipboard->SetData( new wxTextDataObject( xml ) );
			wxTheClipboard->Close();
		}
	}
#endif
}

void MainFrame::OnPasteTransform(wxCommandEvent& event)
{
#ifdef REFLECT_REFACTOR
	if ( m_SceneManager.HasCurrentScene() )
	{
		std::string xml;
		if (wxTheClipboard->Open())
		{
			if (wxTheClipboard->IsSupported( wxDF_TEXT ))
			{
				wxTextDataObject data;
				wxTheClipboard->GetData( data );
				xml = data.GetText();
			}  
			wxTheClipboard->Close();
		}

		std::vector< Reflect::ObjectPtr > elements;
		Reflect::ArchiveXML::FromString( xml, elements );

		std::vector< Reflect::ObjectPtr >::const_iterator itr = elements.begin();
		std::vector< Reflect::ObjectPtr >::const_iterator end = elements.end();
		for ( ; itr != end; ++itr )
		{
			Helium::StrongPtr<Reflect::Matrix4StlVectorData> data = Reflect::SafeCast< Reflect::Matrix4StlVectorData >( *itr );
			if ( data.ReferencesObject() )
			{
				m_SceneManager.GetCurrentScene()->Push( m_SceneManager.GetCurrentScene()->SetSelectedTransforms( *data->m_Data ) );
				break;
			}
		}
	}
#endif
}

void MainFrame::OnSnapToCamera(wxCommandEvent& event)
{
	if ( m_SceneManager.HasCurrentScene() )
	{
		m_SceneManager.GetCurrentScene()->Push( m_SceneManager.GetCurrentScene()->SnapSelectedToCamera() );
	}
}

void MainFrame::OnSnapCameraTo(wxCommandEvent& event)
{
	if ( m_SceneManager.HasCurrentScene() )
	{
		m_SceneManager.GetCurrentScene()->Push( m_SceneManager.GetCurrentScene()->SnapCameraToSelected() );
	}
}

void MainFrame::OnPickWalk( wxCommandEvent& event )
{
	if (m_SceneManager.HasCurrentScene())
	{
		switch (event.GetId())
		{
		case ID_WalkUp:
			{
				m_SceneManager.GetCurrentScene()->Push( m_SceneManager.GetCurrentScene()->PickWalkUp() );
				break;
			}

		case ID_WalkDown:
			{
				m_SceneManager.GetCurrentScene()->Push( m_SceneManager.GetCurrentScene()->PickWalkDown() );
				break;
			}

		case ID_WalkForward:
			{
				m_SceneManager.GetCurrentScene()->Push( m_SceneManager.GetCurrentScene()->PickWalkSibling(true) );
				break;
			}

		case ID_WalkBackward:
			{
				m_SceneManager.GetCurrentScene()->Push( m_SceneManager.GetCurrentScene()->PickWalkSibling(false) );
				break;
			}
		}
	}
}

void MainFrame::SceneStatusChanged( const SceneStatusChangeArgs& args )
{
	m_MainStatusBar->SetStatusText( args.m_Status.c_str() );
}

void MainFrame::SceneContextChanged( const SceneContextChangeArgs& args )
{
	static wxCursor busyCursor;
	busyCursor = wxCursor( wxCURSOR_WAIT );

	static wxCursor pickingCursor;
	pickingCursor = wxCursor( wxCURSOR_BULLSEYE );

	switch ( args.m_NewContext )
	{
	case SceneContexts::Loading:
	case SceneContexts::Saving:
		wxBeginBusyCursor( &busyCursor );
		break;

	case SceneContexts::Picking:
		wxBeginBusyCursor( &pickingCursor );
		break;

	case SceneContexts::Normal:
	default:
		wxEndBusyCursor();
		wxSetCursor( wxCURSOR_ARROW );
		break;
	}
}

void MainFrame::Executed( const ExecuteArgs& args )
{
	if ( !m_PropertiesPanel->GetPropertiesManager().IsActive() && !args.m_Interactively )
	{
		m_PropertiesPanel->GetCanvas().Read();
	}
}

void MainFrame::SelectionChanged( const SelectionChangeArgs& args )
{
	m_PropertiesPanel->GetPropertiesManager().SetSelection( args.m_Selection );
}

void MainFrame::OnExit( wxCommandEvent& event )
{
	wxCloseEvent closeEvent( wxEVT_CLOSE_WINDOW );
	GetEventHandler()->AddPendingEvent( closeEvent );
}

///////////////////////////////////////////////////////////////////////////////
// Called when the frame is about to be closed.  Closes all open files.
// 
void MainFrame::OnExiting( wxCloseEvent& args )
{
	m_PropertiesPanel->GetPropertiesManager().SyncThreads();

	m_UndoQueue.Reset();

	if ( !m_DocumentManager.CloseAll() )
	{
		if ( args.CanVeto() )
		{
			args.Veto();
			return;
		}
	}

	args.Skip();
}

void MainFrame::OnAbout( wxCommandEvent& event )
{
	wxString caption = wxT( "About " );
	caption += wxGetApp().AppVerName().c_str();
	caption += wxT( "..." );

	wxString message = wxT( "" );
	message += wxGetApp().AppName().c_str();
	message += wxT( " Editor version " );
	message += wxGetApp().AppVersion().c_str();
	message += wxT( ".\n(c) 2010 - " );
	message += wxGetApp().AppName().c_str();

	wxMessageBox( message, caption, wxOK | wxCENTER, this );
}

void MainFrame::OnSettings( wxCommandEvent& event )
{
	SettingsDialog dlg ( this, wxID_ANY, TXT( "Settings" ) );
	dlg.ShowModal( wxGetApp().GetSettingsManager() );
}

///////////////////////////////////////////////////////////////////////////////
//Pushes the selected menu item as the current selected item.  
//
void MainFrame::OnManifestContextMenu(wxCommandEvent& event)
{
	if( !m_OrderedContextItems.empty() )
	{ 
		uint32_t selectionIndex = event.GetId() - EventIds::ID_SelectContextMenu;

		SceneGraph::HierarchyNode* selection = m_OrderedContextItems[ selectionIndex ];

		if( selection )
		{
			OS_SceneNodeDumbPtr newSelection;
			newSelection.Append( selection );
			m_SceneManager.GetCurrentScene()->Push( m_SceneManager.GetCurrentScene()->GetSelection().SetItems( newSelection ) );
		}
	}
}

void MainFrame::OnSelectTool( wxCommandEvent& event )
{
	if ( !m_ViewPanel->HasFocus() )
	{
		event.Skip();
		return;
	}

	wxKeyEvent evt( wxEVT_KEY_DOWN );
	evt.m_keyCode = wxT( 'Q' );
	m_ViewPanel->GetViewCanvas()->OnKeyDown( evt );
}

void MainFrame::OnTranslateTool( wxCommandEvent& event )
{
	if ( !m_ViewPanel->HasFocus() )
	{
		event.Skip();
		return;
	}

	wxKeyEvent evt( wxEVT_KEY_DOWN );
	evt.m_keyCode = wxT( 'W' );
	m_ViewPanel->GetViewCanvas()->OnKeyDown( evt );
}

void MainFrame::OnRotateTool( wxCommandEvent& event )
{
	if ( !m_ViewPanel->HasFocus() )
	{
		event.Skip();
		return;
	}

	wxKeyEvent evt( wxEVT_KEY_DOWN );
	evt.m_keyCode = wxT( 'E' );
	m_ViewPanel->GetViewCanvas()->OnKeyDown( evt );
}

void MainFrame::OnScaleTool( wxCommandEvent& event )
{
	if ( !m_ViewPanel->HasFocus() )
	{
		event.Skip();
		return;
	}

	wxKeyEvent evt( wxEVT_KEY_DOWN );
	evt.m_keyCode = wxT( 'R' );
	m_ViewPanel->GetViewCanvas()->OnKeyDown( evt );
}

void MainFrame::OnMovePivotTool( wxCommandEvent& event )
{
	if ( !m_ViewPanel->HasFocus() )
	{
		event.Skip();
		return;
	}

	wxKeyEvent evt( wxEVT_KEY_DOWN );
	evt.m_keyCode = wxT( 'T' );
	m_ViewPanel->GetViewCanvas()->OnKeyDown( evt );
}

void MainFrame::OnDuplicateTool( wxCommandEvent& event )
{
	if ( !m_ViewPanel->HasFocus() )
	{
		event.Skip();
		return;
	}

	wxKeyEvent evt( wxEVT_KEY_DOWN );
	evt.m_keyCode = wxT( 'D' );
	evt.SetAltDown( true );
	m_ViewPanel->GetViewCanvas()->OnKeyDown( evt );
}


///////////////////////////////////////////////////////////////////////////////
// Copies the currently selected items from the specified scene into the
// clipboard.
// 
bool MainFrame::Copy( SceneGraph::Scene* scene )
{
	EDITOR_SCOPE_TIMER( ("") );
	bool isOk = true;
	HELIUM_ASSERT( scene );

	if ( scene->GetSelection().GetItems().Size() > 0 )
	{
		std::string xml;
		if ( !scene->ExportXML( xml, Scene::ExportFlags::Default | Scene::ExportFlags::SelectedNodes ) )
		{
			Log::Error( TXT( "There was an error while generating XML data from the selection.\n" ) );
			isOk = false;
		}
		else
		{
			if ( wxTheClipboard->Open() )
			{
				wxTheClipboard->SetData( new wxTextDataObject( xml ) );
				wxTheClipboard->Close();
			}
		}
	}

	return isOk;
}

///////////////////////////////////////////////////////////////////////////////
// Fetches data from the clipboard (if there is any) and inserts it into the
// specified scene.
// 
bool MainFrame::Paste( SceneGraph::Scene* scene )
{
	EDITOR_SCOPE_TIMER( ("") );
	HELIUM_ASSERT( scene );

	bool isOk = false;
	std::string xml;
	if (wxTheClipboard->Open())
	{
		if (wxTheClipboard->IsSupported( wxDF_TEXT ))
		{
			wxTextDataObject data;
			wxTheClipboard->GetData( data );
			xml = data.GetText();
		}  
		wxTheClipboard->Close();
	}

	// Import data into the scene
	if ( !xml.empty() )
	{
		// Create a batch to add the objects to the scene
		BatchUndoCommandPtr batch = new BatchUndoCommand ();

		// Import the data as children of the paste root
		batch->Push( scene->ImportXML( xml, Scene::ImportFlags::Select ) );

		scene->Push( batch );
		scene->Execute(false);
	}

	return isOk;
}

void MainFrame::Render( RenderVisitor* render )
{
#pragma TODO("Render the project's root scene -Geoff")
}

void MainFrame::Select(const SelectArgs& args)
{
	if (m_SceneManager.HasCurrentScene())
	{
		//if we are using manifest select
		switch( args.m_Mode )
		{
		case SelectionModes::Manifest:
			{
				OpenManifestContextMenu( args );      
				break;
			}

		default:
			{
				// do select
				m_SceneManager.GetCurrentScene()->Select(args);
				break;
			}
		}
	}
}

void MainFrame::SetHighlight( const SetHighlightArgs& args )
{
	if( m_SceneManager.HasCurrentScene() )
	{
		m_SceneManager.GetCurrentScene()->SetHighlight(args);    
	}
}

void MainFrame::ClearHighlight( const ClearHighlightArgs& args )
{
	if( m_SceneManager.HasCurrentScene() )
	{
		m_SceneManager.GetCurrentScene()->ClearHighlight(args);    
	}
}

///////////////////////////////////////////////////////////////////////////////
// Upon selection from the context menu, performs a select operation
void MainFrame::SelectItemInScene( wxCommandEvent& event )
{
	DataObject<const SelectArgs*>* data = static_cast<DataObject<const SelectArgs*>*>( event.m_callbackUserData );

	SelectArgs args ( *data->m_Data );
	args.m_Mode = SelectionModes::Replace;
	args.m_Target = SelectionTargetModes::Single;
	m_SceneManager.GetCurrentScene()->Select(args);
}

///////////////////////////////////////////////////////////////////////////////
// Selects all the items similar to what is highlighted
void MainFrame::SelectSimilarItemsInScene( wxCommandEvent& event )
{
	m_SceneManager.GetCurrentScene()->Push( m_SceneManager.GetCurrentScene()->SelectSimilar() );
}

///////////////////////////////////////////////////////////////////////////////
// Adds items that have been selected via the viewing rectangle into the context menu
// for further selection
void MainFrame::OpenManifestContextMenu(const SelectArgs& args)
{
	m_SceneManager.GetCurrentScene()->ClearHighlight( ClearHighlightArgs (false) );

	bool result = m_SceneManager.GetCurrentScene()->Pick(args.m_Pick);

	OS_ObjectDumbPtr selectableItems;
	V_PickHitSmartPtr::const_iterator itr = args.m_Pick->GetHits().begin();
	V_PickHitSmartPtr::const_iterator end = args.m_Pick->GetHits().end();
	for ( ; itr != end; ++itr )
	{
		SceneNode* node = Reflect::SafeCast<SceneNode>((*itr)->GetHitObject());
		if (node)
		{
			// add it to the new persistent list
			selectableItems.Append(node);
		}
	}

	if (m_SceneManager.GetCurrentScene()->GetTool() && !m_SceneManager.GetCurrentScene()->GetTool()->ValidateSelection (selectableItems))
	{
		return;
	}

	// add new items to the context menu
	if( !selectableItems.Empty() )
	{
		{
			OS_ObjectDumbPtr::Iterator itr = selectableItems.Begin();
			OS_ObjectDumbPtr::Iterator end = selectableItems.End();
			for( ; itr != end; ++itr)
			{
				SceneNode* node = Reflect::SafeCast<SceneNode>(*itr);

				if( node && node->IsSelectable() )
				{
					SceneGraph::HierarchyNode* hierarchyNode = Reflect::SafeCast< SceneGraph::HierarchyNode >( node );
					if ( hierarchyNode )
					{
						m_OrderedContextItems.push_back( hierarchyNode );
					}
				}
			}
		}

		if( !m_OrderedContextItems.empty() )
		{ 
			wxMenu contextMenu;

			V_HierarchyNodeDumbPtr::iterator itr = m_OrderedContextItems.begin();
			V_HierarchyNodeDumbPtr::iterator end = m_OrderedContextItems.end();

			// sort the context items
			std::sort( itr, end, SortContextItemsByName);

			// append items to the context menu
			uint32_t index = 0;
			for( ;itr != end; ++itr, ++index )
			{
				std::string str = (*itr)->GetName();

				std::string desc = (*itr)->GetDescription();

				if (!desc.empty())
				{
					str += '\t' + desc;
				}

				contextMenu.Append( EventIds::ID_SelectContextMenu + index , str.c_str() );
			}

			contextMenu.SetEventHandler( GetEventHandler() );
			GetEventHandler()->Connect( EventIds::ID_SelectContextMenu, EventIds::ID_SelectContextMenu +  (uint32_t)m_OrderedContextItems.size(),wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnManifestContextMenu ), NULL, this );
			PopupMenu( &contextMenu );
			GetEventHandler()->Disconnect( EventIds::ID_SelectContextMenu, EventIds::ID_SelectContextMenu +  (uint32_t)m_OrderedContextItems.size(),wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnManifestContextMenu ), NULL, this ); 
			m_OrderedContextItems.clear();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// Static function used to sort context items by name
bool MainFrame::SortContextItemsByName( SceneGraph::SceneNode* lhs, SceneGraph::SceneNode* rhs )
{
	std::string lname( lhs->GetName() );
	std::string rname( rhs->GetName() );

	std::transform(lname.begin(), lname.end(), lname.begin(), toupper); 
	std::transform(rname.begin(), rname.end(), rname.begin(), toupper); 

	return lname < rname;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the specified scene and increases the reference count on that scene.
// If the scene was not yet created, this function will create a new scene.  If
// there was a problem loading the scene, it will be empty.  If you allocate a
// scene, you must call ReleaseNestedScene to free it.
// 
void MainFrame::AllocateNestedScene( const ResolveSceneArgs& args )
{
	args.m_Scene = m_SceneManager.GetScene( args.m_Path );
	if ( !args.m_Scene )
	{
		// Try to load nested scene.
		//ChangeStatus( TXT("Loading ") + args.m_Path + TXT( "..." ) );

		DocumentPtr document = new Document( args.m_Path );
		document->HasChanged( true );

		std::string error;
		bool result = m_DocumentManager.OpenDocument( document, error );
		HELIUM_ASSERT( result );

		ScenePtr scenePtr = m_SceneManager.NewScene( args.m_Viewport, document, true );
		if ( !scenePtr->Load( args.m_Path ) )
		{
			Log::Error( TXT( "Failed to load scene from %s\n" ), args.m_Path.c_str() );
			m_SceneManager.RemoveScene( scenePtr );
			scenePtr = NULL;
		}

		//ChangeStatus( TXT( "Ready" ) );
		args.m_Scene = scenePtr;
	}
}

void MainFrame::ReleaseNestedScene( const ReleaseSceneArgs& args )
{
	if ( m_SceneManager.IsNestedScene( args.m_Scene ) )
	{
		m_SceneManager.ReleaseNestedScene( args.m_Scene );
	}
}
