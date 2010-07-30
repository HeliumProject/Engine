#pragma once

#include "Editor/API.h"
#include "Locator.h"
#include "CreateTool.h"

#include "Foundation/TUID.h"

namespace Helium
{
    namespace Editor
    {
        class LocatorCreateTool : public Editor::CreateTool
        {
            //
            // Members
            //

        public:
            static Content::LocatorShape s_Shape;

            //
            // RTTI
            //

            EDITOR_DECLARE_TYPE(Editor::LocatorCreateTool, Editor::CreateTool);
            static void InitializeType();
            static void CleanupType();

        public:
            LocatorCreateTool(Editor::Scene* scene, PropertiesGenerator* generator);

            virtual ~LocatorCreateTool();

            virtual Editor::TransformPtr CreateNode() HELIUM_OVERRIDE;

            virtual void CreateProperties() HELIUM_OVERRIDE;

            int GetLocatorShape() const;

            void SetLocatorShape(int value);
        };
    }
}