#include "Precompile.h" 
#include "CharacterEditor.h" 

#include "CharacterForms.h" 
#include "CharacterMainPanel.h"
#include "CharacterPreferences.h"
#include "CharacterPhysicsPanel.h" 
#include "CharacterDynJointPanel.h" 
#include "CharacterIKPanel.h"
#include "CharacterPropertiesPanel.h" 
#include "CharacterAttributesPanel.h" 
#include "CharacterLooseAttachPanel.h"
#include "CharacterLooseAttachWizard.h"
#include "RemoteCharacter.h"
#include "Browser/BrowserToolBar.h"

#include "Editor/EditorInfo.h" 
#include "Editor/SessionManager.h" 
#include "Task/Build.h"

#include "Attribute/AttributeHandle.h" 
#include "Asset/AssetVersion.h" 
#include "Asset/AssetClass.h" 
#include "Asset/ArtFileAttribute.h" 
#include "Finder/AssetSpecs.h" 
#include "Finder/ContentSpecs.h"

#include "File/Manager.h" 
#include "FileSystem/FileSystem.h" 
#include "FileBrowser/FileBrowser.h" 

#include "Content/Scene.h" 

#include "UIToolKit/FileDialog.h" 
#include "UIToolKit/ImageManager.h"

#include "Live/RuntimeConnection.h" 
#include "rpc/interfaces/rpc_lunaview.h" 
#include "rpc/interfaces/rpc_lunaview_host.h" 


#define ERROR_MSG_STYLE (wxOK | wxCENTER | wxICON_ERROR)


using namespace Asset; 
using namespace Content; 
using namespace UIToolKit; 
using namespace Luna; 

namespace Luna
{

  BEGIN_EVENT_TABLE(CharacterEditor, Editor)
    EVT_MENU_OPEN( OnMenuOpen )

    EVT_MENU(wxID_OPEN,  OnOpen)
    EVT_MENU(wxID_SAVE,  OnSave)
    EVT_MENU(wxID_CLOSE, OnClose)
    EVT_MENU(wxID_EXIT,  OnExit) 

    EVT_CLOSE(CharacterEditor::OnExiting)

    EVT_MENU(CharacterEditor::CMD_SEARCH_FOR_FILE, OnFind)
    EVT_MENU(CharacterEditor::CMD_BUILD, OnBuild)
    EVT_MENU(CharacterEditor::CMD_VIEW_PHYSICS, OnViewPhysics)
    EVT_MENU(CharacterEditor::CMD_VIEW_LOOSE, OnViewLoose)

    EVT_MENU(CharacterEditor::CMD_IMPORT_PHYSICS, OnImportPhysics)

    EVT_MENU(CharacterEditor::CMD_ADD_PHYSICS_ATTRIBUTE, OnAddPhysicsAttribute)
    EVT_MENU(CharacterEditor::CMD_REMOVE_PHYSICS_ATTRIBUTE, OnRemovePhysicsAttribute)

    EVT_MENU(CharacterEditor::CMD_ADD_IK_ATTRIBUTE, OnAddIKAttribute)
    EVT_MENU(CharacterEditor::CMD_REMOVE_IK_ATTRIBUTE, OnRemoveIKAttribute)

    EVT_MENU(CharacterEditor::CMD_NEW_LOOSE_ATTACH_CHAIN_SINGLE, OnNewLooseAttachSingle)
    EVT_MENU(CharacterEditor::CMD_NEW_LOOSE_ATTACH_CHAIN_DOUBLE, OnNewLooseAttachDouble)
    EVT_MENU(CharacterEditor::CMD_REMOVE_LOOSE_ATTACH_CHAIN, OnRemoveLooseAttach)

