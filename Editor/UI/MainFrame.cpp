#include "Precompile.h"

#include "MainFrame.h"

#include "Foundation/Reflect/ArchiveXML.h"

#include "Pipeline/Content/ContentVersion.h"
#include "Pipeline/Content/Scene.h"

#include "Application/UI/FileDialog.h"
#include "Application/Inspect/DragDrop/ClipboardFileList.h"
#include "Application/Inspect/DragDrop/ClipboardDataObject.h"

#include "Editor/Scene/Scene.h"
#include "Editor/Scene/InstanceSet.h"
#include "Editor/Scene/EntityType.h"
#include "Editor/Scene/EntityAssetSet.h"

#include "Editor/Scene/TransformManipulator.h"

#include "Editor/Scene/CurveCreateTool.h"
#include "Editor/Scene/CurveEditTool.h"
#include "Editor/Scene/DuplicateTool.h"
#include "Editor/Scene/EntityCreateTool.h"
#include "Editor/Scene/LocatorCreateTool.h"
#include "Editor/Scene/NavMeshCreateTool.h"
#include "Editor/Scene/VolumeCreateTool.h"

#include "Editor/Scene/ScaleManipulator.h"
#include "Editor/Scene/TranslateManipulator.h"

#include "Editor/UI/PreferencesDialog.h"

#include "EditorIDs.h"
#include "ArtProvider.h"
#include "ImportOptionsDlg.h"
#include "ExportOptionsDlg.h"
#include "Editor/App.h"

using namespace Helium;
using namespace Helium::Core;
using namespace Helium::Editor;

tstring MainFrame::s_PreferencesPrefix = TXT( "MainFrame" );

///////////////////////////////////////////////////////////////////////////////
// Wraps up a pointer to an Editor::Scene so that it can be stored in the combo box that
// is used for selecting the current scene.  Each item in the combo box stores 
// the scene that it refers to.
// 
class SceneSelectData : public wxClientData
{
public:
    Editor::Scene* m_Scene;

    SceneSelectData( Editor::Scene* scene )
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
    const Editor::SceneNodeType* m_NodeType;
    const Editor::InstanceSet* m_InstanceSet;
    Editor::SceneNode* m_Nodes;
};


MainFrame::MainFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style )
: MainFrameGenerated( parent, id, title, pos, size, style )
, m_MRU( new Helium::MenuMRU( 30, this ) )
, m_TreeMonitor( &m_SceneManager )
, m_TreeSortTimer( &m_TreeMonitor )
, m_MessageDisplayer( this )
, m_SceneManager( MessageSignature::Delegate( &m_MessageDisplayer, &MessageDisplayer::DisplayMessage ) )
{
    SetLabel( TXT("Helium Editor") );

    //
    // Dynamic Menu Generation
    //
    Connect( wxEVT_MENU_OPEN, wxMenuEventHandler( MainFrame::OnMenuOpen ) );

    //
    // General Events
    //
    Connect( wxID_CLOSE, wxEVT_CLOSE_WINDOW, wxCloseEventHandler( MainFrame::OnExiting ) );
    Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( MainFrame::OnExiting ) );

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
    Connect( EventIds::ID_ToolsNavMesh, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnToolSelected ) );

    //
    // Tools
    //
    m_ToolbarPanel = new ToolbarPanel( this );
    m_ToolEnumerator = new PropertiesGenerator( &m_ToolProperties );
    m_ToolPropertiesManager = new PropertiesManager( m_ToolEnumerator );
    m_ToolPropertiesManager->AddPropertiesCreatedListener( PropertiesCreatedSignature::Delegate( this, &MainFrame::OnPropertiesCreated ) );
    m_ToolProperties.SetControl( new Inspect::CanvasWindow ( m_ToolbarPanel->GetToolsPropertiesPanel(), EventIds::ID_ToolProperties, wxPoint(0,0), wxSize(250,250), wxNO_BORDER | wxCLIP_CHILDREN) );
    m_FrameManager.AddPane( m_ToolbarPanel, wxAuiPaneInfo().Name( wxT( "tools" ) ).Top().Layer( 5 ).Position( 1 ).CaptionVisible( false ).PaneBorder( false ).Gripper( false ).CloseButton( false ).MaximizeButton( false ).MinimizeButton( false ).PinButton( false ).Movable( false ).MinSize( wxSize( -1, 52 ) ) );
    m_ToolbarPanel->GetToolsPanel()->Disable();
    m_ToolbarPanel->GetToolsPanel()->Refresh();

    //
    // View panel area
    //
    m_ViewPanel = new ViewPanel( this );
    m_ViewPanel->GetViewport()->AddRenderListener( RenderSignature::Delegate ( this, &MainFrame::Render ) );
    m_ViewPanel->GetViewport()->AddSelectListener( SelectSignature::Delegate ( this, &MainFrame::Select ) ); 
    m_ViewPanel->GetViewport()->AddSetHighlightListener( SetHighlightSignature::Delegate ( this, &MainFrame::SetHighlight ) );
    m_ViewPanel->GetViewport()->AddClearHighlightListener( ClearHighlightSignature::Delegate ( this, &MainFrame::ClearHighlight ) );
    m_ViewPanel->GetViewport()->AddToolChangedListener( ToolChangeSignature::Delegate ( this, &MainFrame::ViewToolChanged ) );
    m_FrameManager.AddPane( m_ViewPanel, wxAuiPaneInfo().Name( wxT( "view" ) ).CenterPane() );

    //
    // Project/Help area
    //
    m_ProjectPanel = new ProjectPanel( this );
    wxAuiPaneInfo projectPaneInfo = wxAuiPaneInfo().Name( wxT( "project" ) ).Caption( wxT( "Project" ) ).Left().Layer( 2 ).Position( 1 ).BestSize( 200, 700 );
    projectPaneInfo.dock_proportion = 30000;
    m_FrameManager.AddPane( m_ProjectPanel, projectPaneInfo );

    m_HelpPanel = new HelpPanel( this );
    wxAuiPaneInfo helpPaneInfo = wxAuiPaneInfo().Name( wxT( "help" ) ).Caption( wxT( "Help" ) ).Left().Layer( 2 ).Position( 2 ).MinSize( 200, 200 ).BestSize( wxSize( 200, 200 ) );
    helpPaneInfo.dock_proportion = 10000;
    m_FrameManager.AddPane( m_HelpPanel, helpPaneInfo );

    //
    // Directory area
    //
    m_DirectoryPanel = new DirectoryPanel( &m_SceneManager, &m_TreeMonitor, this );
    m_FrameManager.AddPane( m_DirectoryPanel, wxAuiPaneInfo().Name( wxT( "directory" ) ).Caption( wxT( "Directory" ) ).Left().Layer( 1 ).Position( 1 ).BestSize( wxSize( 200, 900 ) ) );

    //
    // Properties/Layers/Types area
    //
    m_SelectionEnumerator = new PropertiesGenerator( &m_SelectionProperties );
    m_SelectionPropertiesManager = new PropertiesManager( m_SelectionEnumerator );
    m_SelectionPropertiesManager->AddPropertiesCreatedListener( PropertiesCreatedSignature::Delegate( this, &MainFrame::OnPropertiesCreated ) );
    m_PropertiesPanel = new PropertiesPanel( m_SelectionPropertiesManager, this );
    m_SelectionProperties.SetControl( m_PropertiesPanel->GetPropertiesCanvas() );
    m_FrameManager.AddPane( m_PropertiesPanel, wxAuiPaneInfo().Name( wxT( "properties" ) ).Caption( wxT( "Properties" ) ).Right().Layer( 1 ).Position( 1 ) );

    m_LayersPanel = new LayersPanel( &m_SceneManager, this );
    m_FrameManager.AddPane( m_LayersPanel, wxAuiPaneInfo().Name( wxT( "layers" ) ).Caption( wxT( "Layers" ) ).Right().Layer( 1 ).Position( 2 ) );

    m_TypesPanel = new TypesPanel( &m_SceneManager, this );
    m_FrameManager.AddPane( m_TypesPanel, wxAuiPaneInfo().Name( wxT( "types" ) ).Caption( wxT( "Types" ) ).Right().Layer( 1 ).Position( 3 ) );

    m_FrameManager.Update();

    CreatePanelsMenu( m_MenuPanels );

    //
    // Restore layout if any
    //

    wxGetApp().GetPreferences()->GetScenePreferences()->GetWindowSettings()->ApplyToWindow( this, &m_FrameManager, true );
    wxGetApp().GetPreferences()->GetViewportPreferences()->ApplyToViewport( m_ViewPanel->GetViewport() ); 

    //
    // Attach event handlers
    //

    m_SceneManager.AddCurrentSceneChangingListener( SceneChangeSignature::Delegate (this, &MainFrame::CurrentSceneChanging) );
    m_SceneManager.AddCurrentSceneChangedListener( SceneChangeSignature::Delegate (this, &MainFrame::CurrentSceneChanged) );
    m_SceneManager.AddSceneAddedListener( SceneChangeSignature::Delegate( this, &MainFrame::SceneAdded ) );
    m_SceneManager.AddSceneRemovingListener( SceneChangeSignature::Delegate( this, &MainFrame::SceneRemoving ) );

    m_MRU->AddItemSelectedListener( Helium::MRUSignature::Delegate( this, &MainFrame::OnMRUOpen ) );

