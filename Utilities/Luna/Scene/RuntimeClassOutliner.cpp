#include "Precompile.h"
#include "RuntimeClassOutliner.h"
#include "Instance.h"
#include "InstanceType.h"
#include "InstanceCodeSet.h"
#include "Scene.h"
#include "SceneOutlinerItemData.h"
#include "UIToolKit/ImageManager.h"

#include "UIToolKit/SortTreeCtrl.h"

// Using
using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
RuntimeClassOutliner::RuntimeClassOutliner( Luna::SceneManager* sceneManager )
: SceneOutliner( sceneManager )
, m_InvisibleRoot( NULL )
{
  m_DisplayCounts = true; 
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
RuntimeClassOutliner::~RuntimeClassOutliner()
{
}

///////////////////////////////////////////////////////////////////////////////
// If a valid scene is specified, iterates over all the nodes in the scene
// and adds all the runtime class sets (aka code sets) to the tree.
// 
void RuntimeClassOutliner::AddInstanceTypes()
{
  if ( m_CurrentScene )
  {
    if ( m_CurrentScene->GetNodeTypesByName().size() > 0 )
    {
      m_TreeCtrl->Freeze();
      bool isSortingEnabled = m_TreeCtrl->IsSortingEnabled();
      m_TreeCtrl->DisableSorting();

      // Iterate over the node types, looking for the instance type
      Luna::InstanceType* instanceType = NULL;
      HM_StrToSceneNodeTypeSmartPtr::const_iterator typeItr = m_CurrentScene->GetNodeTypesByName().begin();
      HM_StrToSceneNodeTypeSmartPtr::const_iterator typeEnd = m_CurrentScene->GetNodeTypesByName().end();
      for ( ; typeItr != typeEnd; ++typeItr )
      {
        // If the cast is successful, we will break out of the loop
        instanceType = Reflect::ObjectCast< Luna::InstanceType >( typeItr->second );
        if ( instanceType )
        {
          AddInstanceType( instanceType );
        }
      }

      m_TreeCtrl->EnableSorting( isSortingEnabled );
      Sort( m_InvisibleRoot );
      m_TreeCtrl->Thaw();
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Hooks up listeners to the instance type for changes and attempts to add any
// existing instance class sets to the tree.
// 
void RuntimeClassOutliner::AddInstanceType( Luna::InstanceType* instanceType )
{
  // Iterate over all the instance instances and add them to the tree
  M_InstanceSetSmartPtr::const_iterator classItr = instanceType->GetSets().begin();
  M_InstanceSetSmartPtr::const_iterator classEnd = instanceType->GetSets().end();
  for ( ; classItr != classEnd; ++classItr )
  {
    Luna::InstanceCodeSet* set = Reflect::ObjectCast<Luna::InstanceCodeSet>( classItr->second );
    if (set)
    {
      AddRuntimeClassSet( set );
    }
  }

  // Connect listeners
  instanceType->AddSetAddedListener( InstanceTypeChangeSignature::Delegate ( this, &RuntimeClassOutliner::SetAdded ) );
  instanceType->AddSetRemovedListener( InstanceTypeChangeSignature::Delegate ( this, &RuntimeClassOutliner::SetRemoved ) );
}

///////////////////////////////////////////////////////////////////////////////
// Unhooks listeners to the instance type and attempts to remove any existing
// instance class sets from the tree.
// 
void RuntimeClassOutliner::RemoveInstanceType( Luna::InstanceType* instanceType )
{
  // Iterate over all the instance instances and add them to the tree
  M_InstanceSetSmartPtr::const_iterator classItr = instanceType->GetSets().begin();
  M_InstanceSetSmartPtr::const_iterator classEnd = instanceType->GetSets().end();
  for ( ; classItr != classEnd; ++classItr )
  {
    Luna::InstanceCodeSet* set = Reflect::ObjectCast<Luna::InstanceCodeSet>( classItr->second );
    if (set)
    {
      RemoveRuntimeClassSet( set );
    }
  }

  // Disconnect listeners
  instanceType->RemoveSetAddedListener( InstanceTypeChangeSignature::Delegate ( this, &RuntimeClassOutliner::SetAdded ) );
  instanceType->RemoveSetRemovedListener( InstanceTypeChangeSignature::Delegate ( this, &RuntimeClassOutliner::SetRemoved ) );
}

///////////////////////////////////////////////////////////////////////////////
// Adds the specified class set and all of the instance instances that belong to 
// the class set to the tree.
// 
void RuntimeClassOutliner::AddRuntimeClassSet( Luna::InstanceCodeSet* classSet )
{
  LUNA_SCENE_SCOPE_TIMER( ("") );

  if ( m_CurrentScene )
  {
    m_TreeCtrl->Freeze();
    bool isSortingEnabled = m_TreeCtrl->IsSortingEnabled();
    m_TreeCtrl->DisableSorting();

    const i32 image = UIToolKit::GlobalImageManager().GetImageIndex( "folder_16.png" );
    std::string name = classSet->GetName();
    if ( name.empty() )
    {
      name = "<NONE>";
    }
    wxTreeItemId classItem = AddItem( m_InvisibleRoot, 
                                      name, 
                                      image, 
                                      new SceneOutlinerItemData( classSet ), 
                                      false,
                                      false);

    // Connect listeners
    classSet->AddInstanceAddedListener( InstanceSetChangeSignature::Delegate ( this, &RuntimeClassOutliner::InstanceAdded ) );
    classSet->AddInstanceRemovedListener( InstanceSetChangeSignature::Delegate ( this, &RuntimeClassOutliner::InstanceRemoved ) );

    // Add all existing instances
    S_InstanceDumbPtr::const_iterator instanceItr = classSet->GetInstances().begin();
    S_InstanceDumbPtr::const_iterator instanceEnd = classSet->GetInstances().end();
    for ( ; instanceItr != instanceEnd; ++instanceItr )
    {
      AddInstance( *instanceItr );
    }

    m_TreeCtrl->EnableSorting( isSortingEnabled );
    m_TreeCtrl->SortChildren( classItem );
    m_TreeCtrl->SortChildren( m_InvisibleRoot );
    m_TreeCtrl->Thaw();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Removes the specified runtime class set from the tree.  All of the entities
// that belong to the class set should have already been removed.
// 
void RuntimeClassOutliner::RemoveRuntimeClassSet( Luna::InstanceCodeSet* classSet )
{
  M_TreeItems::const_iterator found = m_Items.find( classSet );
  if ( found != m_Items.end() )
  {
    // All instances of this class set should have already been cleared out
    NOC_ASSERT( !m_TreeCtrl->HasChildren( found->second ) );

    // Disconnect listeners
    classSet->RemoveInstanceAddedListener( InstanceSetChangeSignature::Delegate ( this, &RuntimeClassOutliner::InstanceAdded ) );
    classSet->RemoveInstanceRemovedListener( InstanceSetChangeSignature::Delegate ( this, &RuntimeClassOutliner::InstanceRemoved ) );

    // Actually delete the item
    DeleteItem( classSet );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Adds the specified instance to the tree.  The instance will be parented under
// the code set that it belongs to.
// 
void RuntimeClassOutliner::AddInstance( Luna::Instance* instance )
{
  LUNA_SCENE_SCOPE_TIMER( ("") );

  // Find the tree item that corresponds to the class set that this instance
  // belongs to.
  if ( instance->GetCodeSet() )
  {
    M_TreeItems::const_iterator foundSet = m_Items.find( instance->GetCodeSet() );
    NOC_ASSERT( foundSet != m_Items.end() );
    if ( foundSet != m_Items.end() )
    {
      m_TreeCtrl->Freeze();

      // Add the instance as a child of the code set
      const wxTreeItemId& parent = foundSet->second;
      wxTreeItemId insertedItem = AddItem( parent, instance->GetName(), instance->GetImageIndex(), new SceneOutlinerItemData( instance ), instance->IsSelected() );

      m_TreeCtrl->Thaw();
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Removes the specified instance from the tree.
// 
void RuntimeClassOutliner::RemoveInstance( Luna::Instance* instance )
{
  LUNA_SCENE_SCOPE_TIMER( ("") );

  // Remove the item from the tree
  DeleteItem( instance );
}

///////////////////////////////////////////////////////////////////////////////
// Creates the tree control managed by this class.  This function should only
// be called once.  The tree control is returned so that the caller can
// insert it into the UI in the proper place.
// 
UIToolKit::SortTreeCtrl* RuntimeClassOutliner::CreateTreeCtrl( wxWindow* parent, wxWindowID id )
{
  UIToolKit::SortTreeCtrl* tree = new UIToolKit::SortTreeCtrl( parent, id, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE | wxNO_BORDER | wxTR_HIDE_ROOT | wxTR_EDIT_LABELS | wxTR_MULTIPLE, wxDefaultValidator, "RuntimeClassOutliner" );
  m_InvisibleRoot = tree->AddRoot( "INVISIBLE_ROOT" );

  // Override dynamic GUI event handlers here
  tree->Connect( tree->GetId(), wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT, wxTreeEventHandler( RuntimeClassOutliner::OnBeginLabelEdit ), NULL, this );

  return tree;
}

///////////////////////////////////////////////////////////////////////////////
// Clears out the tree.
// 
void RuntimeClassOutliner::Clear()
{
  __super::Clear();

  // ? Disconnect all listeners on existing tree items?

  // Put the invisible root back into the tree so we are ready to populate it again
  m_InvisibleRoot = m_TreeCtrl->AddRoot( "INVISIBLE_ROOT" );
}

///////////////////////////////////////////////////////////////////////////////
// Called when the base class has finished changing the current scene.  Loads
// the runtime class sets into the tree control.
// 
void RuntimeClassOutliner::CurrentSceneChanged( Luna::Scene* oldScene )
{
  AddInstanceTypes();
}

///////////////////////////////////////////////////////////////////////////////
// Connects all the required event listeners to the current scene.
// 
void RuntimeClassOutliner::ConnectSceneListeners()
{
  __super::ConnectSceneListeners();

  if ( m_CurrentScene )
  {
    // Connect listeners
    m_CurrentScene->AddNodeTypeAddedListener( NodeTypeExistenceSignature::Delegate ( this, &RuntimeClassOutliner::NodeTypeAdded ) );
    m_CurrentScene->AddNodeTypeRemovedListener( NodeTypeExistenceSignature::Delegate ( this, &RuntimeClassOutliner::NodeTypeRemoved ) );
    m_CurrentScene->AddNodeAddedListener( NodeChangeSignature::Delegate ( this, &RuntimeClassOutliner::NodeAdded ) );
    m_CurrentScene->AddNodeRemovedListener( NodeChangeSignature::Delegate ( this, &RuntimeClassOutliner::NodeRemoved ) );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Disconnects all the registered event listeners from this class on the current
// scene.
// 
void RuntimeClassOutliner::DisconnectSceneListeners()
{
  if ( m_CurrentScene )
  {
    // Disconnect listeners
    m_CurrentScene->RemoveNodeTypeAddedListener( NodeTypeExistenceSignature::Delegate ( this, &RuntimeClassOutliner::NodeTypeAdded ) );
    m_CurrentScene->RemoveNodeTypeRemovedListener( NodeTypeExistenceSignature::Delegate ( this, &RuntimeClassOutliner::NodeTypeRemoved ) );
    m_CurrentScene->RemoveNodeAddedListener( NodeChangeSignature::Delegate ( this, &RuntimeClassOutliner::NodeAdded ) );
    m_CurrentScene->RemoveNodeRemovedListener( NodeChangeSignature::Delegate ( this, &RuntimeClassOutliner::NodeRemoved ) );
  }

  __super::DisconnectSceneListeners();
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a class set is added to an instance type.  Adds the class
// set (if it is for a runtime class, aka code class) to the tree.
// 
void RuntimeClassOutliner::SetAdded( const InstanceTypeChangeArgs& args )
{
  if ( args.m_InstanceSet->HasType( Reflect::GetType<Luna::InstanceCodeSet>() ) )
  {
    AddRuntimeClassSet( Reflect::DangerousCast< Luna::InstanceCodeSet >( args.m_InstanceSet ) );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a class set is removed from an instance type.  Updates the
// tree accordingly.
// 
void RuntimeClassOutliner::SetRemoved( const InstanceTypeChangeArgs& args )
{
  if ( args.m_InstanceSet->HasType( Reflect::GetType<Luna::InstanceCodeSet>() ) )
  {
    RemoveRuntimeClassSet( Reflect::DangerousCast< Luna::InstanceCodeSet >( args.m_InstanceSet ) );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when an instance is added to an instance set.  Updates the tree to
// Reflect the change.
// 
void RuntimeClassOutliner::InstanceAdded( const InstanceSetChangeArgs& args )
{
  AddInstance( args.m_Instance );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when an instance is removed from an instance set.  Updates the tree
// to Reflect the change.
// 
void RuntimeClassOutliner::InstanceRemoved( const InstanceSetChangeArgs& args )
{
  RemoveInstance( args.m_Instance );
}

///////////////////////////////////////////////////////////////////////////////
// Callback when a node type is added to the scene.  If the node type is an
// Luna::InstanceType, this class starts tracking it.
// 
void RuntimeClassOutliner::NodeTypeAdded( const NodeTypeExistenceArgs& args )
{
  if ( args.m_NodeType->HasType( Reflect::GetType<Luna::InstanceType>() ) )
  {
    AddInstanceType( Reflect::DangerousCast< Luna::InstanceType >( args.m_NodeType ) );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback when a node type is removed from the scene.  If the node type is
// an Luna::InstanceType, disconnect from it.
// 
void RuntimeClassOutliner::NodeTypeRemoved( const NodeTypeExistenceArgs& args )
{
  if ( args.m_NodeType->HasType( Reflect::GetType<Luna::InstanceType>() ) )
  {
    RemoveInstanceType( Reflect::DangerousCast< Luna::InstanceType >( args.m_NodeType ) );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback when a node is added to the scene.  Restores the item into the
// tree control.
// 
void RuntimeClassOutliner::NodeAdded( const NodeChangeArgs& args )
{
  if ( args.m_Node->HasType( Reflect::GetType<Luna::Instance>() ) )
  {
    Luna::Instance* instance = Reflect::DangerousCast< Luna::Instance >( args.m_Node );
    // If this instance is not already in the tree, add it.
    if ( m_Items.find( instance ) == m_Items.end() )
    {
      AddInstance( instance );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback when a node is removed from the scene.  Deletes the item from the
// tree control.
// 
void RuntimeClassOutliner::NodeRemoved( const NodeChangeArgs& args )
{
  if ( args.m_Node->HasType( Reflect::GetType<Luna::Instance>() ) )
  {
    RemoveInstance( Reflect::DangerousCast< Luna::Instance >( args.m_Node ) );
  }
}

///////////////////////////////////////////////////////////////////////////////
// GUI callback for when the user tries to edit an item in the tree.  Vetos the
// event if the item is not allowed to be renamed.
// 
void RuntimeClassOutliner::OnBeginLabelEdit( wxTreeEvent& args )
{
  Object* found = GetTreeItemData( args.GetItem() )->GetObject();

  // If a valid Object was not found, or if the the object is not
  // an instance node, we won't allow it's name to be changed.
  if ( !found || !found->HasType( Reflect::GetType<Luna::Instance>() ) )
  {
    args.Veto();
  }
}