    EVT_MENU(CharacterEditor::CMD_ADD_LOOSE_ATTACH_ATTRIBUTE, OnAddLooseAttachAttribute)
    EVT_MENU(CharacterEditor::CMD_REMOVE_LOOSE_ATTACH_ATTRIBUTE, OnRemoveLooseAttachAttribute)
    EVT_MENU(CharacterEditor::CMD_ADD_LOOSE_ATTACH_COLLISION_ATTRIBUTE, OnAddLooseAttachCollisionAttribute)
    EVT_MENU(CharacterEditor::CMD_REMOVE_LOOSE_ATTACH_COLLISION_ATTRIBUTE, OnRemoveLooseAttachCollisionAttribute)
    EVT_MENU(CharacterEditor::CMD_RECOVER_ATTRIBUTES_BY_NAME, OnRecover)
    EVT_MENU(CharacterEditor::CMD_COPY_ALL_PHYSICS_TO_IK, OnCopyAllPhysicsToIK)
    EVT_MENU(CharacterEditor::CMD_COPY_ONE_PHYSICS_TO_IK, OnCopyOnePhysicsToIK)


  END_EVENT_TABLE()

    CharacterEditor::CharacterEditor() 
    : Editor( EditorTypes::Character
    , NULL
    , wxID_ANY
    , wxT("Luna Character Editor")
    , wxDefaultPosition
    , wxSize(800, 600)
    , wxDEFAULT_FRAME_STYLE | wxSUNKEN_BORDER)
    , m_Manager(this)
    , m_MRU( new UIToolKit::MenuMRU( 30, this ) )
    , m_CurrentWizard(NULL)
    , m_RemoteCharacter( new RemoteCharacter( this ) )
  {
    // some weird boilerplate icon setup, using the moon icon because i've got nothing cool

    wxIconBundle iconBundle;
    wxIcon tempIcon;
    tempIcon.CopyFromBitmap( UIToolKit::GlobalImageManager().GetBitmap( "moon_128.png" ) );
    iconBundle.AddIcon( tempIcon );
    tempIcon.CopyFromBitmap( UIToolKit::GlobalImageManager().GetBitmap( "moon_64.png" ) );
    iconBundle.AddIcon( tempIcon );
    tempIcon.CopyFromBitmap( UIToolKit::GlobalImageManager().GetBitmap( "moon_32.png" ) );
    iconBundle.AddIcon( tempIcon );
    tempIcon.CopyFromBitmap( UIToolKit::GlobalImageManager().GetBitmap( "moon_16.png" ) );
    iconBundle.AddIcon( tempIcon );
    SetIcons( iconBundle );

    m_BrowserToolBar = new BrowserToolBar( this );
    m_BrowserToolBar->Realize();

    // asset browser search bar
    {
      m_FrameManager.AddPane( m_BrowserToolBar, m_BrowserToolBar->GetAuiPaneInfo() ); 
    }

    // main panel with tree ctrl
    // 
    {
      m_MainPanel = new CharacterMainPanel(this); 

      wxAuiPaneInfo info; 
      info.Name( wxT("Hierarchy") ); 
      info.DestroyOnClose( false ); 
      info.Caption( wxT("Hierarchy" ) ); 
      info.CenterPane(); 

      m_FrameManager.AddPane( m_MainPanel, info); 
    }

    // loose attach tree view, dockable
    {
      m_LooseAttachPanel = new CharacterLooseAttachPanel(this); 

      wxAuiPaneInfo info; 
      info.Name( wxT("Loose Attachment Chains") ); 
      info.DestroyOnClose( false ); 
      info.Caption( wxT("Loose Attachment Chains") ); 
      info.Right(); 
      info.Layer(1); 
      info.Position(1); 

      m_FrameManager.AddPane( m_LooseAttachPanel, info); 
    }

    // dynamic joint list panel, dockable
    {
      m_DynJointPanel = new CharacterDynJointPanel(this); 

      wxAuiPaneInfo info; 
      info.Name( wxT("Dynamic Joints") ); 
      info.DestroyOnClose( false ); 
      info.Caption( wxT("Dynamic Joints") ); 
      info.Right(); 
      info.Layer(1); 
      info.Position(2); 

      m_FrameManager.AddPane( m_DynJointPanel, info ); 
    }

    // physics list panel, dockable
    // 
    {
      m_PhysicsPanel = new CharacterPhysicsPanel(this); 

      wxAuiPaneInfo info; 
      info.Name( wxT("Physics Joints") ); 
      info.DestroyOnClose( false ); 
      info.Caption( wxT("Physics Joints") ); 
      info.Right(); 
      info.Layer(1); 
      info.Position(3); 

      m_FrameManager.AddPane( m_PhysicsPanel, info); 
    }

    // ik list panel, dockable
    {
      m_IKPanel = new CharacterIKPanel(this); 

      wxAuiPaneInfo info; 
      info.Name( wxT("IK Joints") ); 
      info.DestroyOnClose( false ); 
      info.Caption( wxT("IK Joints") ); 
      info.Right(); 
      info.Layer(1); 
      info.Position(4); 

      m_FrameManager.AddPane( m_IKPanel, info); 
    }

    // attributes panel, dockable
    //
    {
      m_AttributesPanel = new CharacterAttributesPanel(this); 

      wxAuiPaneInfo info; 
      info.Name( wxT("Joint Attributes") ); 
      info.DestroyOnClose( false ); 
      info.Caption( wxT("Joint Attributes") ); 
      info.Right(); 
      info.Layer(2); 
      info.Position(1); 

      m_FrameManager.AddPane( m_AttributesPanel, info); 
    }

    // properties panel, dockable
    // 
    {
      m_PropertiesPanel = new CharacterPropertiesPanel(this); 

      wxAuiPaneInfo info; 
      info.Name( wxT("Properties") ); 
      info.DestroyOnClose( false ); 
      info.Caption( wxT("Properties") ); 
      info.Right(); 
      info.Layer(2); 
      info.Position(2); 

      m_FrameManager.AddPane( m_PropertiesPanel, info); 
    }


    // menu bar
    // 
    wxMenuBar* menuBar = new wxMenuBar(); 

    wxMenu* fileMenu     = m_Menus[MENU_FILE] = new wxMenu; 
    wxMenu* editMenu     = m_Menus[MENU_EDIT] = new wxMenu; 
    wxMenu* panelsMenu   = m_Menus[MENU_PANELS] = new wxMenu; 
    wxMenu* uberMenu     = m_Menus[MENU_UBER]   = new wxMenu; 
    wxMenu* mruMenu      = m_Menus[MENU_MRU]    = new wxMenu; 
    wxMenu* recoveryMenu = m_Menus[MENU_RECOVERY] = new wxMenu; 

    CreatePanelsMenu( panelsMenu );

    fileMenu->Append(wxID_OPEN,           "Open"); 
    m_MenuItemOpenRecent = fileMenu->AppendSubMenu(mruMenu, "Open Recent"); 
    fileMenu->Append(CMD_SEARCH_FOR_FILE, "Find..."); 
    fileMenu->Append(wxID_SAVE,           "Save"); 
    fileMenu->Append(wxID_CLOSE,          "Close"); 
    fileMenu->Append(wxID_EXIT,           "Exit"); 

    editMenu->Append(CMD_ADD_PHYSICS_ATTRIBUTE,         "Add Physics Attribute"); 
    editMenu->Append(CMD_REMOVE_PHYSICS_ATTRIBUTE,      "Remove Physics Attribute"); 
    editMenu->AppendSeparator(); 
    editMenu->Append(CMD_ADD_IK_ATTRIBUTE,              "Add IK Attribute"); 
    editMenu->Append(CMD_REMOVE_IK_ATTRIBUTE,           "Remove IK Attribute"); 
    editMenu->Append(CMD_COPY_ONE_PHYSICS_TO_IK,        "Copy Physics To IK"); 
    editMenu->AppendSeparator(); 
    editMenu->Append(CMD_NEW_LOOSE_ATTACH_CHAIN_SINGLE, "New Loose Attachment Chain: Single-Ended"); 
    editMenu->Append(CMD_NEW_LOOSE_ATTACH_CHAIN_DOUBLE, "New Loose Attachment Chain: Double-Ended"); 
    editMenu->Append(CMD_REMOVE_LOOSE_ATTACH_CHAIN,     "Remove Loose Attachment Chain"); 
    editMenu->AppendSeparator(); 
    editMenu->Append(CMD_ADD_LOOSE_ATTACH_ATTRIBUTE,    "Add Loose Attachment Attribute"); 
    editMenu->Append(CMD_REMOVE_LOOSE_ATTACH_ATTRIBUTE, "Remove Loose Attachment Attribute");
    editMenu->AppendSeparator(); 
    editMenu->Append(CMD_ADD_LOOSE_ATTACH_COLLISION_ATTRIBUTE,    "Add Loose Attachment Collision Attribute"); 
    editMenu->Append(CMD_REMOVE_LOOSE_ATTACH_COLLISION_ATTRIBUTE, "Remove Loose Attachment Collision Attribute");

    recoveryMenu->Append(CMD_IMPORT_PHYSICS,                "Import Physics Joints From Export File"); 
    recoveryMenu->Append(CMD_RECOVER_ATTRIBUTES_BY_NAME,    "Recover Attributes By Joint Name"); 
    recoveryMenu->Append(CMD_COPY_ALL_PHYSICS_TO_IK,        "Copy All Physics Joints to IK Joints"); 

    uberMenu->Append(CMD_BUILD, "Build"); 
    uberMenu->Append(CMD_VIEW_PHYSICS, "View - Physics Joints"); 
    uberMenu->Append(CMD_VIEW_LOOSE,   "View - Loose Attachments"); 

    menuBar->Append(fileMenu,     "File"); 
    menuBar->Append(editMenu,     "Edit"); 
    menuBar->Append(panelsMenu,   "Panels"); 
    menuBar->Append(uberMenu,     "Uber"); 
    menuBar->Append(recoveryMenu, "Recovery"); 

    SetMenuBar(menuBar); 

    m_StatusBar = this->CreateStatusBar(); 

    m_MRU->AddItemSelectedListener( UIToolKit::MRUSignature::Delegate( this, &CharacterEditor::OnMRUOpen ) );
    m_MRU->FromVector( CharacterEditorPreferences()->GetMRU()->GetPaths() );
    m_Manager.StatusChangedEvent()->Add( StatusChangeSignature::Delegate(this, &CharacterEditor::OnStatusChanged)); 

    // load the UI state and MRU list from the registry.
    CharacterEditorPreferences()->GetCharacterEditorWindowSettings()->ApplyToWindow( this, &m_FrameManager, true );

    // this will trigger an update of all the UI...
    m_Manager.ClearSelections(); 

  }

