#pragma once

#include "Foundation/TUID.h"

#include "EditorScene/API.h"
#include "EditorScene/Locator.h"
#include "EditorScene/CreateTool.h"

namespace Helium
{
	namespace Editor
	{
		class HELIUM_EDITOR_SCENE_API LocatorCreateTool : public CreateTool
		{
		public:
			static LocatorShape s_Shape;

		public:
			HELIUM_DECLARE_ABSTRACT(Editor::LocatorCreateTool, Editor::CreateTool);

		public:
			LocatorCreateTool(Editor::Scene* scene, PropertiesGenerator* generator);
			virtual ~LocatorCreateTool();

			virtual Editor::TransformPtr CreateNode() HELIUM_OVERRIDE;

			virtual void CreateProperties() HELIUM_OVERRIDE;

			int GetLocatorShape() const;
			void SetLocatorShape(int value);
		};
	}
}