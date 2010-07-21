#pragma once

#include "VaultGenerated.h"
#include "VaultFrame.h"
#include "VaultMenuIDs.h"
#include "CollectionManager.h"

#include "DocumentManager.h"
#include "Application/Inspect/DragDrop/DropTarget.h"
#include "Application/Inspect/DragDrop/ReflectClipboardData.h"

namespace Luna
{
  //
  // Forwrds
  //
  class SortTreeCtrl;
  struct PreferencesLoadedArgs;
  class AssetCollectionItemData;

  namespace CollectionActions
  {
      enum CollectionAction
      {
          Add,
          Remove,
      };
  }
  typedef CollectionActions::CollectionAction CollectionAction;

  ///////////////////////////////////////////////////////////////////////////////
  /// Class CollectionsPanel
  ///////////////////////////////////////////////////////////////////////////////
  class CollectionsPanel : public CollectionsPanelGenerated 
  {
  private:
      void UpdateCollection( CollectionAction action );

  public:
    CollectionsPanel( VaultFrame* browserFrame );
    ~CollectionsPanel();

  protected:
    // Virtual event handlers, overide them in your derived class
    virtual void OnSizeCollectionsPanel( wxSizeEvent& event );
    virtual void OnTempCollectionsDoubleClick( wxTreeEvent& event );

    virtual void OnMyCollectionsTitleMenu( wxMouseEvent& event );
    virtual void OnMyCollectionsBeginLabelEdit( wxTreeEvent& event );
	virtual void OnMyCollectionsEndLabelEdit( wxTreeEvent& event );
    virtual void OnMyCollectionsDoubleClick( wxTreeEvent& event );
    virtual void OnMyCollectionsMenu( wxTreeEvent& event );

    // Context menu event handlers
    void OnShowCollection( wxCommandEvent& event );
    void OnNewCollection( wxCommandEvent& event );
    void OnOpenCollection( wxCommandEvent& event );
    void OnCloseCollection( wxCommandEvent& event );
    void OnRenameCollection( wxCommandEvent& event );
    void OnImportIntoCollection( wxCommandEvent& event );
    void OnSaveCollection( wxCommandEvent& event );
    void OnAddToCollection( wxCommandEvent& event );
    void OnRemoveFromCollection( wxCommandEvent& event );

    // Listeners
    void OnPreferencesLoaded( const PreferencesLoadedArgs& args );
    void OnPrefrencesChanged( const Reflect::ElementChangeArgs& args );
    void OnAssetCollectionsChanged( const Reflect::ElementChangeArgs& args );
    void OnCollectionModified( const Reflect::ElementChangeArgs& args );
    void OnCollectionAdded( const CollectionManagerArgs& args );
    void OnCollectionRemoving( const CollectionManagerArgs& args );
    void OnClearAllCollections( const CollectionManagerArgs& args );

    // Drag-n-Drop Listeners
    wxDragResult DragOver( const Inspect::DragArgs& args );
    wxDragResult Drop( const Inspect::DragArgs& args );
    void DragLeave( Nocturnal::Void );

  private:
    wxTreeItemId DragHitTest( SortTreeCtrl* treeCtrl, wxPoint point );

    static AssetCollection* NewCollection( CollectionManager* collectionManager, const i32 typeID, const tstring& tryName = TXT("") );

    template <class T>
    static T* NewCollection( CollectionManager* collectionManager, const i32 typeID, const tstring& tryName = tstring("") )
    {
      return Reflect::TryCast<T>( NewCollection( collectionManager, typeID, tryName ) );
    }

    void UpdateCollectionManager();
    void ConnectCollectionManagerListeners();
    void DisconnectCollectionManagerListeners();
    void ConnectCollectionListeners();
    void UpdateCollections();
    void PrePopulateTreeCtrl( SortTreeCtrl* treeCtrl );
    void PostPopulateTreeCtrl( SortTreeCtrl* treeCtrl );

    static AssetCollectionItemData* GetItemData( SortTreeCtrl* treeCtrl, const wxTreeItemId& id );

  private:
    VaultFrame*      m_VaultFrame;
    CollectionManager* m_CollectionManager;

    wxTreeItemId       m_DragOverItem;
    bool               m_DragOriginatedHere;

    int                m_ContainerImageIndex;
    int                m_DependencyImageIndex;
    int                m_UsageImageIndex;

    typedef std::map< u64, wxTreeItemId > M_CollectionToItemID;
    M_CollectionToItemID m_CollectionToItemIDs;

  private:
    enum ContextMenuIDs
    {
      ID_ShowCollection = VaultMenu::ShowCollection,
      ID_NewCollection = VaultMenu::NewCollection,
      ID_OpenCollection = VaultMenu::OpenCollection,
      ID_CloseCollection = VaultMenu::CloseCollection,
      ID_RenameCollection = VaultMenu::RenameCollection,
      ID_DeleteCollection = VaultMenu::DeleteCollection,
      ID_ImportCollection = VaultMenu::ImportCollection,
      ID_ImportIntoCollection = VaultMenu::ImportIntoCollection,
      ID_SaveCollection = VaultMenu::SaveCollection,
      ID_AddToCollection = VaultMenu::AddToCollection,
      ID_RemoveFromCollection = VaultMenu::RemoveFromCollection,
    };

    DECLARE_EVENT_TABLE();
  };
}
