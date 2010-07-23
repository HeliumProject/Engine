#include "Precompile.h"

#include "Foundation/Reflect/ArchiveXML.h"

#include "Pipeline/Content/ContentVersion.h"
#include "Pipeline/Content/Scene.h"

#include "Application/UI/FileDialog.h"
#include "Application/Inspect/DragDrop/ClipboardDataObject.h"

#include "Scene/Scene.h"
#include "Scene/SceneEditorIDs.h"
#include "Scene/InstanceSet.h"
#include "Scene/EntityType.h"
#include "Scene/EntityAssetSet.h"

#include "Scene/TransformManipulator.h"

#include "Scene/CurveCreateTool.h"
#include "Scene/CurveEditTool.h"
#include "Scene/DuplicateTool.h"
#include "Scene/EntityCreateTool.h"
#include "Scene/LocatorCreateTool.h"
#include "Scene/NavMeshCreateTool.h"
#include "Scene/VolumeCreateTool.h"

#include "Scene/ScaleManipulator.h"
#include "Scene/TranslateManipulator.h"

#include "UI/PreferencesDialog.h"

#include "ArtProvider.h"
#include "ImportOptionsDlg.h"
#include "ExportOptionsDlg.h"
#include "MainFrame.h"
#include "App.h"

using namespace Luna;

tstring MainFrame::s_PreferencesPrefix = TXT( "MainFrame" );

///////////////////////////////////////////////////////////////////////////////
// Wraps up a pointer to an Luna::Scene so that it can be stored in the combo box that
// is used for selecting the current scene.  Each item in the combo box stores 
// the scene that it refers to.
// 
class SceneSelectData : public wxClientData
{
public:
    Luna::Scene* m_Scene;

    SceneSelectData( Luna::Scene* scene )
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
    const Luna::SceneNodeType* m_NodeType;
    const Luna::InstanceSet* m_InstanceSet;
    Luna::SceneNode* m_Nodes;
};


