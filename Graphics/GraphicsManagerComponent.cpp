
#include "GraphicsPch.h"
#include "Graphics/GraphicsManagerComponent.h"
#include "Graphics/GraphicsScene.h"
#include "Graphics/RenderResourceManager.h"
#include "Rendering/Renderer.h"
#include "Framework/TaskScheduler.h"

using namespace Helium;

HELIUM_IMPLEMENT_ASSET(Helium::GraphicsManagerComponentDefinition, Components, 0);

void GraphicsManagerComponentDefinition::PopulateMetaType( Reflect::MetaStruct& comp )
{

}

GraphicsManagerComponentDefinition::GraphicsManagerComponentDefinition()
{

}


HELIUM_DEFINE_COMPONENT(Helium::GraphicsManagerComponent, 16);

void GraphicsManagerComponent::PopulateMetaType( Reflect::MetaStruct& comp )
{

}

void GraphicsManagerComponent::Initialize( const GraphicsManagerComponentDefinition &definition)
{
	m_spGraphicsScene = Reflect::AssertCast<GraphicsScene>(GraphicsScene::CreateObject());
	HELIUM_ASSERT( m_spGraphicsScene );
	if( !m_spGraphicsScene )
	{
		HELIUM_TRACE( TraceLevels::Error, TXT( "World::Initialize(): Failed to create a primary graphics scene.\n" ) );
		return;
	}
	
	RenderResourceManager &rRenderResourceManager = RenderResourceManager::GetStaticInstance();

	Renderer *pRenderer = Renderer::GetStaticInstance();

	RRenderContext *rRenderContext = pRenderer->GetMainContext();
	HELIUM_ASSERT( rRenderContext );

	uint32_t mainSceneViewId = m_spGraphicsScene->AllocateSceneView();
	if( IsValid( mainSceneViewId ) )
	{
		float32_t aspectRatio =
			static_cast< float32_t >( 800 ) / static_cast< float32_t >( 600 );

		RSurface* pDepthStencilSurface = rRenderResourceManager.GetDepthStencilSurface();
		HELIUM_ASSERT( pDepthStencilSurface );

		GraphicsSceneView* pMainSceneView = m_spGraphicsScene->GetSceneView( mainSceneViewId );
		HELIUM_ASSERT( pMainSceneView );
		pMainSceneView->SetRenderContext( rRenderContext );
		pMainSceneView->SetDepthStencilSurface( pDepthStencilSurface );
		pMainSceneView->SetAspectRatio( aspectRatio );
		pMainSceneView->SetViewport( 0, 0, 800, 600 );
		pMainSceneView->SetClearColor( Color( 0x00202020 ) );
	}
}

void DrawGraphics( World *pWorld )
{
	GraphicsManagerComponent *pGraphicsManager = pWorld->GetComponents().GetFirst<GraphicsManagerComponent>();
	HELIUM_ASSERT( pGraphicsManager );

	pGraphicsManager->GetGraphicsScene()->Update( pWorld );
}

// NOTE: We don't want this to be a render task because the editor explicitly call Update on graphics scene
// on paint.
HELIUM_DEFINE_TASK( GraphicsManagerDrawTask, ForEachWorld< DrawGraphics >, TickTypes::Client )

void Helium::GraphicsManagerDrawTask::DefineContract( TaskContract &rContract )
{
	rContract.ExecutesWithin< Helium::StandardDependencies::Render >();
}