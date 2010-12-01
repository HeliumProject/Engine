#include "Precompile.h"
#include "MainFrame.h"

#include "Platform/Platform.h"

#include "Foundation/Reflect/ArchiveXML.h"

#include "Pipeline/Asset/AssetClass.h"
#include "Pipeline/SceneGraph/Scene.h"
#include "Pipeline/SceneGraph/InstanceSet.h"
#include "Pipeline/SceneGraph/EntityInstanceType.h"
#include "Pipeline/SceneGraph/EntitySet.h"
#include "Pipeline/SceneGraph/TransformManipulator.h"
#include "Pipeline/SceneGraph/CurveCreateTool.h"
#include "Pipeline/SceneGraph/CurveEditTool.h"
#include "Pipeline/SceneGraph/DuplicateTool.h"
#include "Pipeline/SceneGraph/EntityInstanceCreateTool.h"
#include "Pipeline/SceneGraph/LocatorCreateTool.h"
#include "Pipeline/SceneGraph/VolumeCreateTool.h"
#include "Pipeline/SceneGraph/ScaleManipulator.h"
#include "Pipeline/SceneGraph/RotateManipulator.h"
#include "Pipeline/SceneGraph/TranslateManipulator.h"

#include "Editor/App.h"
#include "Editor/EditorIDs.h"
#include "Editor/FileDialog.h"
#include "Editor/ArtProvider.h"
#include "Editor/EditorSettings.h"
#include "Editor/SettingsDialog.h"
#include "Editor/WindowSettings.h"
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
        , m_NodeType( NULL )
        , m_Nodes( NULL )
        , m_InstanceSet( NULL )
    {

    }

    ContextCallbackTypes::ContextCallbackType m_ContextCallbackType;
    const SceneGraph::SceneNodeType* m_NodeType;
    const SceneGraph::InstanceSet* m_InstanceSet;
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

    /*
    EVT_MENU(wxID_HELP_INDEX, MainFrame::OnHelpIndex)
    EVT_MENU(wxID_HELP_SEARCH, MainFrame::OnHelpSearch)

    */
    //
    // Toolbox
    //
    Connect( EventIds::ID_ToolsSelect, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnToolSelected ) );
    Connect( EventIds::ID_ToolsScale, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnToolSelected ) );
    Connect( EventIds::ID_ToolsScalePivot, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnToolSelected ) );
    Connect( EventIds::ID_ToolsRotate, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnToolSelected ) );
    Connect( EventIds::ID_ToolsRotatePivot, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnToolSelected ) );
    Connect( EventIds::ID_ToolsTranslate, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnToolSelected ) );
    Connect( EventIds::ID_ToolsTranslatePivot, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnToolSelected ) );
    Connect( EventIds::ID_ToolsPivot, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnToolSelected ) );
    Connect( EventIds::ID_ToolsDuplicate, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnToolSelected ) );

    Connect( EventIds::ID_ToolsLocatorCreate, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnToolSelected ) );
    Connect( EventIds::ID_ToolsVolumeCreate, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnToolSelected ) );
    Connect( EventIds::ID_ToolsEntityCreate, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnToolSelected ) );
    Connect( EventIds::ID_ToolsCurveCreate, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnToolSelected ) );
    Connect( EventIds::ID_ToolsCurveEdit, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnToolSelected ) );

    // Help menu
    wxString caption = wxT( "About " );
    caption += wxGetApp().AppVerName().c_str();
    caption += wxT( "..." );
    wxMenuItem* aboutMenuItem = m_MenuHelp->FindItem( ID_About );
    aboutMenuItem->SetItemLabel( caption );

    // Tool Bar
    m_ToolbarPanel = new ToolbarPanel( this );
    m_FrameManager.AddPane( m_ToolbarPanel, wxAuiPaneInfo().Name( wxT( "tools" ) ).Top().Layer( 5 ).Position( 1 ).CaptionVisible( false ).PaneBorder( false ).Gripper( false ).CloseButton( false ).MaximizeButton( false ).MinimizeButton( false ).PinButton( false ).Movable( false ).MinSize( wxSize( -1, 52 ) ) );
    m_ToolbarPanel->EnableTools( false );

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

    // Vault
    m_VaultPanel = new VaultPanel( this );
    wxAuiPaneInfo vaultPanelInfo = wxAuiPaneInfo().Name( wxT( "vault" ) ).Caption( wxT( "Asset Vault" ) ).Right().Layer( 1 ).Position( 4 );
    m_FrameManager.AddPane( m_VaultPanel, vaultPanelInfo );
    //m_ExcludeFromPanelsMenu.insert( vaultPanelInfo.name );

    // Help
    m_HelpPanel = new HelpPanel( this );
    wxAuiPaneInfo helpPaneInfo = wxAuiPaneInfo().Name( wxT( "help" ) ).Caption( wxT( "Help" ) ).Left().Layer( 2 ).Position( 2 ).MinSize( 200, 200 ).BestSize( wxSize( 200, 200 ) );
    helpPaneInfo.dock_proportion = 10000;
    m_FrameManager.AddPane( m_HelpPanel, helpPaneInfo );

    // Directory
    m_DirectoryPanel = new DirectoryPanel( &m_SceneManager, &m_TreeMonitor, this );
    m_FrameManager.AddPane( m_DirectoryPanel, wxAuiPaneInfo().Name( wxT( "directory" ) ).Caption( wxT( "Directory" ) ).Left().Layer( 1 ).Position( 1 ).BestSize( wxSize( 200, 900 ) ) );

    // Properties/Layers/Types area
    m_PropertiesPanel = new PropertiesPanel( this );
    m_FrameManager.AddPane( m_PropertiesPanel, wxAuiPaneInfo().Name( wxT( "properties" ) ).Caption( wxT( "Properties" ) ).Right().Layer( 1 ).Position( 1 ) );

    m_LayersPanel = new LayersPanel( &m_SceneManager, this );
    m_FrameManager.AddPane( m_LayersPanel, wxAuiPaneInfo().Name( wxT( "layers" ) ).Caption( wxT( "Layers" ) ).Right().Layer( 1 ).Position( 2 ) );

    m_TypesPanel = new TypesPanel( &m_SceneManager, this );
    m_FrameManager.AddPane( m_TypesPanel, wxAuiPaneInfo().Name( wxT( "types" ) ).Caption( wxT( "Types" ) ).Right().Layer( 1 ).Position( 3 ) );

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

    m_MenuMRU->FromVector( wxGetApp().GetSettingsManager()->GetSettings<GeneralSettings>()->GetMRUProjects() );

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
    wxGetApp().GetSettingsManager()->GetSettings<GeneralSettings>()->SetMRUProjects( m_MenuMRU );

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

