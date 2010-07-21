#include "Precompile.h"

#include "MainFrame.h"
#include "ArtProvider.h"

#include "Scene/Scene.h"
#include "Scene/SceneEditorIDs.h"

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


using namespace Luna;

tstring MainFrame::s_PreferencesPrefix = TXT( "MainFrame" );

MainFrame::MainFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style )
: MainFrameGenerated( parent, id, title, pos, size, style )
{

    Connect( wxEVT_MENU_OPEN, wxMenuEventHandler( MainFrame::OnMenuOpen ) );

    Connect( ID_NewScene, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::OnNewScene ) );

    Connect( SceneEditorIDs::ID_ToolsSelect, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnToolSelected ) );
    Connect( SceneEditorIDs::ID_ToolsScale, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnToolSelected ) );
    Connect( SceneEditorIDs::ID_ToolsScalePivot, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnToolSelected ) );
    Connect( SceneEditorIDs::ID_ToolsRotate, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnToolSelected ) );
    Connect( SceneEditorIDs::ID_ToolsRotatePivot, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnToolSelected ) );
    Connect( SceneEditorIDs::ID_ToolsTranslate, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnToolSelected ) );
    Connect( SceneEditorIDs::ID_ToolsTranslatePivot, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnToolSelected ) );
    Connect( SceneEditorIDs::ID_ToolsPivot, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnToolSelected ) );
    Connect( SceneEditorIDs::ID_ToolsDuplicate, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnToolSelected ) );

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

    m_DirectoryPanel = new DirectoryPanel( this );
    m_FrameManager.AddPane( m_DirectoryPanel, wxAuiPaneInfo().Name( wxT( "directory" ) ).Caption( wxT( "Directory" ) ).Left().Layer( 1 ).Position( 1 ) );

    m_LayersPanel = new LayersPanel( this );
    m_FrameManager.AddPane( m_LayersPanel, wxAuiPaneInfo().Name( wxT( "layers" ) ).Caption( wxT( "Layers" ) ).Left().Layer( 1 ).Position( 1 ) );

    m_TypesPanel = new TypesPanel( this );
    m_FrameManager.AddPane( m_TypesPanel, wxAuiPaneInfo().Name( wxT( "types" ) ).Caption( wxT( "Types" ) ).Left().Layer( 1 ).Position( 1 ) );

    m_HelpPanel = new HelpPanel( this );
    m_FrameManager.AddPane( m_HelpPanel, wxAuiPaneInfo().Name( wxT( "help" ) ).Caption( wxT( "Help" ) ).Left().Layer( 1 ).Position( 1 ) );

    m_ViewPanel = new ViewPanel( this );
    //m_ViewPanel->GetViewport()->AddRenderListener( RenderSignature::Delegate ( this, &MainFrame::Render ) );
    //m_ViewPanel->GetViewport()->AddSelectListener( SelectSignature::Delegate ( this, &MainFrame::Select ) ); 
    //m_ViewPanel->GetViewport()->AddSetHighlightListener( SetHighlightSignature::Delegate ( this, &MainFrame::SetHighlight ) );
    //m_ViewPanel->GetViewport()->AddClearHighlightListener( ClearHighlightSignature::Delegate ( this, &MainFrame::ClearHighlight ) );
    m_ViewPanel->GetViewport()->AddToolChangedListener( ToolChangeSignature::Delegate ( this, &MainFrame::ViewToolChanged ) );
    m_FrameManager.AddPane( m_ViewPanel, wxAuiPaneInfo().Name( wxT( "view" ) ).CenterPane() );

    m_PropertiesPanel = new PropertiesPanel( this );
    m_FrameManager.AddPane( m_PropertiesPanel, wxAuiPaneInfo().Name( wxT( "properties" ) ).Caption( wxT( "Properties" ) ).Right().Layer( 1 ).Position( 1 ) );

    m_ToolsPanel = new ToolsPanel( this );
    m_ToolEnumerator = new PropertiesGenerator (&m_ToolProperties);
    m_ToolPropertiesManager = new PropertiesManager (m_ToolEnumerator);
    m_ToolPropertiesManager->AddPropertiesCreatedListener( PropertiesCreatedSignature::Delegate( this, &MainFrame::OnPropertiesCreated ) );
    m_ToolProperties.SetControl( new Inspect::CanvasWindow ( m_ToolsPanel->GetToolsPropertiesPanel(), SceneEditorIDs::ID_ToolProperties, wxPoint(0,0), wxSize(250,250), wxNO_BORDER | wxCLIP_CHILDREN) );
    m_ToolsPanel->Create( m_ToolProperties.GetControl() );
    m_ToolsPanel->Disable();
    m_ToolsPanel->Refresh();
    m_FrameManager.AddPane( m_ToolsPanel, wxAuiPaneInfo().Name( wxT( "tools" ) ).Caption( wxT( "Tools" ) ).Right().Layer( 1 ).Position( 1 ) );

    m_FrameManager.Update();

    CreatePanelsMenu( m_MenuPanels );

    m_SceneManager.AddCurrentSceneChangingListener( SceneChangeSignature::Delegate (this, &MainFrame::CurrentSceneChanging) );
    m_SceneManager.AddCurrentSceneChangedListener( SceneChangeSignature::Delegate (this, &MainFrame::CurrentSceneChanged) );

}