#pragma TODO("MRU")
#if 0
    std::vector< tstring > paths;
    std::vector< tstring >::const_iterator itr = wxGetApp().GetPreferences()->GetMRU()->GetPaths().begin();
    std::vector< tstring >::const_iterator end = wxGetApp().GetPreferences()->GetMRU()->GetPaths().end();
    for ( ; itr != end; ++itr )
    {
        Helium::Path path( *itr );
        if ( path.Exists() )
        {
            paths.push_back( *itr );
        }
    }
    m_MRU->FromVector( paths );
#endif

    Inspect::DropTarget* dropTarget = new Inspect::DropTarget();
    dropTarget->SetDragOverCallback( Inspect::DragOverCallback::Delegate( this, &MainFrame::DragOver ) );
    dropTarget->SetDropCallback( Inspect::DropCallback::Delegate( this, &MainFrame::Drop ) );
    m_ViewPanel->GetViewport()->SetDropTarget( dropTarget );

}

MainFrame::~MainFrame()
{
    // Remove any straggling document listeners
    OS_DocumentSmartPtr::Iterator docItr = m_SceneManager.GetDocuments().Begin();
    OS_DocumentSmartPtr::Iterator docEnd = m_SceneManager.GetDocuments().End();
    for ( ; docItr != docEnd; ++docItr )
    {
        ( *docItr )->RemoveDocumentModifiedListener( DocumentChangedSignature::Delegate( this, &MainFrame::DocumentModified ) );
        ( *docItr )->RemoveDocumentSavedListener( DocumentChangedSignature::Delegate( this, &MainFrame::DocumentModified ) );
        ( *docItr )->RemoveDocumentClosedListener( DocumentChangedSignature::Delegate( this, &MainFrame::DocumentModified ) );
    }

    // Save preferences and MRU
#pragma TODO("MRU")
#if 0
    std::vector< tstring > mruPaths;
    m_MRU->ToVector( mruPaths );
    wxGetApp().GetPreferences()->GetScenePreferences()->GetMRU()->SetPaths( mruPaths );
#endif
    wxGetApp().GetPreferences()->GetViewportPreferences()->LoadFromViewport( m_ViewPanel->GetViewport() ); 
    wxGetApp().SavePreferences();

    //
    // Detach event handlers
    //

    m_SceneManager.RemoveCurrentSceneChangingListener( SceneChangeSignature::Delegate (this, &MainFrame::CurrentSceneChanging) );
    m_SceneManager.RemoveCurrentSceneChangedListener( SceneChangeSignature::Delegate (this, &MainFrame::CurrentSceneChanged) );
    m_SceneManager.RemoveSceneAddedListener( SceneChangeSignature::Delegate( this, &MainFrame::SceneAdded ) );
    m_SceneManager.RemoveSceneRemovingListener( SceneChangeSignature::Delegate( this, &MainFrame::SceneRemoving ) );

    m_MRU->RemoveItemSelectedListener( Helium::MRUSignature::Delegate( this, &MainFrame::OnMRUOpen ) );

    m_SelectionPropertiesManager->RemovePropertiesCreatedListener( PropertiesCreatedSignature::Delegate( this, &MainFrame::OnPropertiesCreated ) );
    m_ToolPropertiesManager->RemovePropertiesCreatedListener( PropertiesCreatedSignature::Delegate( this, &MainFrame::OnPropertiesCreated ) );

    m_ViewPanel->GetViewport()->RemoveRenderListener( RenderSignature::Delegate ( this, &MainFrame::Render ) );
    m_ViewPanel->GetViewport()->RemoveSelectListener( SelectSignature::Delegate ( this, &MainFrame::Select ) ); 
    m_ViewPanel->GetViewport()->RemoveSetHighlightListener( SetHighlightSignature::Delegate ( this, &MainFrame::SetHighlight ) );
    m_ViewPanel->GetViewport()->RemoveClearHighlightListener( ClearHighlightSignature::Delegate ( this, &MainFrame::ClearHighlight ) );
    m_ViewPanel->GetViewport()->RemoveToolChangedListener( ToolChangeSignature::Delegate ( this, &MainFrame::ViewToolChanged ) );

#pragma TODO( "We shouldn't really have to do these if we clean up how some of our objects reference each other" )
    m_DirectoryPanel->Destroy();
    m_LayersPanel->Destroy();
}

void MainFrame::SetHelpText( const tchar* text )
{
    m_HelpPanel->SetText( text );
}

///////////////////////////////////////////////////////////////////////////////
// Helper function for common opening code.
// 
bool MainFrame::OpenProject( const Helium::Path& path )
{
    bool opened = false;

    if ( !path.empty() && path.Exists() )
    {
        tstring error;
        try
        {
            m_Project = Reflect::Archive::FromFile< Core::Project >( path );
        }
        catch ( const Helium::Exception& ex )
        {
            error = ex.What();
        }

        opened = m_Project.ReferencesObject();

        if ( opened )
        {
            m_MRU->Insert( path );

            m_ProjectPanel->SetProject( m_Project );
        }
        else
        {
            m_MRU->Remove( path );
            if ( !error.empty() )
            {
                wxMessageBox( error.c_str(), wxT( "Error" ), wxCENTER | wxICON_ERROR | wxOK, this );
            }
        }
    }

    return opened;
}

bool MainFrame::AddScene( const Helium::Path& path )
{
    bool opened = false;

    if ( !path.empty() && path.Exists() )
    {
        if ( m_SceneManager.CloseAll() )
        {
            tstring error;

            try
            {
                opened = m_SceneManager.OpenPath( path, error ) != NULL;
            }
            catch ( const Helium::Exception& ex )
            {
                error = ex.What();
            }
        }
    }

    return opened;
}

bool MainFrame::ValidateDrag( const Inspect::DragArgs& args )
{
    bool canHandleArgs = false;

    std::set< tstring > reflectExtensions;
    Reflect::Archive::GetExtensions( reflectExtensions );

    Inspect::ClipboardFileListPtr fileList = Reflect::ObjectCast< Inspect::ClipboardFileList >( args.m_ClipboardData->FromBuffer() );
    if ( fileList )
    {
        for ( std::set< tstring >::const_iterator fileItr = fileList->GetFilePaths().begin(), fileEnd = fileList->GetFilePaths().end();
            fileItr != fileEnd && !canHandleArgs;
            ++fileItr )
        {
            Helium::Path path( *fileItr );

            if ( path.Exists() )
            {
                tstring ext = path.Extension();
                if ( reflectExtensions.find( ext ) != reflectExtensions.end() )
                {
                    canHandleArgs = true;
                }
            }
        }
    }

    return canHandleArgs;
}

