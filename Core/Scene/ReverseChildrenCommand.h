#pragma once

#include "Foundation/Undo/Command.h"

#include "Core/API.h"
#include "Core/Scene/HierarchyNode.h"

namespace Helium
{
    namespace Core
    {
        class CORE_API ReverseChildrenCommand : public Undo::Command
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