#pragma once

#include "Application/UndoQueue.h"
#include "Foundation/ReferenceCounting.h"

#include "EditorScene/API.h"

namespace Helium
{
	namespace Editor
	{
		// Forwards and typedefs
		class SceneNode;
		typedef Helium::StrongPtr< Editor::SceneNode > SceneNodePtr;

		/////////////////////////////////////////////////////////////////////////////
		// Command for making or breaking connections between ancestor and descendant
		// dependency nodes.
		// 
		class HELIUM_EDITOR_SCENE_API DependencyCommand : public UndoCommand
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
			virtual void Undo() override;
			virtual void Redo() override;
		};
	}
}