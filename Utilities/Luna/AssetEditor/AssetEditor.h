#pragma once

#include "Luna/API.h"
#include "AssetManager.h"
#include "Core/PropertiesManager.h"
#include "Editor/Editor.h"
#include "Application/UI/MenuMRU.h"

// Forwards
namespace AssetManager { class CreateAssetWizard; }
namespace Inspect { class CanvasWindow; }
namespace Luna { struct MRUArgs; }

namespace Luna
{
  // Forwards
  namespace FilePathOptions{ enum FilePathOption; }
  class AssetOutliner;
  class BrowserToolBar;
  class ContextMenuArgs;
  typedef Nocturnal::SmartPtr< ContextMenuArgs > ContextMenuArgsPtr;
  class AssetPreviewWindow;

  ///////////////////////////////////////////////////////////////////////////
  // Main UI window for editing AssetClasses and their Components.
  // 
  class AssetEditor : public Luna::Editor
  {
  private:
    typedef std::map< i32, FilePathOptions::FilePathOption > M_MenuToFileOption;

  private:
    Luna::AssetManager m_AssetManager;
    Nocturnal::MenuMRUPtr m_MRU;
    AssetOutliner* m_Outliner;
    Inspect::Canvas m_PropertyCanvas;
    EnumeratorPtr m_Enumerator;
    PropertiesManagerPtr m_PropertiesManager;
    wxMenu* m_MenuPanels;
    wxMenu* m_MenuFile;
    wxMenu* m_MenuEdit;
    wxMenu* m_MenuView;
    wxMenu* m_MenuNew;
    wxMenu* m_MenuMRU;
    wxMenu* m_MenuOptions;
    wxMenu* m_MenuFilePathOptions;
    wxMenu m_MenuAddAnimClip;
    wxMenuItem* m_MenuItemOpenRecent;
    wxToolBar* m_MainToolBar;
    BrowserToolBar* m_BrowserToolBar;
    M_i32 m_MenuItemToAssetType;
    M_MenuToFileOption m_MenuItemToFilePathOption;
    AssetPreviewWindow* m_AssetPreviewWindow;
    bool m_PromptModifiedFiles;

  public:
    AssetEditor();
    virtual ~AssetEditor();

    Luna::AssetManager* GetAssetManager();

    bool Open( const std::string& file );

    virtual void SaveWindowState() NOC_OVERRIDE;
    virtual DocumentManager* GetDocumentManager() NOC_OVERRIDE;

    // 
    // Context menu callbacks
    // 
    void PromptAddComponents( const ContextMenuArgsPtr& args );
    void RemoveSelectedComponents( const ContextMenuArgsPtr& args );
    void CloseSelectedAssetClasses( const ContextMenuArgsPtr& args );
    void OnAssetPreview( const ContextMenuArgsPtr& args );
    void OnExpandSelectedAssets( const ContextMenuArgsPtr& args );
    void OnCollapseSelectedAssets( const ContextMenuArgsPtr& args );

    // 
    // Shortcuts for complex commands
    // 
    Undo::CommandPtr RemoveComponents( const S_ComponentSmartPtr& componentsToDelete );

  private:
    
    void ExpandSelectedAssets( bool expand );

  protected:

    // 
    // Data change callbacks
    // 
    void MRUOpen( const Nocturnal::MRUArgs& args );
    bool PropertyChanging( const Inspect::ChangingArgs& args );
    void PropertyChanged( const Inspect::ChangeArgs& args );
    void UndoQueueChanged( const Undo::QueueChangeArgs& args );
    void SelectionChanged( const OS_SelectableDumbPtr& selection );
    void AssetLoaded( const AssetLoadArgs& args );
    void AssetUnloading( const AssetLoadArgs& args );
    void DocumentModified( const DocumentChangedArgs& args );
    void DocumentSaved( const DocumentChangedArgs& args );

    // 
    // Helper functions
    // 
  private:
    bool ToClipboard( const Inspect::ReflectClipboardDataPtr& clipboardData );
    Inspect::ReflectClipboardDataPtr FromClipboard();
    void UpdateUIElements();
    bool DoOpen( const S_string& files );
    bool CanMoveSelectedItems( Luna::AssetNode*& commonParent );
    void PreviewSelectedItem();

    // 
    // UI callbacks
    // 
  private:
    void OnMenuOpen( wxMenuEvent& args );
    void OnNewAsset( wxCommandEvent& args );
    void OnNew( wxCommandEvent& args );
    void OnOpen( wxCommandEvent& args );
    void OnFind( wxCommandEvent& args );
    void OnSortFiles( wxCommandEvent& args );
    void OnClose( wxCommandEvent& args );
    void OnExit( wxCommandEvent& args );
    void OnExiting( wxCloseEvent& args );
    void OnSave( wxCommandEvent& args );
    void OnSaveAll( wxCommandEvent& args );
    void OnExpandAll( wxCommandEvent& args );
    void OnCollapseAll( wxCommandEvent& args );
    void OnUndo( wxCommandEvent& args );
    void OnRedo( wxCommandEvent& args );
    void OnCut( wxCommandEvent& args );
    void OnCopy( wxCommandEvent& args );
    void OnPaste( wxCommandEvent& args );
    void OnMoveUp( wxCommandEvent& args );
    void OnMoveDown( wxCommandEvent& args );
    void OnBuild( wxCommandEvent& args );
    void OnFileOption( wxCommandEvent& args );
    void OnHelpIndex( wxCommandEvent& args );
    void OnHelpSearch( wxCommandEvent& args );
    void OnCheckout( wxCommandEvent& args );
    void OnPreview( wxCommandEvent& args );

  private:
    DECLARE_EVENT_TABLE();
  };
}