  CharacterEditor::~CharacterEditor()
  {
    // save the UI state and MRU list
    V_string mruPaths;
    m_MRU->ToVector( mruPaths );
    CharacterEditorPreferences()->GetMRU()->SetPaths( mruPaths );
    CharacterEditorPreferences()->SavePreferences();

    m_MRU->RemoveItemSelectedListener( UIToolKit::MRUSignature::Delegate( this, &CharacterEditor::OnMRUOpen ) );
    m_Manager.StatusChangedEvent()->Remove( StatusChangeSignature::Delegate(this, &CharacterEditor::OnStatusChanged)); 

    delete m_RemoteCharacter;
  }

  static Editor* CreateEditor()
  {
    return new CharacterEditor(); 
  }

  void CharacterEditor::InitializeEditor()
  {
    static Finder::FilterSpec filter( "CharacterEditor::filter", "All Character Files" );
    filter.AddSpec( FinderSpecs::Asset::ENTITY_DECORATION );

    SessionManager::GetInstance()->RegisterEditor( new EditorInfo( EditorTypes::Character, &CreateEditor, &filter ) );
  }

  void CharacterEditor::CleanupEditor()
  {
  }

  void CharacterEditor::SaveWindowState()
  {
    CharacterEditorPreferences()->GetCharacterEditorWindowSettings()->SetFromWindow( this, &m_FrameManager );
  }

