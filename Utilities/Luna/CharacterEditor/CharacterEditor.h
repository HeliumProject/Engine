#pragma once

#include "API.h" 
#include "Editor/Editor.h"
#include "CharacterForms.h" 
#include "CharacterManager.h" 
#include "UIToolKit/MenuMRU.h"

namespace Luna
{
  class BrowserToolBar;
  class CharacterMainPanel; 
  class CharacterPhysicsPanel; 
  class CharacterDynJointPanel; 
  class CharacterIKPanel; 
  class CharacterPropertiesPanel; 
  class CharacterAttributesPanel; 
  class CharacterLooseAttachPanel; 
  class CharacterManager; 
  class RemoteCharacter;

  class CharacterEditor : public Editor
  {
    enum
    {
      // commands
      CMD_SEARCH_FOR_FILE = wxID_HIGHEST + 1, 
      CMD_BUILD, 
      CMD_VIEW_PHYSICS, 
      CMD_VIEW_LOOSE,
      CMD_IMPORT_PHYSICS,
      CMD_ADD_PHYSICS_ATTRIBUTE, 
      CMD_REMOVE_PHYSICS_ATTRIBUTE,
      CMD_ADD_IK_ATTRIBUTE, 
      CMD_REMOVE_IK_ATTRIBUTE,
      CMD_NEW_LOOSE_ATTACH_CHAIN_SINGLE, 
      CMD_NEW_LOOSE_ATTACH_CHAIN_DOUBLE, 
      CMD_REMOVE_LOOSE_ATTACH_CHAIN, 
      CMD_ADD_LOOSE_ATTACH_ATTRIBUTE, 
      CMD_REMOVE_LOOSE_ATTACH_ATTRIBUTE,
      CMD_ADD_LOOSE_ATTACH_COLLISION_ATTRIBUTE, 
      CMD_REMOVE_LOOSE_ATTACH_COLLISION_ATTRIBUTE,
      CMD_RECOVER_ATTRIBUTES_BY_NAME, 
      CMD_COPY_ALL_PHYSICS_TO_IK, 
      CMD_COPY_ONE_PHYSICS_TO_IK,
      CMD_OPEN_VAULT

    }; 

    enum MenuID
    {
      MENU_FILE,
      MENU_MRU,
      MENU_EDIT,
      MENU_PANELS, 
      MENU_UBER, 
      MENU_RECOVERY, 
      MENU_COUNT
    }; 

  public: 
    CharacterEditor(); 
    virtual ~CharacterEditor(); 

    static void InitializeEditor();
    static void CleanupEditor();

    virtual void SaveWindowState() NOC_OVERRIDE;
    virtual DocumentManager* GetDocumentManager() NOC_OVERRIDE;

    CharacterManager* GetManager()
    {
      return &m_Manager; 
    }

    void CloseWizard(wxWindow* wizard); 
    
  private: 
    void OnMRUOpen( const UIToolKit::MRUArgs& args );
    void PerformOpen(const std::string& fileName); 
    bool PerformClose();
    bool PerformSave(); 

    virtual void TakeViewerControl() NOC_OVERRIDE;
    virtual void ReleaseViewerControl() NOC_OVERRIDE;

  private: 
    CharacterManager           m_Manager; 
    CharacterMainPanel*        m_MainPanel;
    CharacterPhysicsPanel*     m_PhysicsPanel; 
    CharacterIKPanel*          m_IKPanel; 
    CharacterDynJointPanel*    m_DynJointPanel; 
    CharacterPropertiesPanel*  m_PropertiesPanel; 
    CharacterAttributesPanel*  m_AttributesPanel; 
    CharacterLooseAttachPanel* m_LooseAttachPanel; 
    wxStatusBar*                m_StatusBar; 

    UIToolKit::MenuMRUPtr       m_MRU;
    wxMenu*                     m_Menus[MENU_COUNT]; 
    wxWindow*                   m_CurrentWizard; 
    wxMenuItem*                 m_MenuItemOpenRecent;
    BrowserToolBar*             m_BrowserToolBar;

    RemoteCharacter*           m_RemoteCharacter;

    DECLARE_EVENT_TABLE(); 

    void OnOpen( wxCommandEvent& args );
    void OnFind( wxCommandEvent& args );
    void OnClose( wxCommandEvent& args );
    void OnSave( wxCommandEvent& args );
    void OnExit( wxCommandEvent& args );
    void OnExiting( wxCloseEvent& args );
    void OnShowPanel( wxCommandEvent& args ); 
    void OnMenuOpen( wxMenuEvent& args ); 

    void OnBuild( wxCommandEvent& args ); 
    void OnViewPhysics( wxCommandEvent& args ); 
    void OnViewLoose( wxCommandEvent& args ); 

    void OnRefresh( wxCommandEvent& args ); 
    void OnImportPhysics( wxCommandEvent& args ); 
    void OnRecover( wxCommandEvent& args ); 

    void OnCopyAllPhysicsToIK( wxCommandEvent& args ); 
    void OnCopyOnePhysicsToIK( wxCommandEvent& args ); 

    void OnAddPhysicsAttribute( wxCommandEvent& args ); 
    void OnRemovePhysicsAttribute( wxCommandEvent& args ); 

    void OnAddIKAttribute( wxCommandEvent& args ); 
    void OnRemoveIKAttribute( wxCommandEvent& args ); 

    void OnNewLooseAttachSingle( wxCommandEvent& args ); 
    void OnNewLooseAttachDouble( wxCommandEvent& args ); 
    void OnRemoveLooseAttach( wxCommandEvent& args ); 

    void OnAddLooseAttachAttribute( wxCommandEvent& args ); 
    void OnRemoveLooseAttachAttribute( wxCommandEvent& args ); 

    void OnAddLooseAttachCollisionAttribute( wxCommandEvent& args ); 
    void OnRemoveLooseAttachCollisionAttribute( wxCommandEvent& args ); 

    void OnStatusChanged( StatusChangeArgs& args );
  }; 
}
