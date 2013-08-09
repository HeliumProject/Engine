
#pragma once

#include "Bullet/Bullet.h"
#include "Bullet/BulletWorld.h"
#include "Bullet/BulletWorldDefinition.h"
#include "Framework/ComponentDefinition.h"
#include "Framework/TaskScheduler.h"

namespace Helium
{
	class BulletWorldComponentDefinition;

	class HELIUM_BULLET_API BulletWorldComponent : public Component
	{
	public:
		BulletWorldComponent();
		~BulletWorldComponent();

		HELIUM_DECLARE_COMPONENT( Helium::BulletWorldComponent, Helium::Component );
		static void PopulateMetaType( Reflect::MetaStruct& comp );

		void Initialize( const BulletWorldComponentDefinition &definition);

		void Simulate(float dt);

		BulletWorld *GetBulletWorld() { return m_World; }

	private:
		
		// I would love to use an auto_ptr here but microsoft's compiler breaks when I try to do that. 
		// http://www.youtube.com/watch?v=1ytCEuuW2_A
		BulletWorld *m_World;
	};

	class HELIUM_BULLET_API BulletWorldComponentDefinition : public Helium::ComponentDefinitionHelper<BulletWorldComponent, BulletWorldComponentDefinition>
	{
		 HELIUM_DECLARE_ASSET( Helium::BulletWorldComponentDefinition, Helium::ComponentDefinition );
		 static void PopulateMetaType( Reflect::MetaStruct& comp );
 
		 BulletWorldDefinitionPtr m_WorldDefinition;
	};
	typedef StrongPtr<BulletWorldComponentDefinition> BulletWorldComponentDefinitionPtr;

	struct HELIUM_BULLET_API ProcessPhysics : public Helium::TaskDefinition
	{
		HELIUM_DECLARE_TASK(ProcessPhysics)

		virtual void DefineContract(Helium::TaskContract &rContract);
	};
}
