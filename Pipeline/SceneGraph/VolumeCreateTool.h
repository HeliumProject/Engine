#pragma once

#include "Foundation/TUID.h"

#include "Pipeline/API.h"
#include "Pipeline/SceneGraph/Volume.h"
#include "Pipeline/SceneGraph/CreateTool.h"

namespace Helium
{
    namespace SceneGraph
    {
        class PIPELINE_API VolumeCreateTool : public SceneGraph::CreateTool
        {
        public:
            static VolumeShape s_Shape;

        public:
            REFLECT_DECLARE_ABSTRACT(SceneGraph::VolumeCreateTool, SceneGraph::CreateTool);
            static void InitializeType();
            static void CleanupType();

        public:
            VolumeCreateTool(SceneGraph::Scene* scene, PropertiesGenerator* generator);
            virtual ~VolumeCreateTool();

            virtual SceneGraph::TransformPtr CreateNode() HELIUM_OVERRIDE;

            virtual void CreateProperties() HELIUM_OVERRIDE;

            int GetVolumeShape() const;
            void SetVolumeShape(int value);
        };
    }
}