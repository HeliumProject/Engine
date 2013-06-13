
#include "BulletPch.h"
#include "BulletDebugDraw.h"

#include "LinearMath/btIDebugDraw.h"

#include "Graphics/BufferedDrawer.h"
#include "Graphics/GraphicsManagerComponent.h"

#include "Bullet/BulletWorldComponent.h"


using namespace Helium;

BulletDebugDrawer::BulletDebugDrawer( BufferedDrawer &pBufferedDrawer, int debugMode ) 
	: m_DebugMode( debugMode )
	, m_pDrawer( pBufferedDrawer )
{

}

void BulletDebugDrawer::drawLine( const btVector3& from, const btVector3& to, const btVector3& color )
{
	SimpleVertex v[2];

	v[0].position[0] = from.x();
	v[0].position[1] = from.y();
	v[0].position[2] = from.z();
	v[0].color[0] = static_cast<uint8_t>(color.x() * 255.0);
	v[0].color[1] = static_cast<uint8_t>(color.y() * 255.0);
	v[0].color[2] = static_cast<uint8_t>(color.z() * 255.0);

	v[1].position[0] = to.x();
	v[1].position[1] = to.y();
	v[1].position[2] = to.z();
	v[1].color[0] = v[0].color[0];
	v[1].color[1] = v[0].color[1];
	v[1].color[2] = v[0].color[2];

	m_pDrawer.DrawLineList( v, 2 );
}

void BulletDebugDrawer::drawContactPoint( const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color )
{
	SimpleVertex v;
	v.position[0] = PointOnB.x();
	v.position[1] = PointOnB.y();
	v.position[2] = PointOnB.z();
	v.color[0] = static_cast<uint8_t>(color.x() * 255.0);
	v.color[1] = static_cast<uint8_t>(color.y() * 255.0);
	v.color[2] = static_cast<uint8_t>(color.z() * 255.0);

	m_pDrawer.DrawPoints( &v, 1 );

	btVector3 normal_end = PointOnB + normalOnB * distance;

	drawLine( PointOnB, normalOnB, color );
}

void BulletDebugDrawer::reportErrorWarning( const char* warningString )
{
	HELIUM_TRACE(
		TraceLevels::Warning,
		"Helium bullet integration reportErrorWarning() - %s",
		warningString);
}

void BulletDebugDrawer::draw3dText( const btVector3& location,const char* textString )
{
	Simd::Matrix44 transform(Simd::Matrix44::INIT_TRANSLATION, Simd::Vector3(location.getX(), location.getY(), location.getZ()));
	m_pDrawer.DrawWorldText(transform, String(textString));
}

void BulletDebugDrawer::setDebugMode( int debugMode )
{
	m_DebugMode = debugMode;
}

int BulletDebugDrawer::getDebugMode() const
{
	return m_DebugMode;
}

//////////////////////////////////////////////////////////////////////////

void DoDrawDebugPhysics( World *pWorld )
{
	BulletWorldComponent *pWorldC = pWorld->GetComponents().GetFirst<BulletWorldComponent>();
	GraphicsManagerComponent *pGraphicsC = pWorld->GetComponents().GetFirst<Helium::GraphicsManagerComponent>();

	if ( pWorldC && pGraphicsC )
	{
		btDynamicsWorld *pBulletWorld = pWorldC->GetBulletWorld()->GetBulletWorld();

		BulletDebugDrawer bdd( pGraphicsC->GetBufferedDrawer(), btIDebugDraw::DBG_DrawWireframe );
		pBulletWorld->setDebugDrawer( &bdd );
		pBulletWorld->debugDrawWorld();
		pBulletWorld->setDebugDrawer( NULL );
	}
};

HELIUM_DEFINE_TASK( DrawDebugPhysics, (ForEachWorld< DoDrawDebugPhysics >) )

void DrawDebugPhysics::DefineContract( Helium::TaskContract &rContract )
{
	rContract.ExecuteAfter<Helium::StandardDependencies::ProcessPhysics>();
	rContract.ExecuteBefore<Helium::StandardDependencies::Render>();
}
