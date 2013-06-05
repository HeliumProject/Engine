
#pragma once

#include "Bullet/Bullet.h"
#include "Bullet/BulletBodyDefinition.h"
#include "Framework/ComponentDefinition.h"
#include "Framework/TaskScheduler.h"

namespace Helium
{
    class BulletBodyComponentDefinition;

	class HELIUM_BULLET_API BulletBodyComponent : public Component
	{
		HELIUM_DECLARE_COMPONENT( Helium::BulletBodyComponent, Helium::Component );
        static void PopulateStructure( Reflect::Structure& comp );

        void Finalize( const BulletBodyComponentDefinition *pDefinition) { }
	};

	class HELIUM_BULLET_API BulletBodyComponentDefinition : public Helium::ComponentDefinitionHelper<BulletBodyComponent, BulletBodyComponentDefinition>
	{
		HELIUM_DECLARE_ASSET( Helium::BulletBodyComponentDefinition, Helium::ComponentDefinition );
        static void PopulateStructure( Reflect::Structure& comp );

        BulletBodyDefinitionPtr m_BodyDefinition;
	};
    typedef StrongPtr<BulletBodyComponentDefinition> BulletBodyComponentDefinitionPtr;
}
