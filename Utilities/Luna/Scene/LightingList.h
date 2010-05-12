#pragma once

#include "Scene.h"
#include "SceneEditor.h"

#include "Common/Container/ReversibleMap.h"
#include "Undo/PropertyCommand.h"
#include "UIToolKit/SortableListView.h"

#include "Common/Container/Insert.h" 

namespace Luna
{
  /////////////////////////////////////////////////////////////////////////////
  // Base class for the list view controls used in the lighting UI.  Displays
  // a list of common items.  The type of items is indicated by the template
  // parameter.
  // 
  template< class T >
  class LightingList NOC_ABSTRACT : public UIToolKit::SortableListView
  {
  public:
    typedef Nocturnal::ReversibleMap< T*, i32 > RM_ObjToId;

  private:
    bool m_IsInitialized;

  protected:
    SceneEditor* m_SceneEditor;
    RM_ObjToId m_Map;

  public:
    ///////////////////////////////////////////////////////////////////////////
    // Constructor
    // 
    LightingList( wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxLC_ICON, const wxValidator& validator = wxDefaultValidator, const wxString& name = "LightingListView" )
    : UIToolKit::SortableListView( parent, id, pos, size, style, validator, name )
    , m_IsInitialized( false )
    , m_SceneEditor( NULL )
    {
    }

    ///////////////////////////////////////////////////////////////////////////
    // Destructor
    // 
    virtual ~LightingList()
    {
      // This code is commented out because it is not currently needed.  This window's
      // lifetime is tied to the Scene Editor.  The Scene Editor will be in the
      // process of being deleted when this item gets deleted, so we can't safely 
      // access it.  If, in the future, we decide to destroy the contents of the
      // lighting UI when the window is hidden, we will need to remove these listeners.

      //m_SceneEditor->GetSceneManager()->RemoveSceneAddedListener( SceneChangeSignature::Delegate ( this, &LightingList::SceneAdded ) );
      //m_SceneEditor->GetSceneManager()->RemoveSceneRemovingListener( SceneChangeSignature::Delegate ( this, &LightingList::SceneRemoved ) );

      //// Make sure to remove our listener callbacks from all the items in the list.
      //RM_ObjToId::M_AToB::const_iterator itemItr = m_Map.AToB().begin();
      //RM_ObjToId::M_AToB::const_iterator itemEnd = m_Map.AToB().end();
      //for ( ; itemItr != itemEnd; ++itemItr )
      //{
      //  T* item = itemItr->first;
      //  item->RemoveNameChangedListener( SceneNodeChangeSignature::Delegate ( this, &LightingList::NodeNameChanged ) );
      //}

      //// Disconnect any listeners that we attached to any scenes
      //M_SceneSmartPtr::const_iterator sceneItr = m_SceneEditor->GetSceneManager()->GetScenes().begin();
      //M_SceneSmartPtr::const_iterator sceneEnd = m_SceneEditor->GetSceneManager()->GetScenes().end();
      //for ( ; sceneItr != sceneEnd; ++sceneItr )
      //{
      //  const ScenePtr& scene = sceneItr->second;
      //  DisconnectSceneListeners( scene );
      //}

      // Disconnect UI list listeners
      Disconnect( GetId(), wxEVT_COMMAND_LIST_COL_CLICK, wxListEventHandler( LightingList::OnColumnClick ), NULL, this );
      Disconnect( GetId(), wxEVT_COMMAND_LIST_END_LABEL_EDIT, wxListEventHandler( LightingList::OnEndLabelEdit ), NULL, this );
      RemoveSelectionChangingListener( UIToolKit::ListSelectionChangingSignature::Delegate ( this, &LightingList::ListSelectionChanging ) );
      RemoveSelectionChangedListener( UIToolKit::ListSelectionChangedSignature::Delegate ( this, &LightingList::ListSelectionChanged ) );
    }

    ///////////////////////////////////////////////////////////////////////////
    // Call this function once before calling any other functions on this class.
    // 
    void Init( SceneEditor* sceneEditor )
    {
      // Only call this function once
      NOC_ASSERT( !m_IsInitialized );

      if ( !m_IsInitialized )
      {
        m_SceneEditor = sceneEditor;
        NOC_ASSERT( m_SceneEditor );
        m_SceneEditor->GetSceneManager()->AddSceneAddedListener( SceneChangeSignature::Delegate ( this, &LightingList::SceneAdded ) );
        m_SceneEditor->GetSceneManager()->AddSceneRemovingListener( SceneChangeSignature::Delegate ( this, &LightingList::SceneRemoved ) );

        // Connect listeners
        Connect( GetId(), wxEVT_COMMAND_LIST_COL_CLICK, wxListEventHandler( LightingList::OnColumnClick ), NULL, this );
        Connect( GetId(), wxEVT_COMMAND_LIST_END_LABEL_EDIT, wxListEventHandler( LightingList::OnEndLabelEdit ), NULL, this );
        AddSelectionChangingListener( UIToolKit::ListSelectionChangingSignature::Delegate ( this, &LightingList::ListSelectionChanging ) );
        AddSelectionChangedListener( UIToolKit::ListSelectionChangedSignature::Delegate ( this, &LightingList::ListSelectionChanged ) );

        m_IsInitialized = true;
      }
    }

