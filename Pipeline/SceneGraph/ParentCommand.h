#pragma once

#include "Foundation/Undo/BatchCommand.h"

#include "Pipeline/API.h"
#include "Foundation/Memory/ReferenceCounting.h"

namespace Helium
{
    namespace SceneGraph
    {
        class HierarchyNode;
        typedef Helium::StrongPtr< SceneGraph::HierarchyNode > HierarchyNodePtr;

        class PIPELINE_API ParentCommand : public Undo::BatchCommand
        {
        public:
            typedef Undo::BatchCommand Base;

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