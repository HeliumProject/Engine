#pragma once

#include "Core/API.h"
#include "Locator.h"
#include "CreateTool.h"

#include "Foundation/TUID.h"

namespace Helium
{
    namespace Core
    {
        class LocatorCreateTool : public Core::CreateTool
        {
            //
            // Members
            //

        public:
            static Content::LocatorShape s_Shape;

            //
            // RTTI
            //

            SCENE_DECLARE_TYPE(Core::LocatorCreateTool, Core::CreateTool);
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