MainFrame::MainFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style )
: MainFrameGenerated( parent, id, title, pos, size, style )
, m_MRU( new Nocturnal::MenuMRU( 30, this ) )
{

    //
    // Dynamic Menu Generation
    //
    Connect( wxEVT_MENU_OPEN, wxMenuEventHandler( MainFrame::OnMenuOpen ) );

    //
    // General Events
    //
    Connect( wxID_CLOSE, wxEVT_CLOSE_WINDOW, wxCloseEventHandler( MainFrame::OnExiting ) );

    //
    // File Handling
    //
    Connect( ID_NewScene, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnNewScene ) );
    Connect( ID_NewEntity, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnNewEntity ) );
    Connect( ID_NewProject, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnNewProject ) );
    Connect( ID_Open, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnOpen ) );
    Connect( ID_Close, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnClose ) );

    Connect( ID_SaveAll, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnSaveAll ) );

    Connect( ID_Import, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnImport ) );
    Connect( ID_ImportFromClipboard, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnImport ) );
    Connect( ID_Export, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnExport ) );
    Connect( ID_ExportToClipboard, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnExport ) );

    //
    // Editing
    //

    Connect( wxID_UNDO, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnUndo ) );
    Connect( wxID_REDO, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnRedo ) );
    Connect( wxID_CUT, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnCut ) );
    Connect( wxID_COPY, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnCopy ) );
    Connect( wxID_PASTE, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnPaste ) );
    Connect( wxID_DELETE, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnDelete ) );

    Connect( ID_SelectAll, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnSelectAll ) );
    Connect( ID_InvertSelection, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnInvertSelection ) );

    Connect( ID_Parent, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnParent ) );
    Connect( ID_Unparent, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnUnparent ) );
    Connect( ID_Group, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnGroup ) );
    Connect( ID_Ungroup, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnUngroup ) );
    Connect( ID_Center, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnCenter ) );
    Connect( ID_Duplicate, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnDuplicate ) );
    Connect( ID_SmartDuplicate, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnSmartDuplicate ) );
    Connect( ID_CopyTransform, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnCopyTransform ) );
    Connect( ID_PasteTransform, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnPasteTransform ) );
    Connect( ID_SnapToCamera, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnSnapToCamera ) );
    Connect( ID_SnapCameraTo, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnSnapCameraTo ) );

    Connect( ID_WalkUp, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnPickWalk ) );
    Connect( ID_WalkDown, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnPickWalk ) );
    Connect( ID_WalkForward, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnPickWalk ) );
    Connect( ID_WalkBackward, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnPickWalk ) );

    /*
EVT_MENU(wxID_HELP_INDEX, MainFrame::OnHelpIndex)
EVT_MENU(wxID_HELP_SEARCH, MainFrame::OnHelpSearch)

EVT_MENU(SceneEditorIDs::ID_EditPreferences, SceneEditor::OnEditPreferences)

EVT_MENU(SceneEditorIDs::ID_ViewAxes, SceneEditor::OnViewChange)
EVT_MENU(SceneEditorIDs::ID_ViewGrid, SceneEditor::OnViewChange)
EVT_MENU(SceneEditorIDs::ID_ViewBounds, SceneEditor::OnViewChange)
EVT_MENU(SceneEditorIDs::ID_ViewStatistics, SceneEditor::OnViewChange)

EVT_MENU(SceneEditorIDs::ID_ViewNone, SceneEditor::OnViewChange)
EVT_MENU(SceneEditorIDs::ID_ViewRender, SceneEditor::OnViewChange)
EVT_MENU(SceneEditorIDs::ID_ViewCollision, SceneEditor::OnViewChange)

EVT_MENU(SceneEditorIDs::ID_ViewWireframeOnMesh, SceneEditor::OnViewChange)
EVT_MENU(SceneEditorIDs::ID_ViewWireframeOnShaded, SceneEditor::OnViewChange)
EVT_MENU(SceneEditorIDs::ID_ViewWireframe, SceneEditor::OnViewChange)
EVT_MENU(SceneEditorIDs::ID_ViewMaterial, SceneEditor::OnViewChange)
EVT_MENU(SceneEditorIDs::ID_ViewTexture, SceneEditor::OnViewChange)

EVT_MENU(SceneEditorIDs::ID_ViewFrustumCulling, SceneEditor::OnViewChange)
EVT_MENU(SceneEditorIDs::ID_ViewBackfaceCulling, SceneEditor::OnViewChange)

EVT_MENU(SceneEditorIDs::ID_ViewOrbit, SceneEditor::OnViewCameraChange)
EVT_MENU(SceneEditorIDs::ID_ViewFront, SceneEditor::OnViewCameraChange)
EVT_MENU(SceneEditorIDs::ID_ViewSide, SceneEditor::OnViewCameraChange)
EVT_MENU(SceneEditorIDs::ID_ViewTop, SceneEditor::OnViewCameraChange)

EVT_MENU(SceneEditorIDs::ID_ViewShowAll, SceneEditor::OnViewVisibleChange)
EVT_MENU(SceneEditorIDs::ID_ViewShowAllGeometry, SceneEditor::OnViewVisibleChange)
EVT_MENU(SceneEditorIDs::ID_ViewShowSelected, SceneEditor::OnViewVisibleChange)
EVT_MENU(SceneEditorIDs::ID_ViewShowSelectedGeometry, SceneEditor::OnViewVisibleChange)
EVT_MENU(SceneEditorIDs::ID_ViewShowUnrelated, SceneEditor::OnViewVisibleChange)
EVT_MENU(SceneEditorIDs::ID_ViewShowLastHidden, SceneEditor::OnViewVisibleChange)

EVT_MENU(SceneEditorIDs::ID_ViewHideAll, SceneEditor::OnViewVisibleChange)
EVT_MENU(SceneEditorIDs::ID_ViewHideAllGeometry, SceneEditor::OnViewVisibleChange)
EVT_MENU(SceneEditorIDs::ID_ViewHideSelected, SceneEditor::OnViewVisibleChange)
EVT_MENU(SceneEditorIDs::ID_ViewHideSelectedGeometry, SceneEditor::OnViewVisibleChange)
EVT_MENU(SceneEditorIDs::ID_ViewHideUnrelated, SceneEditor::OnViewVisibleChange)

EVT_MENU(SceneEditorIDs::ID_ViewFrameOrigin, SceneEditor::OnFrameOrigin)
EVT_MENU(SceneEditorIDs::ID_ViewFrameSelected, SceneEditor::OnFrameSelected)
EVT_MENU(SceneEditorIDs::ID_ViewHighlightMode, SceneEditor::OnHighlightMode)
EVT_MENU(SceneEditorIDs::ID_ViewPreviousView, SceneEditor::OnPreviousView)
EVT_MENU(SceneEditorIDs::ID_ViewNextView, SceneEditor::OnNextView)

EVT_MENU(SceneEditorIDs::ID_ViewDefaultShowLayers, SceneEditor::OnViewDefaultsChange)
EVT_MENU(SceneEditorIDs::ID_ViewDefaultShowInstances, SceneEditor::OnViewDefaultsChange)
EVT_MENU(SceneEditorIDs::ID_ViewDefaultShowGeometry, SceneEditor::OnViewDefaultsChange) 
EVT_MENU(SceneEditorIDs::ID_ViewDefaultShowPointer, SceneEditor::OnViewDefaultsChange) 
EVT_MENU(SceneEditorIDs::ID_ViewDefaultShowBounds, SceneEditor::OnViewDefaultsChange) 
*/
    //
    // Toolbox
    //
    Connect( SceneEditorIDs::ID_ToolsSelect, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnToolSelected ) );
    Connect( SceneEditorIDs::ID_ToolsScale, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnToolSelected ) );
    Connect( SceneEditorIDs::ID_ToolsScalePivot, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnToolSelected ) );
    Connect( SceneEditorIDs::ID_ToolsRotate, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnToolSelected ) );
    Connect( SceneEditorIDs::ID_ToolsRotatePivot, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnToolSelected ) );
    Connect( SceneEditorIDs::ID_ToolsTranslate, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnToolSelected ) );
    Connect( SceneEditorIDs::ID_ToolsTranslatePivot, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnToolSelected ) );
    Connect( SceneEditorIDs::ID_ToolsPivot, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnToolSelected ) );
    Connect( SceneEditorIDs::ID_ToolsDuplicate, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnToolSelected ) );

    Connect( SceneEditorIDs::ID_ToolsLocatorCreate, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnToolSelected ) );
    Connect( SceneEditorIDs::ID_ToolsVolumeCreate, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnToolSelected ) );
    Connect( SceneEditorIDs::ID_ToolsEntityCreate, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnToolSelected ) );
    Connect( SceneEditorIDs::ID_ToolsCurveCreate, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnToolSelected ) );
    Connect( SceneEditorIDs::ID_ToolsCurveEdit, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnToolSelected ) );
    Connect( SceneEditorIDs::ID_ToolsNavMesh, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnToolSelected ) );