  DocumentManager* CharacterEditor::GetDocumentManager()
  {
    return &m_Manager;
  }

  void CharacterEditor::OnOpen( wxCommandEvent& args )
  {
    if(m_CurrentWizard)
    {
      return; 
    }


    static std::string defaultDir = FinderSpecs::Asset::ENTITY_FOLDER.GetFolder(); 

    FileDialog browser(this, "Open", defaultDir); 
    browser.AddFilter( FinderSpecs::Asset::ENTITY_DECORATION.GetDialogFilter() ); 

    if(browser.ShowModal() != wxID_OK)
    {
      return; 
    }

    std::string fullPath = browser.GetPath(); 

    if(!FileSystem::Exists(fullPath))
    {
      return; 
    }

    PerformOpen(fullPath); 
  }

  void CharacterEditor::OnFind( wxCommandEvent& args )
  {
    if(m_CurrentWizard)
    {
      return; 
    }

    File::FileBrowser browser(this, -1, "Open"); 
    browser.SetFilter(FinderSpecs::Asset::ENTITY_DECORATION); 

    if(browser.ShowModal() != wxID_OK)
    {
      return; 
    }

    std::string fullPath = browser.GetPath(); 

    if(!FileSystem::Exists(fullPath))
    {
      return; 
    }

    PerformOpen(fullPath); 
  }

