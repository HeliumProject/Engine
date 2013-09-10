#include "ExampleGamePch.h"

#include "ExampleGame/Components/Graphics/ScreenSpaceText.h"
#include "Reflect/TranslatorDeduction.h"
#include "Framework/ComponentQuery.h"
#include "Graphics/BufferedDrawer.h"
#include "Graphics/GraphicsManagerComponent.h"

using namespace Helium;
using namespace ExampleGame;

//////////////////////////////////////////////////////////////////////////
// PlayerComponent

HELIUM_DEFINE_COMPONENT(ExampleGame::ScreenSpaceTextComponent, 16);

void ScreenSpaceTextComponent::PopulateMetaType( Reflect::MetaStruct& comp )
{

}

ExampleGame::ScreenSpaceTextComponent::ScreenSpaceTextComponent()
{

}

void ScreenSpaceTextComponent::Initialize( const ScreenSpaceTextComponentDefinition &definition )
{
	m_Definition.Set( &definition );
}

void ExampleGame::ScreenSpaceTextComponent::Render( Helium::GraphicsManagerComponent &rGraphicsManager )
{
#if GRAPHICS_SCENE_BUFFERED_DRAWER
	Helium::BufferedDrawer &rBufferedDrawer = rGraphicsManager.GetBufferedDrawer();

	float width = static_cast<float>(rGraphicsManager.GetGraphicsScene()->GetSceneView(0)->GetViewportWidth());
	float height = static_cast<float>(rGraphicsManager.GetGraphicsScene()->GetSceneView(0)->GetViewportHeight());

	// TODO: Make this not use buffered drawer
	rBufferedDrawer.DrawScreenText(
		static_cast<int32_t>(width * m_Definition->m_Position.GetX()), 
		static_cast<int32_t>(height * m_Definition->m_Position.GetY()), 
		m_Definition->m_Text);
#endif
}

HELIUM_IMPLEMENT_ASSET(ExampleGame::ScreenSpaceTextComponentDefinition, Components, 0);

void ExampleGame::ScreenSpaceTextComponentDefinition::PopulateMetaType( Helium::Reflect::MetaStruct& comp )
{
	comp.AddField( &ScreenSpaceTextComponentDefinition::m_Text, "m_Text" );
	comp.AddField( &ScreenSpaceTextComponentDefinition::m_Position, "m_Position" );
}

ExampleGame::ScreenSpaceTextComponentDefinition::ScreenSpaceTextComponentDefinition()
	: m_Position(Simd::Vector2::Zero)
{

}

static GraphicsManagerComponent *g_pGraphicsManager;
static BufferedDrawer *g_pBufferedDrawer;

void DrawScreenSpaceText( ScreenSpaceTextComponent *pShaderComponent )
{
	pShaderComponent->Render( *g_pGraphicsManager );
};

void DrawScreenSpaceText( World *pWorld )
{
#if !GRAPHICS_SCENE_BUFFERED_DRAWER
	HELIUM_ASSERT( 0 );
#else // GRAPHICS_SCENE_BUFFERED_DRAWER

	g_pGraphicsManager = pWorld->GetComponents().GetFirst<GraphicsManagerComponent>();
	HELIUM_ASSERT( g_pGraphicsManager );

	QueryComponents< ScreenSpaceTextComponent, DrawScreenSpaceText >( pWorld );
#endif
}


HELIUM_DEFINE_TASK( DrawScreenSpaceTextTask, (ForEachWorld< DrawScreenSpaceText >) )

void ExampleGame::DrawScreenSpaceTextTask::DefineContract( Helium::TaskContract &rContract )
{
	rContract.ExecutesWithin<Helium::StandardDependencies::Render>();
}
