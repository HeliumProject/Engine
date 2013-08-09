
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
		static void PopulateMetaType( Reflect::MetaStruct& comp );

		~BulletBodyComponent();

		void Initialize( const BulletBodyComponentDefinition &definition);
		void Finalize( const BulletBodyComponentDefinition &definition );
		
		bool ShouldTrackCollisions() { return m_TrackCollisions; }

		void WakeUp();
		void ApplyForce( const Simd::Vector3 &force );
		void SetVelocity( const Simd::Vector3 &velocity );
		void SetAngularVelocity( const Simd::Vector3 &velocity );
		
		// Physical contact tracking
		inline HasPhysicalContactsComponent *GetOrCreateHasPhysicalContactsComponent();
		inline bool                          GetShouldTrackPhysicalContact( BulletBodyComponent *pOther );

		BulletBody &GetBody() { return m_Body; }

		enum
		{
			MAX_BULLET_BODY_FLAGS = 16
		};

	private:
		BulletBody m_Body;
		uint16_t m_AssignedGroups;
		uint16_t m_TrackPhysicalContactGroupMask;

		ComponentPtr< HasPhysicalContactsComponent > m_HasPhysicalContactsComponent;
		bool m_TrackCollisions; 
	};

	struct HELIUM_BULLET_API BulletBodyComponentDefinition : public Helium::ComponentDefinitionHelperFinalizeOnly<BulletBodyComponent, BulletBodyComponentDefinition>
	{
		HELIUM_DECLARE_ASSET( Helium::BulletBodyComponentDefinition, Helium::ComponentDefinition );
		static void PopulateMetaType( Reflect::MetaStruct& comp );

		BulletBodyComponentDefinition();

		BulletBodyDefinitionPtr m_BodyDefinition;
		Simd::Vector3 m_InitialVelocity;

		int m_AssignedGroups;
		int m_TrackPhysicalContactGroupMask;

		DynamicArray< Name > m_AssignedGroupFlags;
		DynamicArray< Name > m_TrackPhysicalContactGroupFlags;

		virtual void FinalizeLoad();
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
