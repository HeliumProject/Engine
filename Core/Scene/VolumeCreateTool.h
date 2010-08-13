#pragma once

#include "Foundation/TUID.h"

#include "Core/API.h"
#include "Core/Scene/Volume.h"
#include "Core/Scene/CreateTool.h"

namespace Helium
{
    namespace Core
    {
        class CORE_API VolumeCreateTool : public Core::CreateTool
        {
            //
            // Members
            //

        public:
            static Content::VolumeShape s_Shape;

            //
            // RTTI
            //

            SCENE_DECLARE_TYPE(Core::VolumeCreateTool, Core::CreateTool);
            static void InitializeType();
            static void CleanupType();

        public:
            VolumeCreateTool(Core::Scene* scene, PropertiesGenerator* generator);

            virtual ~VolumeCreateTool();

            virtual Core::TransformPtr CreateNode() HELIUM_OVERRIDE;

            virtual void CreateProperties() HELIUM_OVERRIDE;

            int GetVolumeShape() const;

            void SetVolumeShape(int value);
        };
    }
}