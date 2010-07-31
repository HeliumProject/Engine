#pragma once

#include <set>

#include "Platform/Types.h"

namespace Helium
{
    namespace Editor
    {
        // Forwards
        class SceneManager;
        class SortTreeCtrl;
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
            typedef std::set< SortTreeCtrl* > S_Trees;

            Editor::SceneManager* m_SceneManager;
            S_Trees m_Trees;
            u32 m_FreezeTreeSorting;
            bool m_NeedsSorting;

        public:
            TreeMonitor( Editor::SceneManager* sceneManager );
            virtual ~TreeMonitor();

            void AddTree( SortTreeCtrl* tree );
            void RemoveTree( SortTreeCtrl* tree );
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
}