
#pragma once

#include "Bullet/Bullet.h"
#include "Engine/Asset.h"
#include "Engine/AssetType.h"
#include "MathSimd/Vector3.h"

namespace Helium
{
    struct BulletBodyDefinition;

    class HELIUM_BULLET_API BulletWorldDefinition : public Asset
    {
        HELIUM_DECLARE_ASSET(Helium::BulletWorldDefinition, Helium::Asset);
        static void PopulateComposite( Reflect::Composite& comp );

        Helium::Simd::Vector3 m_Gravity;
    };
    typedef Helium::StrongPtr<BulletWorldDefinition> BulletWorldDefinitionPtr;
}
