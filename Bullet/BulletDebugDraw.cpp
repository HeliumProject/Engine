
#include "Precompile.h"
#include "BulletDebugDraw.h"

#include "LinearMath/btIDebugDraw.h"

#include "Rendering/Renderer.h"

#include "Graphics/BufferedDrawer.h"
#include "Graphics/GraphicsManagerComponent.h"
#include "Rendering/RVertexBuffer.h"

#include "Bullet/BulletWorldComponent.h"

#include "Framework/World.h"

#define POINTS_IN_SPHERE (288)

using namespace Helium;

BulletDebugDrawer::BulletDebugDrawer( BufferedDrawer &pBufferedDrawer, int debugMode ) 
	: m_DebugMode( debugMode )
	, m_pDrawer( pBufferedDrawer )
{
	m_Sphere.Reserve(POINTS_IN_SPHERE);

	int32_t dphi = 180 / 8;
	int32_t dtheta = 360 / 8;

	for (int32_t phi=-90; phi<=90; phi+=dphi)
	{
		for (int32_t theta=0; theta<=360-dtheta; theta+=dtheta)
		{
			float32_t sinTheta = Sin( theta * static_cast< float32_t >( HELIUM_DEG_TO_RAD ) );
			float32_t sinTheta2 = Sin( ( theta + dtheta ) * static_cast< float32_t >( HELIUM_DEG_TO_RAD ) );
			float32_t cosTheta = Cos( theta * static_cast< float32_t >( HELIUM_DEG_TO_RAD ) );
			float32_t cosTheta2 = Cos( ( theta + dtheta ) * static_cast< float32_t >( HELIUM_DEG_TO_RAD ) );

			float32_t sinPhi = Sin( phi * static_cast< float32_t >( HELIUM_DEG_TO_RAD ) );
			float32_t cosPhi = Cos( phi * static_cast< float32_t >( HELIUM_DEG_TO_RAD ) );

			{
				SimpleVertex *v = m_Sphere.New();
				v->position[0] = sinTheta * cosPhi;
				v->position[1] = sinPhi;
				v->position[2] = cosTheta * cosPhi;
				v->color[0] = 0xFF;
				v->color[1] = 0xFF;
				v->color[2] = 0xFF;
				v->color[3] = 0xFF;
			}

			{
				SimpleVertex *v = m_Sphere.New();
				v->position[0] = sinTheta2 * cosPhi;
				v->position[1] = sinPhi;
				v->position[2] = cosTheta2 * cosPhi;
				v->color[0] = 0xFF;
				v->color[1] = 0xFF;
				v->color[2] = 0xFF;
				v->color[3] = 0xFF;
			}
		}
	}

	for (int32_t phi=-90; phi<=90; phi+=dphi)
	{
		for (int32_t theta=0; theta<=360-dtheta; theta+=dtheta)
		{
			float32_t sinTheta = Sin( theta * static_cast< float32_t >( HELIUM_DEG_TO_RAD ) );
			float32_t cosTheta = Cos( theta * static_cast< float32_t >( HELIUM_DEG_TO_RAD ) );

			float32_t sinPhi = Sin( phi * static_cast< float32_t >( HELIUM_DEG_TO_RAD ) );
			float32_t sinPhi2 = Sin( ( phi + dphi ) * static_cast< float32_t >( HELIUM_DEG_TO_RAD ) );
			float32_t cosPhi = Cos( phi * static_cast< float32_t >( HELIUM_DEG_TO_RAD ) );
			float32_t cosPhi2 = Cos( ( phi + dphi ) * static_cast< float32_t >( HELIUM_DEG_TO_RAD ) );

			{
				SimpleVertex *v = m_Sphere.New();
				v->position[0] = sinTheta * cosPhi;
				v->position[1] = sinPhi;
				v->position[2] = cosTheta * cosPhi;
				v->color[0] = 0xFF;
				v->color[1] = 0xFF;
				v->color[2] = 0xFF;
				v->color[3] = 0xFF;
			}

			{
				SimpleVertex *v = m_Sphere.New();
				v->position[0] = sinTheta * cosPhi2;
				v->position[1] = sinPhi2;
				v->position[2] = cosTheta * cosPhi2;
				v->color[0] = 0xFF;
				v->color[1] = 0xFF;
				v->color[2] = 0xFF;
				v->color[3] = 0xFF;
			}
		}
	}

	HELIUM_ASSERT( m_Sphere.GetSize() == POINTS_IN_SPHERE );

	m_pSphereVertexBuffer = Renderer::GetInstance()->CreateVertexBuffer(sizeof(SimpleVertex) * POINTS_IN_SPHERE, RENDERER_BUFFER_USAGE_STATIC, m_Sphere.GetData());

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

void BulletDebugDrawer::drawSphere(btScalar radius, const btTransform& transform, const btVector3& color)
{
	uint8_t vColor[ 4 ];

	vColor[0] = static_cast<uint8_t>(color.x() * 255.0);
	vColor[1] = static_cast<uint8_t>(color.y() * 255.0);
	vColor[2] = static_cast<uint8_t>(color.z() * 255.0);
	vColor[3] = 0xFF;

	Simd::Matrix44 scaling(Simd::Matrix44::INIT_SCALING, radius);
	Simd::Matrix44 rotateTranslate;

	ConvertFromBullet( transform, rotateTranslate );
	
	m_pDrawer.DrawLineList(
		scaling * rotateTranslate,
		m_pSphereVertexBuffer.Get(), 
		0,
		POINTS_IN_SPHERE,
		Color(color.x(), color.y(), color.z(), 1.0f) );
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

	drawLine( PointOnB, normal_end, color );
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

#if GRAPHICS_SCENE_BUFFERED_DRAWER
		BulletDebugDrawer bdd( pGraphicsC->GetBufferedDrawer(),  btIDebugDraw::DBG_DrawWireframe /* | btIDebugDraw::DBG_DrawContactPoints */ );
		pBulletWorld->setDebugDrawer( &bdd );
		pBulletWorld->debugDrawWorld();
		pBulletWorld->setDebugDrawer( NULL );
#endif
	}
};

HELIUM_DEFINE_TASK( DrawDebugPhysics, (ForEachWorld< DoDrawDebugPhysics >), TickTypes::Render )

void DrawDebugPhysics::DefineContract( Helium::TaskContract &rContract )
{
	rContract.ExecuteAfter<Helium::StandardDependencies::ProcessPhysics>();
	rContract.ExecuteBefore<Helium::StandardDependencies::Render>();
}
