#pragma once

#include "Foundation/TUID.h"

#include "Pipeline/API.h"
#include "Pipeline/SceneGraph/Locator.h"
#include "Pipeline/SceneGraph/CreateTool.h"

namespace Helium
{
    namespace SceneGraph
    {
        class PIPELINE_API LocatorCreateTool : public CreateTool
        {
        public:
            static LocatorShape s_Shape;

        public:
            REFLECT_DECLARE_ABSTRACT(SceneGraph::LocatorCreateTool, SceneGraph::CreateTool);
            static void InitializeType();
            static void CleanupType();

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