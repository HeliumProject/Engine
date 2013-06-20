
#pragma once

#include "Bullet/Bullet.h"
#include "Bullet/BulletBodyDefinition.h"
#include "Framework/ComponentDefinition.h"
#include "Framework/TaskScheduler.h"
#include "Framework/EntityComponent.h"
#include "Bullet/BulletBody.h"
#include "Bullet/HasPhysicalContacts.h"

namespace Helium
{
	struct BulletBodyComponentDefinition;
	
	class HELIUM_BULLET_API BulletBodyComponent : public EntityComponent
	{
	public:
		HELIUM_DECLARE_COMPONENT( Helium::BulletBodyComponent, Helium::Component );
		static void PopulateStructure( Reflect::Structure& comp );

		~BulletBodyComponent();

		void Initialize( const BulletBodyComponentDefinition &definition);
		void Finalize( const BulletBodyComponentDefinition &definition );
		
		bool ShouldTrackCollisions() { return m_TrackCollisions; }

		void Impulse();
		
		// Physical contact tracking
		inline HasPhysicalContactsComponent *GetOrCreateHasPhysicalContactsComponent();
		inline bool                          GetShouldTrackPhysicalContact( BulletBodyComponent *pOther );

		BulletBody &GetBody() { return m_Body; }

	private:
		BulletBody m_Body;
		uint16_t m_TrackPhysicalContactsGroup;
		uint16_t m_TrackPhysicalContactsMask;

		ComponentPtr< HasPhysicalContactsComponent > m_HasPhysicalContactsComponent;
		bool m_TrackCollisions; 
	};

	struct HELIUM_BULLET_API BulletBodyComponentDefinition : public Helium::ComponentDefinitionHelperFinalizeOnly<BulletBodyComponent, BulletBodyComponentDefinition>
	{
		HELIUM_DECLARE_ASSET( Helium::BulletBodyComponentDefinition, Helium::ComponentDefinition );
		static void PopulateStructure( Reflect::Structure& comp );

		BulletBodyComponentDefinition();

		BulletBodyDefinitionPtr m_BodyDefinition;
		uint16_t m_TrackPhysicalContactsGroup;
		uint16_t m_TrackPhysicalContactsMask;
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

#include "BulletBodyComponent.inl"
