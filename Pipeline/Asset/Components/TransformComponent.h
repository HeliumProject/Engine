#pragma once

#include "Pipeline/API.h"
#include "Foundation/Component/Component.h"

namespace Helium
{
    namespace Asset
    {
        class PIPELINE_API TransformComponent : public Component::ComponentBase
        {
        private:
            Vector3 m_Translation;
            Vector3 m_Rotation;
            Vector3 m_Scale;

        public:

            REFLECT_DECLARE_CLASS( TransformComponent, Component::ComponentBase );

            static void EnumerateClass( Reflect::Compositor< TransformComponent >& comp );

        public:
            TransformComponent()
            {
            }

            virtual ~TransformComponent()
            {
            }

        public:
            virtual Component::ComponentUsage GetComponentUsage() const HELIUM_OVERRIDE;

        public:

            const Vector3& GetTranslation() const;
            const Vector3& GetRotation() const;
            const Vector3& GetScale() const;

            void SetTranslation( const Vector3& translation );
            void SetRotation( const Vector3& rotation );
            void SetScale( const Vector3& scale );

        };
    }
}