#pragma TODO( "Remove this block of code if/when wxFormBuilder supports wxArtProvider" )
    {
        Freeze();

        m_MainToolbar->FindById( ID_NewScene )->SetNormalBitmap( wxArtProvider::GetBitmap( Luna::ArtIDs::NewScene ) );
        m_MainToolbar->FindById( ID_Open )->SetNormalBitmap( wxArtProvider::GetBitmap( Nocturnal::ArtIDs::Open ) );
        m_MainToolbar->FindById( ID_SaveAll )->SetNormalBitmap( wxArtProvider::GetBitmap( Nocturnal::ArtIDs::SaveAll ) );
        m_MainToolbar->FindById( ID_Cut )->SetNormalBitmap( wxArtProvider::GetBitmap( Nocturnal::ArtIDs::Cut ) );
        m_MainToolbar->FindById( ID_Copy )->SetNormalBitmap( wxArtProvider::GetBitmap( Nocturnal::ArtIDs::Copy ) );
        m_MainToolbar->FindById( ID_Paste )->SetNormalBitmap( wxArtProvider::GetBitmap( Nocturnal::ArtIDs::Paste ) );
        m_MainToolbar->FindById( ID_Undo )->SetNormalBitmap( wxArtProvider::GetBitmap( Nocturnal::ArtIDs::Undo ) );
        m_MainToolbar->FindById( ID_Redo )->SetNormalBitmap( wxArtProvider::GetBitmap( Nocturnal::ArtIDs::Redo ) );

        m_MainToolbar->Realize();

        Layout();
        Thaw();
    }

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
    m_DirectoryPanel = new DirectoryPanel( this );
    m_FrameManager.AddPane( m_DirectoryPanel, wxAuiPaneInfo().Name( wxT( "directory" ) ).Caption( wxT( "Directory" ) ).Left().Layer( 1 ).Position( 1 ).BestSize( wxSize( 200, 900 ) ) );

    //
    // Properties/Layers/Types area
    //
    m_PropertiesPanel = new PropertiesPanel( this );
    m_SelectionEnumerator = new PropertiesGenerator( &m_SelectionProperties );
    m_SelectionPropertiesManager = new PropertiesManager( m_SelectionEnumerator );
    m_SelectionPropertiesManager->AddPropertiesCreatedListener( PropertiesCreatedSignature::Delegate( this, &MainFrame::OnPropertiesCreated ) );
    //m_SelectionProperties.SetControl( selectionProperties->m_PropertyCanvas );
    m_SelectionProperties.SetControl( new Inspect::CanvasWindow ( m_PropertiesPanel, SceneEditorIDs::ID_SelectionProperties, wxPoint(0,0), wxSize(250,250), wxNO_BORDER | wxCLIP_CHILDREN) );
    m_FrameManager.AddPane( m_PropertiesPanel, wxAuiPaneInfo().Name( wxT( "properties" ) ).Caption( wxT( "Properties" ) ).Right().Layer( 1 ).Position( 1 ) );

    m_LayersPanel = new LayersPanel( this );
    m_FrameManager.AddPane( m_LayersPanel, wxAuiPaneInfo().Name( wxT( "layers" ) ).Caption( wxT( "Layers" ) ).Right().Layer( 1 ).Position( 2 ) );

    m_TypesPanel = new TypesPanel( this );
    m_FrameManager.AddPane( m_TypesPanel, wxAuiPaneInfo().Name( wxT( "types" ) ).Caption( wxT( "Types" ) ).Right().Layer( 1 ).Position( 3 ) );

    //
    // Tools panel
    //
    m_ToolsPanel = new ToolsPanel( this );
    m_ToolEnumerator = new PropertiesGenerator (&m_ToolProperties);
    m_ToolPropertiesManager = new PropertiesManager (m_ToolEnumerator);
    m_ToolPropertiesManager->AddPropertiesCreatedListener( PropertiesCreatedSignature::Delegate( this, &MainFrame::OnPropertiesCreated ) );
    m_ToolProperties.SetControl( new Inspect::CanvasWindow ( m_ToolsPanel->GetToolsPropertiesPanel(), SceneEditorIDs::ID_ToolProperties, wxPoint(0,0), wxSize(250,250), wxNO_BORDER | wxCLIP_CHILDREN) );
    m_ToolsPanel->Create( m_ToolProperties.GetControl() );
    m_ToolsPanel->Disable();
    m_ToolsPanel->Refresh();
    m_FrameManager.AddPane( m_ToolsPanel, wxAuiPaneInfo().Name( wxT( "tools" ) ).Caption( wxT( "Tools" ) ).BestSize( wxSize( 200, 500 ) ).MinSize( wxSize( 150, 250 ) ).FloatingSize( wxSize( 200, 500 ) ).Float() );

    m_FrameManager.Update();

    CreatePanelsMenu( m_MenuPanels );

    m_SceneManager.AddCurrentSceneChangingListener( SceneChangeSignature::Delegate (this, &MainFrame::CurrentSceneChanging) );
    m_SceneManager.AddCurrentSceneChangedListener( SceneChangeSignature::Delegate (this, &MainFrame::CurrentSceneChanged) );

}

