#include "ExampleGamePch.h"

#include "ExampleGame/Components/Graphics/CameraManager.h"
#include "Reflect/TranslatorDeduction.h"
#include "Graphics/GraphicsManagerComponent.h"
#include "GraphicsTypes/GraphicsSceneView.h"
#include "Components/TransformComponent.h"

using namespace Helium;
using namespace ExampleGame;

//////////////////////////////////////////////////////////////////////////
// CameraManagerComponent

HELIUM_DEFINE_COMPONENT(ExampleGame::CameraManagerComponent, EXAMPLE_GAME_MAX_WORLDS);

void CameraManagerComponent::PopulateStructure( Reflect::Structure& comp )
{

}

void CameraManagerComponent::Finalize( const CameraManagerComponentDefinition *pDefinition )
{
	m_CurrentCameraName = pDefinition->m_DefaultCameraName;
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

HELIUM_IMPLEMENT_ASSET(ExampleGame::CameraManagerComponentDefinition, Components, 0);

ExampleGame::CameraManagerComponentDefinition::CameraManagerComponentDefinition()
	: m_DefaultCameraName( Helium::NULL_NAME )
{

}

void CameraManagerComponentDefinition::PopulateStructure( Reflect::Structure& comp )
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
