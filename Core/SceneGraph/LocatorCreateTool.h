#pragma once

#include "Foundation/TUID.h"

#include "Core/API.h"
#include "Core/SceneGraph/Locator.h"
#include "Core/SceneGraph/CreateTool.h"

namespace Helium
{
    namespace SceneGraph
    {
        class CORE_API LocatorCreateTool : public SceneGraph::CreateTool
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