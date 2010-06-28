#include "Precompile.h"
#include "Application.h"
#include "AssetEditor.h"

#include "AssetDocument.h"
#include "AssetEditorIDs.h"
#include "AssetOutliner.h"
#include "AssetPreferences.h"
#include "AssetReferenceNode.h"
#include "ComponentChooserDlg.h"
#include "ComponentExistenceCommand.h"
#include "ElementArrayNode.h"
#include "PersistentNode.h"
#include "AssetPreviewWindow.h"
#include "AssetEditorGenerated.h"
#include "Browser/BrowserToolBar.h"

#include "Pipeline/Asset/Components/ArtFileComponent.h"
#include "Pipeline/Asset/AssetClass.h"
#include "Pipeline/Asset/AssetInit.h"
#include "Pipeline/Component/ComponentHandle.h"
#include "Pipeline/Asset/Classes/EntityAsset.h"
#include "Pipeline/Asset/Classes/StandardShaderAsset.h"

#include "Foundation/File/Path.h"

#include "Foundation/Log.h"
#include "Application/Inspect/DragDrop/ClipboardDataObject.h"
#include "Editor/ContextMenu.h"
#include "Editor/EditorInfo.h"

#include "Task/Export.h"
#include "Application/Inspect/Widgets/Canvas.h"
#include "Application/RCS/RCS.h"
#include "Application/UI/FileDialog.h"
#include "Application/UI/ImageManager.h"
#include "Application/UI/ListDialog.h"
#include "Platform/Process.h"
#include <wx/clipbrd.h>

// Using
using namespace Luna;

// Static event table
BEGIN_EVENT_TABLE( AssetEditor, Luna::Editor )
EVT_MENU_OPEN( OnMenuOpen )
EVT_MENU( wxID_NEW, OnNew )
EVT_MENU( wxID_OPEN, OnOpen )
EVT_MENU( AssetEditorIDs::SearchForFile, OnFind )
EVT_MENU( AssetEditorIDs::SortOpenFiles, OnSortFiles )
EVT_MENU( wxID_SAVE, OnSave )
EVT_MENU( AssetEditorIDs::SaveAllAssetClasses, OnSaveAll )
EVT_MENU( wxID_CLOSE, OnClose )
EVT_MENU( wxID_EXIT, OnExit )
EVT_CLOSE( AssetEditor::OnExiting )
EVT_MENU( wxID_UNDO, OnUndo )
EVT_MENU( wxID_REDO, OnRedo )
EVT_MENU( wxID_CUT, OnCut )
EVT_MENU( wxID_COPY, OnCopy )
EVT_MENU( wxID_PASTE, OnPaste )
EVT_MENU( AssetEditorIDs::MoveUp, OnMoveUp )
EVT_MENU( AssetEditorIDs::MoveDown, OnMoveDown )
EVT_MENU( AssetEditorIDs::Preview, OnPreview )
EVT_MENU( AssetEditorIDs::Build, OnBuild )
EVT_MENU( wxID_HELP_INDEX, OnHelpIndex )
EVT_MENU( wxID_HELP_SEARCH, OnHelpSearch )
EVT_MENU( AssetEditorIDs::Checkout, OnCheckout )
EVT_MENU( AssetEditorIDs::ExpandAll, OnExpandAll )
EVT_MENU( AssetEditorIDs::CollapseAll, OnCollapseAll )
END_EVENT_TABLE()

// Statics
static const char* s_EditorTitle = "Luna Asset Editor";

#pragma TODO( "This needs to be rethought" )
static const char* s_FileFilter = "*.entity.*;*.font.*;*.scene.*";

