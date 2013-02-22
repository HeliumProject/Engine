#include "EditorPch.h"

#include "EditorEngine.h"

#include "RenderingD3D9/D3D9Renderer.h"
#include "Graphics/DynamicDrawer.h"
#include "Framework/WorldManager.h"
#include "Reflect/Object.h"


using namespace Helium;
using namespace Helium::Editor;


EditorEngine::EditorEngine()
: m_SceneManager( NULL )
{
}

EditorEngine::~EditorEngine()
{
    HELIUM_ASSERT( m_SceneProxyToRuntimeMap.IsEmpty() );
}

bool EditorEngine::Initialize( SceneGraph::SceneManager* sceneManager, HWND hwnd )
{
    HELIUM_VERIFY( m_SceneManager = sceneManager );

    InitRenderer( hwnd );

    HELIUM_VERIFY( WorldManager::GetStaticInstance().Initialize() );

    m_SceneManager->e_SceneAdded.AddMethod( this, &EditorEngine::OnSceneAdded );
    m_SceneManager->e_SceneRemoving.AddMethod( this, &EditorEngine::OnSceneRemoving );

    return true;
}

void EditorEngine::Shutdown()
{
    m_SceneManager->e_SceneAdded.RemoveMethod( this, &EditorEngine::OnSceneAdded );
    m_SceneManager->e_SceneRemoving.RemoveMethod( this, &EditorEngine::OnSceneRemoving );

    WorldManager::DestroyStaticInstance();
    DynamicDrawer::DestroyStaticInstance();
    RenderResourceManager::DestroyStaticInstance();
    Renderer::DestroyStaticInstance();
}

void EditorEngine::InitRenderer( HWND hwnd )
{
    HELIUM_VERIFY( D3D9Renderer::CreateStaticInstance() );

    Renderer* pRenderer = Renderer::GetStaticInstance();
    HELIUM_ASSERT( pRenderer );
    pRenderer->Initialize();

    Renderer::ContextInitParameters mainCtxInitParams;
    mainCtxInitParams.pWindow = hwnd;
    mainCtxInitParams.bFullscreen = false;
    mainCtxInitParams.bVsync = true;
    mainCtxInitParams.displayWidth = 64;
    mainCtxInitParams.displayHeight = 64;

    HELIUM_VERIFY( pRenderer->CreateMainContext( mainCtxInitParams ) );

    RenderResourceManager& rRenderResourceManager = RenderResourceManager::GetStaticInstance();
    rRenderResourceManager.Initialize();
    rRenderResourceManager.UpdateMaxViewportSize( wxSystemSettings::GetMetric(wxSYS_SCREEN_X), wxSystemSettings::GetMetric(wxSYS_SCREEN_Y) );

    HELIUM_VERIFY( DynamicDrawer::GetStaticInstance().Initialize() );
}

void EditorEngine::OnViewCanvasPaint()
{
    WorldManager& rWorldManager = WorldManager::GetStaticInstance();
    rWorldManager.Update();
}

bool EditorEngine::CreateRuntimeForScene( SceneGraph::Scene* scene )
{
    HELIUM_ASSERT( scene->GetType() == SceneGraph::Scene::SceneTypes::World );

    HELIUM_ASSERT( m_SceneProxyToRuntimeMap.Find( scene ) == m_SceneProxyToRuntimeMap.End() );

    switch ( scene->GetType() )
    {
        case SceneGraph::Scene::SceneTypes::World:
            {
                WorldPtr world = WorldManager::GetStaticInstance().CreateWorld( scene->GetDefinition() );
                scene->SetRuntimeObject( world );
                m_SceneProxyToRuntimeMap[scene] = world;

                return true;
            }
    }

    return false;
}

bool EditorEngine::ReleaseRuntimeForScene( SceneGraph::Scene* scene )
{
    HELIUM_ASSERT( scene->GetType() == SceneGraph::Scene::SceneTypes::World );

    HELIUM_ASSERT( m_SceneProxyToRuntimeMap.Find( scene ) != m_SceneProxyToRuntimeMap.End() );

    switch ( scene->GetType() )
    {
        case SceneGraph::Scene::SceneTypes::World:
            {
                World* world = Reflect::AssertCast<World>( m_SceneProxyToRuntimeMap[scene] );
                scene->SetRuntimeObject( NULL );
                m_SceneProxyToRuntimeMap.Remove( scene );
                WorldManager::GetStaticInstance().ReleaseWorld( world );

                return true;
            }
    }

    return false;
}

void EditorEngine::OnSceneAdded( const SceneGraph::SceneChangeArgs& args )
{
    HELIUM_VERIFY( CreateRuntimeForScene( args.m_Scene ) );
}

void EditorEngine::OnSceneRemoving( const SceneGraph::SceneChangeArgs& args )
{
    HELIUM_VERIFY( ReleaseRuntimeForScene( args.m_Scene ) );
}