  void CharacterEditor::OnClose( wxCommandEvent& args )
  {
    if(m_CurrentWizard)
    {
      return; 
    }

    m_Manager.Close();
  }

  void CharacterEditor::OnSave( wxCommandEvent& args )
  {
    if(m_CurrentWizard)
    {
      return; 
    }

    PerformSave(); 
  }

  void CharacterEditor::OnExit( wxCommandEvent& args )
  {
    if(m_CurrentWizard)
    {
      return; 
    }

    wxCloseEvent closeEvent(wxEVT_CLOSE_WINDOW); 
    GetEventHandler()->AddPendingEvent(closeEvent); 
  }

  void CharacterEditor::OnExiting(wxCloseEvent& args)
  {
    if(m_CurrentWizard)
    {
      args.Veto(); 
      return; 
    }

    if(!PerformClose())
    {
      args.Veto(); 
      return; 
    }

    args.Skip(); 
  }

  void CharacterEditor::OnMRUOpen( const UIToolKit::MRUArgs& args )
  {
    PerformOpen( args.m_Item );
  }

  void CharacterEditor::PerformOpen(const std::string& fileName)
  {
    // close already open file
    bool ok = m_Manager.Close();

    if(!ok)
    {
      return; 
    }

    std::string error;
    ok = m_Manager.OpenPath(fileName, error) != NULL; 
    if(!ok)
    {
      wxMessageBox( error.c_str(), "Error", ERROR_MSG_STYLE, this );
      return;
    }

    m_MRU->Insert( fileName );

    // all the work for building UIs is generated by events on the manager
  }

  bool CharacterEditor::PerformClose()
  {
    return m_Manager.Close();
  }

  bool CharacterEditor::PerformSave()
  {
    std::string error;
    if ( !m_Manager.Save( error ) && !error.empty() )
    {
      wxMessageBox( error.c_str(), "Error", ERROR_MSG_STYLE, this );
      return false; 
    }
    return true;
  }

  void CharacterEditor::OnBuild( wxCommandEvent& args)
  {
    if(m_CurrentWizard)
    {
      return; 
    }

    bool showOptions = wxIsShiftDown();

    PerformSave(); 

    const Asset::EntityAssetPtr& mobyClass = m_Manager.GetMobyClass(); 
    if(mobyClass)
    {
      Luna::BuildAsset( mobyClass->m_AssetClassID, this, NULL, showOptions );
    }
  }

