#include "BulletPch.h"
#include "Bullet/BulletUtilities.h"
#include "Bullet/BulletBody.h"
#include "Bullet/BulletBodyDefinition.h"
#include "Bullet/BulletShapes.h"
#include "Bullet/BulletWorld.h"

using namespace Helium;

namespace Helium
{
	struct BulletMotionState : public btMotionState
	{
		BulletMotionState(const btTransform &worldTrans)
			: m_Transform(worldTrans)
		{

		}

		virtual void getWorldTransform( btTransform& worldTrans ) const
		{
			worldTrans = m_Transform;
		}

		virtual void setWorldTransform( const btTransform& worldTrans ) 
		{
			m_Transform = worldTrans;
		}

		btTransform m_Transform;
	};
}

Helium::BulletBody::BulletBody()
	: m_Body(0),
	  m_MotionState(0)
{

}

Helium::BulletBody::~BulletBody()
{
	// If this trips, you probably didn't call Destruct. Destruct needs to be called explicitly so that
	// a world can be passed in. We don't want to duplicate references to world in BulletBody because
	// generall we can infer the correct bullet world via the world that hosts this object. If this is
	// an onorous requirement, then wrap this object yourself and set up your destructor to call Destruct
	// passing in a cached reference to the world.
	HELIUM_ASSERT(!m_Body);
}

void BulletBody::Initialize( BulletWorld &rWorld, const BulletBodyDefinition &rBodyDefinition, const Helium::Simd::Vector3 &rInitialPosition, const Helium::Simd::Quat &rInitialRotation )
{
	HELIUM_ASSERT(rWorld.GetBulletWorld());

	if (rBodyDefinition.m_Shapes.IsEmpty())
	{
		HELIUM_TRACE( TraceLevels::Warning, "BulletBody::Initialize - Tried to create a bullet body with definition %s, but definition has 0 shapes.", 
			*rBodyDefinition.GetPath().ToString());

		return;
	}

	btCollisionShape *pFinalShape = 0;
	btVector3 finalInertia;
	float finalMass;

	if (rBodyDefinition.m_Shapes.GetSize() > 1 || rBodyDefinition.m_Shapes[0]->m_Position.GetMagnitudeSquared() > HELIUM_EPSILON)
	{
		btCompoundShape *pCompoundShape = new btCompoundShape( true );

		float mass = 0.0f;
		for (size_t i = 0; i < rBodyDefinition.m_Shapes.GetSize(); ++i)
		{
			btVector3 position;
			btQuaternion rotation;

			ConvertToBullet( rBodyDefinition.m_Shapes[i]->m_Position, position );
			ConvertToBullet( rBodyDefinition.m_Shapes[i]->m_Rotation, rotation );

			btCollisionShape *pBulletShape = rBodyDefinition.m_Shapes[i]->CreateShape();
			pCompoundShape->addChildShape(
				btTransform(rotation, position), 
				pBulletShape);

			mass += rBodyDefinition.m_Shapes[i]->m_Mass;
		}

		if (mass != 0.0f)
		{
			pCompoundShape->calculateLocalInertia(mass, finalInertia);
		}
		pFinalShape = pCompoundShape;
		finalMass = mass;
	}
	else
	{
		BulletShape *pShape = rBodyDefinition.m_Shapes[0];
		btCollisionShape *pBulletShape = pShape->CreateShape();
		
		if (pShape->m_Mass != 0.0f)
		{
			pBulletShape->calculateLocalInertia(pShape->m_Mass, finalInertia);
		}

		pFinalShape = pBulletShape;
		finalMass = pShape->m_Mass;
	}

	btVector3 origin;
	ConvertToBullet(rInitialPosition, origin);

	btQuaternion rotation;
	ConvertToBullet(rInitialRotation, rotation);
	
	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setRotation(rotation);
	startTransform.setOrigin(origin);

	if (rBodyDefinition.m_IsKinematic)
	{
		finalMass = 0.0f;
	}
	
	m_MotionState = new BulletMotionState(startTransform);
	m_Body = new btRigidBody(finalMass, m_MotionState, pFinalShape, finalInertia);
	m_Body->setRestitution(rBodyDefinition.m_Restitution);
	
	m_Body->setLinearFactor(
		btVector3(
		rBodyDefinition.m_LockPositionX ? 0.0f : 1.0f, 
		rBodyDefinition.m_LockPositionY ? 0.0f : 1.0f, 
		rBodyDefinition.m_LockPositionZ ? 0.0f : 1.0f));

	m_Body->setAngularFactor(
		btVector3(
			rBodyDefinition.m_LockRotationX ? 0.0f : 1.0f, 
			rBodyDefinition.m_LockRotationY ? 0.0f : 1.0f, 
			rBodyDefinition.m_LockRotationZ ? 0.0f : 1.0f));

	m_Body->setDamping( rBodyDefinition.m_LinearDamping, rBodyDefinition.m_AngularDamping );

	if (rBodyDefinition.m_IsKinematic)
	{
		m_Body->setCollisionFlags( m_Body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT );
	}

	if (rBodyDefinition.m_DisableCollisionResponse)
	{
		m_Body->setCollisionFlags( m_Body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE );
	}
	
	rWorld.GetBulletWorld()->addRigidBody(m_Body);
}

void Helium::BulletBody::GetPosition( Helium::Simd::Vector3 &rPosition )
{
#ifdef BT_USE_SSE
	rPosition.SetSimdVector(m_MotionState->m_Transform.getOrigin().get128());
#else
	rPosition.m_x = m_MotionState->m_Transform.getOrigin().getX();
	rPosition.m_y = m_MotionState->m_Transform.getOrigin().getY();
	rPosition.m_z = m_MotionState->m_Transform.getOrigin().getZ();
#endif

	ConvertFromBullet( m_MotionState->m_Transform.getOrigin(), rPosition );
}

void Helium::BulletBody::GetRotation( Helium::Simd::Quat &rRotation )
{
	ConvertFromBullet( m_MotionState->m_Transform.getRotation(), rRotation );
}

void Helium::BulletBody::SetPosition( const Helium::Simd::Vector3 &rPosition )
{
	HELIUM_ASSERT(m_MotionState);

	btVector3 position;
	ConvertToBullet(rPosition, position);
	m_MotionState->m_Transform.setOrigin(position);
	m_Body->activate();
}

void Helium::BulletBody::SetRotation( const Helium::Simd::Quat &rRotation )
{
	HELIUM_ASSERT(m_MotionState);
	
	btQuaternion q;
	ConvertToBullet( rRotation, q );
	m_MotionState->m_Transform.getBasis().setRotation(q);
	m_Body->activate();
}

void Helium::BulletBody::Destruct( BulletWorld &rWorld )
{
	delete m_MotionState;

	rWorld.GetBulletWorld()->removeCollisionObject(m_Body);
	delete m_Body;

	for (DynamicArray<btCollisionShape *>::Iterator shape = m_Shapes.Begin();
		shape != m_Shapes.End(); ++shape)
	{
		delete *shape;
	}

#if HELIUM_ASSERT_ENABLED
	// Clear m_Body so that the assert will succeed
	m_Body = NULL;
#endif
}