    ///////////////////////////////////////////////////////////////////////////
    // Returns the templated item that is represented by the list index specified.
    // 
    T* FindListItem( const i32 itemIndex ) const
    {
      T* item = NULL;
      RM_ObjToId::M_BToA::const_iterator found = m_Map.BToA().find( GetItemData( itemIndex ) );
      if ( found != m_Map.BToA().end() )
      {
        item = *found->second;
      }
      return item;
    }

    // 
    // Overrideable functions
    // 

  public:
    ///////////////////////////////////////////////////////////////////////////
    // Derived classes should NOC_OVERRIDE this function to remove all items that
    // are in their list which belong to the specified scene.
    // 
    virtual void RemoveSceneItems( Luna::Scene* scene ) = 0;

  protected:
    ///////////////////////////////////////////////////////////////////////////
    // Derived classes should NOC_OVERRIDE this function to actually build the 
    // list item and add it (and all its column data) to the list UI.  This
    // function will be called from AddListItem.
    // 
    virtual void DoAddListItem( T* item, const i32 itemID ) = 0;

    ///////////////////////////////////////////////////////////////////////////
    // Derived classes should NOC_OVERRIDE this function to indicate whether they
    // can accept the specified item or not.
    // 
    virtual bool ShouldAddItem( T* item )
    {
      return item != NULL;
    }

    ///////////////////////////////////////////////////////////////////////////
    // Connects listeners to the specified scene so that this list will stay
    // up to date as the scene is changed.
    // 
    void ConnectSceneListeners( Luna::Scene* scene )
    {
      scene->AddNodeAddedListener( NodeChangeSignature::Delegate ( this, &LightingList::NodeCreated ) );
      scene->AddNodeRemovedListener( NodeChangeSignature::Delegate ( this, &LightingList::NodeDeleted ) );
      scene->AddLoadStartedListener( LoadSignature::Delegate ( this, &LightingList::SceneLoadStarted ) );
      scene->AddLoadFinishedListener( LoadSignature::Delegate ( this, &LightingList::SceneLoadFinished ) );
      scene->AddSelectionChangedListener( SelectionChangedSignature::Delegate ( this, &LightingList::SelectionChanged ) );
    }

    ///////////////////////////////////////////////////////////////////////////
    // Removes all listeners that this list added to the specified scene.
    // 
    void DisconnectSceneListeners( Luna::Scene* scene )
    {
      scene->RemoveNodeAddedListener( NodeChangeSignature::Delegate ( this, &LightingList::NodeCreated ) );
      scene->RemoveNodeRemovedListener( NodeChangeSignature::Delegate ( this, &LightingList::NodeDeleted ) );
      scene->RemoveLoadStartedListener( LoadSignature::Delegate ( this, &LightingList::SceneLoadStarted ) );
      scene->RemoveLoadFinishedListener( LoadSignature::Delegate ( this, &LightingList::SceneLoadFinished ) );
      scene->RemoveSelectionChangedListener( SelectionChangedSignature::Delegate ( this, &LightingList::SelectionChanged ) );
    }

    ///////////////////////////////////////////////////////////////////////////
    // Adds the specified typed item to the list.  Derived classes handle building
    // the UI (see DoAddListItem function).  This function ensures that the 
    // specified item has not already been added to the list before it adds it.
    // Returns true if the item was inserted, false if the item was already in
    // the list.
    // 
    bool AddListItem( T* item )
    {
      NOC_ASSERT( m_IsInitialized );

      // Maintain unique IDs for every item in every list
      static i32 instanceID = 0;

      bool inserted = false;

      // If the item is not already in the list
      if ( m_Map.AToB().find( item ) == m_Map.AToB().end() )
      {
        // Let the derived class really add the item to the list UI
        DoAddListItem( item, ++instanceID );

        // Listen for name changes on this item.
        item->AddNameChangedListener( SceneNodeChangeSignature::Delegate ( this, &LightingList::NodeNameChanged ) );

        // Maintain a mapping of all the items in this list to their ids
        inserted = m_Map.Insert( item, instanceID );
        NOC_ASSERT( inserted );
      }

      return inserted;
    }

