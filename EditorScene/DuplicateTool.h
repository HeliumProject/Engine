#pragma once

#include "Foundation/TUID.h"

#include "SceneGraph/API.h"
#include "SceneGraph/CreateTool.h"

namespace Helium
{
	namespace SceneGraph
	{
		class HELIUM_SCENE_GRAPH_API DuplicateTool : public CreateTool
		{
		public:
			HELIUM_DECLARE_ABSTRACT(SceneGraph::DuplicateTool, SceneGraph::CreateTool);

			DuplicateTool(SceneGraph::Scene* scene, PropertiesGenerator* generator);
			virtual ~DuplicateTool();

			virtual SceneGraph::TransformPtr CreateNode() HELIUM_OVERRIDE;

		private:
			SceneGraph::Transform* m_Source;
		};
	}
}