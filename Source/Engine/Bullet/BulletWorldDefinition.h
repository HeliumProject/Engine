
#pragma once

#include "Bullet/Bullet.h"
#include "Engine/Asset.h"
#include "MathSimd/Vector3.h"

namespace Helium
{
    struct BulletBodyDefinition;

	class HELIUM_BULLET_API BulletWorldDefinition : public Reflect::Struct
    {
        HELIUM_DECLARE_BASE_STRUCT(Helium::BulletWorldDefinition);
        static void PopulateMetaType( Reflect::MetaStruct& comp );

        Helium::Simd::Vector3 m_Gravity;
    };
}
