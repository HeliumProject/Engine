#pragma once

#include "Application/UndoQueue.h"

#include "EditorScene/API.h"
#include "EditorScene/HierarchyNode.h"

namespace Helium
{
	namespace Editor
	{
		class HELIUM_EDITOR_SCENE_API ReverseChildrenCommand : public UndoCommand
		{
		private:
			HierarchyNodePtr m_Node;

		public:
			ReverseChildrenCommand( const HierarchyNodePtr& node );
			virtual ~ReverseChildrenCommand();
			virtual void Undo() override;
			virtual void Redo() override;
		};
	}
}