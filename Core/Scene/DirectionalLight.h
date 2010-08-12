#pragma once

#include "Light.h"
#include "Core/Content/Nodes/DirectionalLight.h"

namespace Helium
{
    namespace Core
    {
        class DirectionalLight : public Core::Light
        {
            //
            // Runtime Type Info
            //

        public:
            SCENE_DECLARE_TYPE( Core::DirectionalLight, Core::Light );
            static void InitializeType();
            static void CleanupType();


            //
            // Member functions
            //

            DirectionalLight(Core::Scene* scene);
            DirectionalLight(Core::Scene* scene, Content::DirectionalLight* light);

            virtual bool ValidatePanel(const tstring& name) HELIUM_OVERRIDE;
            static void CreatePanel( CreatePanelArgs& args );

            virtual i32 GetImageIndex() const HELIUM_OVERRIDE;
            virtual tstring GetApplicationTypeName() const HELIUM_OVERRIDE;

            // returns true if this is the main sun in a level, there can only be one
            bool GetGlobalSun() const;
            void SetGlobalSun( bool globalSun );

            float GetShadowSoftness() const;
            void SetShadowSoftness( float multiplier );

            int GetSoftShadowSamples() const;
            void SetSoftShadowSamples( int multiplier );
        };
    }
}