MainFrame::~MainFrame()
{
    m_SceneManager.RemoveCurrentSceneChangingListener( SceneChangeSignature::Delegate (this, &MainFrame::CurrentSceneChanging) );
    m_SceneManager.RemoveCurrentSceneChangedListener( SceneChangeSignature::Delegate (this, &MainFrame::CurrentSceneChanged) );

    m_ViewPanel->GetViewport()->RemoveRenderListener( RenderSignature::Delegate ( this, &MainFrame::Render ) );
    m_ViewPanel->GetViewport()->RemoveSelectListener( SelectSignature::Delegate ( this, &MainFrame::Select ) ); 
    m_ViewPanel->GetViewport()->RemoveSetHighlightListener( SetHighlightSignature::Delegate ( this, &MainFrame::SetHighlight ) );
    m_ViewPanel->GetViewport()->RemoveClearHighlightListener( ClearHighlightSignature::Delegate ( this, &MainFrame::ClearHighlight ) );
    m_ViewPanel->GetViewport()->RemoveToolChangedListener( ToolChangeSignature::Delegate ( this, &MainFrame::ViewToolChanged ) );
}

void MainFrame::SetHelpText( const tchar* text )
{
    m_HelpPanel->SetHelpText( text );
}

///////////////////////////////////////////////////////////////////////////////
// Helper function for common opening code.
// 
bool MainFrame::OpenProject( const Nocturnal::Path& path )
{
    bool opened = false;

    if ( !path.empty() && path.Exists() )
    {
        tstring error;
        try
        {
            m_Project = Reflect::Archive::FromFile< Luna::Project >( path );
        }
        catch ( const Nocturnal::Exception& ex )
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

bool MainFrame::AddScene( const Nocturnal::Path& path )
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
            catch ( const Nocturnal::Exception& ex )
            {
                error = ex.What();
            }
        }
    }

    return opened;
}


void MainFrame::OnMenuOpen( wxMenuEvent& event )
{
    const wxMenu* menu = event.GetMenu();

    if ( menu == m_MenuFile )
    {
        //// File->Import is enabled if there is a current editing scene
        //m_MenuFile->Enable( SceneEditorIDs::ID_FileImport, m_SceneManager.HasCurrentScene() );
        //m_MenuFile->Enable( SceneEditorIDs::ID_FileImportFromClipboard, m_SceneManager.HasCurrentScene() );

        //// File->Export is only enabled if there is something selected
        //const bool enableExport = m_SceneManager.HasCurrentScene() && m_SceneManager.GetCurrentScene()->GetSelection().GetItems().Size() > 0;
        //m_MenuFile->Enable( SceneEditorIDs::ID_FileExport, enableExport );
        //m_MenuFile->Enable( SceneEditorIDs::ID_FileExportToClipboard, enableExport );

        //m_MRUMenuItem->Enable( !m_MRU->GetItems().Empty() );
        //m_MRU->PopulateMenu( m_MRUMenu );
    }
    else if ( menu == m_MenuPanels )
    {
        UpdatePanelsMenu( m_MenuPanels );
    }
    else if ( menu == m_MenuEdit )
    {
        //// Edit->Undo/Redo is only enabled if there are commands in the queue
        //const bool canUndo = m_SceneManager.HasCurrentScene() && m_SceneManager.CanUndo();
        //const bool canRedo = m_SceneManager.HasCurrentScene() && m_SceneManager.CanRedo();
        //m_MenuEdit->Enable( wxID_UNDO, canUndo );
        //m_MenuEdit->Enable( wxID_REDO, canRedo );

        //// Edit->Invert Selection is only enabled if something is selected
        //const bool isAnythingSelected = m_SceneManager.HasCurrentScene() && m_SceneManager.GetCurrentScene()->GetSelection().GetItems().Size() > 0;
        //m_MenuEdit->Enable( SceneEditorIDs::ID_EditInvertSelection, isAnythingSelected );

        //// Cut/copy/paste
        //m_MenuEdit->Enable( wxID_CUT, isAnythingSelected );
        //m_MenuEdit->Enable( wxID_COPY, isAnythingSelected );
        //m_MenuEdit->Enable( wxID_PASTE, m_SceneManager.HasCurrentScene() && IsClipboardFormatAvailable( CF_TEXT ) );
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
    m_Project = new Project;
    m_ProjectPanel->SetProject( m_Project );
}

void MainFrame::OnOpen( wxCommandEvent& event )
{
    Nocturnal::FileDialog openDlg( this, TXT( "Open" ) );

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
        Luna::Scene* currentScene = m_SceneManager.GetCurrentScene();

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
            case SceneEditorIDs::ID_FileImport:
                {
                    Nocturnal::FileDialog fileDialog( this, TXT( "Import" ) );

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

            case SceneEditorIDs::ID_FileImportFromClipboard:
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

            u64 startTimer = Platform::TimerGetClock();

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
                case SceneEditorIDs::ID_FileExport:
                    {
                        Nocturnal::FileDialog fileDialog( this, TXT( "Export Selection" ), TXT( "" ), TXT( "" ), wxFileSelectorDefaultWildcardStr, Nocturnal::FileDialogStyles::DefaultSave );

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
                        catch ( Nocturnal::Exception& ex )
                        {
                            tostringstream str;
                            str << "Failed to generate file '" << file << "': " << ex.What();
                            wxMessageBox( str.str(), wxT( "Error" ), wxOK|wxCENTRE|wxICON_ERROR );
                            result = false;
                        }

                        break;
                    }

                case SceneEditorIDs::ID_FileExportToClipboard:
                    {
                        tstring xml;

                        try
                        {
                            Reflect::ArchiveXML::ToString( elements, xml, m_SceneManager.GetCurrentScene() );
                        }
                        catch ( Nocturnal::Exception& ex )
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
                str << "Export Complete: " << std::fixed << Platform::CyclesToMillis( Platform::TimerGetClock() - startTimer ) / 1000.f << " seconds...";
                SceneStatusChanged( str.str() );
            }
        }
    }
}


