#pragma once

#include "Foundation/TUID.h"

#include "Core/API.h"
#include "Core/Scene/Locator.h"
#include "Core/Scene/CreateTool.h"

namespace Helium
{
    namespace Core
    {
        class CORE_API LocatorCreateTool : public Core::CreateTool
        {
            //
            // Members
            //

        public:
            static Content::LocatorShape s_Shape;

            //
            // RTTI
            //

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