MainFrame::~MainFrame()
{
    m_SceneManager.RemoveCurrentSceneChangingListener( SceneChangeSignature::Delegate (this, &MainFrame::CurrentSceneChanging) );
    m_SceneManager.RemoveCurrentSceneChangedListener( SceneChangeSignature::Delegate (this, &MainFrame::CurrentSceneChanged) );

    //m_ViewPanel->GetViewport()->RemoveRenderListener( RenderSignature::Delegate ( this, &MainFrame::Render ) );
    //m_ViewPanel->GetViewport()->RemoveSelectListener( SelectSignature::Delegate ( this, &MainFrame::Select ) ); 
    //m_ViewPanel->GetViewport()->RemoveSetHighlightListener( SetHighlightSignature::Delegate ( this, &MainFrame::SetHighlight ) );
    //m_ViewPanel->GetViewport()->RemoveClearHighlightListener( ClearHighlightSignature::Delegate ( this, &MainFrame::ClearHighlight ) );
    m_ViewPanel->GetViewport()->RemoveToolChangedListener( ToolChangeSignature::Delegate ( this, &MainFrame::ViewToolChanged ) );
}

void MainFrame::SetHelpText( const tchar* text )
{
    m_HelpPanel->SetHelpText( text );
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
    else if ( menu == m_MenuView )
    {
        //m_HelperMenu->Check( SceneEditorIDs::ID_ViewAxes, m_View->IsAxesVisible() );
        //m_HelperMenu->Check( SceneEditorIDs::ID_ViewGrid, m_View->IsGridVisible() );
        //m_HelperMenu->Check( SceneEditorIDs::ID_ViewBounds, m_View->IsBoundsVisible() );
        //m_HelperMenu->Check( SceneEditorIDs::ID_ViewStatistics, m_View->IsStatisticsVisible() );

        //m_CameraMenu->Check( SceneEditorIDs::ID_ViewOrbit, m_View->GetCameraMode() == CameraModes::Orbit );
        //m_CameraMenu->Check( SceneEditorIDs::ID_ViewFront, m_View->GetCameraMode() == CameraModes::Front );
        //m_CameraMenu->Check( SceneEditorIDs::ID_ViewSide, m_View->GetCameraMode() == CameraModes::Side );
        //m_CameraMenu->Check( SceneEditorIDs::ID_ViewTop, m_View->GetCameraMode() == CameraModes::Top );

        //m_GeometryMenu->Check( SceneEditorIDs::ID_ViewNone, m_View->GetGeometryMode() == GeometryModes::None );
        //m_GeometryMenu->Check( SceneEditorIDs::ID_ViewRender, m_View->GetGeometryMode() == GeometryModes::Render );
        //m_GeometryMenu->Check( SceneEditorIDs::ID_ViewCollision, m_View->GetGeometryMode() == GeometryModes::Collision );
        //m_GeometryMenu->Check( SceneEditorIDs::ID_ViewPathfinding, m_View->IsPathfindingVisible() );

        //ViewColorMode colorMode = SceneEditorPreferences()->GetViewPreferences()->GetColorMode();
        //M_IDToColorMode::const_iterator colorModeItr = m_ColorModeLookup.begin();
        //M_IDToColorMode::const_iterator colorModeEnd = m_ColorModeLookup.end();
        //for ( ; colorModeItr != colorModeEnd; ++colorModeItr )
        //{
        //    m_ViewColorMenu->Check( colorModeItr->first, colorModeItr->second == colorMode );
        //}

        //m_ShadingMenu->Check( SceneEditorIDs::ID_ViewWireframeOnMesh, m_View->GetCamera()->GetWireframeOnMesh() );
        //m_ShadingMenu->Check( SceneEditorIDs::ID_ViewWireframeOnShaded, m_View->GetCamera()->GetWireframeOnShaded() );
        //m_ShadingMenu->Check( SceneEditorIDs::ID_ViewWireframe, m_View->GetCamera()->GetShadingMode() == ShadingModes::Wireframe );
        //m_ShadingMenu->Check( SceneEditorIDs::ID_ViewMaterial, m_View->GetCamera()->GetShadingMode() == ShadingModes::Material );
        //m_ShadingMenu->Check( SceneEditorIDs::ID_ViewTexture, m_View->GetCamera()->GetShadingMode() == ShadingModes::Texture );

        //m_CullingMenu->Check( SceneEditorIDs::ID_ViewFrustumCulling, m_View->GetCamera()->IsViewFrustumCulling() );
        //m_CullingMenu->Check( SceneEditorIDs::ID_ViewBackfaceCulling, m_View->GetCamera()->IsBackFaceCulling() );

        //m_ViewMenu->Check( SceneEditorIDs::ID_ViewHighlightMode, m_View->IsHighlighting() );

        //Content::NodeVisibilityPtr nodeDefaults = SceneEditorPreferences()->GetDefaultNodeVisibility(); 

        //m_ViewDefaultsMenu->Check( SceneEditorIDs::ID_ViewDefaultShowLayers, nodeDefaults->GetVisibleLayer()); 
        //m_ViewDefaultsMenu->Check( SceneEditorIDs::ID_ViewDefaultShowInstances, !nodeDefaults->GetHiddenNode());
        //m_ViewDefaultsMenu->Check( SceneEditorIDs::ID_ViewDefaultShowGeometry, nodeDefaults->GetShowGeometry()); 
        //m_ViewDefaultsMenu->Check( SceneEditorIDs::ID_ViewDefaultShowPointer, nodeDefaults->GetShowPointer());  
        //m_ViewDefaultsMenu->Check( SceneEditorIDs::ID_ViewDefaultShowBounds, nodeDefaults->GetShowBounds());  

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