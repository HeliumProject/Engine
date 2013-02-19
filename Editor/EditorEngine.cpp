#include "EditorPch.h"

#include "EditorEngine.h"

#include "RenderingD3D9/D3D9Renderer.h"
#include "Graphics/DynamicDrawer.h"
#include "Framework/WorldManager.h"


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

    return true;
}

void EditorEngine::Shutdown()
{
    m_WorldProxy.Release();

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
    mainCtxInitParams.bVsync = false;
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
    //WorldManager& rWorldManager = WorldManager::GetStaticInstance();
    //rWorldManager.OnViewCanvasPaint();
    //HELIUM_ASSERT(0);
}