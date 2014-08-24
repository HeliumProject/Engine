#include "GameLibraryPch.h"

#include "PlayerInput.h"
#include "Ois/OisSystem.h"
#include "Graphics/GraphicsManagerComponent.h"
#include "Graphics/BufferedDrawer.h"
#include "MathSimd/Plane.h"
#include "Framework/World.h"

using namespace Helium;
using namespace GameLibrary;

//////////////////////////////////////////////////////////////////////////
// PlayerInputComponent

HELIUM_DEFINE_COMPONENT(GameLibrary::PlayerInputComponent, EXAMPLE_GAME_MAX_PLAYERS * EXAMPLE_GAME_MAX_WORLDS);

void PlayerInputComponent::PopulateMetaType( Reflect::MetaStruct& comp )
{

}

void PlayerInputComponent::Initialize( const PlayerInputComponentDefinition &definition )
{
	//m_Definition.Set( definition );
	m_ScreenSpaceFocusPosition = Simd::Vector2::Zero;
	m_WorldSpaceFocusPosition = Simd::Vector3::Zero;
	m_MoveDir = Simd::Vector2::Zero;
	m_bFirePrimary = false;
}

HELIUM_DEFINE_CLASS(GameLibrary::PlayerInputComponentDefinition);

void PlayerInputComponentDefinition::PopulateMetaType( Reflect::MetaStruct& comp )
{
	
}

void GatherInput( PlayerInputComponent *pPlayerInput )
{
	float x = 0.0f;
	float y = 0.0f;

	if ( Helium::Input::IsKeyDown(Input::KeyCodes::KC_LEFT) )
	{
		x -= 1.0f;
	}
	
	if ( Helium::Input::IsKeyDown(Input::KeyCodes::KC_RIGHT) )
	{
		x += 1.0f;
	}

	if ( Helium::Input::IsKeyDown(Input::KeyCodes::KC_UP) )
	{
		y += 1.0f;
	}

	if ( Helium::Input::IsKeyDown(Input::KeyCodes::KC_DOWN) )
	{
		y -= 1.0f;
	}
	
	pPlayerInput->m_ScreenSpaceFocusPosition = Helium::Input::GetMousePosNormalized();


	GraphicsManagerComponent *pGraphics = pPlayerInput->GetWorld()->GetComponents().GetFirst<GraphicsManagerComponent>();
	GraphicsSceneView *pSceneView = pGraphics->GetGraphicsScene()->GetSceneView(0);

	Simd::Vector3 from;
	Simd::Vector3 to;
	pSceneView->ConvertNormalizedScreenCoordinatesToRaycast(pPlayerInput->m_ScreenSpaceFocusPosition, from, to);

	Simd::Plane plane(Simd::Vector3::BasisZ, -750.0f);
	Simd::Vector3 raycastNormalized = (to - from).GetNormalized();
	Simd::Vector3 pointOnPlane;
	
	if ( plane.CalculateLineIntersect( from, raycastNormalized, pointOnPlane) )
	{
#if GRAPHICS_SCENE_BUFFERED_DRAWER
		SimpleVertex verts[2];
		verts[0].position[0] = pointOnPlane.GetElement(0) - 30.0f;
		verts[0].position[1] = pointOnPlane.GetElement(1);
		verts[0].position[2] = pointOnPlane.GetElement(2);
		verts[0].color[0] = 0xFF;
		verts[0].color[1] = 0xFF;
		verts[0].color[2] = 0xFF;
		verts[0].color[3] = 0xFF;

		verts[1].position[0] = pointOnPlane.GetElement(0) + 30.0f;
		verts[1].position[1] = pointOnPlane.GetElement(1);
		verts[1].position[2] = pointOnPlane.GetElement(2);
		verts[1].color[0] = 0xFF;
		verts[1].color[1] = 0xFF;
		verts[1].color[2] = 0xFF;
		verts[1].color[3] = 0xFF;

		pGraphics->GetBufferedDrawer().DrawLineStrip(verts, 2);

		verts[0].position[0] = pointOnPlane.GetElement(0);
		verts[0].position[1] = pointOnPlane.GetElement(1) - 30.0f;

		verts[1].position[0] = pointOnPlane.GetElement(0);
		verts[1].position[1] = pointOnPlane.GetElement(1) + 30.0f;

		pGraphics->GetBufferedDrawer().DrawLineStrip(verts, 2);
#endif

		pPlayerInput->m_bFirePrimary = Helium::Input::IsMouseButtonDown( Helium::Input::MouseButtons::Left );
		pPlayerInput->m_WorldSpaceFocusPosition = pointOnPlane;
		pPlayerInput->m_bHasWorldSpaceFocus = true;
	}
	else
	{
		pPlayerInput->m_bFirePrimary = false;
		pPlayerInput->m_WorldSpaceFocusPosition = Simd::Vector3::Zero;
		pPlayerInput->m_bHasWorldSpaceFocus = false;
	}
	
	pPlayerInput->m_MoveDir = Simd::Vector2(x, y);
	pPlayerInput->m_MoveDir.NormalizeOrZero();

}

HELIUM_DEFINE_TASK( GatherInputForPlayers, (ForEachWorld< QueryComponents< PlayerInputComponent, GatherInput > >), TickTypes::Client )

void GameLibrary::GatherInputForPlayers::DefineContract( Helium::TaskContract &rContract )
{
	rContract.ExecuteAfter<Helium::StandardDependencies::ReceiveInput>();
	rContract.ExecuteBefore<Helium::StandardDependencies::ProcessPhysics>();
}