void MainFrame::CurrentSceneChanged( const SceneChangeArgs& args )
{
    if ( !args.m_Scene )
    {
        return;
    }

    m_ToolsPanel->Enable();
    m_ToolsPanel->Refresh();
}

void MainFrame::CurrentSceneChanging( const SceneChangeArgs& args )
{
    if ( !args.m_Scene )
    {
        return;
    }

    m_ToolsPanel->Disable();
    m_ToolsPanel->Refresh();
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
        case SceneEditorIDs::ID_ToolsSelect:
            {
                m_SceneManager.GetCurrentScene()->SetTool(NULL);
                break;
            }

        case SceneEditorIDs::ID_ToolsScale:
            {
                m_SceneManager.GetCurrentScene()->SetTool(new Luna::ScaleManipulator (ManipulatorModes::Scale, m_SceneManager.GetCurrentScene(), m_ToolEnumerator));
                break;
            }

        case SceneEditorIDs::ID_ToolsScalePivot:
            {
                m_SceneManager.GetCurrentScene()->SetTool(new Luna::TranslateManipulator (ManipulatorModes::ScalePivot, m_SceneManager.GetCurrentScene(), m_ToolEnumerator));
                break;
            }

        case SceneEditorIDs::ID_ToolsRotate:
            {
                m_SceneManager.GetCurrentScene()->SetTool(new Luna::RotateManipulator (ManipulatorModes::Rotate, m_SceneManager.GetCurrentScene(), m_ToolEnumerator));
                break;
            }

        case SceneEditorIDs::ID_ToolsRotatePivot:
            {
                m_SceneManager.GetCurrentScene()->SetTool(new Luna::TranslateManipulator (ManipulatorModes::RotatePivot, m_SceneManager.GetCurrentScene(), m_ToolEnumerator));
                break;
            }

        case SceneEditorIDs::ID_ToolsTranslate:
            {
                m_SceneManager.GetCurrentScene()->SetTool(new Luna::TranslateManipulator (ManipulatorModes::Translate, m_SceneManager.GetCurrentScene(), m_ToolEnumerator));
                break;
            }

        case SceneEditorIDs::ID_ToolsTranslatePivot:
            {
                m_SceneManager.GetCurrentScene()->SetTool(new Luna::TranslateManipulator (ManipulatorModes::TranslatePivot, m_SceneManager.GetCurrentScene(), m_ToolEnumerator));
                break;
            }

        case SceneEditorIDs::ID_ToolsPivot:
            {
                if (m_SceneManager.GetCurrentScene()->GetTool().ReferencesObject())
                {
                    if ( m_SceneManager.GetCurrentScene()->GetTool()->GetType() == Reflect::GetType<Luna::ScaleManipulator>() )
                    {
                        m_SceneManager.GetCurrentScene()->SetTool(new Luna::TranslateManipulator (ManipulatorModes::ScalePivot, m_SceneManager.GetCurrentScene(), m_ToolEnumerator));
                    }
                    else if ( m_SceneManager.GetCurrentScene()->GetTool()->GetType() == Reflect::GetType<Luna::RotateManipulator>() )
                    {
                        m_SceneManager.GetCurrentScene()->SetTool(new Luna::TranslateManipulator (ManipulatorModes::RotatePivot, m_SceneManager.GetCurrentScene(), m_ToolEnumerator));
                    }
                    else if ( m_SceneManager.GetCurrentScene()->GetTool()->GetType() == Reflect::GetType<Luna::TranslateManipulator>() )
                    {
                        Luna::TranslateManipulator* manipulator = Reflect::AssertCast< Luna::TranslateManipulator > (m_SceneManager.GetCurrentScene()->GetTool());

                        if ( manipulator->GetMode() == ManipulatorModes::Translate)
                        {
                            m_SceneManager.GetCurrentScene()->SetTool(new Luna::TranslateManipulator (ManipulatorModes::TranslatePivot, m_SceneManager.GetCurrentScene(), m_ToolEnumerator));
                        }
                        else
                        {
                            switch ( manipulator->GetMode() )
                            {
                            case ManipulatorModes::ScalePivot:
                                m_SceneManager.GetCurrentScene()->SetTool(new Luna::ScaleManipulator (ManipulatorModes::Scale, m_SceneManager.GetCurrentScene(), m_ToolEnumerator));
                                break;
                            case ManipulatorModes::RotatePivot:
                                m_SceneManager.GetCurrentScene()->SetTool(new Luna::RotateManipulator (ManipulatorModes::Rotate, m_SceneManager.GetCurrentScene(), m_ToolEnumerator));
                                break;
                            case ManipulatorModes::TranslatePivot:
                                m_SceneManager.GetCurrentScene()->SetTool(new Luna::TranslateManipulator (ManipulatorModes::Translate, m_SceneManager.GetCurrentScene(), m_ToolEnumerator));
                                break;
                            }
                        }
                    }
                }
                break;
            }

        case SceneEditorIDs::ID_ToolsDuplicate:
            {
                m_SceneManager.GetCurrentScene()->SetTool(new Luna::DuplicateTool (m_SceneManager.GetCurrentScene(), m_ToolEnumerator));
            }
            break;

        case SceneEditorIDs::ID_ToolsLocatorCreate:
            {
                m_SceneManager.GetCurrentScene()->SetTool(new Luna::LocatorCreateTool (m_SceneManager.GetCurrentScene(), m_ToolEnumerator));
            }
            break;

        case SceneEditorIDs::ID_ToolsVolumeCreate:
            {
                m_SceneManager.GetCurrentScene()->SetTool(new Luna::VolumeCreateTool (m_SceneManager.GetCurrentScene(), m_ToolEnumerator));
            }
            break;

        case SceneEditorIDs::ID_ToolsEntityCreate:
            {
                m_SceneManager.GetCurrentScene()->SetTool(new Luna::EntityCreateTool (m_SceneManager.GetCurrentScene(), m_ToolEnumerator));
            }
            break;

        case SceneEditorIDs::ID_ToolsCurveCreate:
            {
                m_SceneManager.GetCurrentScene()->SetTool( new Luna::CurveCreateTool( m_SceneManager.GetCurrentScene(), m_ToolEnumerator ) );
            }
            break;

        case SceneEditorIDs::ID_ToolsCurveEdit:
            {
                Luna::CurveEditTool* curveEditTool = new Luna::CurveEditTool( m_SceneManager.GetCurrentScene(), m_ToolEnumerator );
                m_SceneManager.GetCurrentScene()->SetTool( curveEditTool );
                curveEditTool->StoreSelectedCurves();
            }
            break;

        case SceneEditorIDs::ID_ToolsNavMesh:
            {
                Luna::NavMeshCreateTool* navMeshCreate = new Luna::NavMeshCreateTool (m_SceneManager.GetCurrentScene(), m_ToolEnumerator);
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

void MainFrame::ViewToolChanged( const ToolChangeArgs& args )
{
    i32 selectedTool = SceneEditorIDs::ID_ToolsSelect;
    if ( args.m_NewTool )
    {
        if ( args.m_NewTool->HasType( Reflect::GetType<Luna::TransformManipulator>() ) )
        {
            Luna::TransformManipulator* manipulator = Reflect::DangerousCast< Luna::TransformManipulator >( args.m_NewTool );
            switch ( manipulator->GetMode() )
            {
            case ManipulatorModes::Scale:
                selectedTool = SceneEditorIDs::ID_ToolsScale;
                break;

            case ManipulatorModes::ScalePivot:
                selectedTool = SceneEditorIDs::ID_ToolsScalePivot;
                break;

            case ManipulatorModes::Rotate:
                selectedTool = SceneEditorIDs::ID_ToolsRotate;
                break;

            case ManipulatorModes::RotatePivot:
                selectedTool = SceneEditorIDs::ID_ToolsRotatePivot;
                break;

            case ManipulatorModes::Translate:
                selectedTool = SceneEditorIDs::ID_ToolsTranslate;
                break;

            case ManipulatorModes::TranslatePivot:
                selectedTool = SceneEditorIDs::ID_ToolsTranslatePivot;
                break;
            }
        }
        else if ( args.m_NewTool->GetType() == Reflect::GetType<Luna::EntityCreateTool>() )
        {
            selectedTool = SceneEditorIDs::ID_ToolsEntityCreate;
        }
        else if ( args.m_NewTool->GetType() == Reflect::GetType<Luna::VolumeCreateTool>() )
        {
            selectedTool = SceneEditorIDs::ID_ToolsVolumeCreate;
        }
        else if ( args.m_NewTool->GetType() == Reflect::GetType<Luna::LocatorCreateTool>() )
        {
            selectedTool = SceneEditorIDs::ID_ToolsLocatorCreate;
        }
        else if ( args.m_NewTool->GetType() == Reflect::GetType<Luna::DuplicateTool>() )
        {
            selectedTool = SceneEditorIDs::ID_ToolsDuplicate;
        }
        else if ( args.m_NewTool->GetType() == Reflect::GetType<Luna::CurveCreateTool>() )
        {
            selectedTool = SceneEditorIDs::ID_ToolsCurveCreate;
        }
        else if ( args.m_NewTool->GetType() == Reflect::GetType<Luna::CurveEditTool>() )
        {
            selectedTool = SceneEditorIDs::ID_ToolsCurveEdit;
        }
        else if ( args.m_NewTool->GetType() == Reflect::GetType<Luna::NavMeshCreateTool>() )
        {
            selectedTool = SceneEditorIDs::ID_ToolsNavMesh;
        }
    }

    m_ToolsPanel->ToggleTool( selectedTool );
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
    if ( m_SceneManager.HasCurrentScene() && m_SceneManager.GetCurrentScene()->GetTool() && m_SceneManager.GetCurrentScene()->GetTool()->GetType() == Reflect::GetType<Luna::NavMeshCreateTool>() )
    {
        Luna::NavMeshCreateTool* navMeshCreate = static_cast<NavMeshCreateTool*>( m_SceneManager.GetCurrentScene()->GetTool().Ptr() );
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
    if ( m_SceneManager.HasCurrentScene() && m_SceneManager.GetCurrentScene()->GetTool() && m_SceneManager.GetCurrentScene()->GetTool()->GetType() == Reflect::GetType<Luna::NavMeshCreateTool>() )
    {
        Luna::NavMeshCreateTool* navMeshCreate = static_cast<NavMeshCreateTool*>( m_SceneManager.GetCurrentScene()->GetTool().Ptr() );
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
            Luna::SceneNode* sceneNode = itr->second;
            if ( sceneNode->HasType( Reflect::GetType<Luna::HierarchyNode>() ) )
            {
                selection.Append( sceneNode );
            }
        }

        m_SceneManager.GetCurrentScene()->Push( m_SceneManager.GetCurrentScene()->GetSelection().SetItems( selection ) );
    }
}

static void RecurseToggleSelection( Luna::HierarchyNode* node, const OS_SelectableDumbPtr& oldSelection, OS_SelectableDumbPtr& newSelection )
{
    for ( OS_HierarchyNodeDumbPtr::Iterator itr = node->GetChildren().Begin(), end = node->GetChildren().End(); itr != end; ++itr )
    {
        Luna::HierarchyNode* child = *itr;
        RecurseToggleSelection( child, oldSelection, newSelection );
    }

    bool found = false;
    OS_SelectableDumbPtr::Iterator selItr = oldSelection.Begin();
    OS_SelectableDumbPtr::Iterator selEnd = oldSelection.End();
    for ( ; selItr != selEnd && !found; ++selItr )
    {
        Luna::HierarchyNode* current = Reflect::ObjectCast< Luna::HierarchyNode >( *selItr );
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

        Nocturnal::SmartPtr<Reflect::Matrix4ArraySerializer> data = new Reflect::Matrix4ArraySerializer();
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
            Nocturnal::SmartPtr<Reflect::Matrix4ArraySerializer> data = Reflect::ObjectCast< Reflect::Matrix4ArraySerializer >( *itr );
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
    wxMessageBox( wxT( "Luna" ), wxT( "About" ), wxOK | wxCENTER, this );
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
        u32 selectionIndex = event.GetId() - SceneEditorIDs::ID_SelectContextMenu;

        Luna::HierarchyNode* selection = m_OrderedContextItems[ selectionIndex ];

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
            newSelection.Append( static_cast<Luna::HierarchyNode*>( data->m_Nodes ) );
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
bool MainFrame::Copy( Luna::Scene* scene )
{
    LUNA_SCENE_SCOPE_TIMER( ("") );
    bool isOk = true;
    NOC_ASSERT( scene );

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
bool MainFrame::Paste( Luna::Scene* scene )
{
    LUNA_SCENE_SCOPE_TIMER( ("") );
    NOC_ASSERT( scene );

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

    Luna::Scene* rootScene = m_SceneManager.GetRootScene();

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
                    Luna::HierarchyNode* node = Reflect::ObjectCast<Luna::HierarchyNode>( selection );

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

                contextMenu.Append( SceneEditorIDs::ID_SelectContextMenu + index , str.c_str() );
            }

            contextMenu.SetEventHandler( GetEventHandler() );
            GetEventHandler()->Connect( SceneEditorIDs::ID_SelectContextMenu, SceneEditorIDs::ID_SelectContextMenu +  (u32)m_OrderedContextItems.size(),wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnManifestContextMenu ), NULL, this );
            PopupMenu( &contextMenu );
            GetEventHandler()->Disconnect( SceneEditorIDs::ID_SelectContextMenu, SceneEditorIDs::ID_SelectContextMenu +  (u32)m_OrderedContextItems.size(),wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnManifestContextMenu ), NULL, this ); 
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
        GetEventHandler()->Connect( SceneEditorIDs::ID_SelectContextMenu + numMenuItems, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::SelectItemInScene ), data, this );
        contextMenu.Append( SceneEditorIDs::ID_SelectContextMenu + numMenuItems, TXT( "Select" ) );
        ++numMenuItems;
    }

    if (!m_SceneManager.GetCurrentScene()->GetSelection().GetItems().Empty())
    {
        GetEventHandler()->Connect( SceneEditorIDs::ID_SelectContextMenu + numMenuItems, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::SelectSimilarItemsInScene ), NULL, this );
        contextMenu.Append( SceneEditorIDs::ID_SelectContextMenu + numMenuItems, TXT( "Select Similar" ) );
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
    for ( u32 i = SceneEditorIDs::ID_SelectContextMenu; i < SceneEditorIDs::ID_SelectContextMenu + numMenuItems; i++ )
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
        const Luna::SceneNodeType* type( *itr );
        const HM_SceneNodeSmartPtr& typeInstances( type->GetInstances() );

        if( !typeInstances.empty() )
        {
            wxMenu* subMenu = new wxMenu;

            // add selection for all items
            ContextCallbackData* data = new ContextCallbackData;
            data->m_ContextCallbackType = ContextCallbackTypes::All;
            data->m_NodeType = type;

            GetEventHandler()->Connect( SceneEditorIDs::ID_SelectContextMenu + numMenuItems, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnTypeContextMenu ), data, this );
            subMenu->Append( SceneEditorIDs::ID_SelectContextMenu + numMenuItems, TXT( "Select All" ) );
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

                    GetEventHandler()->Connect( SceneEditorIDs::ID_SelectContextMenu + numMenuItems, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnTypeContextMenu ), data, this );
                    itemMenu->Append( SceneEditorIDs::ID_SelectContextMenu + numMenuItems, (*ord_itr)->GetName().c_str() );
                    ++numMenuItems;
                }

                // add the items menu to the sub menu
                subMenu->Append( SceneEditorIDs::ID_SelectContextMenu + numMenuItems, TXT( "Select Single" ), itemMenu );
                ++numMenuItems;

                // if this is an entity, then we need to check if it has art classes
                const Luna::EntityType* entity = Reflect::ConstObjectCast<Luna::EntityType>( type );

                // if this is an instance, then we need to check if it has code classes
                const Luna::InstanceType* instance = Reflect::ConstObjectCast<Luna::InstanceType>( type );

                if (entity)
                {
                    // set up for entity types
                    SetupEntityTypeMenus( entity, subMenu, numMenuItems );
                }
            }
            contextMenu.Append( SceneEditorIDs::ID_SelectContextMenu + numMenuItems, type->GetName().c_str(), subMenu );
            ++numMenuItems;
        }
    }
}