wxDragResult MainFrame::DragOver( const Inspect::DragArgs& args )
{
    wxDragResult result = args.m_Default;

    if ( !ValidateDrag( args ) )
    {
        result = wxDragNone;
    }

    return result;
}

wxDragResult MainFrame::Drop( const Inspect::DragArgs& args )
{
    wxDragResult result = args.m_Default;

    if ( ValidateDrag( args ) )
    {
        Inspect::ClipboardFileListPtr fileList = Reflect::ObjectCast< Inspect::ClipboardFileList >( args.m_ClipboardData->FromBuffer() );
        if ( fileList )
        {
            for ( std::set< tstring >::const_iterator fileItr = fileList->GetFilePaths().begin(),
                fileEnd = fileList->GetFilePaths().end(); fileItr != fileEnd; ++fileItr )
            {
                Helium::Path path( *fileItr );

#pragma TODO( "Load the files" )
            }
        }
    }

    return result;
}

void MainFrame::SceneAdded( const SceneChangeArgs& args )
{
    if ( !m_SceneManager.IsNestedScene( args.m_Scene ) )
    {
        // Only listen to zone and world files.
        args.m_Scene->AddStatusChangedListener( SceneStatusChangeSignature::Delegate( this, &MainFrame::SceneStatusChanged ) );
        args.m_Scene->AddSceneContextChangedListener( SceneContextChangedSignature::Delegate( this, &MainFrame::SceneContextChanged ) );
        args.m_Scene->AddLoadFinishedListener( LoadSignature::Delegate( this, & MainFrame::SceneLoadFinished ) );

        m_SelectionEnumerator->AddPopulateLinkListener( Inspect::PopulateLinkSignature::Delegate (args.m_Scene, &Editor::Scene::PopulateLink));

        Document* document = args.m_Scene->GetSceneDocument();
        document->AddDocumentModifiedListener( DocumentChangedSignature::Delegate( this, &MainFrame::DocumentModified ) );
        document->AddDocumentSavedListener( DocumentChangedSignature::Delegate( this, &MainFrame::DocumentModified ) );
        document->AddDocumentClosedListener( DocumentChangedSignature::Delegate( this, &MainFrame::DocumentModified ) );
    }
}

void MainFrame::SceneRemoving( const SceneChangeArgs& args )
{
    args.m_Scene->RemoveStatusChangedListener( SceneStatusChangeSignature::Delegate ( this, &MainFrame::SceneStatusChanged ) );
    args.m_Scene->RemoveSceneContextChangedListener( SceneContextChangedSignature::Delegate ( this, &MainFrame::SceneContextChanged ) );
    args.m_Scene->RemoveLoadFinishedListener( LoadSignature::Delegate( this, & MainFrame::SceneLoadFinished ) );

    m_SelectionEnumerator->RemovePopulateLinkListener( Inspect::PopulateLinkSignature::Delegate (args.m_Scene, &Editor::Scene::PopulateLink));

    m_ViewPanel->GetViewport()->Refresh();

    if ( m_SceneManager.IsRoot( args.m_Scene ) )
    {
        m_OutlinerStates.clear();
    }
    else
    {
        m_OutlinerStates.erase( args.m_Scene );
    }
}

void MainFrame::SceneLoadFinished( const LoadArgs& args )
{
    m_ViewPanel->GetViewport()->Refresh();
    DocumentModified( DocumentChangedArgs( args.m_Scene->GetSceneDocument() ) );
}

///////////////////////////////////////////////////////////////////////////////
// Overridden from base class.  Called when attempting to open a file from the
// MRU list.  Closes all currently open files before trying to open the new one.
// 
void MainFrame::OnMRUOpen( const Helium::MRUArgs& args )
{
    DoOpen( args.m_Item );
}

void MainFrame::OnEraseBackground(wxEraseEvent& event)
{
    event.Skip();
}

void MainFrame::OnSize(wxSizeEvent& event)
{
    event.Skip();
}

void MainFrame::OnChar(wxKeyEvent& event)
{
    switch (event.GetKeyCode())
    {
    case WXK_SPACE:
        m_ViewPanel->GetViewport()->NextCameraMode();
        event.Skip(false);
        break;

    case WXK_UP:
        GetEventHandler()->ProcessEvent( wxCommandEvent (wxEVT_COMMAND_MENU_SELECTED, EventIds::ID_EditWalkUp) );
        event.Skip(false);
        break;

    case WXK_DOWN:
        GetEventHandler()->ProcessEvent( wxCommandEvent (wxEVT_COMMAND_MENU_SELECTED, EventIds::ID_EditWalkDown) );
        event.Skip(false);
        break;

    case WXK_RIGHT:
        GetEventHandler()->ProcessEvent( wxCommandEvent (wxEVT_COMMAND_MENU_SELECTED, EventIds::ID_EditWalkForward) );
        event.Skip(false);
        break;

    case WXK_LEFT:
        GetEventHandler()->ProcessEvent( wxCommandEvent (wxEVT_COMMAND_MENU_SELECTED, EventIds::ID_EditWalkBackward) );
        event.Skip(false);
        break;

    case WXK_INSERT:
        GetEventHandler()->ProcessEvent( wxCommandEvent (wxEVT_COMMAND_MENU_SELECTED, EventIds::ID_ToolsPivot) );
        event.Skip(false);
        break;

    case WXK_DELETE:
        GetEventHandler()->ProcessEvent( wxCommandEvent (wxEVT_COMMAND_MENU_SELECTED, wxID_DELETE) );
        event.Skip(false);
        break;

    case WXK_ESCAPE:
        GetEventHandler()->ProcessEvent( wxCommandEvent (wxEVT_COMMAND_MENU_SELECTED, EventIds::ID_ToolsSelect) );
        event.Skip(false);
        break;

    default:
        event.Skip();
        break;
    }

    if (event.GetSkipped())
    {
        switch ( event.GetKeyCode() )
        {
        case wxT('4'):
            GetEventHandler()->ProcessEvent( wxCommandEvent (wxEVT_COMMAND_MENU_SELECTED, EventIds::ID_ViewWireframe) );
            event.Skip(false);
            break;

        case wxT('5'):
            GetEventHandler()->ProcessEvent( wxCommandEvent (wxEVT_COMMAND_MENU_SELECTED, EventIds::ID_ViewMaterial) );
            event.Skip(false);
            break;

        case wxT('6'):
            GetEventHandler()->ProcessEvent( wxCommandEvent (wxEVT_COMMAND_MENU_SELECTED, EventIds::ID_ViewTexture) );
            event.Skip(false);
            break;

        case wxT('7'):
            GetEventHandler()->ProcessEvent( wxCommandEvent (wxEVT_COMMAND_MENU_SELECTED, EventIds::ID_ViewOrbit) );
            event.Skip(false);
            break;

        case wxT('8'):
            GetEventHandler()->ProcessEvent( wxCommandEvent (wxEVT_COMMAND_MENU_SELECTED, EventIds::ID_ViewFront) );
            event.Skip(false);
            break;

        case wxT('9'):
            GetEventHandler()->ProcessEvent( wxCommandEvent (wxEVT_COMMAND_MENU_SELECTED, EventIds::ID_ViewSide) );
            event.Skip(false);
            break;

        case wxT('0'):
            GetEventHandler()->ProcessEvent( wxCommandEvent (wxEVT_COMMAND_MENU_SELECTED, EventIds::ID_ViewTop) );
            event.Skip(false);
            break;

        case wxT('Q'):
            GetEventHandler()->ProcessEvent( wxCommandEvent (wxEVT_COMMAND_MENU_SELECTED, EventIds::ID_ToolsSelect) );
            event.Skip(false);
            break;

        case wxT('W'):
            GetEventHandler()->ProcessEvent( wxCommandEvent (wxEVT_COMMAND_MENU_SELECTED, EventIds::ID_ToolsTranslate) );
            event.Skip(false);
            break;

        case wxT('E'):
            GetEventHandler()->ProcessEvent( wxCommandEvent (wxEVT_COMMAND_MENU_SELECTED, EventIds::ID_ToolsRotate) );
            event.Skip(false);
            break;

        case wxT('R'):
            GetEventHandler()->ProcessEvent( wxCommandEvent (wxEVT_COMMAND_MENU_SELECTED, EventIds::ID_ToolsScale) );
            event.Skip(false);
            break;

        case wxT('O'):
            GetEventHandler()->ProcessEvent( wxCommandEvent (wxEVT_COMMAND_MENU_SELECTED, EventIds::ID_ViewFrameOrigin) );
            event.Skip(false);
            break;

        case wxT('F'):
            GetEventHandler()->ProcessEvent( wxCommandEvent (wxEVT_COMMAND_MENU_SELECTED, EventIds::ID_ViewFrameSelected) );
            event.Skip(false);
            break;

        case wxT('H'):
            GetEventHandler()->ProcessEvent( wxCommandEvent (wxEVT_COMMAND_MENU_SELECTED, EventIds::ID_ViewHighlightMode) );
            event.Skip(false);
            break;

        case wxT(']'):
            GetEventHandler()->ProcessEvent( wxCommandEvent ( wxEVT_COMMAND_MENU_SELECTED, EventIds::ID_ViewNextView) );
            event.Skip(false);
            break;

        case wxT('['):
            GetEventHandler()->ProcessEvent( wxCommandEvent ( wxEVT_COMMAND_MENU_SELECTED, EventIds::ID_ViewPreviousView) );
            event.Skip(false);
            break;

        default:
            event.Skip();
            break;
        }
    }
}

