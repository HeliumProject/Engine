#pragma once

#include "Foundation/TUID.h"

#include "Core/API.h"
#include "Core/Scene/Volume.h"
#include "Core/Scene/CreateTool.h"

namespace Helium
{
    namespace Core
    {
        class CORE_API DuplicateTool : public Core::CreateTool
        {
        public:
            SCENE_DECLARE_TYPE(Core::DuplicateTool, Core::CreateTool);
            static void InitializeType();
            static void CleanupType();

            DuplicateTool(Core::Scene* scene, PropertiesGenerator* generator);
            virtual ~DuplicateTool();

            virtual Core::TransformPtr CreateNode() HELIUM_OVERRIDE;

        private:
            Core::Transform* m_Source;
        };
    }
}