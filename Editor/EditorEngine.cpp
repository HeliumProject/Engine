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

    // Create after the renderer so the World's BufferedDrawer can initialize.
    CreateEditorWorld();

    return true;
}

void EditorEngine::Shutdown()
{
    DynamicDrawer& rDynamicDrawer = DynamicDrawer::GetStaticInstance();
    rDynamicDrawer.Shutdown();

    RenderResourceManager& rRenderResourceManager = RenderResourceManager::GetStaticInstance();
    rRenderResourceManager.Shutdown();
    RenderResourceManager::DestroyStaticInstance();

    m_EditorSlice.Release();
    m_EditorWorld.Release();
    m_EditorPackage.Release();
}

void EditorEngine::Update()
{
    WorldManager& rWorldManager = WorldManager::GetStaticInstance();
    rWorldManager.Update();
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

    DynamicDrawer& rDynamicDrawer = DynamicDrawer::GetStaticInstance();
    HELIUM_VERIFY( rDynamicDrawer.Initialize() );
}

void EditorEngine::CreateEditorWorld()
{
    HELIUM_VERIFY( GameObject::Create< Package >( m_EditorPackage, Name( TXT( "EditorInternalPackage" ) ), NULL ) );

    WorldManager& rWorldManager = WorldManager::GetStaticInstance();
    m_EditorWorld = rWorldManager.CreateDefaultWorld();
    HELIUM_ASSERT( m_EditorWorld );
    HELIUM_VERIFY( m_EditorWorld->Initialize() );

    HELIUM_VERIFY( GameObject::Create< Slice >( m_EditorSlice, Name( TXT( "EditorInternalSlice" ) ), m_EditorPackage ) );
    HELIUM_VERIFY( m_EditorWorld->AddSlice( m_EditorSlice ) );
    m_EditorSlice->BindPackage( m_EditorPackage );
}