///////////////////////////////////////////////////////////////////////////////
// Creates a new Asset Editor.
// 
static Luna::Editor* CreateAssetEditor()
{
    return new AssetEditor();
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
AssetEditor::AssetEditor()
: Luna::Editor( EditorTypes::Asset, NULL, wxID_ANY, wxT( s_EditorTitle ), wxDefaultPosition, wxSize( 800, 600 ), wxDEFAULT_FRAME_STYLE | wxSUNKEN_BORDER )
, m_AssetManager( this )
, m_MRU( new Nocturnal::MenuMRU( 30, this ) )
, m_Outliner( new AssetOutliner( this ) )
, m_MenuPanels( new wxMenu() )
, m_MenuFile( new wxMenu() )
, m_MenuEdit( new wxMenu() )
, m_MenuView( new wxMenu() )
, m_MenuNew( new wxMenu() )
, m_MenuMRU( new wxMenu() )
, m_MenuOptions( new wxMenu() )
, m_MenuFilePathOptions( new wxMenu() )
, m_MenuItemOpenRecent( NULL )
, m_MainToolBar( NULL )
, m_PromptModifiedFiles( true )
{
    wxIconBundle iconBundle;
    wxIcon tempIcon;
    tempIcon.CopyFromBitmap( Nocturnal::GlobalImageManager().GetBitmap( "asset_editor.png", Nocturnal::IconSizes::Size64 ) );
    iconBundle.AddIcon( tempIcon );
    tempIcon.CopyFromBitmap( Nocturnal::GlobalImageManager().GetBitmap( "asset_editor.png", Nocturnal::IconSizes::Size32 ) );
    iconBundle.AddIcon( tempIcon );
    tempIcon.CopyFromBitmap( Nocturnal::GlobalImageManager().GetBitmap( "asset_editor.png" ) );
    iconBundle.AddIcon( tempIcon );
    SetIcons( iconBundle );

    m_PropertyCanvas.SetControl( new Inspect::CanvasWindow( this, wxID_ANY, wxDefaultPosition, wxSize( 250, 250 ), wxALWAYS_SHOW_SB | wxCLIP_CHILDREN ) );
    m_AssetPreviewWindow = new AssetPreviewWindow( GetAssetManager(), this, wxID_ANY, wxDefaultPosition, wxSize( 250, 250 ), wxALWAYS_SHOW_SB | wxCLIP_CHILDREN );

    // MRU callback
    m_MRU->AddItemSelectedListener( Nocturnal::MRUSignature::Delegate( this, &AssetEditor::MRUOpen ) );
    m_MRU->FromVector( GetAssetEditorPreferences()->GetMRU()->GetPaths() );

    // Toolbars
    m_MainToolBar = new wxToolBar( this, -1, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER );
    m_MainToolBar->SetToolBitmapSize( wxSize( 16,16 ) );
    m_MainToolBar->AddTool( wxID_NEW, wxT( "New" ), Nocturnal::GlobalImageManager().GetBitmap( "new_file.png" ), wxT( "New" ) );
    m_MainToolBar->AddTool( wxID_OPEN, wxT( "Open" ), wxArtProvider::GetBitmap( wxART_FILE_OPEN, wxART_OTHER, wxSize( 16, 16 ) ), wxT( "Open" ) );
    m_MainToolBar->AddTool( AssetEditorIDs::SearchForFile, wxT( "Find..." ), wxArtProvider::GetBitmap( wxART_FIND, wxART_OTHER, wxSize( 16, 16 ) ), wxT( "Find..." ) );
    m_MainToolBar->AddTool( wxID_SAVE, wxT( "Save" ), wxArtProvider::GetBitmap( wxART_FILE_SAVE, wxART_OTHER, wxSize( 16, 16 ) ), wxT( "Save" ) );
    m_MainToolBar->AddTool( AssetEditorIDs::SaveAllAssetClasses, wxT( "Save All" ), Nocturnal::GlobalImageManager().GetBitmap( "save_all.png" ), wxT( "Save All" ) );
    m_MainToolBar->AddSeparator();
    m_MainToolBar->AddTool( wxID_CUT, wxT( "Cut" ), wxArtProvider::GetBitmap( wxART_CUT, wxART_OTHER, wxSize( 16, 16 ) ), wxT( "Cut" ) );
    m_MainToolBar->AddTool( wxID_COPY, wxT( "Copy" ), wxArtProvider::GetBitmap( wxART_COPY, wxART_OTHER, wxSize(16, 16 ) ), wxT( "Copy" ) );
    m_MainToolBar->AddTool( wxID_PASTE, wxT( "Paste" ), wxArtProvider::GetBitmap( wxART_PASTE, wxART_OTHER, wxSize(16, 16 ) ), wxT( "Paste" ) );
    m_MainToolBar->AddSeparator();
    m_MainToolBar->AddTool( wxID_UNDO, wxT( "Undo" ), wxArtProvider::GetBitmap( wxART_UNDO, wxART_OTHER, wxSize(16, 16 ) ), wxT( "Undo" ) );
    m_MainToolBar->AddTool( wxID_REDO, wxT( "Redo" ), wxArtProvider::GetBitmap( wxART_REDO, wxART_OTHER, wxSize(16, 16 ) ), wxT( "Redo" ) );
    m_MainToolBar->AddSeparator();
    m_MainToolBar->AddTool( AssetEditorIDs::Preview, wxT( "Preview" ), Nocturnal::GlobalImageManager().GetBitmap( "preview.png" ), wxT( "Preview" ) );
    m_MainToolBar->AddTool( AssetEditorIDs::Build, wxT( "Build" ), Nocturnal::GlobalImageManager().GetBitmap( "build.png" ), wxT( "Build (Shift-click for build options)" ) );
    m_MainToolBar->AddTool( AssetEditorIDs::View, wxT( "View" ), Nocturnal::GlobalImageManager().GetBitmap( "view.png" ), wxT( "View (Shift-click for build options)" ) );
    m_MainToolBar->AddTool( AssetEditorIDs::Export, wxT( "Export" ), Nocturnal::GlobalImageManager().GetBitmap( "export.png" ), wxT( "Export all relevant art assets (Shift-click for export options)" ) );
    m_MainToolBar->AddTool( AssetEditorIDs::SyncShaders, wxT( "Sync Shaders" ), Nocturnal::GlobalImageManager().GetBitmap( "sync_shaders.png" ), wxT( "Synchronize the Shader Usage settings." ) );
    m_MainToolBar->AddTool( AssetEditorIDs::UpdateSymbols, wxT( "Update Symbols" ), Nocturnal::GlobalImageManager().GetBitmap( "header.png" ), wxT( "Update Symbols for Update Classes" ) ); 
    m_MainToolBar->AddSeparator();
    m_MainToolBar->AddTool( AssetEditorIDs::AddAnimationSet, wxT( "Add Set" ), Nocturnal::GlobalImageManager().GetBitmap( "animationset_add.png" ), wxT( "Add an Animation Set to the selected asset(s)." ) );
    m_MainToolBar->AddTool( AssetEditorIDs::AddAnimationGroup, wxT( "Add Group" ), Nocturnal::GlobalImageManager().GetBitmap( "animationgroup_add.png" ), wxT( "Add a new Animation Group to the selected Animation Set(s)." ) );
    m_MainToolBar->AddTool( AssetEditorIDs::EditAnimationGroup, wxT( "Edit Group" ), Nocturnal::GlobalImageManager().GetBitmap( "animationgroup_edit.png" ), wxT( "Edit the selected Animation Group." ) );
    m_MainToolBar->AddTool( AssetEditorIDs::AddAnimationClip, wxT( "Add Clip" ), Nocturnal::GlobalImageManager().GetBitmap( "animationclip_add.png" ), wxT( "Add a new Animation Clip to the selected Animation Chain(s)." ) );
    m_MainToolBar->Realize();
    m_MainToolBar->EnableTool( AssetEditorIDs::Build, false );
    m_MainToolBar->EnableTool( AssetEditorIDs::View, false );
    m_MainToolBar->EnableTool( AssetEditorIDs::Export, false );
    m_MainToolBar->EnableTool( AssetEditorIDs::SyncShaders, false );
    m_MainToolBar->EnableTool( AssetEditorIDs::UpdateSymbols, false ); 
    m_MainToolBar->EnableTool( AssetEditorIDs::AddAnimationSet, false );
    m_MainToolBar->EnableTool( AssetEditorIDs::AddAnimationGroup, false );
    m_MainToolBar->EnableTool( AssetEditorIDs::EditAnimationGroup, false );
    m_MainToolBar->EnableTool( AssetEditorIDs::AddAnimationClip, false );

    m_BrowserToolBar = new BrowserToolBar( this );
    m_BrowserToolBar->Realize();

    // Attach everything to the frame manager
    m_FrameManager.AddPane( m_MainToolBar, wxAuiPaneInfo().Name( wxT( "standard" ) ).DestroyOnClose( false ).Caption( wxT( "Standard Toolbar" ) ).ToolbarPane().Top().LeftDockable( false ).RightDockable( false ) );
    m_FrameManager.AddPane( m_BrowserToolBar, m_BrowserToolBar->GetAuiPaneInfo( 3 ) );
    m_FrameManager.AddPane( m_Outliner->GetWindow(), wxAuiPaneInfo().Name( wxT( "outline" ) ).DestroyOnClose( false ).Caption( wxT( "Outline" ) ).CenterPane() );
    m_FrameManager.AddPane( m_PropertyCanvas.GetControl(), wxAuiPaneInfo().Name( wxT( "properties" ) ).DestroyOnClose( false ).Caption( wxT( "Properties" ) ).Right().Layer( 2 ).Position( 1 ) );
    m_FrameManager.AddPane( m_AssetPreviewWindow, wxAuiPaneInfo().Name( wxT( "preview" ) ).DestroyOnClose( false ).Caption( wxT( "Preview" ) ).Right().Layer( 2 ).Position( 2 ) );


    // Menus
    wxMenuBar* menuBar = new wxMenuBar();

    // Menu for creating an Animation Clip
    m_MenuAddAnimClip.Append( AssetEditorIDs::AddAnimationClipToNewChain, "Add Clip to New Animation Chain", "Add Animation Clip to a new Animation Chain" );
    m_MenuAddAnimClip.Append( AssetEditorIDs::AddAnimationClipToExistingChain, "Add Clip to Existing Animation Chain", "Add Animation Clip to an already existing Animation Chain" );

    // Menu for creating new assets
    typedef std::map< const Reflect::Class*, wxMenu* > M_SubMenus;
    M_SubMenus subMenus;

    // Add a submenu for shaders since there's so many different kinds.
    // Additional submenus could be added here as well.
    const Reflect::Class* shaderBase = Reflect::GetClass< Asset::ShaderAsset >();
    wxMenu* shaderSubMenu = subMenus.insert( M_SubMenus::value_type( shaderBase, new wxMenu() ) ).first->second;
    m_MenuNew->AppendSubMenu( shaderSubMenu, shaderBase->m_UIName.c_str() );

    // Populate the New asset menu
    std::vector< i32 >::const_iterator assetItr = Asset::g_AssetClassTypes.begin();
    std::vector< i32 >::const_iterator assetEnd = Asset::g_AssetClassTypes.end();
    for ( ; assetItr != assetEnd; ++assetItr )
    {
        const i32 typeID = (*assetItr);
        const Reflect::Class* typeInfo = Reflect::Registry::GetInstance()->GetClass( typeID );

        wxMenuItem* menuItem = NULL;

        M_SubMenus::const_iterator foundSubMenu = subMenus.find( Reflect::Registry::GetInstance()->GetClass( typeInfo->m_Base ) );
        if ( foundSubMenu != subMenus.end() )
        {
            menuItem = foundSubMenu->second->Append( wxID_ANY, typeInfo->m_UIName.c_str() );
        }
        else
        {
            menuItem = m_MenuNew->Append( wxID_ANY, typeInfo->m_UIName.c_str() );
        }

        // Map the menu item ID to the asset class ID so that when we get a menu item
        // callback, we know which type of asset to create.
        m_MenuItemToAssetType.insert( std::map< i32, i32 >::value_type( menuItem->GetId(), typeID ) );

        // Connect a callback for when the menu item is selected.  No need to disconnect
        // this handler since the lifetime of this class is tied to the menu.
        Connect( menuItem->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( AssetEditor::OnNewAsset ), NULL, this );
    }

    // File menu

    wxMenuItem* menuItem = new wxMenuItem( m_MenuFile, wxID_ANY, "New", "", wxITEM_NORMAL, m_MenuNew );
    menuItem->SetBitmap( Nocturnal::GlobalImageManager().GetBitmap( "new_file.png" ) );
    m_MenuFile->Append( menuItem );

    menuItem = new wxMenuItem( m_MenuFile, wxID_OPEN, "Open\tCtrl-o" );
    menuItem->SetBitmap( wxArtProvider::GetBitmap( wxART_FILE_OPEN ) );
    m_MenuFile->Append( menuItem );

    m_MenuItemOpenRecent = m_MenuFile->AppendSubMenu( m_MenuMRU, "Open Recent" );

    menuItem = new wxMenuItem( m_MenuFile, AssetEditorIDs::SearchForFile, "Find...\tCtrl-f" );
    menuItem->SetBitmap( wxArtProvider::GetBitmap( wxART_FIND ) );
    m_MenuFile->Append( menuItem );

    //m_MenuFile->Append( AssetEditorIDs::Checkout, "Check Out\tCtrl-e" );

    m_MenuFile->AppendSeparator();

    m_MenuFile->Append( AssetEditorIDs::SortOpenFiles, "Sort Files" );

    m_MenuFile->AppendSeparator();

    menuItem = new wxMenuItem( m_MenuFile, wxID_SAVE, "Save\tCtrl-s" );
    menuItem->SetBitmap( wxArtProvider::GetBitmap( wxART_FILE_SAVE ) );
    m_MenuFile->Append( menuItem );

    menuItem = new wxMenuItem( m_MenuFile, AssetEditorIDs::SaveAllAssetClasses, "Save All\tCtrl-Shift-s" );
    menuItem->SetBitmap( Nocturnal::GlobalImageManager().GetBitmap( "save_all.png" ) );
    m_MenuFile->Append( menuItem );

    m_MenuFile->AppendSeparator();

    m_MenuFile->Append( AssetEditorIDs::OpenSession, "Open Session" );
    m_MenuFile->Append( AssetEditorIDs::SaveSession, "Save Session" );
    m_MenuFile->Append( AssetEditorIDs::SaveSessionAs, "Save Session As..." );

    m_MenuFile->AppendSeparator();

    m_MenuFile->Append( wxID_CLOSE, "Close" );

    m_MenuFile->AppendSeparator();

    m_MenuFile->Append( wxID_EXIT, "Exit" );
    menuBar->Append( m_MenuFile, "File" );


    // Edit menu

    menuItem = new wxMenuItem( m_MenuEdit, wxID_UNDO, "Undo\tCtrl-z" );
    menuItem->SetBitmap( wxArtProvider::GetBitmap( wxART_UNDO ) );
    m_MenuEdit->Append( menuItem );

    menuItem = new wxMenuItem( m_MenuEdit, wxID_REDO, "Redo\tCtrl-Shift-z" );
    menuItem->SetBitmap( wxArtProvider::GetBitmap( wxART_REDO ) );
    m_MenuEdit->Append( menuItem );

    m_MenuEdit->AppendSeparator();

    menuItem = new wxMenuItem( m_MenuEdit, wxID_CUT, "Cut\tCtrl-x" );
    menuItem->SetBitmap( wxArtProvider::GetBitmap( wxART_CUT ) );
    m_MenuEdit->Append( menuItem );

    menuItem = new wxMenuItem( m_MenuEdit, wxID_COPY, "Copy\tCtrl-c" );
    menuItem->SetBitmap( wxArtProvider::GetBitmap( wxART_COPY ) );
    m_MenuEdit->Append( menuItem );

    menuItem = new wxMenuItem( m_MenuEdit, wxID_PASTE, "Paste\tCtrl-v" );
    menuItem->SetBitmap( wxArtProvider::GetBitmap( wxART_PASTE ) );
    m_MenuEdit->Append( menuItem );

    m_MenuEdit->AppendSeparator();

    menuItem = new wxMenuItem( m_MenuEdit, AssetEditorIDs::MoveUp, "Move Up\tAlt-UP" );
    menuItem->SetBitmap( Nocturnal::GlobalImageManager().GetBitmap( "actions/go-up.png" ) );
    m_MenuEdit->Append( menuItem );

    menuItem = new wxMenuItem( m_MenuEdit, AssetEditorIDs::MoveDown, "Move Down\tAlt-DOWN" );
    menuItem->SetBitmap( Nocturnal::GlobalImageManager().GetBitmap( "actions/go-down.png" ) );
    m_MenuEdit->Append( menuItem );

    menuBar->Append( m_MenuEdit, "Edit" );

    // View menu

    m_MenuView->Append( AssetEditorIDs::ExpandAll, "Expand All" );
    m_MenuView->Append( AssetEditorIDs::CollapseAll, "Collapse All" );

    menuBar->Append( m_MenuView, "View" );

    // Create menu items for all the panels so that they can be shown and hidden.  The base
    // class has functions to do this for us.
    CreatePanelsMenu( m_MenuPanels );
    menuBar->Append( m_MenuPanels, "Panels" );

    // Options menu
    wxMenuItem* fileNameOnly = new wxMenuItem( m_MenuFilePathOptions, wxID_ANY, wxString( wxT("Filename only") ) , wxEmptyString, wxITEM_RADIO );
    wxMenuItem* fileNameAndExt = new wxMenuItem( m_MenuFilePathOptions, wxID_ANY, wxString( wxT("Filename + ext") ) , wxEmptyString, wxITEM_RADIO );
    wxMenuItem* partialPath = new wxMenuItem( m_MenuFilePathOptions, wxID_ANY, wxString( wxT("Partial path") ) , wxEmptyString, wxITEM_RADIO );
    wxMenuItem* fullPath = new wxMenuItem( m_MenuFilePathOptions, wxID_ANY, wxString( wxT("Full path") ) , wxEmptyString, wxITEM_RADIO );

    m_MenuItemToFilePathOption[ fileNameOnly->GetId() ] = FilePathOptions::Basename;
    m_MenuItemToFilePathOption[ fileNameAndExt->GetId() ] = FilePathOptions::Filename;
    m_MenuItemToFilePathOption[ partialPath->GetId() ] = FilePathOptions::PartialPath;
    m_MenuItemToFilePathOption[ fullPath->GetId() ] = FilePathOptions::FullPath;

    m_MenuFilePathOptions->Append( fileNameOnly );
    m_MenuFilePathOptions->Append( fileNameAndExt );
    m_MenuFilePathOptions->Append( partialPath );
    m_MenuFilePathOptions->Append( fullPath );

    Connect( fileNameOnly->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( AssetEditor::OnFileOption ), NULL, this );
    Connect( fileNameAndExt->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( AssetEditor::OnFileOption ), NULL, this );
    Connect( partialPath->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( AssetEditor::OnFileOption ), NULL, this );
    Connect( fullPath->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( AssetEditor::OnFileOption ), NULL, this );

    // File path menu option
    m_MenuOptions->Append( -1, wxT("File Path"), m_MenuFilePathOptions );
    FilePathOption filePathOption = FilePathOptions::PartialPath;
    GetAssetEditorPreferences()->GetEnum( GetAssetEditorPreferences()->FilePathOption(), filePathOption );
    fileNameOnly->Check( filePathOption == FilePathOptions::Basename );
    fileNameAndExt->Check( filePathOption == FilePathOptions::Filename );
    partialPath->Check( filePathOption == FilePathOptions::PartialPath );
    fullPath->Check( filePathOption == FilePathOptions::FullPath );

    menuBar->Append( m_MenuOptions, "Options" );

    // Help menu
    wxMenu* helpMenu = new wxMenu();
    helpMenu->Append( wxID_HELP_INDEX, "Index" );
    helpMenu->Append( wxID_HELP_SEARCH, "Search" );
    menuBar->Append( helpMenu, "Help" );

    // Status bar and menu bar
    CreateStatusBar();
    GetStatusBar()->PushStatusText( "Ready" );
    SetMenuBar( menuBar );

    // Enumerator to fill out the property panel
    m_Enumerator = new Enumerator( &m_PropertyCanvas );
    m_PropertiesManager = new PropertiesManager( m_Enumerator );

    // Add listeners
    m_Enumerator->AddPropertyChangingListener( Inspect::ChangingSignature::Delegate( this, &AssetEditor::PropertyChanging ) );
    m_Enumerator->AddPropertyChangedListener( Inspect::ChangedSignature::Delegate(this, &AssetEditor::PropertyChanged));
    m_PropertiesManager->AddPropertiesCreatedListener( PropertiesCreatedSignature::Delegate( this, &AssetEditor::OnPropertiesCreated ) );
    m_AssetManager.GetUndoQueue().AddCommandPushedListener( Undo::QueueChangeSignature::Delegate( this, &AssetEditor::UndoQueueChanged ) );
    m_AssetManager.GetUndoQueue().AddUndoneListener( Undo::QueueChangeSignature::Delegate( this, &AssetEditor::UndoQueueChanged ) );
    m_AssetManager.GetUndoQueue().AddRedoneListener( Undo::QueueChangeSignature::Delegate( this, &AssetEditor::UndoQueueChanged ) );
    m_AssetManager.GetUndoQueue().AddResetListener( Undo::QueueChangeSignature::Delegate( this, &AssetEditor::UndoQueueChanged ) );
    m_AssetManager.GetSelection().AddChangedListener( SelectionChangedSignature::Delegate( this, &AssetEditor::SelectionChanged ) );
    m_AssetManager.AddAssetLoadedListener( AssetLoadSignature::Delegate( this, &AssetEditor::AssetLoaded ) );
    m_AssetManager.AddAssetUnloadingListener( AssetLoadSignature::Delegate( this, &AssetEditor::AssetUnloading ) );

    // Restore layout if any
    GetAssetEditorPreferences()->GetAssetEditorWindowSettings()->ApplyToWindow( this, &m_FrameManager, true );

    // Disable certain toolbar buttons (they'll enable when appropriate)
    m_MainToolBar->EnableTool( wxID_UNDO, false );
    m_MainToolBar->EnableTool( wxID_REDO, false );
    m_MainToolBar->EnableTool( wxID_PASTE, false );
    m_MainToolBar->EnableTool( wxID_SAVE, false );
    m_MainToolBar->EnableTool( AssetEditorIDs::SaveAllAssetClasses, false );




    // The rest of the toolbar buttons are updated by this function which uses
    // the current selection to decide what buttons to enable.
    UpdateUIElements();
}
///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
AssetEditor::~AssetEditor()
{
    std::vector< std::string > mruPaths;
    m_MRU->ToVector( mruPaths );
    GetAssetEditorPreferences()->GetMRU()->SetPaths( mruPaths );
    GetAssetEditorPreferences()->SavePreferences();

    GetStatusBar()->PopStatusText();

    // Remove listeners
    m_Enumerator->RemovePropertyChangingListener( Inspect::ChangingSignature::Delegate( this, &AssetEditor::PropertyChanging ) );
    m_Enumerator->RemovePropertyChangedListener( Inspect::ChangedSignature::Delegate( this, &AssetEditor::PropertyChanged));
    m_PropertiesManager->RemovePropertiesCreatedListener( PropertiesCreatedSignature::Delegate( this, &AssetEditor::OnPropertiesCreated ) );
    m_AssetManager.GetUndoQueue().RemoveCommandPushedListener( Undo::QueueChangeSignature::Delegate( this, &AssetEditor::UndoQueueChanged ) );
    m_AssetManager.GetUndoQueue().RemoveUndoneListener( Undo::QueueChangeSignature::Delegate( this, &AssetEditor::UndoQueueChanged ) );
    m_AssetManager.GetUndoQueue().RemoveRedoneListener( Undo::QueueChangeSignature::Delegate( this, &AssetEditor::UndoQueueChanged ) );
    m_AssetManager.GetUndoQueue().RemoveResetListener( Undo::QueueChangeSignature::Delegate( this, &AssetEditor::UndoQueueChanged ) );
    m_AssetManager.GetSelection().RemoveChangedListener( SelectionChangedSignature::Delegate( this, &AssetEditor::SelectionChanged ) );
    m_AssetManager.RemoveAssetLoadedListener( AssetLoadSignature::Delegate( this, &AssetEditor::AssetLoaded ) );
    m_AssetManager.RemoveAssetUnloadingListener( AssetLoadSignature::Delegate( this, &AssetEditor::AssetUnloading ) );
    m_MRU->RemoveItemSelectedListener( Nocturnal::MRUSignature::Delegate( this, &AssetEditor::MRUOpen ) );

    delete m_Outliner;

    m_Outliner = NULL; 
}

const char* GetFileFilter()
{
    return s_FileFilter;
}

///////////////////////////////////////////////////////////////////////////////
// Return the asset manager so that other UI elements can use it.
// 
Luna::AssetManager* AssetEditor::GetAssetManager()
{
    return &m_AssetManager;
}

///////////////////////////////////////////////////////////////////////////////
// Opens a file and displays any errors that occur to the user.
// 
bool AssetEditor::Open( const std::string& file )
{
    std::set< std::string > fileList;
    fileList.insert( file );
    return DoOpen( fileList );
}

///////////////////////////////////////////////////////////////////////////////
// Save current window position settings.
// 
void AssetEditor::SaveWindowState()
{
    GetAssetEditorPreferences()->GetAssetEditorWindowSettings()->SetFromWindow( this, &m_FrameManager );
}

///////////////////////////////////////////////////////////////////////////////
// Returns the document manager.
// 
DocumentManager* AssetEditor::GetDocumentManager()
{
    return &m_AssetManager;
}

///////////////////////////////////////////////////////////////////////////////
// Displays the attribute chooser dialog, allowing the user to add attributes
// to the selected asset classes.
// 
void AssetEditor::PromptAddComponents( const ContextMenuArgsPtr& args )
{
    AssetPreferences* preferences = GetAssetEditorPreferences();
    WindowSettings* settings = preferences->GetComponentChooserDlgWindowSettings();
    ComponentChooserDlg dlg( this );
    settings->ApplyToWindow( &dlg );
    dlg.ShowModal();
    settings->SetFromWindow( &dlg );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the user chooses to delete attributes.  Runs through the
// selection list and creates an undoable batch command that deletes all of
// the selected attributes.
// 
void AssetEditor::RemoveSelectedComponents( const ContextMenuArgsPtr& args )
{
    S_ComponentSmartPtr attributesToDelete;
    if ( m_AssetManager.GetSelectedComponents( attributesToDelete ) > 0 )
    {
        args->GetBatch()->Push( m_AssetManager.GetSelection().Clear() );
        args->GetBatch()->Push( RemoveComponents( attributesToDelete ) );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the Preview context menu item is pressed.  Displays the
// currently selected entity in the AssetPreviewWindow using Renderlib
// 
void AssetEditor::OnAssetPreview( const ContextMenuArgsPtr& args )
{
    PreviewSelectedItem();
}

///////////////////////////////////////////////////////////////////////////////
// Callback to expand the selected assets.
// 
void AssetEditor::OnExpandSelectedAssets( const ContextMenuArgsPtr& args )
{
    ExpandSelectedAssets( true );
}

///////////////////////////////////////////////////////////////////////////////
// Callback to collapse the selected assets.
// 
void AssetEditor::OnCollapseSelectedAssets( const ContextMenuArgsPtr& args )
{
    ExpandSelectedAssets( false );
}

///////////////////////////////////////////////////////////////////////////////
// Returns an undoable command that removes all the specified attributes
// from their respective asset classes.  Returns the number of attributes 
// removed.
// 
Undo::CommandPtr AssetEditor::RemoveComponents( const S_ComponentSmartPtr& attributesToDelete )
{
    if ( attributesToDelete.size() > 0 && m_AssetManager.IsEditable() )
    {
        Undo::BatchCommandPtr batch = new Undo::BatchCommand ();

        // Remove each of the attributes from their respective Luna::AssetClass
        S_ComponentSmartPtr::const_iterator attrItr = attributesToDelete.begin();
        S_ComponentSmartPtr::const_iterator attrEnd = attributesToDelete.end();
        for ( ; attrItr != attrEnd; ++attrItr )
        {
            const ComponentWrapperPtr& attribute = *attrItr;
            if ( m_AssetManager.IsEditable( attribute->GetAssetClass() ) )
            {
                batch->Push( new ComponentExistenceCommand( Undo::ExistenceActions::Remove, attribute->GetAssetClass(), attribute ) );
            }
        }

        return batch;
    }

    return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when an item is selected from the MRU menu.  Opens the 
// specified item.
// 
void AssetEditor::MRUOpen( const Nocturnal::MRUArgs& args )
{
    if ( !Open( args.m_Item ) )
    {
        m_MRU->Remove( args.m_Item );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Callback when a property in the enumerator has changed, which requires us to
// put an undo command on the stack.
// 
bool AssetEditor::PropertyChanging( const Inspect::ChangingArgs& args )
{
    if ( args.m_Preview )
    {
        return true;
    }

    if ( args.m_Control->GetData().ReferencesObject() )
    {
        // This class is a fix for a crash when using Undo in the asset editor.
        // The data that was being referenced in the command was being removed 
        // because we did not have a ref counted pointer to the memory. We wrapped
        // the reference in this class to ensure that it persists until we are done.

        // this class aggregates a strong references to the interpreter object
        //  this object in turn stores strong references to the serializer objects
        //  that the data objects need to properly interface with the property data
        class InterpreterCommand : public Undo::Command
        {
        public:
            InterpreterCommand( Undo::CommandPtr c, Inspect::InterpreterPtr i )
                : m_Command ( c )
                , m_Interpreter ( i )
            {

            }

            virtual void Undo() NOC_OVERRIDE
            {
                return m_Command->Undo();
            }

            virtual void Redo() NOC_OVERRIDE
            {
                return m_Command->Redo();
            }

            virtual bool IsSignificant() const NOC_OVERRIDE
            {
                return m_Command->IsSignificant();
            }

        private:
            Undo::CommandPtr m_Command;
            Inspect::InterpreterPtr m_Interpreter;
        };
        Undo::CommandPtr command = new InterpreterCommand( args.m_Control->GetData()->GetUndoCommand(), args.m_Control->GetInterpreter() );

        if ( command )
        {
            return m_AssetManager.Push( command );
        }
    }

    return m_AssetManager.IsEditable();
}

///////////////////////////////////////////////////////////////////////////////
// Notifies all selected asset classes that a control in the enumerator changed.
// 
void AssetEditor::PropertyChanged( const Inspect::ChangeArgs& args )
{
    S_AssetClassDumbPtr assetClasses;
    if ( m_AssetManager.GetSelectedAssets( assetClasses ) > 0 )
    {
        for each ( Luna::AssetClass* assetClass in assetClasses )
        {
            assetClass->Changed( args.m_Control );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a command is undone or redone on the undo queue.  Updates
// the UI.
// 
void AssetEditor::UndoQueueChanged( const Undo::QueueChangeArgs& args )
{
    m_MainToolBar->EnableTool( wxID_UNDO, m_AssetManager.GetUndoQueue().CanUndo() );
    m_MainToolBar->EnableTool( wxID_REDO, m_AssetManager.GetUndoQueue().CanRedo() );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when selection has changed.  Updates the properties panel.
// 
void AssetEditor::SelectionChanged( const OS_SelectableDumbPtr& selection )
{
    m_PropertiesManager->SetSelection( selection );

    UpdateUIElements();

    std::ostringstream statusMsg;
    const size_t numSelected = selection.Size();
    if ( numSelected == 0 )
    {
        statusMsg << "Ready";
    }
    else if ( numSelected == 1 )
    {
        statusMsg << "1 item selected";
    }
    else
    {
        statusMsg << numSelected << " items selected";
    }

    DoGiveHelp( statusMsg.str().c_str(), true );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when an asset is loaded.  Listens for modification changes to 
// the asset so that UI can be adjusted accordingly.
// 
void AssetEditor::AssetLoaded( const AssetLoadArgs& args )
{
    AssetDocument* doc = m_AssetManager.FindAssetDocument( args.m_AssetClass );
    doc->AddDocumentModifiedListener( DocumentChangedSignature::Delegate( this, &AssetEditor::DocumentModified ) );
    doc->AddDocumentSavedListener( DocumentChangedSignature::Delegate( this, &AssetEditor::DocumentSaved ) );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when an asset is unloaded.  Removes listeners.
// 
void AssetEditor::AssetUnloading( const AssetLoadArgs& args )
{
    AssetDocument* doc = m_AssetManager.FindAssetDocument( args.m_AssetClass );
    doc->RemoveDocumentModifiedListener( DocumentChangedSignature::Delegate( this, &AssetEditor::DocumentModified ) );
    doc->RemoveDocumentSavedListener( DocumentChangedSignature::Delegate( this, &AssetEditor::DocumentSaved ) );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a file is modified.  Updates the save buttons accordingly.
// 
void AssetEditor::DocumentModified( const DocumentChangedArgs& args )
{
    Luna::AssetClass* assetClass = m_AssetManager.FindAsset( args.m_Document->GetHash() );

    if ( assetClass )
    {
        S_AssetClassDumbPtr selectedAssets;
        m_AssetManager.GetSelectedAssets( selectedAssets );
        if ( selectedAssets.find( assetClass ) != selectedAssets.end() )
        {
            m_MainToolBar->EnableTool( wxID_SAVE, true );
        }

        m_MainToolBar->EnableTool( AssetEditorIDs::SaveAllAssetClasses, true );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a document is saved.  Updates the save buttons.
// 
void AssetEditor::DocumentSaved( const DocumentChangedArgs& args )
{
    // We have to iterate over the selection, so we might as well update all 
    // of the UI while we are at it.
    UpdateUIElements();
}

///////////////////////////////////////////////////////////////////////////////
// Runs the specified wizard and returns the newly created asset file (or NULL
// if the wizard was cancelled or an error occurred).
// 
//AssetDocument* AssetEditor::RunCreateAssetWizard( ::AssetManager::CreateAssetWizard& wizard )
//{
//    AssetDocumentPtr doc;
//
//    S_AssetClassDumbPtr selected;
//    m_AssetManager.GetSelectedAssets( selected );
//
//#pragma TODO( "Offer the user a dialog of possibilities when there's more than one?" )
//    if ( selected.size() > 0 )
//    {
//        Luna::AssetClass* assetClass = *( selected.begin() );
//        wizard.SetSelectedAssetPath( assetClass->GetFilePath() );
//    }
//    //else if ( selected.size() > 1 )
//    //{
//    //Luna::AssetClass* assetClass = *( selected.begin() );
//    //wizard.SetSelectedAssetPath( assetClass->GetFilePath() );
//
//    //std::string path;
//    //
//    //S_AssetClassDumbPtr::iterator itr = selected.begin();
//    //S_AssetClassDumbPtr::iterator itrEnd = selected.end();
//    //for ( ; itr != itrEnd ; ++itr )
//    //{
//    //  Luna::AssetClass* assetClass = *( itr );
//    //  path = assetClass->GetFilePath();
//    //}
//    ////...
//    //if ( !path.empty() )
//    //{
//    //  wizard.SetSelectedAssetPath( path );
//    //} 
//    //}
//
//    if ( wizard.Run() )
//    {
//        wxBusyCursor bc;
//
//        Asset::AssetClassPtr package = wizard.GetAssetClass();
//        if ( !package.ReferencesObject() )
//        {
//            Log::Error( "CreateAssetWizard returned a NULL asset when attempting to create new asset at location %s.", wizard.GetNewFileLocation().c_str() );
//            return NULL;
//        }
//
//        std::string error;
//        Luna::AssetClass* asset = m_AssetManager.Open( package->GetFilePath(), error, true );
//        if ( !asset )
//        {
//            Log::Error( "Unable to create new asset at location %s.", wizard.GetNewFileLocation().c_str() );
//            return NULL;
//        }
//
//        m_MRU->Insert( asset->GetFilePath() );
//    }
//
//    return doc.Ptr();
//}

///////////////////////////////////////////////////////////////////////////////
// Sets the clipboard to contain the specified Reflect data.  Returns true if
// successful.
// 
bool AssetEditor::ToClipboard( const Inspect::ReflectClipboardDataPtr& clipboardData )
{
    bool result = false;
    if ( clipboardData.ReferencesObject() )
    {
        if ( wxTheClipboard->Open() )
        {
            // Will be owned by the clipboard
            Inspect::ClipboardDataObject* dataObject = new Inspect::ClipboardDataObject();
            dataObject->ToBuffer( clipboardData );
            if ( wxTheClipboard->SetData( dataObject ) )
            {
                DoGiveHelp( "Copied data to clipboard", true );
                m_MainToolBar->EnableTool( wxID_PASTE, true );
                result = true;
            }
            else
            {
                DoGiveHelp( "Error: Unable to copy data to the clipboard", true );
            }
            wxTheClipboard->Close();
        }
        else
        {
            DoGiveHelp( "Error: Unable to open the clipboard", true );
        }
    }
    else
    {
        DoGiveHelp( "Nothing to copy!", true );
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////
// Fetches data from the clipboard if any is available.
// 
Inspect::ReflectClipboardDataPtr AssetEditor::FromClipboard()
{
    if ( wxTheClipboard->Open() )
    {
        Inspect::ClipboardDataObject dataObject;
        if ( wxTheClipboard->GetData( dataObject ) )
        {
            Inspect::ReflectClipboardDataPtr clipboardData = dataObject.FromBuffer();
            if ( !clipboardData.ReferencesObject() )
            {
                DoGiveHelp( "The clipboard does not contain valid data!", true );
            }
            return clipboardData;
        }
        else
        {
            DoGiveHelp( "The clipboard does not contain any data!", true );
        }
    }
    else
    {
        DoGiveHelp( "Error: Unable to open the clipboard", true );
    }

    return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Iterates over the current selection and enables/disables toolbar buttons
// accordingly.
// 
void AssetEditor::UpdateUIElements()
{
    bool previewable = false;
    bool buildable = true;
    bool viewable = true;
    bool exportable = true;
    bool canSyncShaders = true;
    bool isEntity = true;
    bool canSaveSelection = false;

    const OS_SelectableDumbPtr selectionItems = m_AssetManager.GetSelection().GetItems();
    if ( selectionItems.Size() )
    {
        Luna::AssetNode* node = Reflect::ObjectCast< Luna::AssetNode >( selectionItems.Front() );
        if ( node )
        {
            Luna::AssetClass* asset = node->GetAssetClass();
            if ( asset )
            {
                //std::vector< std::string > staticContentFiles;
                //::AssetManager::GetStaticContentFiles( asset->GetFileID(), staticContentFiles );
                //if ( staticContentFiles.size() )
                //{
                //    previewable = true;
                //}
            }
        }
    }

    S_AssetClassDumbPtr assets;
    OS_SelectableDumbPtr::Iterator selItr = selectionItems.Begin();
    OS_SelectableDumbPtr::Iterator selEnd = selectionItems.End();
    for ( ; selItr != selEnd; ++selItr )
    {
        Luna::AssetNode* node = Reflect::AssertCast< Luna::AssetNode >( *selItr );

        Luna::AssetClass* asset = node->GetAssetClass();
        if ( asset )
        {
            Nocturnal::Insert< S_AssetClassDumbPtr >::Result inserted = assets.insert( asset );
            if ( inserted.second )
            {
                buildable &= asset->IsBuildable();
                viewable &= asset->IsViewable();
                exportable &= asset->IsExportable();
                isEntity &= asset->GetPackage< Asset::AssetClass >()->HasType( Reflect::GetType< Asset::EntityAsset >() );

                Component::ComponentViewer< Asset::ArtFileComponent > model ( asset->GetPackage< Asset::AssetClass >() );
                canSyncShaders &= model.Valid();
                canSyncShaders &= isEntity;

                // Check to see if the selected item needs to be saved (until we find one that does)
                if ( !canSaveSelection && m_AssetManager.FindAssetDocument( asset )->IsModified() )
                {
                    canSaveSelection = true;
                }
            }
        }
    }

    // Check all open documents to see if any need to be saved.
    bool doAnyDocsNeedSaved = canSaveSelection;
    if ( !doAnyDocsNeedSaved )
    {
        OS_DocumentSmartPtr::Iterator docItr = m_AssetManager.GetDocuments().Begin();
        OS_DocumentSmartPtr::Iterator docEnd = m_AssetManager.GetDocuments().End();
        for ( ; docItr != docEnd; ++docItr )
        {
            if ( ( *docItr )->IsModified() )
            {
                doAnyDocsNeedSaved = true;
                break;
            }
        }
    }

    // Main toolbar
    bool canCopy = true;
    bool canCut = true;
    m_AssetManager.CanCopySelection( canCopy, canCut );
    m_MainToolBar->EnableTool( wxID_CUT, canCut );
    m_MainToolBar->EnableTool( wxID_COPY, canCopy );
    m_MainToolBar->EnableTool( wxID_SAVE, canSaveSelection );
    m_MainToolBar->EnableTool( AssetEditorIDs::SaveAllAssetClasses, doAnyDocsNeedSaved );

    // Asset Toolbar
    bool enableBuild = buildable && assets.size() > 0;
    bool enableView = viewable && assets.size() == 1;
    bool enableExport = exportable && assets.size() > 0;
    bool enableSyncShaders = canSyncShaders && assets.size() > 0;
    m_MainToolBar->EnableTool( AssetEditorIDs::Preview, previewable );
    m_MainToolBar->EnableTool( AssetEditorIDs::Build, enableBuild );
    m_MainToolBar->EnableTool( AssetEditorIDs::View, enableView );
    m_MainToolBar->EnableTool( AssetEditorIDs::Export, enableExport );
    m_MainToolBar->EnableTool( AssetEditorIDs::SyncShaders, enableSyncShaders );

}

///////////////////////////////////////////////////////////////////////////////
// Helper function to handle common code for opening one or more files.
// 
bool AssetEditor::DoOpen( const std::set< std::string >& files )
{
    std::string errorList;
    std::set< std::string >::const_iterator fileItr = files.begin();
    std::set< std::string >::const_iterator fileEnd = files.end();
    for ( ; fileItr != fileEnd; ++fileItr )
    {
        wxBusyCursor wait;
        Nocturnal::Path path( *fileItr );
        if ( path.Exists() )
        {
            std::string error;
            Luna::AssetClass* asset = m_AssetManager.Open( *fileItr, error, true );
            if ( asset )
            {
                m_MRU->Insert( *fileItr );
            }
            else
            {
                errorList += " o " + *fileItr + " (" + error + ")\n";
            }
        }
        else
        {
            errorList += " o " + *fileItr + " does not exist.\n";
        }
    }

    if ( !errorList.empty() )
    {
        std::string message = "The following errors were encountered while opening files: \n" + errorList;
        wxMessageBox( message.c_str(), "Error", wxCENTER | wxOK | wxICON_ERROR, this );
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the selected items are children of a single 
// ElementArrayNode.  The common parent is returned in the parameter and can
// be used to carry out the move operation.
// 
bool AssetEditor::CanMoveSelectedItems( Luna::AssetNode*& commonParent )
{
    OS_SelectableDumbPtr::Iterator selItr = m_AssetManager.GetSelection().GetItems().Begin();
    OS_SelectableDumbPtr::Iterator selEnd = m_AssetManager.GetSelection().GetItems().End();
    for ( ; selItr != selEnd; ++selItr )
    {
        Luna::AssetNode* current = Reflect::TryCast< Luna::AssetNode >( *selItr );
        if ( commonParent == NULL )
        {
            commonParent = current->GetParent();
            if ( !commonParent || !commonParent->HasType( Reflect::GetType< Luna::ElementArrayNode >() ) )
            {
                return false;
            }
        }
        else
        {
            if ( commonParent != current->GetParent() )
            {
                return false;
            }
        }
    }
    return commonParent != NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Displays the currently selected entity in the AssetPreviewWindow using
// Renderlib
// 
void AssetEditor::PreviewSelectedItem()
{
    m_AssetPreviewWindow->RemoveScene();

    Luna::AssetNode* node = Reflect::ObjectCast< Luna::AssetNode >( m_AssetManager.GetSelection().GetItems().Front() );
    NOC_ASSERT( node );

    Luna::AssetClass* assetClass = node->GetAssetClass();
    NOC_ASSERT( assetClass );

#pragma TODO( "allow preview" )
    //std::vector< std::string > staticContentFiles;
    //::AssetManager::GetStaticContentFiles( assetClass->GetFileID(), staticContentFiles );
    //NOC_ASSERT( staticContentFiles.size() );

    //m_AssetPreviewWindow->SetupScene( staticContentFiles[ 0 ] );

    //wxAuiPaneInfo& pane = m_FrameManager.GetPane( m_AssetPreviewWindow );
    //if ( pane.IsOk() && !pane.IsShown() )
    //{
    //    pane.Show( true );
    //    m_FrameManager.Update();
    //    UpdatePanelsMenu( m_MenuPanels );
    //}
}

///////////////////////////////////////////////////////////////////////////////
// Called when a menu is about to be opened.  Enables and disables items in the
// menu as appropriate.
// 
void AssetEditor::OnMenuOpen( wxMenuEvent& args )
{
    args.Skip();
    if ( args.GetMenu() == m_MenuFile )
    {
        m_MenuFile->Enable( wxID_SAVE, m_MainToolBar->GetToolEnabled( wxID_SAVE ) );
        m_MenuFile->Enable( AssetEditorIDs::SaveAllAssetClasses, m_MainToolBar->GetToolEnabled( AssetEditorIDs::SaveAllAssetClasses ) );
        m_MenuFile->Enable( m_MenuItemOpenRecent->GetId(), !m_MRU->GetItems().Empty() );
        m_MRU->PopulateMenu( m_MenuMRU );
    }
    else if ( args.GetMenu() == m_MenuEdit )
    {
        m_MenuEdit->Enable( wxID_UNDO, m_AssetManager.GetUndoQueue().CanUndo() );
        m_MenuEdit->Enable( wxID_REDO, m_AssetManager.GetUndoQueue().CanRedo() );
        m_MenuEdit->Enable( wxID_CUT, m_MainToolBar->GetToolEnabled( wxID_CUT ) );
        m_MenuEdit->Enable( wxID_COPY, m_MainToolBar->GetToolEnabled( wxID_COPY ) );
        m_MenuEdit->Enable( wxID_PASTE, m_MainToolBar->GetToolEnabled( wxID_PASTE ) );

        Luna::AssetNode* unused = NULL;
        bool canMoveUpDown = CanMoveSelectedItems( unused );
        m_MenuEdit->Enable( AssetEditorIDs::MoveUp, canMoveUpDown );
        m_MenuEdit->Enable( AssetEditorIDs::MoveDown, canMoveUpDown );
    }
    else if ( args.GetMenu() == m_MenuPanels )
    {
        UpdatePanelsMenu( m_MenuPanels );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Callback when the user requests to create a new asset.  Creates the asset
// of the type that was chosen.
// 
void AssetEditor::OnNewAsset( wxCommandEvent& args )
{
    std::map< i32, i32 >::const_iterator found = m_MenuItemToAssetType.find( args.GetId() );
    NOC_ASSERT( found != m_MenuItemToAssetType.end() );

    const i32 assetTypeID = found->second;

#pragma TODO( "reimplement asset creation" )
    NOC_BREAK();
    
    //    ::AssetManager::CreateAssetWizard wizard( this, assetTypeID );
    //    RunCreateAssetWizard( wizard );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the user clicks the "new" button the toolbar.  Displays
// the list of assets that can be created, allowing the user to select one.
// 
void AssetEditor::OnNew( wxCommandEvent& args )
{
    PopupMenu( m_MenuNew );
}

///////////////////////////////////////////////////////////////////////////////
// Callback when the user causes a UI event to open a file.  Prompts for the
// file to open, and opens it.
// 
void AssetEditor::OnOpen( wxCommandEvent& args )
{
    Nocturnal::FileDialog browserDlg( this, "Open", "", "", "", Nocturnal::FileDialogStyles::DefaultOpen | Nocturnal::FileDialogStyles::Multiple );

    std::set< std::string > filters;
    Reflect::Archive::GetFileFilters( filters );
    for ( std::set< std::string >::const_iterator itr = filters.begin(), end = filters.end(); itr != end; ++itr )
    {
        browserDlg.AddFilter( (*itr) );
    }

    if ( browserDlg.ShowModal() == wxID_OK )
    {
        const std::set< std::string >& paths = browserDlg.GetFilePaths();
        DoOpen( paths );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Callback when the user causes a UI event to find a file. 
// creates a FileBrowser dialog.
// 
void AssetEditor::OnFind( wxCommandEvent& args )
{
    Nocturnal::FileDialog browserDlg( this, "Open", "", "", "", Nocturnal::FileDialogStyles::DefaultOpen | Nocturnal::FileDialogStyles::Multiple );

    std::set< std::string > filters;
    Reflect::Archive::GetFileFilters( filters );
    for ( std::set< std::string >::const_iterator itr = filters.begin(), end = filters.end(); itr != end; ++itr )
    {
        browserDlg.AddFilter( (*itr) );
    }

    if ( browserDlg.ShowModal() == wxID_OK )
    {
        const std::set< std::string >& paths = browserDlg.GetFilePaths();
        DoOpen( paths );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the user chooses to sort the currently open files (sorting
// is alphabetical).
// 
void AssetEditor::OnSortFiles( wxCommandEvent& args )
{
    m_Outliner->Sort( Nocturnal::SortTreeCtrl::InvalidItemId, false );
}

///////////////////////////////////////////////////////////////////////////////
// Closes the currently selected assets.
// 
void AssetEditor::OnClose( wxCommandEvent& args )
{
    m_AssetManager.CloseSelected();
}

///////////////////////////////////////////////////////////////////////////////
// Called when a user selects "Exit" from the File menu. Creates a close window
// event which should be processed by the OnExisting Luna::Editor function.
// 
void AssetEditor::OnExit( wxCommandEvent& args )
{
    wxCloseEvent closeEvent( wxEVT_CLOSE_WINDOW );
    GetEventHandler()->AddPendingEvent( closeEvent );
}

///////////////////////////////////////////////////////////////////////////////
// Overridden to prevent crashes related to the Property Panel.
// 
void AssetEditor::OnExiting( wxCloseEvent& args )
{
    // Force the property panel to release focus
    SetFocus();

    // Clear the selection
    m_AssetManager.GetSelection().Clear();

    // Close all open documents
    if ( !m_AssetManager.CloseAll() )
    {
        if ( args.CanVeto() )
        {
            args.Veto();
            return;
        }
    }

    // Let default implementation run as well
    args.Skip();
}

///////////////////////////////////////////////////////////////////////////////
// Iterates over all the selected asset classes and saves each one. 
// 
void AssetEditor::OnSave( wxCommandEvent& args )
{
    SetFocus();

    std::string error;
    if ( !m_AssetManager.SaveSelected( error ) )
    {
        wxMessageBox( error.c_str(), "Error", wxCENTER | wxICON_ERROR | wxOK, this );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Saves all currently open assets in need of saving.
// 
void AssetEditor::OnSaveAll( wxCommandEvent& args )
{
    SetFocus();

    std::string error;
    if ( !m_AssetManager.SaveAll( error ) )
    {
        wxMessageBox( error.c_str(), "Error", wxCENTER | wxICON_ERROR | wxOK, this );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Called when the user wants to expand all assets
// 
void AssetEditor::OnExpandAll( wxCommandEvent& args )
{
    m_Outliner->ExpandAll( true );
}

///////////////////////////////////////////////////////////////////////////////
// Called when the user wants to collapse all assets
// 
void AssetEditor::OnCollapseAll( wxCommandEvent& args )
{
    m_Outliner->ExpandAll( false );
}

///////////////////////////////////////////////////////////////////////////////
// Called when the UI initiates an Undo operation.
// 
void AssetEditor::OnUndo( wxCommandEvent& args )
{
    if ( m_AssetManager.GetUndoQueue().CanUndo() )
    {
        m_AssetManager.GetUndoQueue().Undo();
        m_PropertyCanvas.Read();
    }
}

///////////////////////////////////////////////////////////////////////////////
// Called when the UI initiates a Redo operation.  
// 
void AssetEditor::OnRedo( wxCommandEvent& args )
{
    if ( m_AssetManager.GetUndoQueue().CanRedo() )
    {
        m_AssetManager.GetUndoQueue().Redo();
        m_PropertyCanvas.Read();
    }
}

///////////////////////////////////////////////////////////////////////////////
// Called when the Cut button or menu item is selected.
// 
void AssetEditor::OnCut( wxCommandEvent& args )
{
    S_AssetNodeDumbPtr parents;
    bool canBeMoved = true;
    Inspect::ReflectClipboardDataPtr clipboardData = m_AssetManager.CopySelection( parents, canBeMoved );
    if ( canBeMoved && ToClipboard( clipboardData ) )
    {
        Undo::BatchCommandPtr batch = new Undo::BatchCommand();
        for each ( Luna::AssetNode* parent in parents )
        {
            batch->Push( parent->DeleteSelectedChildren() );
        }
        m_AssetManager.Push( batch );

        // HACK
        m_AssetManager.ClearUndoQueue();
    }
}

///////////////////////////////////////////////////////////////////////////////
// Called when the Copy button or menu item is selected.
// 
void AssetEditor::OnCopy( wxCommandEvent& args )
{
    S_AssetNodeDumbPtr unusedParents;
    bool unusedMoveFlag = false;
    Inspect::ReflectClipboardDataPtr clipboardData = m_AssetManager.CopySelection( unusedParents, unusedMoveFlag );
    ToClipboard( clipboardData );
}

///////////////////////////////////////////////////////////////////////////////
// Called when the Paste button or menu item is selected.
// 
void AssetEditor::OnPaste( wxCommandEvent& args )
{
    Inspect::ReflectClipboardDataPtr clipboardData = FromClipboard();
    if ( clipboardData.ReferencesObject() )
    {
        bool handled = false;
        Undo::BatchCommandPtr batch = new Undo::BatchCommand();
        OS_SelectableDumbPtr::Iterator selItr = m_AssetManager.GetSelection().GetItems().Begin();
        OS_SelectableDumbPtr::Iterator selEnd = m_AssetManager.GetSelection().GetItems().End();
        for ( ; selItr != selEnd; ++selItr )
        {
            Luna::AssetNode* node = Reflect::ObjectCast< Luna::AssetNode >( *selItr );
            if ( node )
            {
                Luna::AssetClass* assetClass = node->GetAssetClass();
                if ( assetClass->GetAssetManager()->IsEditable( assetClass ) )
                {
                    if ( node->HandleClipboardData( clipboardData, ClipboardOperations::Copy, batch ) )
                    {
                        handled = true;
                    }
                }
            }
        }

        if ( handled )
        {
            m_AssetManager.GetSelection().Refresh();

            if ( !batch->IsEmpty() )
            {
                m_AssetManager.Push( batch );
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for attempting to move the selected items up.
// 
void AssetEditor::OnMoveUp( wxCommandEvent& args )
{
    Luna::AssetNode* parent = NULL;
    if ( CanMoveSelectedItems( parent ) )
    {
        Luna::ElementArrayNode* arrayNode = Reflect::ObjectCast< Luna::ElementArrayNode >( parent );
        if ( arrayNode )
        {
            m_AssetManager.Push( arrayNode->MoveSelectedChildrenUp() );

            // Putting this is to make sure the selected items are visible after a MoveUp/MoveDown
            // This is a temporary hack and to fix it properly we need to not delete the whole list
            // and recreate it every time a MoveUp/MoveDown occurs
            Nocturnal::SortTreeCtrl* treeCtrl = (Nocturnal::SortTreeCtrl*)m_Outliner->GetWindow();
            wxArrayTreeItemIds selections;
            const size_t numSelections = treeCtrl->GetSelections( selections );

            treeCtrl->EnsureVisible( selections[numSelections-1] );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for attempting to move the selected items down.
// 
void AssetEditor::OnMoveDown( wxCommandEvent& args )
{
    Luna::AssetNode* parent = NULL;
    if ( CanMoveSelectedItems( parent ) )
    {
        Luna::ElementArrayNode* arrayNode = Reflect::ObjectCast< Luna::ElementArrayNode >( parent );
        if ( arrayNode )
        {
            m_AssetManager.Push( arrayNode->MoveSelectedChildrenDown() );

            // Putting this is to make sure the selected items are visible after a MoveUp/MoveDown
            // This is a temporary hack and to fix it properly we need to not delete the whole list
            // and recreate it every time a MoveUp/MoveDown occurs
            Nocturnal::SortTreeCtrl* treeCtrl = (Nocturnal::SortTreeCtrl*)m_Outliner->GetWindow();
            wxArrayTreeItemIds selections;
            const size_t numSelections = treeCtrl->GetSelections( selections );

            treeCtrl->EnsureVisible( selections[numSelections-1] );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the Build button is pressed.  Builds all the selected
// assets.
// 
void AssetEditor::OnBuild( wxCommandEvent& args )
{
    S_AssetClassDumbPtr assets;
    if ( m_AssetManager.GetSelectedAssets( assets ) > 0 )
    {
        bool showOptions = wxIsShiftDown();
        bool shouldBuild = false;
        std::string error;
        if ( wxGetApp().GetDocumentManager()->SaveAll( error ) )
        {
            shouldBuild = true;
        }
        else
        {
            shouldBuild = wxMessageBox( "Errors were encountered while trying to save some of the currently open files.  Would you like to build anyway?", "Build anyway?", wxCENTER | wxICON_QUESTION | wxYES_NO, this ) == wxYES;
        }

        if ( shouldBuild )
        {
            std::set< Nocturnal::Path > assetFiles;
            for ( S_AssetClassDumbPtr::iterator itr = assets.begin(), end = assets.end(); itr != end; ++itr )
            {
                assetFiles.insert( (*itr)->GetPath() );
            }
            NOC_BREAK();
#pragma TODO( "Figure out how to handle the idea of 'building'" )
//            Luna::BuildAssets( assetFiles, this, NULL, showOptions );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the Preview button is pressed.  Displays the currently
// selected entity in the AssetPreviewWindow using Renderlib
// 
void AssetEditor::OnPreview( wxCommandEvent& args )
{
    PreviewSelectedItem();
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the "Options->File Path" menu item is updated.  Changes
// how file paths are displayed in the outliner.
// 
void AssetEditor::OnFileOption( wxCommandEvent& args )
{
    M_MenuToFileOption::const_iterator found = m_MenuItemToFilePathOption.find( args.GetId() );
    if ( found != m_MenuItemToFilePathOption.end() )
    {
        FilePathOption filePathOption = found->second;
        GetAssetEditorPreferences()->SetEnum( GetAssetEditorPreferences()->FilePathOption(), filePathOption );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the user chooses help index from the help menu
//
void AssetEditor::OnHelpIndex( wxCommandEvent& args )
{
    Platform::Execute( "cmd.exe /c start http://wiki.insomniacgames.com/index.php/Luna" );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the user chooses help search from the help menu
//
void AssetEditor::OnHelpSearch( wxCommandEvent& args )
{
    Platform::Execute( "cmd.exe /c start http://wiki.insomniacgames.com/index.php/Special:Search" );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the user chooses to checkout the selected files from 
// revision control.
// 
void AssetEditor::OnCheckout( wxCommandEvent& args )
{
    m_AssetManager.CheckOutSelected();
}

///////////////////////////////////////////////////////////////////////////////
// Expands/collapses the selected assets.
// 
void AssetEditor::ExpandSelectedAssets( bool expand )
{
    S_AssetNodeDumbPtr nodes;

    const OS_SelectableDumbPtr selectionItems = m_AssetManager.GetSelection().GetItems();
    for ( OS_SelectableDumbPtr::Iterator itr = selectionItems.Begin(), end = selectionItems.End(); itr != end; ++itr )
    {
        Luna::AssetNode* node = Reflect::ObjectCast< Luna::AssetNode >( *itr );
        nodes.insert( node );
    }

    m_Outliner->ExpandNodes( nodes, expand );
}
