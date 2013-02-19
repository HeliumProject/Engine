#include "EditorPch.h"

#include "EditorEngine.h"

#include "RenderingD3D9/D3D9Renderer.h"
#include "Graphics/DynamicDrawer.h"


using namespace Helium;
using namespace Helium::Editor;


EditorEngine::EditorEngine()
{

}

EditorEngine::~EditorEngine()
{

}

bool EditorEngine::Initialize( HWND hwnd )
{
    InitRenderer( hwnd );

    // Create after the renderer so the World's BufferedDrawer can correctly initialize.
    CreateEditorWorld();

    return true;
}

void EditorEngine::Shutdown()
{
    m_EditorWorld.Release();
    m_EditorPackage.Release();

    WorldManager::DestroyStaticInstance();
    DynamicDrawer::DestroyStaticInstance();
    RenderResourceManager::DestroyStaticInstance();
    Renderer::DestroyStaticInstance();
}

void EditorEngine::Update()
{
    WorldManager::GetStaticInstance().Update();
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
    mainCtxInitParams.bVsync = false;
    mainCtxInitParams.displayWidth = 64;
    mainCtxInitParams.displayHeight = 64;

    HELIUM_VERIFY( pRenderer->CreateMainContext( mainCtxInitParams ) );

    RenderResourceManager& rRenderResourceManager = RenderResourceManager::GetStaticInstance();
    rRenderResourceManager.Initialize();
    rRenderResourceManager.UpdateMaxViewportSize( wxSystemSettings::GetMetric(wxSYS_SCREEN_X), wxSystemSettings::GetMetric(wxSYS_SCREEN_Y) );

    HELIUM_VERIFY( DynamicDrawer::GetStaticInstance().Initialize() );
}

void EditorEngine::CreateEditorWorld()
{
    HELIUM_VERIFY( GameObject::Create< Package >( m_EditorPackage, Name( TXT( "EditorInternalPackage" ) ), NULL ) );

    WorldManager& rWorldManager = WorldManager::GetStaticInstance();
    HELIUM_VERIFY( rWorldManager.Initialize() );

    m_EditorWorld = rWorldManager.CreateDefaultWorld();
    HELIUM_ASSERT( m_EditorWorld );
    HELIUM_VERIFY( m_EditorWorld->Initialize() );
}