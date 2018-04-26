#pragma once

#include "EditorScene/API.h"
#include "Foundation/Event.h"     // for Helium::Delegate
#include "EditorScene/SceneNode.h"
#include "Reflect/Object.h"

namespace Helium
{
	namespace Editor
	{
		struct HELIUM_EDITOR_SCENE_API EvaluateResult
		{
			float32_t m_TotalTime;
			uint32_t m_NodeCount;

			EvaluateResult()
				: m_TotalTime (0.f)
				, m_NodeCount (0)
			{

			}
		};

		// fwd
		class Graph;
		typedef Helium::StrongPtr< Graph > SceneGraphPtr;

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

		typedef Helium::Signature< const Editor::SceneGraphEvaluatedArgs& > SceneGraphEvaluatedSignature;


		/////////////////////////////////////////////////////////////////////////////
		// Manages the dependency graph defining relationships among dependency nodes.
		// Evaluates dirty nodes when appropriate, and notifies interested listeners
		// that evaluation has occurred.
		// 
		class HELIUM_EDITOR_SCENE_API Graph : public Reflect::Object
		{
		public:
			HELIUM_DECLARE_CLASS( Graph, Reflect::Object );

		public:
			Graph();

			// deconstruct graph
			void Reset();

			// assign traversal id
			uint32_t AssignVisitedID();

			// Resets visited values
			void ResetVisitedIDs();

			// this classifies a node into the node lists
			void Classify(Editor::SceneNode* n);

			// add node to the graph
			void AddNode(Editor::SceneNode* n);

			// remove node from the graph
			void RemoveNode(Editor::SceneNode* n);

			// Propagates dirty status down the graph
			uint32_t DirtyNode(Editor::SceneNode* n, GraphDirection direction);

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
			uint32_t m_NextID;

			// id for evaluating
			uint32_t m_CurrentID;

			// number of nodes evaluated
			S_SceneNodeDumbPtr m_EvaluatedNodes;
		};
	}
}