void MainFrame::SetupEntityTypeMenus( const Luna::EntityType* entity, wxMenu* subMenu, u32& numMenuItems )
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
            const Luna::EntityAssetSet* art = Reflect::ObjectCast<Luna::EntityAssetSet>( itr->second );
            if (art && !art->GetContentFile().empty())
            {
                tstring artPath( art->GetContentFile() );

#pragma TODO( "We need make the artPath relative to the game project file" )

                // Why is the art path blank?
                NOC_ASSERT(!artPath.empty());

                ContextCallbackData* data = new ContextCallbackData;
                data->m_ContextCallbackType = ContextCallbackTypes::Instance;
                data->m_InstanceSet = art;

                GetEventHandler()->Connect( SceneEditorIDs::ID_SelectContextMenu + numMenuItems, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnTypeContextMenu ), data, this );
                menu->Append( SceneEditorIDs::ID_SelectContextMenu + numMenuItems, artPath.c_str() );
                ++numMenuItems;
                added = true;
            }
        }

        if (added)
        {
            subMenu->AppendSeparator();
            subMenu->Append( SceneEditorIDs::ID_SelectContextMenu + numMenuItems, TXT( "Select All With Art Class" ), menu );
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
bool MainFrame::SortContextItemsByName( Luna::SceneNode* lhs, Luna::SceneNode* rhs )
{
    tstring lname( lhs->GetName() );
    tstring rname( rhs->GetName() );

    toUpper( lname );
    toUpper( rname );

    return lname < rname;
}

///////////////////////////////////////////////////////////////////////////////
// Static function used to sort type items by name
bool MainFrame::SortTypeItemsByName( Luna::SceneNodeType* lhs, Luna::SceneNodeType* rhs )
{
    tstring lname( lhs->GetName() );
    tstring rname( rhs->GetName() );

    toUpper( lname );
    toUpper( rname );

    return lname < rname;
}