void MainFrame::OnShow(wxShowEvent& event)
{
#ifdef EDITOR_DEBUG_RUNTIME_DATA_SELECTION
    // Sometimes it's handy to put debug code here for program start up.
    New();
    wxCommandEvent evt( wxEVT_COMMAND_TOOL_CLICKED, EventIds::ID_ToolsVolumeCreate );
    GetEventHandler()->ProcessEvent( evt );
    m_SceneManager.GetCurrentScene()->SetTool(NULL);
    wxCloseEvent close( wxEVT_CLOSE_WINDOW );
    GetEventHandler()->AddPendingEvent( close );
#endif

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

    event.Skip();
}

void MainFrame::OnMenuOpen( wxMenuEvent& event )
{
    const wxMenu* menu = event.GetMenu();

    if ( menu == m_MenuFile )
    {
        // File->Import is enabled if there is a current editing scene
        m_MenuFile->Enable( ID_Import, m_SceneManager.HasCurrentScene() );
        m_MenuFile->Enable( ID_ImportFromClipboard, m_SceneManager.HasCurrentScene() );

        // File->Export is only enabled if there is something selected
        const bool enableExport = m_SceneManager.HasCurrentScene() && m_SceneManager.GetCurrentScene()->GetSelection().GetItems().Size() > 0;
        m_MenuFile->Enable( ID_Export, enableExport );
        m_MenuFile->Enable( ID_ExportToClipboard, enableExport );

        m_MRU->PopulateMenu( m_MenuFileOpenRecent );
    }
    else if ( menu == m_MenuPanels )
    {
        UpdatePanelsMenu( m_MenuPanels );
    }
    else if ( menu == m_MenuEdit )
    {
        // Edit->Undo/Redo is only enabled if there are commands in the queue
        const bool canUndo = m_SceneManager.HasCurrentScene() && m_SceneManager.CanUndo();
        const bool canRedo = m_SceneManager.HasCurrentScene() && m_SceneManager.CanRedo();
        m_MenuEdit->Enable( wxID_UNDO, canUndo );
        m_MenuEdit->Enable( wxID_REDO, canRedo );

        // Edit->Invert Selection is only enabled if something is selected
        const bool isAnythingSelected = m_SceneManager.HasCurrentScene() && m_SceneManager.GetCurrentScene()->GetSelection().GetItems().Size() > 0;
        m_MenuEdit->Enable( ID_InvertSelection, isAnythingSelected );

        // Cut/copy/paste
        m_MenuEdit->Enable( wxID_CUT, isAnythingSelected );
        m_MenuEdit->Enable( wxID_COPY, isAnythingSelected );
        m_MenuEdit->Enable( wxID_PASTE, m_SceneManager.HasCurrentScene() && IsClipboardFormatAvailable( CF_TEXT ) );
    }
    else
    {
        event.Skip();
    }
}

void MainFrame::OnNewScene( wxCommandEvent& event )
{
    if ( m_SceneManager.CloseAll() )
    {
        ScenePtr scene = m_SceneManager.NewScene( m_ViewPanel->GetViewport(), true );
        scene->GetSceneDocument()->SetModified( true );
        m_SceneManager.SetCurrentScene( scene );
    }
}

void MainFrame::OnNewEntity( wxCommandEvent& event )
{
    wxMessageBox( wxT( "Not supported yet." ), wxT( "Error" ), wxOK|wxICON_ERROR );
}

void MainFrame::OnNewProject( wxCommandEvent& event )
{
    m_Project = new Project ();
    m_Project->Path().Set( TXT("New Project") );
    m_ProjectPanel->SetProject( m_Project );
}

bool MainFrame::DoOpen( const tstring& path )
{
    bool opened = false;
    Helium::Path nocPath( path );
    if ( !path.empty() && nocPath.Exists() )
    {
        if ( m_SceneManager.CloseAll() )
        {
            tstring error;

            try
            {
                opened = m_SceneManager.OpenPath( path, error ) != NULL;
            }
            catch ( const Helium::Exception& ex )
            {
                error = ex.What();
            }

            if ( opened )
            {
                m_MRU->Insert( path );
            }
            else
            {
                m_MRU->Remove( path );
                if ( !error.empty() )
                {
                    wxMessageBox( error.c_str(), wxT( "Error" ), wxCENTER | wxICON_ERROR | wxOK, this );
                }
            }
        }
    }
    return opened;
}

void MainFrame::OnOpen( wxCommandEvent& event )
{
    Helium::FileDialog openDlg( this, TXT( "Open" ) );

    if ( openDlg.ShowModal() == wxID_OK )
    {
        OpenProject( (const wxChar*)openDlg.GetPath().c_str() );
    }
}

void MainFrame::OnClose( wxCommandEvent& event )
{
    m_SceneManager.CloseAll();
    m_Project = NULL;
}

void MainFrame::OnSaveAll( wxCommandEvent& event )
{
    tstring error;
    if ( !m_SceneManager.SaveAll( error ) )
    {
        wxMessageBox( error.c_str(), wxT( "Error" ), wxCENTER | wxICON_ERROR | wxOK, this );
    }
}

