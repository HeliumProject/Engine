#pragma once

#include "Foundation/Undo/Command.h"

#include "Pipeline/API.h"

namespace Helium
{
    namespace SceneGraph
    {
        // Forwards and typedefs
        class SceneNode;
        typedef Helium::SmartPtr< SceneGraph::SceneNode > SceneNodePtr;

        /////////////////////////////////////////////////////////////////////////////
        // Command for making or breaking connections between ancestor and descendant
        // dependency nodes.
        // 
        class PIPELINE_API DependencyCommand : public Undo::Command
        {
        public:
            // Is this command connecting or disconnecting the nodes?
            enum DependencyAction
            {
                Connect,
                Disconnect
            };

        private:
            DependencyAction m_Action;
            SceneNodePtr m_Ancestor;
            SceneNodePtr m_Descendant;

        public:
            DependencyCommand( DependencyAction action, const SceneNodePtr& ancestor, const SceneNodePtr& descendant );
            virtual ~DependencyCommand();
            virtual void Undo() HELIUM_OVERRIDE;
            virtual void Redo() HELIUM_OVERRIDE;
        };
    }
}