  void CharacterEditor::OnViewPhysics( wxCommandEvent& args )
  {
    if(m_CurrentWizard)
    {
      return; 
    }

    const Asset::EntityAssetPtr& mobyClass = m_Manager.GetMobyClass(); 
    if(!mobyClass)
      return; 

    bool showOptions = wxIsShiftDown();

    SessionManager::GetInstance()->GiveViewerControl( this );

    m_RemoteCharacter->SetMode(RPC::LunaViewModes::PhysicsView);

    SessionManager::GetInstance()->SaveAllOpenDocuments();
    Luna::ViewAsset( mobyClass->m_AssetClassID, this, NULL, showOptions );

    RPC::ILunaViewHostRemote* remote = RuntimeConnection::GetRemoteLevelView();
  }

  void CharacterEditor::OnViewLoose( wxCommandEvent& args )
  {
    if(m_CurrentWizard)
    {
      return; 
    }

    const Asset::EntityAssetPtr& mobyClass = m_Manager.GetMobyClass(); 
    if(!mobyClass)
      return; 

    bool showOptions = wxIsShiftDown();

    SessionManager::GetInstance()->GiveViewerControl( this );

    m_RemoteCharacter->SetMode(RPC::LunaViewModes::LooseAttachments);
    SessionManager::GetInstance()->SaveAllOpenDocuments();
    Luna::ViewAsset( mobyClass->m_AssetClassID, this, NULL, showOptions ); 
  }

  void CharacterEditor::OnImportPhysics( wxCommandEvent& args )
  {
    if(m_CurrentWizard)
    {
      return; 
    }

    m_Manager.ImportFromExportData(); 
  }

  void CharacterEditor::OnRefresh( wxCommandEvent& args )
  {
    if(m_CurrentWizard)
    {
      return; 
    }

    m_Manager.RefreshExportData(); 
  }

  void CharacterEditor::OnStatusChanged(StatusChangeArgs& args )
  {
    this->DoGiveHelp(args.m_Message, true); 
  }

  void CharacterEditor::OnAddPhysicsAttribute( wxCommandEvent& args )
  {
    if(m_CurrentWizard)
    {
      return; 
    }

    const Content::JointTransformPtr& joint = m_Manager.GetSelectedJoint(); 

    if(joint)
    {
      m_Manager.AddPhysicsAttribute(joint); 
    }
  }

  void CharacterEditor::OnRemovePhysicsAttribute( wxCommandEvent& args )
  {
    if(m_CurrentWizard)
    {
      return; 
    }

    const Content::JointTransformPtr& joint = m_Manager.GetSelectedJoint();

    if(joint)
    {
      m_Manager.RemovePhysicsAttribute(joint); 
    }
  }

  void CharacterEditor::OnAddIKAttribute( wxCommandEvent& args )
  {
    if(m_CurrentWizard)
    {
      return; 
    }

    const Content::JointTransformPtr& joint = m_Manager.GetSelectedJoint(); 

    if(joint)
    {
      m_Manager.AddIKAttribute(joint); 
    }
  }

  void CharacterEditor::OnRemoveIKAttribute( wxCommandEvent& args )
  {
    if(m_CurrentWizard)
    {
      return; 
    }

    const Content::JointTransformPtr& joint = m_Manager.GetSelectedJoint();

    if(joint)
    {
      m_Manager.RemoveIKAttribute(joint); 
    }
  }


  void CharacterEditor::OnNewLooseAttachSingle( wxCommandEvent& args )
  {
    if(!m_Manager.GetFile())
    {
      return; 
    }

    if(m_CurrentWizard)
    {
      return; 
    }

    m_CurrentWizard = new CharacterLooseAttachWizardSingle(this);
    m_CurrentWizard->Show(); 

  }

