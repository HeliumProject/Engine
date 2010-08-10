#pragma once

#include "Core/API.h"
#include "Light.h"

namespace Helium
{
    namespace Content
    {
        class CORE_API DirectionalLight : public Light
        {
        public:
            virtual void Host(ContentVisitor* visitor);

            DirectionalLight ()
                : Light(),
                m_GlobalSun( false ),
                m_ShadowSoftness( 0.0f ),
                m_SoftShadowSamples( 32 )
            {
            }

            DirectionalLight (Helium::TUID &id)
                : Light (id),
                m_GlobalSun( false ),
                m_ShadowSoftness( 0.0f ),
                m_SoftShadowSamples( 32 )
            {

            }

            REFLECT_DECLARE_CLASS(DirectionalLight, Light);

            void GetDirection( Math::Vector3& direction );

            bool m_GlobalSun;

            f32 m_ShadowSoftness;
            u32 m_SoftShadowSamples;

            static void EnumerateClass( Reflect::Compositor<DirectionalLight>& comp );
        };

        typedef Helium::SmartPtr<DirectionalLight> DirectionalLightPtr;
        typedef std::vector<DirectionalLightPtr> V_DirectionalLight;
    }
}