void MainFrame::SetHelpText( const tchar_t* text )
{
    m_HelpPanel->SetText( text );
}

///////////////////////////////////////////////////////////////////////////////
// Helper function for common opening code.
// 
void MainFrame::OpenProject( const Helium::Path& path )
{
    HELIUM_ASSERT( !path.empty() );

    CloseProject();

    bool isNewProject = false;
    if ( path.Exists() )
    {
        bool opened = false;

        // this is our default error
        tstring error = TXT( "We could not parse the project file you selected, it has not been loaded." );
        try
        {
            m_Project = Reflect::FromArchive< Project >( path );
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
        }
        else
        {
            m_MenuMRU->Remove( path );
            if ( !error.empty() )
            {
                wxMessageBox( error.c_str(), wxT( "Error" ), wxCENTER | wxICON_ERROR | wxOK, this );    
            }
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
        tstring error;
        bool result = m_DocumentManager.OpenDocument( new Document( m_Project->a_Path.Get() ), error );
        HELIUM_ASSERT( result );

        document = m_DocumentManager.FindDocument( m_Project->a_Path.Get() );
    }
    ConnectDocument( document );

    document->HasChanged( isNewProject );
    m_Project->ConnectDocument( document );

    m_DocumentManager.e_DocumentOpened.AddMethod( m_Project.Ptr(), &Project::OnDocumentOpened );
    m_DocumentManager.e_DocumenClosed.AddMethod( m_Project.Ptr(), &Project::OnDocumenClosed );

    m_ProjectPanel->OpenProject( m_Project, document );
    m_MenuMRU->Insert( path );

    if ( m_VaultPanel )
    {
        m_VaultPanel->SetDirectory( path );
    }

#pragma TODO( "Turn tracker back on where there are assets to track" )
    //wxGetApp().GetTracker()->SetDirectory( path );
    //if ( !wxGetApp().GetTracker()->IsThreadRunning() )
    //{
    //    wxGetApp().GetTracker()->StartThread();
    //}
}

void MainFrame::CloseProject()
{
    if ( m_Project )
    {
        m_PropertiesPanel->GetPropertiesManager().SyncThreads();

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

void MainFrame::OpenScene( const Path& path )
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
            tstring error;
            document = new Document( path );
            bool result = m_DocumentManager.OpenDocument( document, error );
            HELIUM_ASSERT( result );
        }

        if ( path.Exists() )
        {
            tstring error;
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

bool MainFrame::ValidateDrag( const Editor::DragArgs& args )
{
    bool canHandleArgs = false;

    std::set< tstring > supportedExtensions;
    Reflect::Archive::GetExtensions( supportedExtensions );
    Asset::AssetClass::GetExtensions( supportedExtensions ); 

    ClipboardFileListPtr fileList = Reflect::ObjectCast< ClipboardFileList >( args.m_ClipboardData->FromBuffer() );
    if ( fileList )
    {
        for ( std::set< tstring >::const_iterator fileItr = fileList->GetFilePaths().begin(), fileEnd = fileList->GetFilePaths().end();
            fileItr != fileEnd && !canHandleArgs;
            ++fileItr )
        {
            Path path( *fileItr );

            if ( path.Exists() )
            {
                tstring ext = path.Extension();
                if ( supportedExtensions.find( ext ) != supportedExtensions.end() )
                {
                    canHandleArgs = true;
                }
            }
        }
    }

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
        ClipboardFileListPtr fileList = Reflect::ObjectCast< ClipboardFileList >( args.m_ClipboardData->FromBuffer() );
        if ( fileList )
        {
            for ( std::set< tstring >::const_iterator fileItr = fileList->GetFilePaths().begin(),
                fileEnd = fileList->GetFilePaths().end(); fileItr != fileEnd; ++fileItr )
            {
                Path path( *fileItr );

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
    FileDialog openDlg( this, TXT( "Open" ) );

    if ( openDlg.ShowModal() == wxID_OK )
    {
        OpenProject( (const wxChar*)openDlg.GetPath().c_str() );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Overridden from base class.  Called when attempting to open a file from the
// MRU list.  Closes all currently open files before trying to open the new one.
// 
void MainFrame::OnMRUOpen( const MRUArgs& args )
{
    OpenProject( args.m_Item );
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
            GetEventHandler()->ProcessEvent( wxCommandEvent( wxEVT_COMMAND_MENU_SELECTED, wxID_SELECTALL ) );
            event.Skip( false );
            return;
            break;

        case KeyCodes::i: // ctrl-i
            GetEventHandler()->ProcessEvent( wxCommandEvent( wxEVT_COMMAND_MENU_SELECTED, ID_InvertSelection ) );
            event.Skip( false );
            return;
            break;

        case KeyCodes::o: // ctrl-o
            GetEventHandler()->ProcessEvent( wxCommandEvent( wxEVT_COMMAND_MENU_SELECTED, ID_Open ) );
            event.Skip( false );
            return;
            break;

        case KeyCodes::s: // ctrl-s
            GetEventHandler()->ProcessEvent( wxCommandEvent( wxEVT_COMMAND_MENU_SELECTED, ID_SaveAll ) );
            event.Skip( false );
            return;
            break;

        case KeyCodes::v: // ctrl-v
            GetEventHandler()->ProcessEvent( wxCommandEvent( wxEVT_COMMAND_MENU_SELECTED, wxID_PASTE ) );
            event.Skip( false );
            return;
            break;

        case KeyCodes::w: // ctrl-w
            GetEventHandler()->ProcessEvent( wxCommandEvent( wxEVT_COMMAND_MENU_SELECTED, ID_Close ) );
            event.Skip( false );
            return;
            break;

        case KeyCodes::x: // ctrl-x
            GetEventHandler()->ProcessEvent( wxCommandEvent( wxEVT_COMMAND_MENU_SELECTED, wxID_CUT ) );
            event.Skip( false );
            return;
            break;
        }
    }

    switch ( input.GetKeyCode() )
    {
    case KeyCodes::Space:
        m_ViewPanel->GetViewCanvas()->GetViewport().NextCameraMode();
        event.Skip(false);
        break;

    case KeyCodes::Up:
        GetEventHandler()->ProcessEvent( wxCommandEvent (wxEVT_COMMAND_MENU_SELECTED, EventIds::ID_EditWalkUp) );
        event.Skip(false);
        break;

    case KeyCodes::Down:
        GetEventHandler()->ProcessEvent( wxCommandEvent (wxEVT_COMMAND_MENU_SELECTED, EventIds::ID_EditWalkDown) );
        event.Skip(false);
        break;

    case KeyCodes::Right:
        GetEventHandler()->ProcessEvent( wxCommandEvent (wxEVT_COMMAND_MENU_SELECTED, EventIds::ID_EditWalkForward) );
        event.Skip(false);
        break;

    case KeyCodes::Left:
        GetEventHandler()->ProcessEvent( wxCommandEvent (wxEVT_COMMAND_MENU_SELECTED, EventIds::ID_EditWalkBackward) );
        event.Skip(false);
        break;

    case KeyCodes::Insert:
        GetEventHandler()->ProcessEvent( wxCommandEvent (wxEVT_COMMAND_MENU_SELECTED, EventIds::ID_ToolsPivot) );
        event.Skip(false);
        break;

    case KeyCodes::Delete:
        GetEventHandler()->ProcessEvent( wxCommandEvent (wxEVT_COMMAND_MENU_SELECTED, wxID_DELETE) );
        event.Skip(false);
        break;

    case KeyCodes::Escape:
        GetEventHandler()->ProcessEvent( wxCommandEvent (wxEVT_COMMAND_MENU_SELECTED, EventIds::ID_ToolsSelect) );
        event.Skip(false);
        break;

    default:
        event.Skip();
        event.ResumePropagation( wxEVENT_PROPAGATE_MAX );
        break;
    }
}

void MainFrame::OnMenuOpen( wxMenuEvent& event )
{
    const wxMenu* menu = event.GetMenu();

    const bool isProjectOpen = m_Project.ReferencesObject();
    const bool hasCurrentScene = m_SceneManager.HasCurrentScene();
    const bool hasTextClipboardData = hasCurrentScene && IsClipboardFormatAvailable( CF_TEXT );
    const bool isAnythingSelected = hasCurrentScene && m_SceneManager.GetCurrentScene()->GetSelection().GetItems().Size() > 0;

    if ( menu == m_MenuFile )
    {
        // OnMenuOpen is not called for submenus m_MenuFileNew and m_MenuFileOpenRecent
        m_MenuFileNew->Enable( ID_NewEntity, isProjectOpen );
        m_MenuFileNew->Enable( ID_NewScene, isProjectOpen );

        m_MenuMRU->PopulateMenu( m_MenuFileOpenRecent );

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

///////////////////////////////////////////////////////////////////////////////
// Returns a different name each time this function is called so that scenes
// can be uniquely named.
// 
static void GetUniquePathName( const tchar_t* root, const tchar_t* extension, const std::set< Path >& paths, Helium::Path& name )
{
    int32_t number = 0;

    do
    {
        tostringstream strm;
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

void MainFrame::OnNewScene( wxCommandEvent& event )
{
    HELIUM_ASSERT( m_Project );

    m_PropertiesPanel->GetPropertiesManager().SyncThreads();

    ScenePtr currentScene = m_SceneManager.GetCurrentScene();
    if ( currentScene.ReferencesObject() )
    {
        currentScene->d_ResolveScene.Clear();
        currentScene->d_ReleaseScene.Clear();
    }

    Helium::Path path;
    GetUniquePathName( TXT( "New Scene" ), TXT( ".scene.hrb" ), m_Project->Paths(), path );

    // Add to the project before opening it
    m_Project->AddPath( path );

    DocumentPtr document = new Document( path );
    document->HasChanged( true );

    tstring error;
    bool result = m_DocumentManager.OpenDocument( document, error );
    HELIUM_ASSERT( result );

    ScenePtr scene = m_SceneManager.NewScene( &m_ViewPanel->GetViewCanvas()->GetViewport(), document );
    HELIUM_ASSERT( scene.ReferencesObject() );

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
    FileDialog newProjectDialog( this, TXT( "Select New Project Location" ), wxEmptyString, TXT( "New Project.project.hrb" ), TXT( "*.project.hrb" ), FileDialogStyles::Open );

    if ( newProjectDialog.ShowModal() == wxID_OK )
    {
        OpenProject( (const wxChar*)newProjectDialog.GetPath().c_str() );
    }
}

bool MainFrame::DoOpen( const tstring& path )
{
#pragma TODO( "Rachel WIP: "__FUNCTION__" - This should be opening and closing Projects rather than Scenes." )
#pragma TODO( "Rachel WIP: "__FUNCTION__" - We will need to handle opening/loading Scenes from the projectView" )
    //bool opened = false;
    //Helium::Path nocPath( path );
    //if ( !path.empty() && nocPath.Exists() )
    //{
    //    m_PropertiesPanel->GetPropertiesManager().SyncThreads();

    //    if ( m_DocumentManager.CloseAll() )
    //    {
    //        tstring error;

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
    tstring error;
    if ( !m_DocumentManager.SaveAll( error ) )
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
        wxAuiPaneInfo& pane = m_FrameManager.GetPane( m_VaultPanel );
        if ( pane.IsOk() )
        {
            pane.Show( !pane.IsShown() );
            m_FrameManager.Update();
        }
    }

    if ( !queryString.empty() )
    {
        wxAuiPaneInfo& pane = m_FrameManager.GetPane( m_VaultPanel );
        pane.caption = wxT( "Vault Search: " );
        pane.caption += queryString;
    }

    m_VaultPanel->Search( queryString.wx_str() );
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
        Undo::BatchCommandPtr batch = new Undo::BatchCommand ();

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
                batch->Push( m_SceneManager.GetCurrentScene()->SetGeometryShown( true, true ) );
                batch->Push( m_SceneManager.GetCurrentScene()->SetGeometryShown( true, false ) );
                break;
            }

        case EventIds::ID_ViewShowSelected:
            {
                batch->Push( m_SceneManager.GetCurrentScene()->SetHiddenSelected( false ) );
                break;
            }

        case EventIds::ID_ViewShowSelectedGeometry:
            {
                batch->Push( m_SceneManager.GetCurrentScene()->SetGeometryShown( true, true ) );
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
                batch->Push( m_SceneManager.GetCurrentScene()->SetGeometryShown( false, true ) );
                batch->Push( m_SceneManager.GetCurrentScene()->SetGeometryShown( false, false ) );
                break;
            }

        case EventIds::ID_ViewHideSelected:
            {
                batch->Push( m_SceneManager.GetCurrentScene()->SetHiddenSelected( true ) );
                break;
            }

        case EventIds::ID_ViewHideSelectedGeometry:
            {
                batch->Push( m_SceneManager.GetCurrentScene()->SetGeometryShown( false, true ) );
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
        wxGetApp().GetSettingsManager()->GetSettings< ViewportSettings >()->SetColorMode( ( ViewColorMode )( newColorModeItr->second ) );
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
            uint32_t flags = ImportFlags::Select;
            if ( update )
            {
                flags |= ImportFlags::Merge;
            }

            switch ( event.GetId() )
            {
            case EventIds::ID_FileImport:
                {
                    FileDialog fileDialog( this, TXT( "Import" ) );

                    std::set< tstring > filters;
                    Reflect::Archive::GetFileFilters( filters );
                    for ( std::set< tstring >::const_iterator itr = filters.begin(), end = filters.end(); itr != end; ++itr )
                    {
                        fileDialog.AddFilter( (*itr) );
                    }

                    if ( fileDialog.ShowModal() != wxID_OK )
                    {
                        return;
                    }

                    Helium::Path path( (const wxChar*)fileDialog.GetPath().c_str() );
                    currentScene->Push( currentScene->Import( path, ImportActions::Import, flags, currentScene->GetRoot() ) );
                    break;
                }

            case EventIds::ID_FileImportFromClipboard:
                {
                    tstring xml;
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
            ExportArgs args;

            if ( exportHierarchy )
            {
                args.m_Flags |= ExportFlags::MaintainHierarchy;
            }

            if ( exportDependencies )
            {
                args.m_Flags |= ExportFlags::MaintainDependencies;
            }

            args.m_Flags |= ExportFlags::SelectedNodes;

            uint64_t startTimer = Helium::TimerGetClock();

            SetCursor( wxCursor( wxCURSOR_WAIT ) );

            {
                tostringstream str;
                str << "Preparing to export";
                SceneStatusChanged( str.str() );
            }

            Undo::BatchCommandPtr changes = new Undo::BatchCommand();

            std::vector< Reflect::ElementPtr > elements;
            bool result = m_SceneManager.GetCurrentScene()->Export( elements, args, changes );
            if ( result && !elements.empty() )
            {
                switch ( event.GetId() )
                {
                case EventIds::ID_FileExport:
                    {
                        FileDialog fileDialog( this, TXT( "Export Selection" ), TXT( "" ), TXT( "" ), wxFileSelectorDefaultWildcardStr, FileDialogStyles::DefaultSave );

                        std::set< tstring > filters;
                        Reflect::Archive::GetFileFilters( filters );
                        for ( std::set< tstring >::const_iterator itr = filters.begin(), end = filters.end(); itr != end; ++itr )
                        {
                            fileDialog.AddFilter( (*itr) );
                        }

                        if ( fileDialog.ShowModal() != wxID_OK )
                        {
                            return;
                        }

                        tstring file = fileDialog.GetPath();

                        try
                        {
                            Reflect::ArchivePtr archive = Reflect::GetArchive( file );
                            archive->e_Status.AddMethod( m_SceneManager.GetCurrentScene(), &Scene::ArchiveStatus );
                            archive->d_Exception.Set( m_SceneManager.GetCurrentScene(), &Scene::ArchiveException );
                            archive->Put( elements );
                            archive->Close();
                        }
                        catch ( Helium::Exception& ex )
                        {
                            tostringstream str;
                            str << "Failed to generate file '" << file << "': " << ex.What();
                            wxMessageBox( str.str(), wxT( "Error" ), wxOK|wxCENTRE|wxICON_ERROR );
                            result = false;
                        }

                        break;
                    }

                case EventIds::ID_FileExportToClipboard:
                    {
                        tstring xml;

                        try
                        {
                            Reflect::ArchiveXML::ToString( elements, xml );
                        }
                        catch ( Helium::Exception& ex )
                        {
                            tostringstream str;
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
                tostringstream str;
                str.precision( 2 );
                str << "Export Complete: " << std::fixed << Helium::CyclesToMillis( Helium::TimerGetClock() - startTimer ) / 1000.f << " seconds...";
                SceneStatusChanged( str.str() );
            }
        }
    }
}

void MainFrame::CurrentSceneChanged( const SceneChangeArgs& args )
{
    if ( args.m_PreviousScene )
    {
        m_ProjectPanel->SetActive( args.m_PreviousScene->GetPath(), false );
    }

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

        // Iterate over the node types looking for the layer node type
        HM_StrToSceneNodeTypeSmartPtr::const_iterator nodeTypeItr = args.m_Scene->GetNodeTypesByName().begin();
        HM_StrToSceneNodeTypeSmartPtr::const_iterator nodeTypeEnd = args.m_Scene->GetNodeTypesByName().end();
        for ( ; nodeTypeItr != nodeTypeEnd; ++nodeTypeItr )
        {
            const SceneNodeTypePtr& nodeType = nodeTypeItr->second;
            if ( nodeType->GetInstanceClass()->HasType( Reflect::GetType< SceneGraph::Layer >() ) )
            {
                // Now that we have the layer node type, iterate over all the layer instances and
                // add them to the layer grid UI.
                HM_SceneNodeSmartPtr::const_iterator instItr = nodeTypeItr->second->GetInstances().begin();
                HM_SceneNodeSmartPtr::const_iterator instEnd = nodeTypeItr->second->GetInstances().end();

                //Begin batching
                m_LayersPanel->BeginBatch();

                for ( ; instItr != instEnd; ++instItr )
                {
                    const SceneNodePtr& dependNode    = instItr->second;
                    SceneGraph::Layer*        lunaLayer     = Reflect::AssertCast< SceneGraph::Layer >( dependNode );
                    m_LayersPanel->AddLayer( lunaLayer );
                }

                //End batching
                m_LayersPanel->EndBatch();
            } 
            else if ( nodeType->HasType( Reflect::GetType< SceneGraph::HierarchyNodeType >() ) )
            {
                // Hierarchy node types need to be added to the object grid UI.
                SceneGraph::HierarchyNodeType* hierarchyNodeType = Reflect::AssertCast< SceneGraph::HierarchyNodeType >( nodeTypeItr->second );
                m_TypesPanel->AddType( hierarchyNodeType );
            }
        }

#pragma TODO( "Change the selection or display changes in the Project view" )

        m_ProjectPanel->SetActive( args.m_Scene->GetPath(), true );

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
    if (m_SceneManager.HasCurrentScene())
    {
        switch (event.GetId())
        {
        case EventIds::ID_ToolsSelect:
            {
                m_SceneManager.GetCurrentScene()->SetTool(NULL);
                break;
            }

        case EventIds::ID_ToolsScale:
            {
                m_SceneManager.GetCurrentScene()->SetTool(new SceneGraph::ScaleManipulator( m_SettingsManager, ManipulatorModes::Scale, m_SceneManager.GetCurrentScene(), &m_ToolbarPanel->GetPropertiesGenerator()));
                break;
            }

        case EventIds::ID_ToolsScalePivot:
            {
                m_SceneManager.GetCurrentScene()->SetTool(new SceneGraph::TranslateManipulator( m_SettingsManager, ManipulatorModes::ScalePivot, m_SceneManager.GetCurrentScene(), &m_ToolbarPanel->GetPropertiesGenerator()));
                break;
            }

        case EventIds::ID_ToolsRotate:
            {
                m_SceneManager.GetCurrentScene()->SetTool(new SceneGraph::RotateManipulator( m_SettingsManager, ManipulatorModes::Rotate, m_SceneManager.GetCurrentScene(), &m_ToolbarPanel->GetPropertiesGenerator()));
                break;
            }

        case EventIds::ID_ToolsRotatePivot:
            {
                m_SceneManager.GetCurrentScene()->SetTool(new SceneGraph::TranslateManipulator( m_SettingsManager, ManipulatorModes::RotatePivot, m_SceneManager.GetCurrentScene(), &m_ToolbarPanel->GetPropertiesGenerator()));
                break;
            }

        case EventIds::ID_ToolsTranslate:
            {
                m_SceneManager.GetCurrentScene()->SetTool(new SceneGraph::TranslateManipulator( m_SettingsManager, ManipulatorModes::Translate, m_SceneManager.GetCurrentScene(), &m_ToolbarPanel->GetPropertiesGenerator()));
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
                    if ( m_SceneManager.GetCurrentScene()->GetTool()->GetType() == Reflect::GetType< SceneGraph::ScaleManipulator >() )
                    {
                        m_SceneManager.GetCurrentScene()->SetTool(new SceneGraph::TranslateManipulator( m_SettingsManager, ManipulatorModes::ScalePivot, m_SceneManager.GetCurrentScene(), &m_ToolbarPanel->GetPropertiesGenerator()));
                    }
                    else if ( m_SceneManager.GetCurrentScene()->GetTool()->GetType() == Reflect::GetType< SceneGraph::RotateManipulator >() )
                    {
                        m_SceneManager.GetCurrentScene()->SetTool(new SceneGraph::TranslateManipulator( m_SettingsManager, ManipulatorModes::RotatePivot, m_SceneManager.GetCurrentScene(), &m_ToolbarPanel->GetPropertiesGenerator()));
                    }
                    else if ( m_SceneManager.GetCurrentScene()->GetTool()->GetType() == Reflect::GetType< SceneGraph::TranslateManipulator >() )
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
                            }
                        }
                    }
                }
                break;
            }

        case EventIds::ID_ToolsDuplicate:
            {
                m_SceneManager.GetCurrentScene()->SetTool(new SceneGraph::DuplicateTool(m_SceneManager.GetCurrentScene(), &m_ToolbarPanel->GetPropertiesGenerator()));
            }
            break;

        case EventIds::ID_ToolsLocatorCreate:
            {
                m_SceneManager.GetCurrentScene()->SetTool(new SceneGraph::LocatorCreateTool(m_SceneManager.GetCurrentScene(), &m_ToolbarPanel->GetPropertiesGenerator()));
            }
            break;

        case EventIds::ID_ToolsVolumeCreate:
            {
                m_SceneManager.GetCurrentScene()->SetTool(new SceneGraph::VolumeCreateTool(m_SceneManager.GetCurrentScene(), &m_ToolbarPanel->GetPropertiesGenerator()));
            }
            break;

        case EventIds::ID_ToolsEntityCreate:
            {
                m_SceneManager.GetCurrentScene()->SetTool(new SceneGraph::EntityInstanceCreateTool(m_SceneManager.GetCurrentScene(), &m_ToolbarPanel->GetPropertiesGenerator()));
            }
            break;

        case EventIds::ID_ToolsCurveCreate:
            {
                m_SceneManager.GetCurrentScene()->SetTool( new SceneGraph::CurveCreateTool( m_SceneManager.GetCurrentScene(), &m_ToolbarPanel->GetPropertiesGenerator() ) );
            }
            break;

        case EventIds::ID_ToolsCurveEdit:
            {
                SceneGraph::CurveEditTool* curveEditTool = new SceneGraph::CurveEditTool( m_SettingsManager, m_SceneManager.GetCurrentScene(), &m_ToolbarPanel->GetPropertiesGenerator() );
                m_SceneManager.GetCurrentScene()->SetTool( curveEditTool );
                curveEditTool->StoreSelectedCurves();
            }
            break;
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
    else
    {
        GetStatusBar()->SetStatusText( TXT( "You must create a new scene or open an existing scene to use a tool" ) );
    }
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
    int32_t selectedTool = EventIds::ID_ToolsSelect;
    if ( args.m_NewTool )
    {
        if ( args.m_NewTool->HasType( Reflect::GetType< SceneGraph::TransformManipulator >() ) )
        {
            SceneGraph::TransformManipulator* manipulator = Reflect::DangerousCast< SceneGraph::TransformManipulator >( args.m_NewTool );
            switch ( manipulator->GetMode() )
            {
            case ManipulatorModes::Scale:
                selectedTool = EventIds::ID_ToolsScale;
                break;

            case ManipulatorModes::ScalePivot:
                selectedTool = EventIds::ID_ToolsScalePivot;
                break;

            case ManipulatorModes::Rotate:
                selectedTool = EventIds::ID_ToolsRotate;
                break;

            case ManipulatorModes::RotatePivot:
                selectedTool = EventIds::ID_ToolsRotatePivot;
                break;

            case ManipulatorModes::Translate:
                selectedTool = EventIds::ID_ToolsTranslate;
                break;

            case ManipulatorModes::TranslatePivot:
                selectedTool = EventIds::ID_ToolsTranslatePivot;
                break;
            }
        }
        else if ( args.m_NewTool->GetType() == Reflect::GetType< SceneGraph::EntityInstanceCreateTool >() )
        {
            selectedTool = EventIds::ID_ToolsEntityCreate;
        }
        else if ( args.m_NewTool->GetType() == Reflect::GetType< SceneGraph::VolumeCreateTool >() )
        {
            selectedTool = EventIds::ID_ToolsVolumeCreate;
        }
        else if ( args.m_NewTool->GetType() == Reflect::GetType< SceneGraph::LocatorCreateTool >() )
        {
            selectedTool = EventIds::ID_ToolsLocatorCreate;
        }
        else if ( args.m_NewTool->GetType() == Reflect::GetType< SceneGraph::DuplicateTool >() )
        {
            selectedTool = EventIds::ID_ToolsDuplicate;
        }
        else if ( args.m_NewTool->GetType() == Reflect::GetType< SceneGraph::CurveCreateTool >() )
        {
            selectedTool = EventIds::ID_ToolsCurveCreate;
        }
        else if ( args.m_NewTool->GetType() == Reflect::GetType< SceneGraph::CurveEditTool >() )
        {
            selectedTool = EventIds::ID_ToolsCurveEdit;
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

    HM_SceneNodeDumbPtr::const_iterator itr = m_SceneManager.GetCurrentScene()->GetNodes().begin();
    HM_SceneNodeDumbPtr::const_iterator end = m_SceneManager.GetCurrentScene()->GetNodes().end();
    for ( ; itr != end; ++itr )
    {
        SceneGraph::SceneNode* sceneNode = itr->second;
        if ( sceneNode->HasType( Reflect::GetType< SceneGraph::HierarchyNode >() ) )
        {
            selection.Append( sceneNode );
        }
    }

    m_SceneManager.GetCurrentScene()->Push( m_SceneManager.GetCurrentScene()->GetSelection().SetItems( selection ) );
}

static void RecurseToggleSelection( SceneGraph::HierarchyNode* node, const OS_SceneNodeDumbPtr& oldSelection, OS_SceneNodeDumbPtr& newSelection )
{
    for ( OS_HierarchyNodeDumbPtr::Iterator itr = node->GetChildren().Begin(), end = node->GetChildren().End(); itr != end; ++itr )
    {
        SceneGraph::HierarchyNode* child = *itr;
        RecurseToggleSelection( child, oldSelection, newSelection );
    }

    bool found = false;
    OS_SceneNodeDumbPtr::Iterator selItr = oldSelection.Begin();
    OS_SceneNodeDumbPtr::Iterator selEnd = oldSelection.End();
    for ( ; selItr != selEnd && !found; ++selItr )
    {
        SceneGraph::HierarchyNode* current = Reflect::ObjectCast< SceneGraph::HierarchyNode >( *selItr );
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

void MainFrame::OnInvertSelection(wxCommandEvent& event)
{
    if ( m_SceneManager.HasCurrentScene() )
    {
        const OS_SceneNodeDumbPtr& selection = m_SceneManager.GetCurrentScene()->GetSelection().GetItems();
        if ( selection.Size() > 0 )
        {
            OS_SceneNodeDumbPtr newSelection;
            RecurseToggleSelection( m_SceneManager.GetCurrentScene()->GetRoot(), selection, newSelection );
            m_SceneManager.GetCurrentScene()->Push( m_SceneManager.GetCurrentScene()->GetSelection().SetItems( newSelection ) );
        }
    }
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
    if ( m_SceneManager.HasCurrentScene() )
    {
        V_Matrix4 transforms;
        m_SceneManager.GetCurrentScene()->GetSelectedTransforms(transforms);

        Helium::StrongPtr<Reflect::Matrix4StlVectorData> data = new Reflect::Matrix4StlVectorData();
        data->m_Data.Set( transforms );

        tstring xml;
        data->ToXML( xml );

        if ( wxTheClipboard->Open() )
        {
            wxTheClipboard->SetData( new wxTextDataObject( xml ) );
            wxTheClipboard->Close();
        }
    }
}

void MainFrame::OnPasteTransform(wxCommandEvent& event)
{
    if ( m_SceneManager.HasCurrentScene() )
    {
        tstring xml;
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

        std::vector< Reflect::ElementPtr > elements;
        Reflect::ArchiveXML::FromString( xml, elements );

        std::vector< Reflect::ElementPtr >::const_iterator itr = elements.begin();
        std::vector< Reflect::ElementPtr >::const_iterator end = elements.end();
        for ( ; itr != end; ++itr )
        {
            Helium::StrongPtr<Reflect::Matrix4StlVectorData> data = Reflect::ObjectCast< Reflect::Matrix4StlVectorData >( *itr );
            if ( data.ReferencesObject() )
            {
                m_SceneManager.GetCurrentScene()->Push( m_SceneManager.GetCurrentScene()->SetSelectedTransforms(data->m_Data.Get()) );
                break;
            }
        }
    }
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

///////////////////////////////////////////////////////////////////////////////
//Opens the context menu with items separated by type
//
void MainFrame::OnTypeContextMenu(wxCommandEvent &event)
{
    ContextCallbackData* data = static_cast<ContextCallbackData*>(event.m_callbackUserData);
    OS_SceneNodeDumbPtr newSelection;

    switch( data->m_ContextCallbackType )
    {
    case ContextCallbackTypes::All:
        {
            const HM_SceneNodeSmartPtr& instances( data->m_NodeType->GetInstances() );

            if( !instances.empty() )
            {    
                HM_SceneNodeSmartPtr::const_iterator itr = instances.begin();
                HM_SceneNodeSmartPtr::const_iterator end = instances.end();
                for( ; itr != end; ++itr )
                {
                    newSelection.Append( itr->second );
                }              
            }
            break;
        }

    case ContextCallbackTypes::Item:
        {
            newSelection.Append( static_cast< SceneGraph::HierarchyNode* >( data->m_Nodes ) );
            break;
        }

    case ContextCallbackTypes::Instance:
        {
            const S_InstanceDumbPtr& instances( data->m_InstanceSet->GetInstances() );

            S_InstanceDumbPtr::const_iterator itr = instances.begin();
            S_InstanceDumbPtr::const_iterator end = instances.end();

            for( ; itr != end; ++itr )
            {
                newSelection.Append( *itr );
            }

            break;
        }
    }

    if( !newSelection.Empty() )
    {
        m_SceneManager.GetCurrentScene()->Push( m_SceneManager.GetCurrentScene()->GetSelection().SetItems( newSelection ) );
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
        tstring xml;
        if ( !scene->ExportXML( xml, ExportFlags::Default | ExportFlags::SelectedNodes ) )
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
    tstring xml;
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
        Undo::BatchCommandPtr batch = new Undo::BatchCommand ();

        // Import the data as children of the paste root
        batch->Push( scene->ImportXML( xml, ImportFlags::Select ) );

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
        case SelectionModes::Type:
            {
                OpenTypeContextMenu( args );
                break;
            }

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

    OS_SceneNodeDumbPtr selectableItems;
    V_PickHitSmartPtr::const_iterator itr = args.m_Pick->GetHits().begin();
    V_PickHitSmartPtr::const_iterator end = args.m_Pick->GetHits().end();
    for ( ; itr != end; ++itr )
    {
        SceneNode* node = Reflect::ObjectCast<SceneNode>((*itr)->GetHitObject());
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
            OS_SceneNodeDumbPtr::Iterator itr = selectableItems.Begin();
            OS_SceneNodeDumbPtr::Iterator end = selectableItems.End();
            for( ; itr != end; ++itr)
            {
                SceneNode* node = *itr;

                if( node->IsSelectable() )
                {
                    SceneGraph::HierarchyNode* hierarchyNode = Reflect::ObjectCast< SceneGraph::HierarchyNode >( node );
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
                tstring str = (*itr)->GetName();

                tstring desc = (*itr)->GetDescription();

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
// Opens up a context menu that allows selection basd on type
//
void MainFrame::OpenTypeContextMenu( const SelectArgs& args )
{
    wxMenu contextMenu;
    uint32_t numMenuItems = 0;

    // simple select functionality
    if (m_SceneManager.GetCurrentScene()->HasHighlighted())
    {
        // need to provide the select args if needed
        DataObject<const SelectArgs*>* data = new DataObject<const SelectArgs*> ( &args );
        GetEventHandler()->Connect( EventIds::ID_SelectContextMenu + numMenuItems, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::SelectItemInScene ), data, this );
        contextMenu.Append( EventIds::ID_SelectContextMenu + numMenuItems, TXT( "Select" ) );
        ++numMenuItems;
    }

    if (!m_SceneManager.GetCurrentScene()->GetSelection().GetItems().Empty())
    {
        GetEventHandler()->Connect( EventIds::ID_SelectContextMenu + numMenuItems, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::SelectSimilarItemsInScene ), NULL, this );
        contextMenu.Append( EventIds::ID_SelectContextMenu + numMenuItems, TXT( "Select Similar" ) );
        ++numMenuItems;
    }

    if (numMenuItems > 0)
    {
        contextMenu.AppendSeparator();
    }

    // populate this with the appropriate types
    const HM_StrToSceneNodeTypeSmartPtr& sceneNodeTypes = m_SceneManager.GetCurrentScene()->GetNodeTypesByName();

    if( !sceneNodeTypes.empty() )
    {   
        SetupTypeContextMenu( sceneNodeTypes, contextMenu, numMenuItems );
    }

    // set the current event handler on the context menu and pop it up
    contextMenu.SetEventHandler( GetEventHandler() );
    PopupMenu( &contextMenu );

    // this must be done piecemeal because the range version of Disconnect() will only disconnect the ranged version Connect()
    for ( uint32_t i = EventIds::ID_SelectContextMenu; i < EventIds::ID_SelectContextMenu + numMenuItems; i++ )
    {
        // clean up, disconnect any id that was set up for any of the items
        GetEventHandler()->Disconnect( i, wxEVT_COMMAND_MENU_SELECTED );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Populates the context menu with selections from the various types
//
void MainFrame::SetupTypeContextMenu( const HM_StrToSceneNodeTypeSmartPtr& sceneNodeTypes, wxMenu& contextMenu, uint32_t& numMenuItems )
{
    V_SceneNodeTypeDumbPtr orderedTypes;

    {
        HM_StrToSceneNodeTypeSmartPtr::const_iterator itr = sceneNodeTypes.begin();
        HM_StrToSceneNodeTypeSmartPtr::const_iterator end = sceneNodeTypes.end();

        for( ;itr != end ;++itr )
        {
            orderedTypes.push_back( itr->second );
        }
    }

    V_SceneNodeTypeDumbPtr::iterator itr = orderedTypes.begin();
    V_SceneNodeTypeDumbPtr::iterator end = orderedTypes.end();

    std::sort( itr, end, MainFrame::SortTypeItemsByName );

    // iterate over the scene node types, making a new sub menu for each
    for( ; itr != end; ++itr )
    {
        const SceneGraph::SceneNodeType* type( *itr );
        const HM_SceneNodeSmartPtr& typeInstances( type->GetInstances() );

        if( !typeInstances.empty() )
        {
            wxMenu* subMenu = new wxMenu;

            // add selection for all items
            ContextCallbackData* data = new ContextCallbackData;
            data->m_ContextCallbackType = ContextCallbackTypes::All;
            data->m_NodeType = type;

            GetEventHandler()->Connect( EventIds::ID_SelectContextMenu + numMenuItems, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnTypeContextMenu ), data, this );
            subMenu->Append( EventIds::ID_SelectContextMenu + numMenuItems, TXT( "Select All" ) );
            ++numMenuItems;

            // add selection for individual items
            {   
                wxMenu* itemMenu = new wxMenu;

                HM_SceneNodeSmartPtr::const_iterator inst_itr = typeInstances.begin();
                HM_SceneNodeSmartPtr::const_iterator inst_end = typeInstances.end();

                V_SceneNodeDumbPtr ordered;

                // push the instances into a vector for sorting purposes
                for( ; inst_itr != inst_end; ++inst_itr )
                {
                    ordered.push_back( inst_itr->second );
                }  

                V_SceneNodeDumbPtr::iterator ord_itr = ordered.begin();
                V_SceneNodeDumbPtr::iterator ord_end = ordered.end();

                std::sort( ord_itr, ord_end, SortContextItemsByName );

                // setting up the menu item for each of the items
                for( ;ord_itr != ord_end; ++ord_itr )
                {    
                    ContextCallbackData* data = new ContextCallbackData;
                    data->m_ContextCallbackType = ContextCallbackTypes::Item;
                    data->m_Nodes = *ord_itr;

                    GetEventHandler()->Connect( EventIds::ID_SelectContextMenu + numMenuItems, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnTypeContextMenu ), data, this );
                    itemMenu->Append( EventIds::ID_SelectContextMenu + numMenuItems, (*ord_itr)->GetName().c_str() );
                    ++numMenuItems;
                }

                // add the items menu to the sub menu
                subMenu->Append( EventIds::ID_SelectContextMenu + numMenuItems, TXT( "Select Single" ), itemMenu );
                ++numMenuItems;

                // if this is an entity, then we need to check if it has art classes
                const SceneGraph::EntityInstanceType* entity = Reflect::ConstObjectCast< SceneGraph::EntityInstanceType >( type );

                // if this is an instance, then we need to check if it has code classes
                const SceneGraph::InstanceType* instance = Reflect::ConstObjectCast< SceneGraph::InstanceType >( type );

                if (entity)
                {
                    // set up for entity types
                    SetupEntityTypeMenus( entity, subMenu, numMenuItems );
                }
            }
            contextMenu.Append( EventIds::ID_SelectContextMenu + numMenuItems, type->GetName().c_str(), subMenu );
            ++numMenuItems;
        }
    }
}

void MainFrame::SetupEntityTypeMenus( const SceneGraph::EntityInstanceType* entity, wxMenu* subMenu, uint32_t& numMenuItems )
{
    const M_InstanceSetSmartPtr& sets = entity->GetSets();

    if( !sets.empty() )
    {
        bool added = false;

        wxMenu* menu = new wxMenu;

        M_InstanceSetSmartPtr::const_iterator itr = sets.begin();
        M_InstanceSetSmartPtr::const_iterator end = sets.end();
        for( ;itr != end; ++itr )
        {
            const SceneGraph::EntitySet* art = Reflect::ObjectCast< SceneGraph::EntitySet >( itr->second );
            if (art && art->GetEntity() && !art->GetEntity()->GetSourcePath().empty())
            {
                ContextCallbackData* data = new ContextCallbackData;
                data->m_ContextCallbackType = ContextCallbackTypes::Instance;
                data->m_InstanceSet = art;

                GetEventHandler()->Connect( EventIds::ID_SelectContextMenu + numMenuItems, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnTypeContextMenu ), data, this );
                menu->Append( EventIds::ID_SelectContextMenu + numMenuItems, art->GetEntity()->GetSourcePath().c_str() );
                ++numMenuItems;
                added = true;
            }
        }

        if (added)
        {
            subMenu->AppendSeparator();
            subMenu->Append( EventIds::ID_SelectContextMenu + numMenuItems, TXT( "Select All With Art Class" ), menu );
            ++numMenuItems;
        }
        else
        {
            delete menu;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Static function used to sort context items by name
bool MainFrame::SortContextItemsByName( SceneGraph::SceneNode* lhs, SceneGraph::SceneNode* rhs )
{
    tstring lname( lhs->GetName() );
    tstring rname( rhs->GetName() );

    toUpper( lname );
    toUpper( rname );

    return lname < rname;
}

///////////////////////////////////////////////////////////////////////////////
// Static function used to sort type items by name
bool MainFrame::SortTypeItemsByName( SceneGraph::SceneNodeType* lhs, SceneGraph::SceneNodeType* rhs )
{
    tstring lname( lhs->GetName() );
    tstring rname( rhs->GetName() );

    toUpper( lname );
    toUpper( rname );

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

        tstring error;
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
