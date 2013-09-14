#include "FrameworkWinPch.h"
#include "FrameworkWin/RendererInitializationWin.h"
#include "Windowing/WindowManager.h"
#include "Engine/Config.h"
#include "Graphics/GraphicsConfig.h"

#include "RenderingD3D9/D3D9Renderer.h"
#include "Graphics/RenderResourceManager.h"
#include "Graphics/DynamicDrawer.h"

using namespace Helium;

/// @copydoc RendererInitialization::Initialize()
bool RendererInitializationWin::Initialize()
{
	WindowManager* pWindowManager = WindowManager::GetStaticInstance();
	if( !pWindowManager )
	{
		HELIUM_TRACE(
			TraceLevels::Info,
			( TXT( "RendererInitializationWin::Initialize(): No window manager created.  A window manager is necessary for " )
			TXT( "RendererInitializationWin execution.\n" ) ) );

		return false;
	}

	if( !D3D9Renderer::CreateStaticInstance() )
	{
		return false;
	}

	Renderer* pRenderer = D3D9Renderer::GetStaticInstance();
	HELIUM_ASSERT( pRenderer );
	if( !pRenderer->Initialize() )
	{
		Renderer::DestroyStaticInstance();

		return false;
	}

	// Create the main application window.
	Config& rConfig = Config::GetStaticInstance();
	StrongPtr< GraphicsConfig > spGraphicsConfig(
		rConfig.GetConfigObject< GraphicsConfig >( Name( TXT( "GraphicsConfig" ) ) ) );
	HELIUM_ASSERT( spGraphicsConfig );

	uint32_t displayWidth = spGraphicsConfig->GetWidth();
	uint32_t displayHeight = spGraphicsConfig->GetHeight();
	bool bFullscreen = spGraphicsConfig->GetFullscreen();
	bool bVsync = spGraphicsConfig->GetVsync();

	Window::Parameters windowParameters;
	windowParameters.pTitle = TXT( "Helium" );
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

	m_pMainWindow->SetOnDestroyed( Delegate<Window*>( this, &RendererInitializationWin::OnMainWindowDestroyed ) );

	Renderer::ContextInitParameters contextInitParams;
	contextInitParams.pWindow = m_pMainWindow->GetHandle();
	contextInitParams.displayWidth = displayWidth;
	contextInitParams.displayHeight = displayHeight;
	contextInitParams.bFullscreen = bFullscreen;
	contextInitParams.bVsync = bVsync;

	bool bContextCreateResult = pRenderer->CreateMainContext( contextInitParams );
	HELIUM_ASSERT( bContextCreateResult );
	if( !bContextCreateResult )
	{
		HELIUM_TRACE( TraceLevels::Error, TXT( "Failed to create main renderer context.\n" ) );

		return false;
	}

	// Create and initialize the render resource manager.
	RenderResourceManager& rRenderResourceManager = RenderResourceManager::GetStaticInstance();
	rRenderResourceManager.Initialize();

	// Create and initialize the dynamic drawing interface.
	DynamicDrawer& rDynamicDrawer = DynamicDrawer::GetStaticInstance();
	if( !rDynamicDrawer.Initialize() )
	{
		HELIUM_TRACE( TraceLevels::Error, TXT( "Failed to initialize dynamic drawing support.\n" ) );

		return false;
	}

	return true;
}

/// Callback executed when the main window is actually destroyed.
///
/// @param[in] pWindow  Pointer to the destroyed Window instance.
void RendererInitializationWin::OnMainWindowDestroyed( Window* pWindow )
{
	HELIUM_ASSERT( m_pMainWindow == pWindow );
	HELIUM_UNREF( pWindow );

	m_pMainWindow = NULL;
	WindowManager* pWindowManager = WindowManager::GetStaticInstance();
	HELIUM_ASSERT( pWindowManager );
	pWindowManager->RequestQuit();
}

void Helium::RendererInitializationWin::Shutdown()
{
	DynamicDrawer::DestroyStaticInstance();
	RenderResourceManager::DestroyStaticInstance();

	Renderer* pRenderer = Renderer::GetStaticInstance();
	if( pRenderer )
	{
		pRenderer->Shutdown();
		Renderer::DestroyStaticInstance();
	}

	WindowManager* pWindowManager = WindowManager::GetStaticInstance();
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
		WindowManager::DestroyStaticInstance();
	}
}