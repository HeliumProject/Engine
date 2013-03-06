
#pragma once

#include "Bullet/Bullet.h"
#include "Bullet/BulletWorld.h"
#include "Bullet/BulletWorldDefinition.h"
#include "Framework/ComponentDefinition.h"
#include "Engine/TaskScheduler.h"

namespace Helium
{
    class BulletWorldComponentDefinition;

	class HELIUM_BULLET_API BulletWorldComponent : public Component
	{
    public:
        BulletWorldComponent();
        ~BulletWorldComponent();

		HELIUM_DECLARE_COMPONENT( Helium::BulletWorldComponent, Helium::Component );
        static void PopulateComposite( Reflect::Composite& comp );

        void Finalize( const BulletWorldComponentDefinition *pDefinition);

        void Simulate(float dt);

    private:
        
        // I would love to use an auto_ptr here but microsoft's compiler breaks when I try to do that. 
        // http://www.youtube.com/watch?v=1ytCEuuW2_A
        BulletWorld *m_World;
	};

 	class HELIUM_BULLET_API BulletWorldComponentDefinition : public Helium::ComponentDefinitionHelper<BulletWorldComponent, BulletWorldComponentDefinition>
 	{
 		 HELIUM_DECLARE_ASSET( Helium::BulletWorldComponentDefinition, Helium::ComponentDefinition );
         static void PopulateComposite( Reflect::Composite& comp );
 
         BulletWorldDefinitionPtr m_WorldDefinition;
 	};
    typedef StrongPtr<BulletWorldComponentDefinition> BulletWorldComponentDefinitionPtr;
}
