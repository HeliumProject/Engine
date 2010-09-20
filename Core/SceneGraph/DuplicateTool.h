#pragma once

#include "Foundation/TUID.h"

#include "Core/API.h"
#include "Core/SceneGraph/Volume.h"
#include "Core/SceneGraph/CreateTool.h"

namespace Helium
{
    namespace Core
    {
        class CORE_API DuplicateTool : public Core::CreateTool
        {
        public:
            REFLECT_DECLARE_ABSTRACT(Core::DuplicateTool, Core::CreateTool);
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