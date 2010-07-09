#pragma once

#include "BrowserGenerated.h"
#include "BrowserFrame.h"
#include "BrowserMenuIDs.h"
#include "CollectionManager.h"

#include "Editor/DocumentManager.h"
#include "Application/Inspect/DragDrop/DropTarget.h"
#include "Application/Inspect/DragDrop/ReflectClipboardData.h"

namespace Luna
{
  class SortTreeCtrl;
}

namespace Luna
{
  //
  // Forwrds
  //
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
    CollectionsPanel( BrowserFrame* browserFrame );
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
    wxTreeItemId DragHitTest( Nocturnal::SortTreeCtrl* treeCtrl, wxPoint point );

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
    void PrePopulateTreeCtrl( Nocturnal::SortTreeCtrl* treeCtrl );
    void PostPopulateTreeCtrl( Nocturnal::SortTreeCtrl* treeCtrl );

    static AssetCollectionItemData* GetItemData( Nocturnal::SortTreeCtrl* treeCtrl, const wxTreeItemId& id );

  private:
    BrowserFrame*      m_BrowserFrame;
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
      ID_ShowCollection = BrowserMenu::ShowCollection,

      ID_NewCollection = BrowserMenu::NewCollection,
      ID_NewDependencyCollection = BrowserMenu::NewDependencyCollection,
      ID_NewUsageCollection = BrowserMenu::NewUsageCollection,

      ID_OpenCollection = BrowserMenu::OpenCollection,
      ID_CloseCollection = BrowserMenu::CloseCollection,

      ID_RenameCollection = BrowserMenu::RenameCollection,
      ID_DeleteCollection = BrowserMenu::DeleteCollection,

      ID_ImportCollection = BrowserMenu::ImportCollection,
      ID_ImportIntoCollection = BrowserMenu::ImportIntoCollection,
      ID_SaveCollection = BrowserMenu::SaveCollection,

      ID_AddToCollection = BrowserMenu::AddToCollection,
      ID_RemoveFromCollection = BrowserMenu::RemoveFromCollection,
    };

    DECLARE_EVENT_TABLE();
  };
}
