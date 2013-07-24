#include "ExampleGamePch.h"

#include "ExampleGame/Components/Graphics/Camera.h"
#include "ExampleGame/Components/Graphics/CameraManager.h"
#include "Reflect/TranslatorDeduction.h"
#include "Framework/ComponentQuery.h"
#include "Graphics/GraphicsManagerComponent.h"

using namespace Helium;
using namespace ExampleGame;

//////////////////////////////////////////////////////////////////////////
// PlayerComponent

HELIUM_DEFINE_COMPONENT(ExampleGame::CameraComponent, 4);

void CameraComponent::PopulateMetaType( Reflect::MetaStruct& comp )
{

}

ExampleGame::CameraComponent::CameraComponent()
{

}

ExampleGame::CameraComponent::~CameraComponent()
{
	if ( m_Registered )
	{
		HELIUM_ASSERT( !m_Name.IsEmpty() );

		Helium::World *pWorld = GetWorld();
		CameraManagerComponent *pCameraManager = pWorld->GetComponents().GetFirst<CameraManagerComponent>();

		if ( pCameraManager )
		{
			HELIUM_ASSERT( pCameraManager->UnregisterNamedCamera( m_Name, this ) );
		}
	}
}

void CameraComponent::Initialize( const CameraComponentDefinition &definition )
{
	m_Registered = false;
	m_Up = definition.m_Up;
	m_Name = definition.m_Name;
	m_NearClip = definition.m_NearClip;
	m_FarClip = definition.m_FarClip;
	m_Fov = definition.m_Fov;

	if ( !m_Name.IsEmpty() )
	{
		Helium::World *pWorld = GetWorld();
		CameraManagerComponent *pCameraManager = pWorld->GetComponents().GetFirst<CameraManagerComponent>();

		if ( pCameraManager )
		{
			m_Registered = pCameraManager->RegisterNamedCamera( m_Name, this );
		}
	}
}

HELIUM_IMPLEMENT_ASSET(ExampleGame::CameraComponentDefinition, Components, 0);

void ExampleGame::CameraComponentDefinition::PopulateMetaType( Helium::Reflect::MetaStruct& comp )
{
	comp.AddField( &CameraComponentDefinition::m_Up, "m_Up" );
	comp.AddField( &CameraComponentDefinition::m_Name, "m_Name" );
	comp.AddField( &CameraComponentDefinition::m_NearClip, "m_NearClip" );
	comp.AddField( &CameraComponentDefinition::m_FarClip, "m_FarClip" );
	comp.AddField( &CameraComponentDefinition::m_Fov, "m_Fov" );
}

ExampleGame::CameraComponentDefinition::CameraComponentDefinition()
	: m_Up(Simd::Vector3::BasisY)
	, m_NearClip(1.0f)
	, m_FarClip(5000.0f)
	, m_Fov(90.0f)
{

}