    ///////////////////////////////////////////////////////////////////////////
    // Removes the specified item from the list.
    // 
    void RemoveListItem( T* item )
    {
      NOC_ASSERT( m_IsInitialized );
      if ( m_IsInitialized )
      {
        RM_ObjToId::M_AToB::const_iterator found = m_Map.AToB().find( item );
        if ( found != m_Map.AToB().end() )
        {
          i32 data = *found->second;
          long itemToDelete = FindItem( -1, data );
          if ( itemToDelete != -1 )
          {
            DeleteItem( itemToDelete );
          }
          m_Map.RemoveA( item );
        }
      }
    }

    // 
    // Application and UI callbacks
    // 
  protected:
    ///////////////////////////////////////////////////////////////////////////
    // Callback for when a scene is added to the scene manager.  Connects 
    // our listeners to the specified scene.
    // 
    virtual void SceneAdded( const SceneChangeArgs& args )
    {
      ConnectSceneListeners( args.m_Scene );
    }

    ///////////////////////////////////////////////////////////////////////////
    // Callback for when a scene is removed from the scene manager.  Disconnets
    // our listeners and removes and items from this list that were part of
    // the specified scene.
    // 
    virtual void SceneRemoved( const SceneChangeArgs& args )
    {
      RemoveSceneItems( args.m_Scene );
      DisconnectSceneListeners( args.m_Scene );
    }

    ///////////////////////////////////////////////////////////////////////////
    // Called when a scene has started loading.  Prevents any UI updates from
    // taking place until after the scene load is complete.
    // 
    void SceneLoadStarted( const LoadArgs& args )
    {
      Freeze();
    }

    ///////////////////////////////////////////////////////////////////////////
    // Called when a scene has finished loading.  Resumes the UI updates for
    // this list.
    // 
    void SceneLoadFinished( const LoadArgs& args )
    {
      Thaw();
    }

    ///////////////////////////////////////////////////////////////////////////
    // Callback for when a scene's selection list has been updated.  If the 
    // source of the change was this class, we will not be notified (see the
    // ListSelectionChanged function).  Makes the list UI's selection match
    // the specified selection list.
    // 
    void SelectionChanged( const OS_SelectableDumbPtr& selection )
    {
      Freeze();
      DeselectAll();
      
      i32 lastSelectedItemId = -1;
      const RM_ObjToId::M_AToB::const_iterator mapEnd = m_Map.AToB().end();
      OS_SelectableDumbPtr::Iterator selItr = selection.Begin();
      OS_SelectableDumbPtr::Iterator selEnd = selection.End();
      for ( ; selItr != selEnd; ++selItr )
      {
        RM_ObjToId::M_AToB::const_iterator found = m_Map.AToB().find( Reflect::ObjectCast< T >( *selItr ) );
        if ( found != mapEnd )
        {

          i32 itemData = *found->second;
          i32 itemId = FindItem( -1, itemData );
          if ( itemId >= 0 )
          {
            Select( itemId, true );
            lastSelectedItemId = itemId;
          }
        }
      }

      if ( lastSelectedItemId >= 0 )
      {
        EnsureVisible( lastSelectedItemId );
      }

      Thaw();
    }

    ///////////////////////////////////////////////////////////////////////////
    // Callback for when a node is added to a scene.  Adds this item to the
    // list if it is suppose to be there.
    // 
    void NodeCreated( const NodeChangeArgs& args )
    {
      T* item = Reflect::ObjectCast< T >( args.m_Node );
      if ( item && ShouldAddItem( item ) )
      {
        AddListItem( item );
      }
    }

    ///////////////////////////////////////////////////////////////////////////
    // Callback for when a node is deleted from a scene.  Removes the item from
    // this list (if it was there to begin with).
    // 
    void NodeDeleted( const NodeChangeArgs& args )
    {
      T* item = Reflect::ObjectCast< T >( args.m_Node );
      if ( item )
      {
        RemoveListItem( item );
      }
    }

    ///////////////////////////////////////////////////////////////////////////
    // Callback for when the name of a node is changed.  Updates the list
    // item that goes with the node so that it matches.
    // 
    void NodeNameChanged( const SceneNodeChangeArgs& args )
    {
      RM_ObjToId::M_AToB::const_iterator found = m_Map.AToB().find( Reflect::ObjectCast< T >( args.m_Node ) );
      if ( found != m_Map.AToB().end() )
      {
        i32 data = *found->second;
        long id = FindItem( -1, data );
        if ( id >= 0 )
        {
          SetItemText( id, args.m_Node->GetName().c_str() );
        }
      }
    }