void MainFrame::OnViewChange(wxCommandEvent& event)
{
    switch (event.GetId())
    {
    case EventIds::ID_ViewAxes:
        {
            m_ViewPanel->GetViewport()->SetAxesVisible( !m_ViewPanel->GetViewport()->IsAxesVisible() );
            break;
        }

    case EventIds::ID_ViewGrid:
        {
            m_ViewPanel->GetViewport()->SetGridVisible( !m_ViewPanel->GetViewport()->IsGridVisible() );
            break;
        }

    case EventIds::ID_ViewBounds:
        {
            m_ViewPanel->GetViewport()->SetBoundsVisible( !m_ViewPanel->GetViewport()->IsBoundsVisible() );
            break;
        }

    case EventIds::ID_ViewStatistics:
        {
            m_ViewPanel->GetViewport()->SetStatisticsVisible( !m_ViewPanel->GetViewport()->IsStatisticsVisible() );
            break;
        }

    case EventIds::ID_ViewNone:
        {
            m_ViewPanel->GetViewport()->SetGeometryMode( GeometryModes::None );
            break;
        }

    case EventIds::ID_ViewRender:
        {
            m_ViewPanel->GetViewport()->SetGeometryMode( GeometryModes::Render );
            break;
        }

    case EventIds::ID_ViewCollision:
        {
            m_ViewPanel->GetViewport()->SetGeometryMode( GeometryModes::Collision );
            break;
        }

    case EventIds::ID_ViewWireframeOnMesh:
        {
            m_ViewPanel->GetViewport()->GetCamera()->SetWireframeOnMesh( !m_ViewPanel->GetViewport()->GetCamera()->GetWireframeOnMesh() );
            break;
        }

    case EventIds::ID_ViewWireframeOnShaded:
        {
            m_ViewPanel->GetViewport()->GetCamera()->SetWireframeOnShaded( !m_ViewPanel->GetViewport()->GetCamera()->GetWireframeOnShaded() );
            break;
        }

    case EventIds::ID_ViewWireframe:
        {
            m_ViewPanel->GetViewport()->GetCamera()->SetShadingMode( ShadingModes::Wireframe );
            break;
        }

    case EventIds::ID_ViewMaterial:
        {
            m_ViewPanel->GetViewport()->GetCamera()->SetShadingMode( ShadingModes::Material );
            break;
        }

    case EventIds::ID_ViewTexture:
        {
            m_ViewPanel->GetViewport()->GetCamera()->SetShadingMode( ShadingModes::Texture );
            break;
        }

    case EventIds::ID_ViewFrustumCulling:
        {
            m_ViewPanel->GetViewport()->GetCamera()->SetViewFrustumCulling( !m_ViewPanel->GetViewport()->GetCamera()->IsViewFrustumCulling() );
            break;
        }

    case EventIds::ID_ViewBackfaceCulling:
        {
            m_ViewPanel->GetViewport()->GetCamera()->SetBackFaceCulling( !m_ViewPanel->GetViewport()->GetCamera()->IsBackFaceCulling() );
            break;
        }
    }

    m_ViewPanel->GetViewport()->Refresh();
}

