#pragma once

#include <string>
#include "Editor/ContextMenuGenerator.h"

namespace Luna
{
  // Forwards
  class Document;
  class Scene;
  class SceneEditor;
  class ScenesPanel;
  class Zone;

  struct DocumentChangedArgs;
  struct DocumentPathChangedArgs;
  struct SceneChangeArgs;
  struct LoadArgs;
  struct SceneNodeChangeArgs;

  /////////////////////////////////////////////////////////////////////////////
  // Client data for a wx control.  The delete button uses this to indicate
  // which zone is being deleted.
  // 
  class ZoneClientData : public wxClientData
  {
  public:
    Zone* m_Zone;

  public:
    ZoneClientData( Zone* zone )
    : m_Zone( zone )
    {
    }
  };

  /////////////////////////////////////////////////////////////////////////////
  // UI for a single row in the Zone UI panel.  Has the Checkout/Save, Load/Unload
  // buttons, among other things.
  // 
  class SceneRowPanel : public wxPanel
  {
  private:
    enum ButtonMode
    {
      ModeCheckout,
      ModeSave
    };

  private:
    SceneEditor* m_Editor;
    Luna::Scene* m_RootScene;
    Zone* m_Zone;
    ScenesPanel* m_ScenesPanel;
	  wxRadioButton* m_RadioButton;
	  wxBitmapButton* m_ButtonCheckOutOrSave;
    ButtonMode m_ButtonMode;
	  wxStaticText* m_Text;
    wxCheckBox* m_ToggleLoad;
    wxBitmapButton* m_ButtonDelete;
    ContextMenuItemSet m_ContextMenuItems;

  public:
    SceneRowPanel( Luna::Scene* scene, Zone* zone, ScenesPanel* panel, SceneEditor* editor );
    virtual ~SceneRowPanel();
    Luna::Scene* GetScene();
    Zone* GetZone();
    bool IsRoot() const;
    void SetActiveRow( bool isCurrent );
    void SetLabel( const tstring& label );
    void EnableSceneSwitch( bool enable );
    tstring GetRowLabel() const;

  private:
    bool TrimString( tstring& str, int width );
    void ResizeText();
    void MakeCurrentScene();
    void UpdateCheckOutButton( bool enabled );
    void UpdateDeleteButton();
    tstring GetFilePath() const;
    ButtonMode GetButtonMode();
    void SetButtonMode( ButtonMode mode, bool enabled );
    void CheckOut();
    void Save();

  private:
    // Application callbacks
    void DocumentPathChanged( const DocumentPathChangedArgs& args );
    void SceneAdded( const SceneChangeArgs& args );
    void SceneLoadFinished( const LoadArgs& args );
    void DocumentModified( const DocumentChangedArgs& args );
    void Rename( const ContextMenuArgsPtr& args );
    void NodeRenamed( const SceneNodeChangeArgs& args );
    void CheckOutContext( const ContextMenuArgsPtr& args );
    void RevisionHistory( const ContextMenuArgsPtr& args );

  private:
    // GUI callbacks
    void OnResizeText( wxSizeEvent& args );
    void OnRadioButton( wxCommandEvent& args );
    void OnButtonDown( wxMouseEvent& args );
    void OnDoubleClickRow( wxMouseEvent& args );
    void OnRightClick( wxMouseEvent& args );
    void OnToggleLoad( wxCommandEvent& args );
    void OnCheckOutSaveButton( wxCommandEvent& args );
  };
}
