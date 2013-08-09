#include "ExampleGamePch.h"

#include "ExampleGame/Components/Graphics/CameraManager.h"
#include "Reflect/TranslatorDeduction.h"
#include "Graphics/GraphicsManagerComponent.h"
#include "GraphicsTypes/GraphicsSceneView.h"
#include "Components/TransformComponent.h"
#include "Framework/WorldManager.h"

#if HELIUM_DEBUG_CAMERA_ENABLED
#include "Ois/OisSystem.h"
#endif

using namespace Helium;
using namespace ExampleGame;

//////////////////////////////////////////////////////////////////////////
// CameraManagerComponent

HELIUM_DEFINE_COMPONENT(ExampleGame::CameraManagerComponent, EXAMPLE_GAME_MAX_WORLDS);

void CameraManagerComponent::PopulateMetaType( Reflect::MetaStruct& comp )
{

}

ExampleGame::CameraManagerComponent::CameraManagerComponent()
#if HELIUM_DEBUG_CAMERA_ENABLED
	: m_DebugCameraEnabled( false )
#endif
{

}

void CameraManagerComponent::Initialize( const CameraManagerComponentDefinition &definition )
{
	m_CurrentCameraName = definition.m_DefaultCameraName;
	m_CameraChanged = true;

	m_GraphicsManager = GetWorld()->GetComponents().GetFirst<GraphicsManagerComponent>();
	HELIUM_ASSERT( m_GraphicsManager.IsGood() );
}

bool CameraManagerComponent::RegisterNamedCamera( Helium::Name cameraName, CameraComponent *pCameraC )
{
	Helium::Map<Helium::Name, CameraComponent *>::Iterator iter = m_Cameras.Find( cameraName );
	if ( iter == m_Cameras.End() )
	{
		m_Cameras.Insert( iter, Helium::Map<Helium::Name, CameraComponent *>::ValueType(cameraName, pCameraC) );

		if ( cameraName == m_CurrentCameraName )
		{
			m_CameraChanged = true;
			m_CurrentCamera = pCameraC;
		}
	}
	else
	{
		HELIUM_TRACE(
			TraceLevels::Warning,
			"CameraManagerComponent::RegisterCamera - A camera named %s is already registered",
			*cameraName);

		return false;
	}

	return true;
}

bool ExampleGame::CameraManagerComponent::UnregisterNamedCamera( Helium::Name cameraName, CameraComponent *pCameraC )
{
	Helium::Map<Helium::Name, CameraComponent *>::Iterator iter = m_Cameras.Find( cameraName );
	if ( iter != m_Cameras.End() )
	{
		if ( iter->Second() == pCameraC )
		{
			if (m_CurrentCameraName == cameraName)
			{
				m_CurrentCamera = NULL;
				m_CameraChanged = true;
			}

			m_Cameras.Remove( iter );
		}
		else
		{
			HELIUM_TRACE(
				TraceLevels::Warning,
				"CameraManagerComponent::UnregisterNamedCamera - Attempted to unregister camera named %s, but the registered camera doesn't match the one we are unregistering",
				*cameraName);

			return false;
		}
	}
	else
	{
		HELIUM_TRACE(
			TraceLevels::Warning,
			"CameraManagerComponent::RegisterCamera - No camera is registered as %s",
			*cameraName);

		return false;
	}

	return true;
}

void ExampleGame::CameraManagerComponent::SetCurrentCamera( CameraComponent *pCameraC )
{
	m_CurrentCameraName.Clear();
	m_CurrentCamera.Reset( pCameraC );
}

void CameraManagerComponent::SetCurrentCameraByName( Helium::Name cameraName )
{
	if ( cameraName != m_CurrentCameraName )
	{
		m_CurrentCameraName = cameraName;
		m_CurrentCamera = NULL;
		m_CameraChanged = true;
	}
}

void CameraManagerComponent::Tick()
{
	// If don't have camera set up, something interesting happened to our config this frame, and there's a camera we should look for by name
	if ( !m_CurrentCamera.IsGood() && m_CameraChanged && !m_CurrentCameraName.IsEmpty() )
	{
		// Try to find it
		Helium::Map<Helium::Name, CameraComponent *>::Iterator iter = m_Cameras.Find( m_CurrentCameraName );
		if ( iter != m_Cameras.End() )
		{
			m_CurrentCamera = iter->Second();
		}
		else
		{
			HELIUM_TRACE(
				TraceLevels::Warning,
				"CameraManagerComponent::Tick - No camera named %s currently registered",
				*m_CurrentCameraName);
		}
	}

#if HELIUM_DEBUG_CAMERA_ENABLED
	if (Input::WasKeyPressedThisFrame( Input::KeyCodes::KC_C ))
	{
		SetDebugCameraEnabled( !m_DebugCameraEnabled );
	}

	if (m_DebugCameraEnabled)
	{
		UpdateDebugCamera();
	}
	else
#endif
	if ( m_CurrentCamera.IsGood() )
	{
		HELIUM_ASSERT( m_GraphicsManager.IsGood() );
		Helium::GraphicsSceneView *pView = m_GraphicsManager->GetGraphicsScene()->GetSceneView( 0 );

		Helium::TransformComponent *pTransform = m_CurrentCamera->GetComponentCollection()->GetFirst<TransformComponent>();

		pView->SetView(pTransform->GetPosition(), /*pTransform->GetRotation(). **/ Simd::Vector3::BasisZ, m_CurrentCamera->GetUp() );
		pView->SetNearClip( m_CurrentCamera->GetNearClip() );
		pView->SetFarClip( m_CurrentCamera->GetFarClip() );
		pView->SetHorizontalFov( m_CurrentCamera->GetFov() );
	}
	else if ( m_CameraChanged )
	{
		HELIUM_TRACE(
			TraceLevels::Warning,
			"CameraManagerComponent::Tick - No camera is set",
			*m_CurrentCameraName);
	}

	m_CameraChanged = false;
}

