#include "FrameworkImplPch.h"
#include "FrameworkImpl/RendererInitializationImpl.h"
#include "Windowing/WindowManager.h"
#include "Engine/Config.h"
#include "Graphics/GraphicsConfig.h"

#if HELIUM_DIRECT3D
# include "RenderingD3D9/D3D9Renderer.h"
#else
# include "RenderingGL/GLRenderer.h"
#endif

#include "Graphics/RenderResourceManager.h"
#include "Graphics/DynamicDrawer.h"

using namespace Helium;

/// @copydoc RendererInitialization::Initialize()
bool RendererInitializationImpl::Initialize()
{
	WindowManager* pWindowManager = WindowManager::GetInstance();
	if( !pWindowManager )
	{
		HELIUM_TRACE(
			TraceLevels::Info,
			( TXT( "RendererInitializationImpl::Initialize(): No window manager created.  A window manager is necessary for " )
			TXT( "RendererInitializationImpl execution.\n" ) ) );

		return false;
	}

	Renderer* pRenderer = NULL;

#if HELIUM_DIRECT3D
	D3D9Renderer::Startup();
	pRenderer = D3D9Renderer::GetInstance();
#elif HELIUM_OPENGL
	GLRenderer::Startup();
	pRenderer = GLRenderer::GetInstance();
#endif
	if ( !HELIUM_VERIFY( pRenderer ) )
	{
		return false;
	}

	// Create the main application window.
	Config* pConfig = Config::GetInstance();
	HELIUM_ASSERT( pConfig );

	StrongPtr< GraphicsConfig > spGraphicsConfig( pConfig->GetConfigObject< GraphicsConfig >( Name( "GraphicsConfig" ) ) );
	HELIUM_ASSERT( spGraphicsConfig );

	uint32_t displayWidth = spGraphicsConfig->GetWidth();
	uint32_t displayHeight = spGraphicsConfig->GetHeight();
	bool bFullscreen = spGraphicsConfig->GetFullscreen();
	bool bVsync = spGraphicsConfig->GetVsync();

	Window::Parameters windowParameters;
	windowParameters.pTitle = "Helium";
	windowParameters.width = displayWidth;
	windowParameters.height = displayHeight;
	windowParameters.bFullscreen = bFullscreen;

	m_pMainWindow = pWindowManager->Create( windowParameters );
	HELIUM_ASSERT( m_pMainWindow );
	if( !m_pMainWindow )
	{
		HELIUM_TRACE( TraceLevels::Error, TXT( "Failed to create main application window.\n" ) );

		return false;
	}

	m_pMainWindow->SetOnDestroyed( Delegate<Window*>( this, &RendererInitializationImpl::OnMainWindowDestroyed ) );

	// Create the application rendering context.
	Renderer::ContextInitParameters contextInitParams;
	contextInitParams.pWindow = m_pMainWindow->GetHandle();
	contextInitParams.displayWidth = displayWidth;
	contextInitParams.displayHeight = displayHeight;
	contextInitParams.bFullscreen = bFullscreen;
	contextInitParams.bVsync = bVsync;
	if( !HELIUM_VERIFY( pRenderer->CreateMainContext( contextInitParams ) ) )
	{
		HELIUM_TRACE( TraceLevels::Error, TXT( "Failed to create main renderer context.\n" ) );
		return false;
	}

	RenderResourceManager::Startup();
	DynamicDrawer::Startup();
	return true;
}

/// Callback executed when the main window is actually destroyed.
///
/// @param[in] pWindow  Pointer to the destroyed Window instance.
void RendererInitializationImpl::OnMainWindowDestroyed( Window* pWindow )
{
	HELIUM_ASSERT( m_pMainWindow == pWindow );
	HELIUM_UNREF( pWindow );

#if HELIUM_OPENGL
	// Immediately shut down, since we use GLFW to manage windows, and GLFW
	// windows are inseparable from their render contexts.  Therefore, by the
	// time we've received this callback, our renderer had better be shutting down.
	GLRenderer::Shutdown();
#endif

	m_pMainWindow = NULL;
	WindowManager* pWindowManager = WindowManager::GetInstance();
	HELIUM_ASSERT( pWindowManager );
	pWindowManager->RequestQuit();
}

void Helium::RendererInitializationImpl::Shutdown()
{
	DynamicDrawer::Shutdown();
	RenderResourceManager::Shutdown();

	Renderer* pRenderer = Renderer::GetInstance();
	if( pRenderer )
	{
#if HELIUM_DIRECT3D
		D3D9Renderer::Shutdown();
#elif HELIUM_OPENGL
		GLRenderer::Shutdown();
#endif
	}

	WindowManager* pWindowManager = WindowManager::GetInstance();
	if( pWindowManager )
	{
		if( m_pMainWindow )
		{
			m_pMainWindow->Destroy();
			while( m_pMainWindow )
			{
				pWindowManager->Update();
			}
		}

		pWindowManager->Shutdown();
		WindowManager::Shutdown();
	}
}