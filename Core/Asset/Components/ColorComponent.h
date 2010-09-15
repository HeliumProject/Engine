#pragma once

#include "Core/API.h"
#include "Foundation/Component/Component.h"

namespace Helium
{
    namespace Asset
    {
        class CORE_API ColorComponent : public Component::ComponentBase
        {
        private:
            Math::HDRColor4 m_ColorHDR;

            bool m_Alpha;
            bool m_HDR;

        public:

            REFLECT_DECLARE_CLASS( ColorComponent, Component::ComponentBase );

            static void EnumerateClass( Reflect::Compositor< ColorComponent >& comp );

        public:
            ColorComponent()
            {
            }

            virtual ~ColorComponent()
            {
            }

        public:
            virtual Component::ComponentUsage GetComponentUsage() const HELIUM_OVERRIDE;

        public:

            void GetColor( Math::Color3& color ) const;
            void GetColor( Math::Color4& color ) const;
            void GetColor( Math::HDRColor3& color ) const;
            void GetColor( Math::HDRColor4& color ) const;

            void SetColor( const Math::HDRColor4& color );

            bool Alpha() const;
            void SetAlpha( bool alpha );

            bool HDR() const;
            void SetHDR( bool hdr );
        };
    }
}