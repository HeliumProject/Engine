#pragma once

#include "Foundation/Undo/BatchCommand.h"

#include "Core/API.h"

namespace Helium
{
    namespace SceneGraph
    {
        class HierarchyNode;
        typedef Helium::SmartPtr< SceneGraph::HierarchyNode > HierarchyNodePtr;

        class CORE_API ParentCommand : public Undo::BatchCommand
        {
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