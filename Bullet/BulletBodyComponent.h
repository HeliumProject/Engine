
#pragma once

#include "Bullet/Bullet.h"
#include "Bullet/BulletBodyDefinition.h"
#include "Framework/ComponentDefinition.h"
#include "Framework/TaskScheduler.h"
#include "Bullet/BulletBody.h"

namespace Helium
{
	class BulletBodyComponentDefinition;

	class HELIUM_BULLET_API BulletBodyComponent : public Component
	{
		HELIUM_DECLARE_COMPONENT( Helium::BulletBodyComponent, Helium::Component );
		static void PopulateStructure( Reflect::Structure& comp );

		~BulletBodyComponent();

		void Initialize( const BulletBodyComponentDefinition &definition);
		void Finalize( const BulletBodyComponentDefinition &definition );

		void Impulse();

		BulletBody m_Body;
	};

	class HELIUM_BULLET_API BulletBodyComponentDefinition : public Helium::ComponentDefinitionHelperFinalizeOnly<BulletBodyComponent, BulletBodyComponentDefinition>
	{
		HELIUM_DECLARE_ASSET( Helium::BulletBodyComponentDefinition, Helium::ComponentDefinition );
		static void PopulateStructure( Reflect::Structure& comp );

		BulletBodyDefinitionPtr m_BodyDefinition;
	};
	typedef StrongPtr<BulletBodyComponentDefinition> BulletBodyComponentDefinitionPtr;

	struct HELIUM_BULLET_API PreProcessPhysics : public Helium::TaskDefinition
	{
		HELIUM_DECLARE_TASK(PreProcessPhysics)

		virtual void DefineContract(Helium::TaskContract &rContract);
	};

	struct HELIUM_BULLET_API PostProcessPhysics : public Helium::TaskDefinition
	{
		HELIUM_DECLARE_TASK(PostProcessPhysics)

		virtual void DefineContract(Helium::TaskContract &rContract);
	};
}
