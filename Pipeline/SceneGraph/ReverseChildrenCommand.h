#pragma once

#include "Foundation/Undo/Command.h"

#include "Pipeline/API.h"
#include "Pipeline/SceneGraph/HierarchyNode.h"

namespace Helium
{
    namespace SceneGraph
    {
        class PIPELINE_API ReverseChildrenCommand : public Undo::Command
        {
        private:
            HierarchyNodePtr m_Node;

        public:
            ReverseChildrenCommand( const HierarchyNodePtr& node );
            virtual ~ReverseChildrenCommand();
            virtual void Undo() HELIUM_OVERRIDE;
            virtual void Redo() HELIUM_OVERRIDE;
        };
    }
}