#pragma once

#include "Foundation/TUID.h"

#include "Core/API.h"
#include "Core/SceneGraph/Volume.h"
#include "Core/SceneGraph/CreateTool.h"

namespace Helium
{
    namespace SceneGraph
    {
        class CORE_API DuplicateTool : public SceneGraph::CreateTool
        {
        public:
            REFLECT_DECLARE_ABSTRACT(SceneGraph::DuplicateTool, SceneGraph::CreateTool);
            static void InitializeType();
            static void CleanupType();

            DuplicateTool(SceneGraph::Scene* scene, PropertiesGenerator* generator);
            virtual ~DuplicateTool();

            virtual SceneGraph::TransformPtr CreateNode() HELIUM_OVERRIDE;

        private:
            SceneGraph::Transform* m_Source;
        };
    }
}