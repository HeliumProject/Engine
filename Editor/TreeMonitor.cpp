#include "EditorPch.h"
#include "TreeMonitor.h"

#include "SceneGraph/Scene.h"
#include "SceneGraph/SceneManager.h"
#include "SceneGraph/SceneNode.h"

#include "Editor/Controls/Tree/SortTreeCtrl.h"

using namespace Helium;
using namespace Helium::SceneGraph;
using namespace Helium::Editor;

static const float32_t g_MinTimeBetweenSceneEvents = 1000.0f;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
TreeMonitor::TreeMonitor( SceneGraph::SceneManager* sceneManager )
: m_SceneManager( sceneManager )
, m_FreezeTreeSorting( 0 )
, m_NeedsSorting( false )
, m_ThawTimer( TXT( "Tree Monitor Thaw Timer" ), 2000 )
, m_SelfFrozen( false )
{
    m_SceneManager->e_SceneAdded.AddMethod( this, &TreeMonitor::OnSceneAdded );
    m_SceneManager->e_SceneRemoving.AddMethod( this, &TreeMonitor::OnSceneRemoving );

    m_ThawTimer.AddTickListener( TimerTickSignature::Delegate( this, &TreeMonitor::OnThawTimer ) );
    m_ThawTimer.Start();

    m_SceneChangedTimer.Reset();
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
TreeMonitor::~TreeMonitor()
{
    m_ThawTimer.RemoveTickListener( TimerTickSignature::Delegate( this, &TreeMonitor::OnThawTimer ) );
    m_ThawTimer.Stop();

    m_SceneManager->e_SceneAdded.RemoveMethod( this, &TreeMonitor::OnSceneAdded );
    m_SceneManager->e_SceneRemoving.RemoveMethod( this, &TreeMonitor::OnSceneRemoving );
}

///////////////////////////////////////////////////////////////////////////////
// Adds a tree to the list managed by this class.
// 
void TreeMonitor::AddTree( SortTreeCtrl* tree )
{
    m_Trees.insert( tree );
}

///////////////////////////////////////////////////////////////////////////////
// Removes the specified tree from the list managed by this class.
// 
void TreeMonitor::RemoveTree( SortTreeCtrl* tree )
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
                SortTreeCtrl* tree = *treeItr;
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
        HELIUM_BREAK(); 
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
    args.m_Scene->e_NodeAdded.Add( NodeChangeSignature::Delegate( this, &TreeMonitor::OnNodeAdded ) );
    args.m_Scene->e_NodeRemoved.Add( NodeChangeSignature::Delegate( this, &TreeMonitor::OnNodeRemoved ) );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a scene is removed from the scene manager.  Removes 
// listeners for node changes from the scene.
// 
void TreeMonitor::OnSceneRemoving( const SceneChangeArgs& args )
{
    args.m_Scene->e_NodeAdded.Remove( NodeChangeSignature::Delegate( this, &TreeMonitor::OnNodeAdded ) );
    args.m_Scene->e_NodeRemoved.Remove( NodeChangeSignature::Delegate( this, &TreeMonitor::OnNodeRemoved ) );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a node is added to a scene.  Adds a name change listner
// to the node.  If sorting is currently frozen, this will flag that the tree
// needs sorted once thawed.
// 
void TreeMonitor::OnNodeAdded( const NodeChangeArgs& args )
{
    args.m_Node->AddNameChangedListener( SceneNodeChangeSignature::Delegate( this, &TreeMonitor::OnNodeRenamed ) );

    // freeze sorting if we're getting nodes added too quickly
    // every 2 seconds sorting is automatically thawed
    if ( !IsFrozen() && m_SceneChangedTimer.Elapsed() < g_MinTimeBetweenSceneEvents )
    {
        FreezeSorting();
        m_SelfFrozen = true;
    }

    if ( IsFrozen() && args.m_Node->GetOwner() == m_SceneManager->GetCurrentScene() )
    {
        m_NeedsSorting = true;
    }

    m_SceneChangedTimer.Reset();
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a node is removed from a scene.  Removes the listener for
// rename events on this node.
// 
void TreeMonitor::OnNodeRemoved( const NodeChangeArgs& args )
{
    // freeze sorting if we're getting nodes removed too quickly
    // every 2 seconds sorting is automatically thawed
    if ( !IsFrozen() && m_SceneChangedTimer.Elapsed() < g_MinTimeBetweenSceneEvents )
    {
        FreezeSorting();
        m_SelfFrozen = true;
    }

    // stop listening for rename
    args.m_Node->RemoveNameChangedListener( SceneNodeChangeSignature::Delegate( this, &TreeMonitor::OnNodeRenamed ) );

    m_SceneChangedTimer.Reset();
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a node is renamed.  If sorting is currently frozen, this
// will flag that we need to sort the trees when thawed.
// 
void TreeMonitor::OnNodeRenamed( const SceneNodeChangeArgs& args )
{
    // freeze sorting if we're getting nodes renamed too quickly
    // every 2 seconds sorting is automatically thawed
    if ( !IsFrozen() && m_SceneChangedTimer.Elapsed() < g_MinTimeBetweenSceneEvents )
    {
        FreezeSorting();
        m_SelfFrozen = true;
    }

    if ( IsFrozen() )
    {
        m_NeedsSorting = true;
    }

    m_SceneChangedTimer.Reset();
}

void TreeMonitor::OnThawTimer( const TimerTickArgs& args )
{
    if ( m_SelfFrozen )
    {
        ThawSorting();
        m_SelfFrozen = false;
    }
}