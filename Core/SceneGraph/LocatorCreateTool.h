#pragma once

#include "Foundation/TUID.h"

#include "Core/API.h"
#include "Core/SceneGraph/Locator.h"
#include "Core/SceneGraph/CreateTool.h"

namespace Helium
{
    namespace Core
    {
        class CORE_API LocatorCreateTool : public Core::CreateTool
        {
        public:
            static LocatorShape s_Shape;

        public:
            REFLECT_DECLARE_ABSTRACT(Core::LocatorCreateTool, Core::CreateTool);
            static void InitializeType();
            static void CleanupType();

        public:
            LocatorCreateTool(Core::Scene* scene, PropertiesGenerator* generator);
            virtual ~LocatorCreateTool();

            virtual Core::TransformPtr CreateNode() HELIUM_OVERRIDE;

            virtual void CreateProperties() HELIUM_OVERRIDE;

            int GetLocatorShape() const;
            void SetLocatorShape(int value);
        };
    }
}