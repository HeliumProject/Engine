#include "Precompile.h"
#include "EntityAssetOutliner.h"
#include "Entity.h"
#include "EntityType.h"
#include "EntityAssetSet.h"
#include "Scene.h"
#include "SceneOutlinerItemData.h"
#include "Application/UI/ArtProvider.h"
#include "UI/Controls/Tree/SortTreeCtrl.h"

// Using
using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
EntityAssetOutliner::EntityAssetOutliner( Luna::SceneManager* sceneManager )
: SceneOutliner( sceneManager )
, m_InvisibleRoot( NULL )
{
  m_DisplayCounts = true; 
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
EntityAssetOutliner::~EntityAssetOutliner()
{
}

///////////////////////////////////////////////////////////////////////////////
// If a valid scene is specified, iterates over all the nodes in the scene
// and adds all the entity class sets to the tree.
// 
void EntityAssetOutliner::AddEntityTypes()
{
  if ( m_CurrentScene )
  {
    if ( m_CurrentScene->GetNodeTypesByName().size() > 0 )
    {
      m_TreeCtrl->Freeze();
      bool isSortingEnabled = m_TreeCtrl->IsSortingEnabled();
      m_TreeCtrl->DisableSorting();

      // Iterate over the node types, looking for the entity types
      Luna::EntityType* entityType = NULL;
      HM_StrToSceneNodeTypeSmartPtr::const_iterator typeItr = m_CurrentScene->GetNodeTypesByName().begin();
      HM_StrToSceneNodeTypeSmartPtr::const_iterator typeEnd = m_CurrentScene->GetNodeTypesByName().end();
      for ( ; typeItr != typeEnd; ++typeItr )
      {
        entityType = Reflect::ObjectCast< Luna::EntityType >( typeItr->second );
        if ( entityType )
        {
          AddEntityType( entityType );
        }
      }

      m_TreeCtrl->EnableSorting( isSortingEnabled );
      Sort( m_InvisibleRoot );
      m_TreeCtrl->Thaw();
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Hooks up listeners to the entity type for changes and attempts to add any
// existing entity class sets to the tree.
// 
void EntityAssetOutliner::AddEntityType( Luna::EntityType* entityType )
{
  // Iterate over all the entity instances and add them to the tree
  M_InstanceSetSmartPtr::const_iterator classItr = entityType->GetSets().begin();
  M_InstanceSetSmartPtr::const_iterator classEnd = entityType->GetSets().end();
  for ( ; classItr != classEnd; ++classItr )
  {
    Luna::EntityAssetSet* set = Reflect::ObjectCast<Luna::EntityAssetSet>( classItr->second );
    if (set)
    {
      AddEntityAssetSet( set );
    }
  }

  // Connect listeners
  entityType->AddSetAddedListener( InstanceTypeChangeSignature::Delegate ( this, &EntityAssetOutliner::SetAdded ) );
  entityType->AddSetRemovedListener( InstanceTypeChangeSignature::Delegate ( this, &EntityAssetOutliner::SetRemoved ) );
}

///////////////////////////////////////////////////////////////////////////////
// Unhooks listeners to the entity type and attempts to remove any existing
// entity class sets from the tree.
// 
void EntityAssetOutliner::RemoveEntityType( Luna::EntityType* entityType )
{
  // Iterate over all the entity instances and add them to the tree
  M_InstanceSetSmartPtr::const_iterator classItr = entityType->GetSets().begin();
  M_InstanceSetSmartPtr::const_iterator classEnd = entityType->GetSets().end();
  for ( ; classItr != classEnd; ++classItr )
  {
    Luna::EntityAssetSet* set = Reflect::ObjectCast<Luna::EntityAssetSet>( classItr->second );
    if (set)
    {
      RemoveEntityAssetSet( set );
    }
  }
  // Disconnect listeners
  entityType->RemoveSetAddedListener( InstanceTypeChangeSignature::Delegate ( this, &EntityAssetOutliner::SetAdded ) );
  entityType->RemoveSetRemovedListener( InstanceTypeChangeSignature::Delegate ( this, &EntityAssetOutliner::SetRemoved ) );
}

///////////////////////////////////////////////////////////////////////////////
// Adds the specified entity class set to the tree (including the instances
// that belong to the set).
// 
void EntityAssetOutliner::AddEntityAssetSet( Luna::EntityAssetSet* classSet )
{
  if ( m_CurrentScene )
  {
    m_TreeCtrl->Freeze();
    bool isSortingEnabled = m_TreeCtrl->IsSortingEnabled();
    m_TreeCtrl->DisableSorting();

    const i32 image = Nocturnal::GlobalFileIconsTable().GetIconID( TXT( "folder" ) );
    wxTreeItemId classItem = AddItem( m_InvisibleRoot, 
                                      classSet->GetName(), 
                                      image, 
                                      new SceneOutlinerItemData( classSet ), 
                                      false, 
                                      false);

    // Connect listeners
    classSet->AddInstanceAddedListener( InstanceSetChangeSignature::Delegate ( this, &EntityAssetOutliner::EntityAdded ) );
    classSet->AddInstanceRemovedListener( InstanceSetChangeSignature::Delegate ( this, &EntityAssetOutliner::EntityRemoved ) );

    // Add all existing instances
    S_InstanceDumbPtr::const_iterator entityItr = classSet->GetInstances().begin();
    S_InstanceDumbPtr::const_iterator entityEnd = classSet->GetInstances().end();
    for ( ; entityItr != entityEnd; ++entityItr )
    {
      AddEntity( Reflect::AssertCast<Luna::Entity>(*entityItr) );
    }

    m_TreeCtrl->EnableSorting( isSortingEnabled );
    m_TreeCtrl->SortChildren( classItem );
    m_TreeCtrl->SortChildren( m_InvisibleRoot );
    m_TreeCtrl->Thaw();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Removes the entity class set from the tree (all instances of this set should
// have already been removed).
// 
void EntityAssetOutliner::RemoveEntityAssetSet( Luna::EntityAssetSet* classSet )
{
  M_TreeItems::const_iterator found = m_Items.find( classSet );
  if ( found != m_Items.end() )
  {
    // All instances of this class set should have already been cleared out
    NOC_ASSERT( !m_TreeCtrl->HasChildren( found->second ) );

    // Disconnect listeners
    classSet->RemoveInstanceAddedListener( InstanceSetChangeSignature::Delegate ( this, &EntityAssetOutliner::EntityAdded ) );
    classSet->RemoveInstanceRemovedListener( InstanceSetChangeSignature::Delegate ( this, &EntityAssetOutliner::EntityRemoved ) );

    // Actually delete the item
    DeleteItem( classSet );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Adds the specified entity to the tree.  The entity will be parented under
// its class set.
// 
void EntityAssetOutliner::AddEntity( Luna::Entity* entity )
{
  LUNA_SCENE_SCOPE_TIMER( ("") );

  // Find the tree item that corresponds to the class set that this entity
  // belongs to.
  NOC_ASSERT( entity->GetClassSet() );
  M_TreeItems::const_iterator foundSet = m_Items.find( entity->GetClassSet() );
  NOC_ASSERT( foundSet != m_Items.end() );
  if ( foundSet != m_Items.end() )
  {
    m_TreeCtrl->Freeze();

    // Add the entity as a child of the class set
    const wxTreeItemId& parent = foundSet->second;
    wxTreeItemId insertedItem = AddItem( parent, entity->GetName(), entity->GetImageIndex(), new SceneOutlinerItemData( entity ), entity->IsSelected() );
    m_TreeCtrl->Thaw();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Removes the specified entity instance from the tree.
// 
void EntityAssetOutliner::RemoveEntity( Luna::Entity* entity )
{
  LUNA_SCENE_SCOPE_TIMER( ("") );

  // Remove the item from the tree
  DeleteItem( entity );
}

///////////////////////////////////////////////////////////////////////////////
// Creates the tree control managed by this class.  This function should only
// be called once.  The tree control is returned so that the caller can
// insert it into the UI in the proper place.
// 
SortTreeCtrl* EntityAssetOutliner::CreateTreeCtrl( wxWindow* parent, wxWindowID id )
{
  SortTreeCtrl* tree = new SortTreeCtrl( parent, id, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE | wxNO_BORDER | wxTR_HIDE_ROOT | wxTR_EDIT_LABELS | wxTR_MULTIPLE, wxDefaultValidator, wxT( "EntityAssetOutliner" ) );
  m_InvisibleRoot = tree->AddRoot( TXT( "INVISIBLE_ROOT" ) );

  // Override dynamic GUI event handlers here
  tree->Connect( tree->GetId(), wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT, wxTreeEventHandler( EntityAssetOutliner::OnBeginLabelEdit ), NULL, this );

  return tree;
}

///////////////////////////////////////////////////////////////////////////////
// Clears out the tree.
// 
void EntityAssetOutliner::Clear()
{
  __super::Clear();

  // ? Disconnect all listeners on existing tree items?

  // Put the invisible root back into the tree so we are ready to populate it again
  m_InvisibleRoot = m_TreeCtrl->AddRoot( TXT( "INVISIBLE_ROOT" ) );
}

///////////////////////////////////////////////////////////////////////////////
// Called when the base class has finished changing the current scene.  Loads
// the entity class sets into the tree control.
// 
void EntityAssetOutliner::CurrentSceneChanged( Luna::Scene* oldScene )
{
  AddEntityTypes();
}

///////////////////////////////////////////////////////////////////////////////
// Connects all the required event listeners to the current scene.
// 
void EntityAssetOutliner::ConnectSceneListeners()
{
  __super::ConnectSceneListeners();

  if ( m_CurrentScene )
  {
    // Connect listeners
    m_CurrentScene->AddNodeTypeAddedListener( NodeTypeExistenceSignature::Delegate ( this, &EntityAssetOutliner::NodeTypeAdded ) );
    m_CurrentScene->AddNodeTypeRemovedListener( NodeTypeExistenceSignature::Delegate ( this, &EntityAssetOutliner::NodeTypeRemoved ) );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Disconnects all the registered event listeners from this class on the current
// scene.
// 
void EntityAssetOutliner::DisconnectSceneListeners()
{
  if ( m_CurrentScene )
  {
    // Disconnect listeners
    m_CurrentScene->RemoveNodeTypeAddedListener( NodeTypeExistenceSignature::Delegate ( this, &EntityAssetOutliner::NodeTypeAdded ) );
    m_CurrentScene->RemoveNodeTypeRemovedListener( NodeTypeExistenceSignature::Delegate ( this, &EntityAssetOutliner::NodeTypeRemoved ) );
  }

  __super::DisconnectSceneListeners();
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when an entity set is added to an entity type.  If the set is
// an entity class set, it will be added to the tree.
// 
void EntityAssetOutliner::SetAdded( const InstanceTypeChangeArgs& args )
{
  if ( args.m_InstanceSet->HasType( Reflect::GetType<Luna::EntityAssetSet>() ) )
  {
    AddEntityAssetSet( Reflect::DangerousCast< Luna::EntityAssetSet >( args.m_InstanceSet ) );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when an entity set is removed from an entity type.  If the set
// is an entity class set, it will be removed from the tree.
// 
void EntityAssetOutliner::SetRemoved( const InstanceTypeChangeArgs& args )
{
  if ( args.m_InstanceSet->HasType( Reflect::GetType<Luna::EntityAssetSet>() ) )
  {
    RemoveEntityAssetSet( Reflect::DangerousCast< Luna::EntityAssetSet >( args.m_InstanceSet ) );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when an entity is added to the scene.  Updates the tree control.
// 
void EntityAssetOutliner::EntityAdded( const InstanceSetChangeArgs& args )
{
  AddEntity( Reflect::AssertCast<Luna::Entity>(args.m_Instance) );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when an entity is removed from the scene.  Updates the tree 
// control.
// 
void EntityAssetOutliner::EntityRemoved( const InstanceSetChangeArgs& args )
{
  RemoveEntity( Reflect::AssertCast<Luna::Entity>(args.m_Instance) );
}

///////////////////////////////////////////////////////////////////////////////
// Callback when a node type is added to the scene.  If the node type is an
// Luna::EntityType, this class starts tracking it.
// 
void EntityAssetOutliner::NodeTypeAdded( const NodeTypeExistenceArgs& args )
{
  if ( args.m_NodeType->HasType( Reflect::GetType<Luna::EntityType>() ) )
  {
    AddEntityType( Reflect::DangerousCast< Luna::EntityType >( args.m_NodeType ) );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback when a node type is removed from the scene.  If the node type is
// an Luna::EntityType, disconnect from it.
// 
void EntityAssetOutliner::NodeTypeRemoved( const NodeTypeExistenceArgs& args )
{
  if ( args.m_NodeType->HasType( Reflect::GetType<Luna::EntityType>() ) )
  {
    RemoveEntityType( Reflect::DangerousCast< Luna::EntityType >( args.m_NodeType ) );
  }
}

///////////////////////////////////////////////////////////////////////////////
// GUI callback for when the user tries to edit an item in the tree.  Vetos the
// event if the item is not allowed to be renamed.
// 
void EntityAssetOutliner::OnBeginLabelEdit( wxTreeEvent& args )
{
  Object* found = GetTreeItemData( args.GetItem() )->GetObject();

  // If a valid Object was not found, or if the the object is not
  // an entity node, we won't allow it's name to be changed.
  if ( !found || !found->HasType( Reflect::GetType<Luna::Entity>() ) )
  {
    args.Veto();
  }
}
