#pragma once

#include "Foundation/TUID.h"

#include "EditorScene/API.h"
#include "EditorScene/CreateTool.h"

namespace Helium
{
	namespace Editor
	{
		class HELIUM_EDITOR_SCENE_API DuplicateTool : public CreateTool
		{
		public:
			HELIUM_DECLARE_ABSTRACT(Editor::DuplicateTool, Editor::CreateTool);

			DuplicateTool(Editor::Scene* scene, PropertiesGenerator* generator);
			virtual ~DuplicateTool();

			virtual Editor::TransformPtr CreateNode() HELIUM_OVERRIDE;

		private:
			Editor::Transform* m_Source;
		};
	}
}