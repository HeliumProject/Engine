#pragma once

#include "Core/API.h"
#include "ParametricKey.h"
#include "Foundation/Reflect/SimpleSerializer.h"

namespace Helium
{
    namespace Content
    {
        class CORE_API ParametricIntensityKey : public ParametricKey
        {
        public:
            u8 m_Intensity;

        public:
            ParametricIntensityKey();
            virtual ~ParametricIntensityKey();

            virtual Math::Color3 GetColor() const HELIUM_OVERRIDE;
            virtual void SetColor( const Math::Color3& color ) HELIUM_OVERRIDE;

            REFLECT_DECLARE_CLASS( ParametricIntensityKey, ParametricKey );

            static void EnumerateClass( Reflect::Compositor<ParametricIntensityKey>& comp );
        };
        typedef Helium::SmartPtr< ParametricIntensityKey > ParametricIntensityKeyPtr;
        typedef std::vector< ParametricIntensityKeyPtr > V_ParametricIntensityKeyPtr;
    }
}