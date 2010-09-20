#pragma once

#include "Core/API.h"
#include "Core/SceneGraph/SceneNode.h"

namespace Helium
{
    namespace Core
    {
        class Scene;
        class HierarchyNode;

        class PickVisitor;
        class RenderVisitor;


        //
        // What action to take after the traverser visits a node
        //

        namespace TraversalActions
        {
            enum TraversalAction
            {
                Continue,   // keep traversing
                Prune,      // do not visit my children
                Abort,      // abort and unwind traversal
            };
        }

        typedef TraversalActions::TraversalAction TraversalAction;


        //
        // Hierarchy traverser walks the hierarchy of object in the scene
        //

        class HierarchyTraverser
        {
        public:
            virtual TraversalAction VisitHierarchyNode(Core::HierarchyNode* node) = 0;
        };


        //
        // Hierarchy traverser walks the hierarchy of object in the scene
        //

        class HierarchyChildTraverser : public HierarchyTraverser
        {
        public:
            OS_SceneNodeDumbPtr m_Children;

            virtual TraversalAction VisitHierarchyNode(Core::HierarchyNode* node) HELIUM_OVERRIDE;
        };


        //
        // Render the scene
        //

        class HierarchyRenderTraverser : public HierarchyTraverser
        {
        private:
            RenderVisitor* m_RenderVisitor;

        public:
            HierarchyRenderTraverser(RenderVisitor* renderVisitor);

            virtual TraversalAction VisitHierarchyNode(Core::HierarchyNode* node) HELIUM_OVERRIDE;
        };


        //
        // Pick items in the scene
        //

        class HierarchyPickTraverser : public HierarchyTraverser
        {
        private:
            PickVisitor* m_PickVisitor;

        public:
            HierarchyPickTraverser(PickVisitor* pickVisitor);

            virtual TraversalAction VisitHierarchyNode(Core::HierarchyNode* node) HELIUM_OVERRIDE;
        };
    }
}