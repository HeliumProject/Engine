#pragma once

#include "Foundation/Undo/UndoCommand.h"

#include "SceneGraph/API.h"
#include "SceneGraph/HierarchyNode.h"

namespace Helium
{
    namespace SceneGraph
    {
        class HELIUM_SCENE_GRAPH_API ReverseChildrenCommand : public UndoCommand
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