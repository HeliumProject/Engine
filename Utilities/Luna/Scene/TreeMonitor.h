#pragma once

#include "Platform/Types.h"

namespace UIToolKit { class SortTreeCtrl; }

namespace Luna
{
  // Forwards
  class SceneManager;
  struct NodeChangeArgs;
  struct SceneChangeArgs;
  struct SceneNodeChangeArgs;

  /////////////////////////////////////////////////////////////////////////////
  // This class allows you to add a number of tree controls to it, and it will
  // manage whether or not to enable sorting in those tree controls.  This 
  // is so that during operations that would normally cause multiple sorting
  // calls on the tree (such as renaming multiple nodes at a time) you can
  // freeze tree sorting until all the nodes are renamed, then just do one sort
  // at the end.
  // 
  class TreeMonitor
  {
  private:
    typedef std::set< UIToolKit::SortTreeCtrl* > S_Trees;

    Luna::SceneManager* m_SceneManager;
    S_Trees m_Trees;
    u32 m_FreezeTreeSorting;
    bool m_NeedsSorting;

  public:
    TreeMonitor( Luna::SceneManager* sceneManager );
    virtual ~TreeMonitor();

    void AddTree( UIToolKit::SortTreeCtrl* tree );
    void RemoveTree( UIToolKit::SortTreeCtrl* tree );
    void ClearTrees();

    void FreezeSorting();
    void ThawSorting();
    bool IsFrozen() const;

  private:
    void OnSceneAdded( const SceneChangeArgs& args );
    void OnSceneRemoving( const SceneChangeArgs& args );
    void OnNodeAdded( const NodeChangeArgs& args );
    void OnNodeRemoved( const NodeChangeArgs& args );
    void OnNodeRenamed( const SceneNodeChangeArgs& args );
  };
}
