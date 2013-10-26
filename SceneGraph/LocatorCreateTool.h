#pragma once

#include "Foundation/TUID.h"

#include "SceneGraph/API.h"
#include "SceneGraph/Locator.h"
#include "SceneGraph/CreateTool.h"

namespace Helium
{
    namespace SceneGraph
    {
        class HELIUM_SCENE_GRAPH_API LocatorCreateTool : public CreateTool
        {
        public:
            static LocatorShape s_Shape;

        public:
            HELIUM_DECLARE_ABSTRACT(SceneGraph::LocatorCreateTool, SceneGraph::CreateTool);

        public:
            LocatorCreateTool(SceneGraph::Scene* scene, PropertiesGenerator* generator);
            virtual ~LocatorCreateTool();

            virtual SceneGraph::TransformPtr CreateNode() HELIUM_OVERRIDE;

            virtual void CreateProperties() HELIUM_OVERRIDE;

            int GetLocatorShape() const;
            void SetLocatorShape(int value);
        };
    }
}