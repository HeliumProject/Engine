#pragma once

#include "Editor/API.h"
#include "Foundation/Automation/Event.h"     // for Helium::Delegate
#include "Editor/Scene/SceneNode.h"
#include "Editor/Object.h"

namespace Helium
{
    namespace Editor
    {
        struct EDITOR_SCENE_API EvaluateResult
        {
            f32 m_TotalTime;
            u32 m_NodeCount;

            EvaluateResult()
                : m_TotalTime (0.f)
                , m_NodeCount (0)
            {

            }
        };

        // fwd
        class SceneGraph;
        typedef Helium::SmartPtr< SceneGraph > SceneGraphPtr;

        //
        //  
        //

        struct SceneGraphEvaluatedArgs
        {
            S_SceneNodeDumbPtr m_Nodes;

            SceneGraphEvaluatedArgs( S_SceneNodeDumbPtr& nodes )
                : m_Nodes( nodes )
            {

            }
        };

        typedef Helium::Signature< void, const Editor::SceneGraphEvaluatedArgs& > SceneGraphEvaluatedSignature;


        /////////////////////////////////////////////////////////////////////////////
        // Manages the dependency graph defining relationships among dependency nodes.
        // Evaluates dirty nodes when appropriate, and notifies interested listeners
        // that evaluation has occurred.
        // 
        class EDITOR_SCENE_API SceneGraph : public Object
        {
            //
            // Members
            //

        private:
            // original nodes have no ancestors
            S_SceneNodeDumbPtr m_OriginalNodes;

            // intermediate nodes have ancestors and descendants
            S_SceneNodeDumbPtr m_IntermediateNodes;

            // terminal nodes have no descendants
            S_SceneNodeDumbPtr m_TerminalNodes;

            // roots for the cleanup traversal
            S_SceneNodeDumbPtr m_CleanupRoots;

            // id for assignment
            u32 m_NextID;

            // id for evaluating
            u32 m_CurrentID;

            // number of nodes evaluated
            S_SceneNodeDumbPtr m_EvaluatedNodes;


            //
            // Runtime Type Info
            //

        public:
            EDITOR_DECLARE_TYPE( SceneGraph, Object );
            static void InitializeType();
            static void CleanupType();


            //
            // Implementation
            // 

        public:
            SceneGraph();

            // deconstruct graph
            void Reset();

            // assign traversal id
            u32 AssignVisitedID();

            // Resets visited values
            void ResetVisitedIDs();

            // this classifies a node into the node lists
            void Classify(Editor::SceneNode* n);

            // add node to the graph
            void AddNode(Editor::SceneNode* n);

            // remove node from the graph
            void RemoveNode(Editor::SceneNode* n);

            // Propagates dirty status down the graph
            u32 DirtyNode(Editor::SceneNode* n, GraphDirection direction);

            // do setup and traversal work to make all dirty nodes clean
            EvaluateResult EvaluateGraph(bool silent = false);

        private:
            // walk upstream and evaluate graph
            void Evaluate(Editor::SceneNode* node, GraphDirection direction);

        protected:
            mutable SceneGraphEvaluatedSignature::Event m_EvaluatedEvent;
        public:
            void AddEvaluatedListener( const SceneGraphEvaluatedSignature::Delegate& listener ) const
            {
                m_EvaluatedEvent.Add( listener );
            }
            void RemoveEvaluatedListener( const SceneGraphEvaluatedSignature::Delegate& listener ) const
            {
                m_EvaluatedEvent.Remove( listener );
            }
        };
    }
}