  void CharacterEditor::OnNewLooseAttachDouble( wxCommandEvent& args )
  {
    if(!m_Manager.GetFile())
    {
      return; 
    }

    if(m_CurrentWizard)
    {
      return; 
    }

    m_CurrentWizard = new CharacterLooseAttachWizardDouble(this); 
    m_CurrentWizard->Show(); 

  }

  void CharacterEditor::OnRemoveLooseAttach( wxCommandEvent& args)
  {
    if(!m_Manager.GetFile())
    {
      return; 
    }

    if(m_CurrentWizard)
    {
      return;
    }

    m_LooseAttachPanel->DeleteSelectedChain(); 

  }

  void CharacterEditor::OnAddLooseAttachAttribute( wxCommandEvent& args )
  {
    if(!m_Manager.GetFile())
    {
      return; 
    }

    if(m_CurrentWizard)
    {
      return; 
    }

    m_Manager.AddLooseAttachAttribute( m_Manager.GetSelectedJoint() ); 
  }

  void CharacterEditor::OnRemoveLooseAttachAttribute( wxCommandEvent& args )
  {
    if(!m_Manager.GetFile())
    {
      return; 
    }

    if(m_CurrentWizard)
    {
      return; 
    }

    m_Manager.RemoveLooseAttachAttribute( m_Manager.GetSelectedJoint() ); 
  }

  void CharacterEditor::OnAddLooseAttachCollisionAttribute( wxCommandEvent& args )
  {
    if(!m_Manager.GetFile())
    {
      return; 
    }

    if(m_CurrentWizard)
    {
      return; 
    }

    m_Manager.AddLooseAttachCollisionAttribute( m_Manager.GetSelectedJoint() ); 
  }

  void CharacterEditor::OnRemoveLooseAttachCollisionAttribute( wxCommandEvent& args )
  {
    if(!m_Manager.GetFile())
    {
      return; 
    }

    if(m_CurrentWizard)
    {
      return; 
    }

    m_Manager.RemoveLooseAttachCollisionAttribute( m_Manager.GetSelectedJoint() ); 
  }
  
  void CharacterEditor::CloseWizard(wxWindow* wizard)
  {
    if(m_CurrentWizard == wizard)
    {
      m_CurrentWizard->Hide(); 
      m_CurrentWizard->Destroy(); 
      m_CurrentWizard = NULL; 

    }
  }

  void CharacterEditor::OnMenuOpen( wxMenuEvent& args )
  {
    args.Skip();
    if ( args.GetMenu() == m_Menus[MENU_FILE] )
    {
      m_Menus[MENU_FILE]->Enable( m_MenuItemOpenRecent->GetId(), !m_MRU->GetItems().Empty() );
      m_MRU->PopulateMenu( m_Menus[MENU_MRU] );

      bool canSave = (m_Manager.GetFile() && m_Manager.GetFile()->IsModified()); 
      m_Menus[MENU_FILE]->Enable( wxID_SAVE, canSave ); 
    }
    else if ( args.GetMenu() == m_Menus[MENU_PANELS] )
    {
      UpdatePanelsMenu( m_Menus[MENU_PANELS] );
    }
  }

  void CharacterEditor::TakeViewerControl()
  {
    m_RemoteCharacter->Enable( true );
  }

  void CharacterEditor::ReleaseViewerControl()
  {
    m_RemoteCharacter->Enable( false );
  }

  void CharacterEditor::OnRecover( wxCommandEvent& args )
  {
    m_Manager.RecoverAttributesByName(); 
  }

  void CharacterEditor::OnCopyAllPhysicsToIK( wxCommandEvent& args )
  {
    m_Manager.CopyAllPhysicsAttributesToIK(); 
  }

  void CharacterEditor::OnCopyOnePhysicsToIK( wxCommandEvent& args )
  {
    m_Manager.CopyOnePhysicsAttributeToIK(); 
  }


}