    ///////////////////////////////////////////////////////////////////////////
    // Sorts the list by the text in the column that was clicked on.
    // 
    void OnColumnClick( wxListEvent& args )
    {
      if ( args.GetColumn() >= 0 )
      {
        SortItems( args.GetColumn() );
      }
    }

    ///////////////////////////////////////////////////////////////////////////
    // UI callback for when the user is done renaming a list item.  Renames
    // the scene node that is represented by this list item.  This operation
    // is undoable.
    // 
    void OnEndLabelEdit( wxListEvent& args )
    {
      args.Skip();
      if ( !args.IsEditCancelled() )
      {
        RM_ObjToId::M_BToA::const_iterator found = m_Map.BToA().find( args.GetData() );
        if ( found != m_Map.BToA().end() )
        {
          Luna::SceneNode* node = *found->second;
          const std::string newName( args.GetLabel().c_str() );
          if ( node->GetName() != newName )
          {
            node->GetScene()->Push( new Undo::PropertyCommand< std::string >( new Nocturnal::MemberProperty< Luna::SceneNode, std::string >( node, &Luna::SceneNode::GetName, &Luna::SceneNode::SetGivenName ), newName ) );
            node->GetScene()->Execute( false );
          }
        }
      }
    }

    ///////////////////////////////////////////////////////////////////////////
    // UI callback for when the user is changing the selection within this list.
    // Vetos the change if the item they are trying to select is not selectable.
    // 
    void ListSelectionChanging( UIToolKit::ListSelectionChangeArgs& args )
    {
      if ( args.m_Select )
      {
        RM_ObjToId::M_BToA::const_iterator found = m_Map.BToA().find( args.m_Data );
        if ( found != m_Map.BToA().end() )
        {
          Selectable* item = *found->second;
          args.m_Veto = !item->IsSelectable();
        }
      }
    }

    ///////////////////////////////////////////////////////////////////////////
    // UI callback for when selection has changed within this list.  Updates
    // all the scene selection lists so that they match what is shown in this
    // list.
    // 
    void ListSelectionChanged( const UIToolKit::ListChangeArgs& args )
    {
      typedef std::map< Luna::Scene*, OS_SelectableDumbPtr > M_SceneSelections;
      M_SceneSelections selections;

      // Make a copy of all the scenes that are currently open
      M_SceneSmartPtr sceneList = m_SceneEditor->GetSceneManager()->GetScenes();

      // Go through all the newly selected items in the list
      long id = GetFirstSelected();
      while ( id >= 0 )
      {
        Luna::SceneNode* sceneNode = NULL;

        RM_ObjToId::M_BToA::const_iterator found = m_Map.BToA().find( args.m_List->GetItemData( id ) );
        if ( found != m_Map.BToA().end() )
        {
          sceneNode = *found->second;
        }

        if ( sceneNode )
        {
          Nocturnal::Insert<M_SceneSelections>::Result inserted = 
                    selections.insert( M_SceneSelections::value_type( sceneNode->GetScene(), OS_SelectableDumbPtr() ) );

          OS_SelectableDumbPtr& selectionList = inserted.first->second;
          selectionList.Append( sceneNode );

          // For each scene that gets added to the selectionList, remove it from the sceneList
          sceneList.erase( sceneNode->GetScene()->GetFullPath() );
        }

        id = GetNextSelected( id );
      }
     
      // We use this delegate to prevent SelectionChanged callbacks from coming back
      // into this class and creating an infinite feedback loop.
      SelectionChangedSignature::Delegate emitterChanged = SelectionChangedSignature::Delegate( this, &LightingList::SelectionChanged );

      // All the scenes left over in the sceneList need to have the selection cleared
      M_SceneSmartPtr::const_iterator sceneItr = sceneList.begin();
      M_SceneSmartPtr::const_iterator sceneEnd = sceneList.end();
      for ( ; sceneItr != sceneEnd; ++sceneItr )
      {
        const ScenePtr& scene = sceneItr->second;
        Undo::CommandPtr command = scene->GetSelection().Clear( SelectionChangingSignature::Delegate (), emitterChanged );
        scene->Push( command );
      }

      // Add new selections to each scene
      M_SceneSelections::const_iterator selectionItr = selections.begin();
      M_SceneSelections::const_iterator selectionEnd = selections.end();
      for ( ; selectionItr != selectionEnd; ++selectionItr )
      {
        Luna::Scene* scene = selectionItr->first;
        scene->Push( scene->GetSelection().SetItems( selectionItr->second, SelectionChangingSignature::Delegate (), emitterChanged ) );
      }
    }
  };
}
