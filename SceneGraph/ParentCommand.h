#pragma once

#include "SceneGraph/API.h"

#include "Foundation/Memory/ReferenceCounting.h"
#include "Foundation/UndoQueue.h"

namespace Helium
{
    namespace SceneGraph
    {
        class HierarchyNode;
        typedef Helium::StrongPtr< SceneGraph::HierarchyNode > HierarchyNodePtr;

        class HELIUM_SCENE_GRAPH_API ParentCommand : public BatchUndoCommand
        {
        public:
            typedef BatchUndoCommand Base;

        private:
            HierarchyNodePtr m_Node;
            HierarchyNodePtr m_NextParent;
            HierarchyNodePtr m_PreviousParent;

        public:
            ParentCommand(const HierarchyNodePtr& child, const HierarchyNodePtr& parent);

            virtual void Undo() HELIUM_OVERRIDE;

            virtual void Redo() HELIUM_OVERRIDE;

            void Swap();
        };
    }
}