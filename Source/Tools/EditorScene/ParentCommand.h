#pragma once

#include "EditorScene/API.h"

#include "Foundation/ReferenceCounting.h"
#include "Application/UndoQueue.h"

namespace Helium
{
	namespace Editor
	{
		class HierarchyNode;
		typedef Helium::StrongPtr< Editor::HierarchyNode > HierarchyNodePtr;

		class HELIUM_EDITOR_SCENE_API ParentCommand : public BatchUndoCommand
		{
		public:
			typedef BatchUndoCommand Base;

		private:
			HierarchyNodePtr m_Node;
			HierarchyNodePtr m_NextParent;
			HierarchyNodePtr m_PreviousParent;

		public:
			ParentCommand(const HierarchyNodePtr& child, const HierarchyNodePtr& parent);

			virtual void Undo() override;

			virtual void Redo() override;

			void Swap();
		};
	}
}