#include "Precompile.h"
#include "TreeMonitor.h"

#include "Scene.h"
#include "SceneManager.h"
#include "SceneNode.h"

#include "Application/UI/SortTreeCtrl.h"

using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
TreeMonitor::TreeMonitor( Luna::SceneManager* sceneManager )
: m_SceneManager( sceneManager )
, m_FreezeTreeSorting( 0 )
, m_NeedsSorting( false )
{
  m_SceneManager->AddSceneAddedListener( SceneChangeSignature::Delegate( this, &TreeMonitor::OnSceneAdded ) );
  m_SceneManager->AddSceneRemovingListener( SceneChangeSignature::Delegate( this, &TreeMonitor::OnSceneRemoving ) );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
TreeMonitor::~TreeMonitor()
{
  m_SceneManager->RemoveSceneAddedListener( SceneChangeSignature::Delegate( this, &TreeMonitor::OnSceneAdded ) );
  m_SceneManager->RemoveSceneRemovingListener( SceneChangeSignature::Delegate( this, &TreeMonitor::OnSceneRemoving ) );
}

///////////////////////////////////////////////////////////////////////////////
// Adds a tree to the list managed by this class.
// 
void TreeMonitor::AddTree( Nocturnal::SortTreeCtrl* tree )
{
  m_Trees.insert( tree );
}

///////////////////////////////////////////////////////////////////////////////
// Removes the specified tree from the list managed by this class.
// 
void TreeMonitor::RemoveTree( Nocturnal::SortTreeCtrl* tree )
{
  m_Trees.erase( tree );
}

///////////////////////////////////////////////////////////////////////////////
// Removes all the trees from the list managed by this class.
// 
void TreeMonitor::ClearTrees()
{
  m_Trees.clear();
}

///////////////////////////////////////////////////////////////////////////////
// Disables sorting in all tree controls.  Every call to FreezeSorting should
// be matched by a call to ThawSorting.  It is safe to call FreezeSorting 
// multiple times before calling ThawSorting for the first time.
// 
void TreeMonitor::FreezeSorting()
{
  if ( ++m_FreezeTreeSorting == 1 )
  {
    m_NeedsSorting = false;

    // for each tree, disable sorting
    S_Trees::const_iterator treeItr = m_Trees.begin();
    S_Trees::const_iterator treeEnd = m_Trees.end();
    for ( ; treeItr != treeEnd; ++treeItr )
    {
      ( *treeItr )->DisableSorting();
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// If this is the last call to thaw, sorting is re-enabled on all the tree
// controls, and if this class determines that the trees need to be sorted,
// that sort operation will also be carried out.
// 
void TreeMonitor::ThawSorting()
{
  if ( m_FreezeTreeSorting )
  {
    if ( --m_FreezeTreeSorting == 0 )
    {
      // for each tree, enable sorting and sort if necessary
      S_Trees::const_iterator treeItr = m_Trees.begin();
      S_Trees::const_iterator treeEnd = m_Trees.end();
      for ( ; treeItr != treeEnd; ++treeItr )
      {
        Nocturnal::SortTreeCtrl* tree = *treeItr;
        tree->EnableSorting();
        if ( m_NeedsSorting )
        {
          tree->Freeze();
          tree->Sort();
          tree->Thaw();
        }
      }

      m_NeedsSorting = false;
    }
  }
  else
  {
    // this means you are trying to thaw something that isn't frozen
    //  could be due to Changed() being emitted without a Changing() event preceeding it?
    NOC_BREAK(); 
  }
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if sorting is frozen at the moment.
// 
bool TreeMonitor::IsFrozen() const
{
  return m_FreezeTreeSorting > 0;
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a scene is added to the scene manager.  Adds listeners for
// nodes being added or removed from the scene.
// 
void TreeMonitor::OnSceneAdded( const SceneChangeArgs& args )
{
  args.m_Scene->AddNodeAddedListener( NodeChangeSignature::Delegate( this, &TreeMonitor::OnNodeAdded ) );
  args.m_Scene->AddNodeRemovedListener( NodeChangeSignature::Delegate( this, &TreeMonitor::OnNodeRemoved ) );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a scene is removed from the scene manager.  Removes 
// listeners for node changes from the scene.
// 
void TreeMonitor::OnSceneRemoving( const SceneChangeArgs& args )
{
  args.m_Scene->RemoveNodeAddedListener( NodeChangeSignature::Delegate( this, &TreeMonitor::OnNodeAdded ) );
  args.m_Scene->RemoveNodeRemovedListener( NodeChangeSignature::Delegate( this, &TreeMonitor::OnNodeRemoved ) );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a node is added to a scene.  Adds a name change listner
// to the node.  If sorting is currently frozen, this will flag that the tree
// needs sorted once thawed.
// 
void TreeMonitor::OnNodeAdded( const NodeChangeArgs& args )
{
  args.m_Node->AddNameChangedListener( SceneNodeChangeSignature::Delegate( this, &TreeMonitor::OnNodeRenamed ) );

  if ( IsFrozen() && args.m_Node->GetScene() == m_SceneManager->GetCurrentScene() )
  {
    m_NeedsSorting = true;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a node is removed from a scene.  Removes the listener for
// rename events on this node.
// 
void TreeMonitor::OnNodeRemoved( const NodeChangeArgs& args )
{
  // stop listening for rename
  args.m_Node->RemoveNameChangedListener( SceneNodeChangeSignature::Delegate( this, &TreeMonitor::OnNodeRenamed ) );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a node is renamed.  If sorting is currently frozen, this
// will flag that we need to sort the trees when thawed.
// 
void TreeMonitor::OnNodeRenamed( const SceneNodeChangeArgs& args )
{
  if ( IsFrozen() )
  {
    m_NeedsSorting = true;
  }
}