#if HELIUM_DEBUG_CAMERA_ENABLED
void ExampleGame::CameraManagerComponent::SetDebugCameraEnabled( bool enabled )
{
	m_DebugCameraEnabled = enabled;

	if (m_DebugCameraEnabled)
	{
		if ( m_CurrentCamera.IsGood() )
		{
			Helium::TransformComponent *pTransform = m_CurrentCamera->GetComponentCollection()->GetFirst<TransformComponent>();
			HELIUM_ASSERT( pTransform );

			m_DebugCameraPosition = pTransform->GetPosition();
			m_DebugCameraPitch = 0.0f;
			m_DebugCameraYaw = 0.0f;
		}
		else
		{
			m_DebugCameraPosition = Simd::Vector3::Zero;
			m_DebugCameraPitch = 0.0f;
			m_DebugCameraYaw = 0.0f;
		}
	}
}

void ExampleGame::CameraManagerComponent::UpdateDebugCamera()
{
	static const float CAMERA_LINEAR_VELOCITY = 300.0f;
	static const float CAMERA_ANGULAR_VELOCITY = 0.16f;
	float forwardMotion = 0.0f;
	float sideMotion = 0.0f;
	float dt = Helium::WorldManager::GetStaticInstance().GetFrameDeltaSeconds();

	float speed = Input::IsModifierDown( Input::KeyboardModifiers::Shift ) ? 3.0f * CAMERA_LINEAR_VELOCITY : CAMERA_LINEAR_VELOCITY;

	if ( Input::IsKeyDown( Input::KeyCodes::KC_W ) )
	{
		forwardMotion += speed;
	}

	if ( Input::IsKeyDown( Input::KeyCodes::KC_S ) )
	{
		forwardMotion -= speed;
	}

	if ( Input::IsKeyDown( Input::KeyCodes::KC_A ) )
	{
		sideMotion += speed;
	}

	if ( Input::IsKeyDown( Input::KeyCodes::KC_D ) )
	{
		sideMotion -= speed;
	}

	Simd::Vector2 mouseDelta = Input::GetMousePosDelta();

	m_DebugCameraYaw += mouseDelta.GetX() * dt * CAMERA_ANGULAR_VELOCITY;
	m_DebugCameraPitch += mouseDelta.GetY() * dt * CAMERA_ANGULAR_VELOCITY;

	Helium::Clamp(m_DebugCameraPitch, -HELIUM_PI_2 *0.98f, HELIUM_PI_2 *0.98f);

	Simd::Quat q1(
		m_DebugCameraPitch, 
		0.0f, 
		0.0f);

	Simd::Quat q2(
		0.0f, 
		m_DebugCameraYaw, 
		0.0f);

	Simd::Matrix44 matrix( Simd::Matrix44::INIT_ROTATION, q1 * q2 );
	Simd::Vector3 forward = matrix.TransformVector( Simd::Vector3::BasisZ );
	Simd::Vector3 side = forward.Cross( Simd::Vector3::BasisY );

	m_DebugCameraPosition += forward * Simd::Vector3( forwardMotion * dt );
	m_DebugCameraPosition += side * Simd::Vector3( sideMotion * dt );

	HELIUM_ASSERT( m_GraphicsManager.IsGood() );
	Helium::GraphicsSceneView *pView = m_GraphicsManager->GetGraphicsScene()->GetSceneView( 0 );
	pView->SetView(m_DebugCameraPosition, forward, Simd::Vector3::BasisY);
	pView->SetNearClip( 1.0f );
	pView->SetFarClip( 20000.0f );
	pView->SetHorizontalFov( 60 );
}
#endif // #if HELIUM_DEBUG_CAMERA_ENABLED

HELIUM_IMPLEMENT_ASSET(ExampleGame::CameraManagerComponentDefinition, Components, 0);

ExampleGame::CameraManagerComponentDefinition::CameraManagerComponentDefinition()
	: m_DefaultCameraName( Helium::NULL_NAME )
{

}

void CameraManagerComponentDefinition::PopulateMetaType( Reflect::MetaStruct& comp )
{
	comp.AddField( &CameraManagerComponentDefinition::m_DefaultCameraName, "m_DefaultCameraName" );
}

//////////////////////////////////////////////////////////////////////////
// CameraManagerTick

void TickCameraManager(CameraManagerComponent *c)
{
	c->Tick();
}

void TickCameras(World *pWorld)
{
	QueryComponents< CameraManagerComponent, TickCameraManager >( pWorld );
}

HELIUM_DEFINE_TASK(CameraManagerTick, ( ForEachWorld< TickCameras > ) )
	
void CameraManagerTick::DefineContract( TaskContract &rContract )
{
	rContract.Fulfills<Helium::StandardDependencies::Render>();
	rContract.ExecuteBefore<Helium::GraphicsManagerDrawTask>();
}