void MainFrame::OnViewCameraChange(wxCommandEvent& event)
{
    switch (event.GetId())
    {
    case EventIds::ID_ViewOrbit:
        {
            m_ViewPanel->GetViewport()->SetCameraMode(CameraModes::Orbit);
            break;
        }

    case EventIds::ID_ViewFront:
        {
            m_ViewPanel->GetViewport()->SetCameraMode(CameraModes::Front);
            break;
        }

    case EventIds::ID_ViewSide:
        {
            m_ViewPanel->GetViewport()->SetCameraMode(CameraModes::Side);
            break;
        }

    case EventIds::ID_ViewTop:
        {
            m_ViewPanel->GetViewport()->SetCameraMode(CameraModes::Top);
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
    const ViewColorMode previousColorMode = wxGetApp().GetPreferences()->GetViewportPreferences()->GetColorMode();

    const M_IDToColorMode::const_iterator newColorModeItr = m_ColorModeLookup.find( event.GetId() );
    if ( newColorModeItr != m_ColorModeLookup.end() )
    {
        wxGetApp().GetPreferences()->GetViewportPreferences()->SetColorMode( ( ViewColorMode )( newColorModeItr->second ) );
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
        Editor::Scene* currentScene = m_SceneManager.GetCurrentScene();

        ImportOptionsDlg dlg( this, update );

        if ( dlg.ShowModal() == wxID_OK && currentScene->IsEditable() )
        {
            u32 flags = ImportFlags::Select;
            if ( update )
            {
                flags |= ImportFlags::Merge;
            }

            switch ( event.GetId() )
            {
            case EventIds::ID_FileImport:
                {
                    Helium::FileDialog fileDialog( this, TXT( "Import" ) );

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

                    currentScene->Push( currentScene->ImportFile( (const wxChar*)fileDialog.GetPath().c_str(), ImportActions::Import, flags, currentScene->GetRoot() ) );
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
        ExportOptionsDlg dlg ( this, exportDependencies, exportHierarchy );

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

            u64 startTimer = Helium::TimerGetClock();

            SetCursor( wxCursor( wxCURSOR_WAIT ) );

            {
                tostringstream str;
                str << "Preparing to export";
                SceneStatusChanged( str.str() );
            }

            Undo::BatchCommandPtr changes = new Undo::BatchCommand();

            Reflect::V_Element elements;
            bool result = m_SceneManager.GetCurrentScene()->Export( elements, args, changes );
            if ( result && !elements.empty() )
            {
                switch ( event.GetId() )
                {
                case EventIds::ID_FileExport:
                    {
                        Helium::FileDialog fileDialog( this, TXT( "Export Selection" ), TXT( "" ), TXT( "" ), wxFileSelectorDefaultWildcardStr, Helium::FileDialogStyles::DefaultSave );

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
                            Reflect::Archive::ToFile( elements, file.c_str(), new Content::ContentVersion (), m_SceneManager.GetCurrentScene() );
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
                            Reflect::ArchiveXML::ToString( elements, xml, m_SceneManager.GetCurrentScene() );
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
    if ( args.m_Scene )
    {
        m_ToolbarPanel->GetToolsPanel()->Enable();
        m_ToolbarPanel->GetToolsPanel()->Refresh();

        // Hook our event handlers
        args.m_Scene->AddStatusChangedListener( SceneStatusChangeSignature::Delegate ( this, &MainFrame::SceneStatusChanged ) );
        args.m_Scene->AddSceneContextChangedListener( SceneContextChangedSignature::Delegate ( this, &MainFrame::SceneContextChanged ) );
        args.m_Scene->AddExecutedListener( ExecuteSignature::Delegate ( this, &MainFrame::Executed ) );

        // Selection event handlers
        args.m_Scene->AddSelectionChangedListener( SelectionChangedSignature::Delegate ( this, &MainFrame::SelectionChanged ) );

        // These events are emitted from the attribute editor and cause execution of the scene to occur, and interactive goodness
        m_SelectionEnumerator->AddPropertyChangingListener( Inspect::ChangingSignature::Delegate (args.m_Scene, &Editor::Scene::PropertyChanging));
        m_SelectionEnumerator->AddPropertyChangedListener( Inspect::ChangedSignature::Delegate (args.m_Scene, &Editor::Scene::PropertyChanged));
        m_SelectionEnumerator->AddPickLinkListener( Inspect::PickLinkSignature::Delegate (args.m_Scene, &Editor::Scene::PickLink));
        m_SelectionEnumerator->AddSelectLinkListener( Inspect::SelectLinkSignature::Delegate (args.m_Scene, &Editor::Scene::SelectLink));

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
            if ( Reflect::Registry::GetInstance()->GetClass( nodeType->GetInstanceType() )->HasType( Reflect::GetType<Editor::Layer>() ) )
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
                    Editor::Layer*        lunaLayer     = Reflect::AssertCast< Editor::Layer >( dependNode );
                    m_LayersPanel->AddLayer( lunaLayer );
                }

                //End batching
                m_LayersPanel->EndBatch();
            } 
            else if ( nodeType->HasType( Reflect::GetType<Editor::HierarchyNodeType>() ) )
            {
                // Hierarchy node types need to be added to the object grid UI.
                Editor::HierarchyNodeType* hierarchyNodeType = Reflect::AssertCast< Editor::HierarchyNodeType >( nodeTypeItr->second );
                m_TypesPanel->AddType( hierarchyNodeType );
            }
        }

        // Restore selection-sensitive settings
        args.m_Scene->RefreshSelection();

        // Restore tool
        if (args.m_Scene->GetTool().ReferencesObject())
        {
            // Restore tool to the view from the scene
            m_ViewPanel->GetViewport()->SetTool(args.m_Scene->GetTool());

            // Restore tool attributes
            args.m_Scene->GetTool()->CreateProperties();

            // Layout ui
            m_ToolProperties.Layout();

            // Read state
            m_ToolProperties.Read();
        }
    }
}

void MainFrame::CurrentSceneChanging( const SceneChangeArgs& args )
{
    if ( !args.m_Scene )
    {
        return;
    }

    // Unhook our event handlers
    args.m_Scene->RemoveStatusChangedListener( SceneStatusChangeSignature::Delegate ( this, &MainFrame::SceneStatusChanged ) );
    args.m_Scene->RemoveSceneContextChangedListener( SceneContextChangedSignature::Delegate ( this, &MainFrame::SceneContextChanged ) );
    args.m_Scene->RemoveExecutedListener( ExecuteSignature::Delegate ( this, &MainFrame::Executed ) );

    // Selection event handlers
    args.m_Scene->RemoveSelectionChangedListener( SelectionChangedSignature::Delegate ( this, &MainFrame::SelectionChanged ) );

    // Remove attribute listeners
    m_SelectionEnumerator->RemovePropertyChangingListener( Inspect::ChangingSignature::Delegate (args.m_Scene, &Editor::Scene::PropertyChanging));
    m_SelectionEnumerator->RemovePropertyChangedListener( Inspect::ChangedSignature::Delegate (args.m_Scene, &Editor::Scene::PropertyChanged));
    m_SelectionEnumerator->RemovePickLinkListener( Inspect::PickLinkSignature::Delegate (args.m_Scene, &Editor::Scene::PickLink));
    m_SelectionEnumerator->RemoveSelectLinkListener( Inspect::SelectLinkSignature::Delegate (args.m_Scene, &Editor::Scene::SelectLink));

    // If we were editing a scene, save the outliner info before changing to the new one.
    OutlinerStates* stateInfo = &m_OutlinerStates.insert( M_OutlinerStates::value_type( args.m_Scene, OutlinerStates() ) ).first->second;
    m_DirectoryPanel->SaveState( stateInfo->m_Hierarchy, stateInfo->m_Entities, stateInfo->m_Types );

    // Clear the selection attribute canvas
    m_SelectionProperties.Clear();

    // Clear the tool attribute canavs
    m_ToolProperties.Clear();

    // Release the tool from the VIEW and Scene, saving the tool in the scene isn't a desirable behavior and the way it is currently
    // implimented it will cause a crash under certain scenarios (see trac #1322)
    args.m_Scene->SetTool( NULL );
    m_ViewPanel->GetViewport()->SetTool( NULL );

    m_ToolbarPanel->GetToolsPanel()->Disable();
    m_ToolbarPanel->GetToolsPanel()->Refresh();
}

void MainFrame::OnPropertiesCreated( const PropertiesCreatedArgs& args )
{
    PostCommand( new PropertiesCreatedCommand( args.m_PropertiesManager, args.m_SelectionId, args.m_Controls ) );
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
                m_SceneManager.GetCurrentScene()->SetTool(new Editor::ScaleManipulator (ManipulatorModes::Scale, m_SceneManager.GetCurrentScene(), m_ToolEnumerator));
                break;
            }

        case EventIds::ID_ToolsScalePivot:
            {
                m_SceneManager.GetCurrentScene()->SetTool(new Editor::TranslateManipulator (ManipulatorModes::ScalePivot, m_SceneManager.GetCurrentScene(), m_ToolEnumerator));
                break;
            }

        case EventIds::ID_ToolsRotate:
            {
                m_SceneManager.GetCurrentScene()->SetTool(new Editor::RotateManipulator (ManipulatorModes::Rotate, m_SceneManager.GetCurrentScene(), m_ToolEnumerator));
                break;
            }

        case EventIds::ID_ToolsRotatePivot:
            {
                m_SceneManager.GetCurrentScene()->SetTool(new Editor::TranslateManipulator (ManipulatorModes::RotatePivot, m_SceneManager.GetCurrentScene(), m_ToolEnumerator));
                break;
            }

        case EventIds::ID_ToolsTranslate:
            {
                m_SceneManager.GetCurrentScene()->SetTool(new Editor::TranslateManipulator (ManipulatorModes::Translate, m_SceneManager.GetCurrentScene(), m_ToolEnumerator));
                break;
            }

        case EventIds::ID_ToolsTranslatePivot:
            {
                m_SceneManager.GetCurrentScene()->SetTool(new Editor::TranslateManipulator (ManipulatorModes::TranslatePivot, m_SceneManager.GetCurrentScene(), m_ToolEnumerator));
                break;
            }

        case EventIds::ID_ToolsPivot:
            {
                if (m_SceneManager.GetCurrentScene()->GetTool().ReferencesObject())
                {
                    if ( m_SceneManager.GetCurrentScene()->GetTool()->GetType() == Reflect::GetType<Editor::ScaleManipulator>() )
                    {
                        m_SceneManager.GetCurrentScene()->SetTool(new Editor::TranslateManipulator (ManipulatorModes::ScalePivot, m_SceneManager.GetCurrentScene(), m_ToolEnumerator));
                    }
                    else if ( m_SceneManager.GetCurrentScene()->GetTool()->GetType() == Reflect::GetType<Editor::RotateManipulator>() )
                    {
                        m_SceneManager.GetCurrentScene()->SetTool(new Editor::TranslateManipulator (ManipulatorModes::RotatePivot, m_SceneManager.GetCurrentScene(), m_ToolEnumerator));
                    }
                    else if ( m_SceneManager.GetCurrentScene()->GetTool()->GetType() == Reflect::GetType<Editor::TranslateManipulator>() )
                    {
                        Editor::TranslateManipulator* manipulator = Reflect::AssertCast< Editor::TranslateManipulator > (m_SceneManager.GetCurrentScene()->GetTool());

                        if ( manipulator->GetMode() == ManipulatorModes::Translate)
                        {
                            m_SceneManager.GetCurrentScene()->SetTool(new Editor::TranslateManipulator (ManipulatorModes::TranslatePivot, m_SceneManager.GetCurrentScene(), m_ToolEnumerator));
                        }
                        else
                        {
                            switch ( manipulator->GetMode() )
                            {
                            case ManipulatorModes::ScalePivot:
                                m_SceneManager.GetCurrentScene()->SetTool(new Editor::ScaleManipulator (ManipulatorModes::Scale, m_SceneManager.GetCurrentScene(), m_ToolEnumerator));
                                break;
                            case ManipulatorModes::RotatePivot:
                                m_SceneManager.GetCurrentScene()->SetTool(new Editor::RotateManipulator (ManipulatorModes::Rotate, m_SceneManager.GetCurrentScene(), m_ToolEnumerator));
                                break;
                            case ManipulatorModes::TranslatePivot:
                                m_SceneManager.GetCurrentScene()->SetTool(new Editor::TranslateManipulator (ManipulatorModes::Translate, m_SceneManager.GetCurrentScene(), m_ToolEnumerator));
                                break;
                            }
                        }
                    }
                }
                break;
            }

        case EventIds::ID_ToolsDuplicate:
            {
                m_SceneManager.GetCurrentScene()->SetTool(new Editor::DuplicateTool (m_SceneManager.GetCurrentScene(), m_ToolEnumerator));
            }
            break;

        case EventIds::ID_ToolsLocatorCreate:
            {
                m_SceneManager.GetCurrentScene()->SetTool(new Editor::LocatorCreateTool (m_SceneManager.GetCurrentScene(), m_ToolEnumerator));
            }
            break;

        case EventIds::ID_ToolsVolumeCreate:
            {
                m_SceneManager.GetCurrentScene()->SetTool(new Editor::VolumeCreateTool (m_SceneManager.GetCurrentScene(), m_ToolEnumerator));
            }
            break;

        case EventIds::ID_ToolsEntityCreate:
            {
                m_SceneManager.GetCurrentScene()->SetTool(new Editor::EntityCreateTool (m_SceneManager.GetCurrentScene(), m_ToolEnumerator));
            }
            break;

        case EventIds::ID_ToolsCurveCreate:
            {
                m_SceneManager.GetCurrentScene()->SetTool( new Editor::CurveCreateTool( m_SceneManager.GetCurrentScene(), m_ToolEnumerator ) );
            }
            break;

        case EventIds::ID_ToolsCurveEdit:
            {
                Editor::CurveEditTool* curveEditTool = new Editor::CurveEditTool( m_SceneManager.GetCurrentScene(), m_ToolEnumerator );
                m_SceneManager.GetCurrentScene()->SetTool( curveEditTool );
                curveEditTool->StoreSelectedCurves();
            }
            break;

        case EventIds::ID_ToolsNavMesh:
            {
                Editor::NavMeshCreateTool* navMeshCreate = new Editor::NavMeshCreateTool (m_SceneManager.GetCurrentScene(), m_ToolEnumerator);
                m_SceneManager.GetCurrentScene()->SetTool( navMeshCreate );
                navMeshCreate->SetEditMode(NavMeshCreateTool::EDIT_MODE_ADD);
            }
            break;
        }

        m_ToolProperties.GetCanvas()->Clear();

        if (m_SceneManager.GetCurrentScene()->GetTool().ReferencesObject())
        {
            m_SceneManager.GetCurrentScene()->GetTool()->CreateProperties();

            m_ToolProperties.GetCanvas()->Layout();

            m_ToolProperties.GetCanvas()->Read();
        }

        m_ViewPanel->Refresh();
    }
    else
    {
        GetStatusBar()->SetStatusText( TXT( "You must create a new scene or open an existing scene to use a tool" ) );
    }
}

void MainFrame::DocumentModified( const DocumentChangedArgs& args )
{
    bool doAnyDocsNeedSaved = false;
    OS_DocumentSmartPtr::Iterator docItr = m_SceneManager.GetDocuments().Begin();
    OS_DocumentSmartPtr::Iterator docEnd = m_SceneManager.GetDocuments().End();
    for ( ; docItr != docEnd; ++docItr )
    {
        if ( ( *docItr )->IsModified() || !( *docItr )->GetPath().Exists() )
        {
            doAnyDocsNeedSaved = true;
            break;
        }
    }

    m_ToolbarPanel->SetSaveButtonState( doAnyDocsNeedSaved );
    m_MenuFile->Enable( ID_SaveAll, doAnyDocsNeedSaved );
}

void MainFrame::DocumentClosed( const DocumentChangedArgs& args )
{
    DocumentModified( args );

    args.m_Document->RemoveDocumentModifiedListener( DocumentChangedSignature::Delegate( this, &MainFrame::DocumentModified ) );
    args.m_Document->RemoveDocumentSavedListener( DocumentChangedSignature::Delegate( this, &MainFrame::DocumentModified ) );
    args.m_Document->RemoveDocumentClosedListener( DocumentChangedSignature::Delegate( this, &MainFrame::DocumentModified ) );
}

void MainFrame::ViewToolChanged( const ToolChangeArgs& args )
{
    i32 selectedTool = EventIds::ID_ToolsSelect;
    if ( args.m_NewTool )
    {
        if ( args.m_NewTool->HasType( Reflect::GetType<Editor::TransformManipulator>() ) )
        {
            Editor::TransformManipulator* manipulator = Reflect::DangerousCast< Editor::TransformManipulator >( args.m_NewTool );
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
        else if ( args.m_NewTool->GetType() == Reflect::GetType<Editor::EntityCreateTool>() )
        {
            selectedTool = EventIds::ID_ToolsEntityCreate;
        }
        else if ( args.m_NewTool->GetType() == Reflect::GetType<Editor::VolumeCreateTool>() )
        {
            selectedTool = EventIds::ID_ToolsVolumeCreate;
        }
        else if ( args.m_NewTool->GetType() == Reflect::GetType<Editor::LocatorCreateTool>() )
        {
            selectedTool = EventIds::ID_ToolsLocatorCreate;
        }
        else if ( args.m_NewTool->GetType() == Reflect::GetType<Editor::DuplicateTool>() )
        {
            selectedTool = EventIds::ID_ToolsDuplicate;
        }
        else if ( args.m_NewTool->GetType() == Reflect::GetType<Editor::CurveCreateTool>() )
        {
            selectedTool = EventIds::ID_ToolsCurveCreate;
        }
        else if ( args.m_NewTool->GetType() == Reflect::GetType<Editor::CurveEditTool>() )
        {
            selectedTool = EventIds::ID_ToolsCurveEdit;
        }
        else if ( args.m_NewTool->GetType() == Reflect::GetType<Editor::NavMeshCreateTool>() )
        {
            selectedTool = EventIds::ID_ToolsNavMesh;
        }
    }

    m_ToolbarPanel->ToggleTool( selectedTool );
}

void MainFrame::OnUndo( wxCommandEvent& event )
{
    if ( m_SceneManager.CanUndo() )
    {
        m_SceneManager.Undo();
        m_ToolProperties.Read();
        if ( m_SceneManager.HasCurrentScene() )
        {
            m_SceneManager.GetCurrentScene()->Execute(false);
        }
    }
}

void MainFrame::OnRedo( wxCommandEvent& event )
{
    if ( m_SceneManager.CanRedo() )
    {
        m_SceneManager.Redo();
        m_ToolProperties.Read();
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
    // special copy handler for navmesh
    if ( m_SceneManager.HasCurrentScene() && m_SceneManager.GetCurrentScene()->GetTool() && m_SceneManager.GetCurrentScene()->GetTool()->GetType() == Reflect::GetType<Editor::NavMeshCreateTool>() )
    {
        Editor::NavMeshCreateTool* navMeshCreate = static_cast<NavMeshCreateTool*>( m_SceneManager.GetCurrentScene()->GetTool().Ptr() );
        navMeshCreate->CopySelected();
    }
    else if ( m_SceneManager.HasCurrentScene() && m_SceneManager.GetCurrentScene()->GetSelection().GetItems().Size() > 0 )
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
    // special copy handler for navmesh
    if ( m_SceneManager.HasCurrentScene() && m_SceneManager.GetCurrentScene()->GetTool() && m_SceneManager.GetCurrentScene()->GetTool()->GetType() == Reflect::GetType<Editor::NavMeshCreateTool>() )
    {
        Editor::NavMeshCreateTool* navMeshCreate = static_cast<NavMeshCreateTool*>( m_SceneManager.GetCurrentScene()->GetTool().Ptr() );
        navMeshCreate->Paste();
    }
    else if ( m_SceneManager.HasCurrentScene() )
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
    if ( m_SelectionPropertiesManager->ThreadsActive() )
    {
        wxMessageBox( wxT( "You cannot delete items while the Properties Panel is generating." ), wxT( "Error" ), wxCENTER | wxOK | wxICON_ERROR, this );
        return;
    }

    if ( m_SceneManager.HasCurrentScene() )
    {
        m_SceneManager.GetCurrentScene()->Push( m_SceneManager.GetCurrentScene()->DeleteSelected() );
    }
}

void MainFrame::OnSelectAll(wxCommandEvent& event)
{
    if ( m_SceneManager.HasCurrentScene() )
    {
        OS_SelectableDumbPtr selection;

        HM_SceneNodeDumbPtr::const_iterator itr = m_SceneManager.GetCurrentScene()->GetNodes().begin();
        HM_SceneNodeDumbPtr::const_iterator end = m_SceneManager.GetCurrentScene()->GetNodes().end();
        for ( ; itr != end; ++itr )
        {
            Editor::SceneNode* sceneNode = itr->second;
            if ( sceneNode->HasType( Reflect::GetType<Editor::HierarchyNode>() ) )
            {
                selection.Append( sceneNode );
            }
        }

        m_SceneManager.GetCurrentScene()->Push( m_SceneManager.GetCurrentScene()->GetSelection().SetItems( selection ) );
    }
}

static void RecurseToggleSelection( Editor::HierarchyNode* node, const OS_SelectableDumbPtr& oldSelection, OS_SelectableDumbPtr& newSelection )
{
    for ( OS_HierarchyNodeDumbPtr::Iterator itr = node->GetChildren().Begin(), end = node->GetChildren().End(); itr != end; ++itr )
    {
        Editor::HierarchyNode* child = *itr;
        RecurseToggleSelection( child, oldSelection, newSelection );
    }

    bool found = false;
    OS_SelectableDumbPtr::Iterator selItr = oldSelection.Begin();
    OS_SelectableDumbPtr::Iterator selEnd = oldSelection.End();
    for ( ; selItr != selEnd && !found; ++selItr )
    {
        Editor::HierarchyNode* current = Reflect::ObjectCast< Editor::HierarchyNode >( *selItr );
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
        const OS_SelectableDumbPtr& selection = m_SceneManager.GetCurrentScene()->GetSelection().GetItems();
        if ( selection.Size() > 0 )
        {
            OS_SelectableDumbPtr newSelection;
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
        Math::V_Matrix4 transforms;
        m_SceneManager.GetCurrentScene()->GetSelectedTransforms(transforms);

        Helium::SmartPtr<Reflect::Matrix4ArraySerializer> data = new Reflect::Matrix4ArraySerializer();
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

        Reflect::V_Element elements;
        Reflect::ArchiveXML::FromString( xml, elements );

        Reflect::V_Element::const_iterator itr = elements.begin();
        Reflect::V_Element::const_iterator end = elements.end();
        for ( ; itr != end; ++itr )
        {
            Helium::SmartPtr<Reflect::Matrix4ArraySerializer> data = Reflect::ObjectCast< Reflect::Matrix4ArraySerializer >( *itr );
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
    if ( args.m_OldContext != SceneContexts::Normal )
    {
        wxEndBusyCursor();
    }

    static wxCursor busyCursor;
    busyCursor = wxCursor( wxCURSOR_WAIT );

    static wxCursor pickingCursor;
    pickingCursor = wxCursor( wxCURSOR_BULLSEYE );

    switch ( args.m_NewContext )
    {
    case SceneContexts::Loading:
        wxBeginBusyCursor( &busyCursor );
        break;

    case SceneContexts::Picking:
        wxBeginBusyCursor( &pickingCursor );
        break;

    case SceneContexts::Normal:
    default:
        wxSetCursor( wxCURSOR_ARROW );
        break;
    }
}

void MainFrame::Executed( const ExecuteArgs& args )
{
    if (!m_SelectionPropertiesManager->ThreadsActive() && !args.m_Interactively)
    {
        m_SelectionProperties.Read();
    }
}

void MainFrame::SelectionChanged( const OS_SelectableDumbPtr& selection )
{
    m_SelectionPropertiesManager->SetSelection( selection );
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
    if ( !m_SceneManager.CloseAll() )
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
    wxMessageBox( wxT( "Editor" ), wxT( "About" ), wxOK | wxCENTER, this );
}

void MainFrame::OnPreferences( wxCommandEvent& event )
{
    PreferencesDialog dlg ( this, wxID_ANY, TXT( "Preferences" ) );
    dlg.ShowModal( wxGetApp().GetPreferences() );
}

///////////////////////////////////////////////////////////////////////////////
//Pushes the selected menu item as the current selected item.  
//
void MainFrame::OnManifestContextMenu(wxCommandEvent& event)
{
    if( !m_OrderedContextItems.empty() )
    { 
        u32 selectionIndex = event.GetId() - EventIds::ID_SelectContextMenu;

        Editor::HierarchyNode* selection = m_OrderedContextItems[ selectionIndex ];

        if( selection )
        {
            OS_SelectableDumbPtr newSelection;
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
    OS_SelectableDumbPtr newSelection;

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
            newSelection.Append( static_cast<Editor::HierarchyNode*>( data->m_Nodes ) );
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

///////////////////////////////////////////////////////////////////////////////
// Copies the currently selected items from the specified scene into the
// clipboard.
// 
bool MainFrame::Copy( Editor::Scene* scene )
{
    EDITOR_SCENE_SCOPE_TIMER( ("") );
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
bool MainFrame::Paste( Editor::Scene* scene )
{
    EDITOR_SCENE_SCOPE_TIMER( ("") );
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
    //
    // Top level draw routine
    //

    Editor::Scene* rootScene = m_SceneManager.GetRootScene();

    if (rootScene)
    {
        rootScene->Render( render );
    }
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

    OS_SelectableDumbPtr selectableItems;
    V_PickHitSmartPtr::const_iterator itr = args.m_Pick->GetHits().begin();
    V_PickHitSmartPtr::const_iterator end = args.m_Pick->GetHits().end();
    for ( ; itr != end; ++itr )
    {
        Selectable* selectable = Reflect::ObjectCast<Selectable>((*itr)->GetObject());
        if (selectable)
        {
            // add it to the new selection list
            selectableItems.Append(selectable);
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
            OS_SelectableDumbPtr::Iterator itr = selectableItems.Begin();
            OS_SelectableDumbPtr::Iterator end = selectableItems.End();
            for( ; itr != end; ++itr)
            {
                Selectable* selection = *itr;

                if( selection->IsSelectable() )
                {
                    Editor::HierarchyNode* node = Reflect::ObjectCast<Editor::HierarchyNode>( selection );

                    if( node )
                    {
                        m_OrderedContextItems.push_back( node );
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
            u32 index = 0;
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
            GetEventHandler()->Connect( EventIds::ID_SelectContextMenu, EventIds::ID_SelectContextMenu +  (u32)m_OrderedContextItems.size(),wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnManifestContextMenu ), NULL, this );
            PopupMenu( &contextMenu );
            GetEventHandler()->Disconnect( EventIds::ID_SelectContextMenu, EventIds::ID_SelectContextMenu +  (u32)m_OrderedContextItems.size(),wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnManifestContextMenu ), NULL, this ); 
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
    u32 numMenuItems = 0;

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
    for ( u32 i = EventIds::ID_SelectContextMenu; i < EventIds::ID_SelectContextMenu + numMenuItems; i++ )
    {
        // clean up, disconnect any id that was set up for any of the items
        GetEventHandler()->Disconnect( i, wxEVT_COMMAND_MENU_SELECTED );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Populates the context menu with selections from the various types
//
void MainFrame::SetupTypeContextMenu( const HM_StrToSceneNodeTypeSmartPtr& sceneNodeTypes, wxMenu& contextMenu, u32& numMenuItems )
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
        const Editor::SceneNodeType* type( *itr );
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
                const Editor::EntityType* entity = Reflect::ConstObjectCast<Editor::EntityType>( type );

                // if this is an instance, then we need to check if it has code classes
                const Editor::InstanceType* instance = Reflect::ConstObjectCast<Editor::InstanceType>( type );

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

void MainFrame::SetupEntityTypeMenus( const Editor::EntityType* entity, wxMenu* subMenu, u32& numMenuItems )
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
            const Editor::EntityAssetSet* art = Reflect::ObjectCast<Editor::EntityAssetSet>( itr->second );
            if (art && !art->GetContentFile().empty())
            {
                tstring artPath( art->GetContentFile() );

#pragma TODO( "We need make the artPath relative to the entity file" )

                // Why is the art path blank?
                HELIUM_ASSERT(!artPath.empty());

                ContextCallbackData* data = new ContextCallbackData;
                data->m_ContextCallbackType = ContextCallbackTypes::Instance;
                data->m_InstanceSet = art;

                GetEventHandler()->Connect( EventIds::ID_SelectContextMenu + numMenuItems, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnTypeContextMenu ), data, this );
                menu->Append( EventIds::ID_SelectContextMenu + numMenuItems, artPath.c_str() );
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
bool MainFrame::SortContextItemsByName( Editor::SceneNode* lhs, Editor::SceneNode* rhs )
{
    tstring lname( lhs->GetName() );
    tstring rname( rhs->GetName() );

    toUpper( lname );
    toUpper( rname );

    return lname < rname;
}

///////////////////////////////////////////////////////////////////////////////
// Static function used to sort type items by name
bool MainFrame::SortTypeItemsByName( Editor::SceneNodeType* lhs, Editor::SceneNodeType* rhs )
{
    tstring lname( lhs->GetName() );
    tstring rname( rhs->GetName() );

    toUpper( lname );
    toUpper( rname );

    return lname < rname;
}

void MainFrame::SyncPropertyThread()
{
    while ( m_SelectionPropertiesManager->ThreadsActive() )
    {
        ::Sleep( 500 );
    }
}