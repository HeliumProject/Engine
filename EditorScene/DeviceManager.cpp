#include "EditorScenePch.h"
#include "DeviceManager.h"

#include "Foundation/Log.h"
#include "Rendering/RRenderContext.h"

#if HELIUM_DIRECT3D
#include "RenderingD3D9/D3D9Renderer.h"
#endif

using namespace Helium;

bool                DeviceManager::m_Unique = false;
uint32_t            DeviceManager::m_InitCount = 0;
DeviceManager*      DeviceManager::m_Clients[MAX_DEVICE_COUNT] = {0};
RRenderContextPtr   DeviceManager::sm_spMainRenderContext;
#if HELIUM_OS_WIN
HWND                DeviceManager::sm_hMainRenderContextWnd;
#else
void*               DeviceManager::sm_hMainRenderContextWnd;
#endif
uint32_t            DeviceManager::sm_mainRenderContextWidth;
uint32_t            DeviceManager::sm_mainRenderContextHeight;

///////////////////////////////////////////////////////////////////////////////////////////////////
DeviceManager::DeviceManager()
	: m_hWnd( NULL )
	, m_Width( 0 )
	, m_Height( 0 )
	, m_UsingSwapchain( false )
{
	//record the this pointer in the client array so we can call back to free/recreate default pool resources
	// first look for empty entries in the client array
	size_t clientIndex;
	for ( clientIndex = 0; clientIndex < HELIUM_ARRAY_COUNT( m_Clients ); ++clientIndex )
	{
		if ( !m_Clients[ clientIndex ] )
		{
			m_Clients[ clientIndex ] = this;
			++m_InitCount;

			break;
		}
	}

	HELIUM_ASSERT( clientIndex < HELIUM_ARRAY_COUNT( m_Clients ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
DeviceManager::~DeviceManager()
{
	--m_InitCount;

	// this is the last client, we need to free up the global resources before we free the device
	if ( m_InitCount == 0 )
	{
		sm_spMainRenderContext.Release();
	}

	m_spRenderContext.Release();

	// go through all the clients and remove ourself
	for ( size_t clientIndex = 0; clientIndex < HELIUM_ARRAY_COUNT( m_Clients ); ++clientIndex )
	{
		if ( m_Clients[ clientIndex ] == this )
		{
			m_Clients[ clientIndex ] = NULL;
			break;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DeviceManager::SetUnique()
{
	if ( !sm_spMainRenderContext )
	{
		m_Unique = true;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#if HELIUM_OS_WIN
bool DeviceManager::Init( HWND hwnd, uint32_t back_buffer_width, uint32_t back_buffer_height, uint32_t /*init_flags*/ )
#else
bool DeviceManager::Init( void* hwnd, uint32_t back_buffer_width, uint32_t back_buffer_height, uint32_t /*init_flags*/ )
#endif
{
	Helium::Renderer* pRenderer = NULL;
	if ( !sm_spMainRenderContext )
	{
		bool bCreatedRenderer = false;
#if HELIUM_DIRECT3D
		bCreatedRenderer = Helium::D3D9Renderer::CreateStaticInstance();
#endif
		HELIUM_ASSERT( bCreatedRenderer );
		if ( !bCreatedRenderer )
		{
			return false;
		}

		if ( m_Unique )
		{
			sm_hMainRenderContextWnd = hwnd;
			sm_mainRenderContextWidth = back_buffer_width;
			sm_mainRenderContextHeight = back_buffer_height;
		}
		else
		{
			// if we are not running in unique mode then everything if a flip chain, the default back buffer is 64x64
			sm_mainRenderContextWidth = 64;
			sm_mainRenderContextHeight = 64;
		}

		pRenderer = Helium::Renderer::GetInstance();
		HELIUM_ASSERT( pRenderer );
		pRenderer->Initialize();

		Helium::Renderer::ContextInitParameters initParameters;
		initParameters.pWindow = hwnd;
		initParameters.bFullscreen = false;
		initParameters.bVsync = false;
		initParameters.displayWidth = sm_mainRenderContextWidth;
		initParameters.displayHeight = sm_mainRenderContextHeight;

		bool bCreateResult = pRenderer->CreateMainContext( initParameters );
		HELIUM_ASSERT( bCreateResult );
		if ( !bCreateResult )
		{
			return false;
		}

		sm_spMainRenderContext = pRenderer->GetMainContext();
		HELIUM_ASSERT( sm_spMainRenderContext );

		if ( m_Unique )
		{
			m_spRenderContext = sm_spMainRenderContext;
		}
	}
	else
	{
		// if we get to here it must be a second instance and when running unique that is not allowed.
		HELIUM_ASSERT( !m_Unique );
		if ( m_Unique )
		{
			return false;
		}

		pRenderer = Helium::Renderer::GetInstance();
		HELIUM_ASSERT( pRenderer );
	}

	if ( !m_Unique )
	{
		// Create an additional render context.
		Helium::Renderer::ContextInitParameters initParameters;
		initParameters.pWindow = hwnd;
		initParameters.bFullscreen = false;
		initParameters.bVsync = false;
		initParameters.displayWidth = back_buffer_width;
		initParameters.displayHeight = back_buffer_height;

		m_spRenderContext = pRenderer->CreateSubContext( initParameters );
		HELIUM_ASSERT( m_spRenderContext );
		if ( !m_spRenderContext )
		{
			return false;
		}

		m_UsingSwapchain = true;
	}

	m_hWnd = hwnd;
	m_Width = back_buffer_width;
	m_Height = back_buffer_height;

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DeviceManager::ResizeSwapChain( uint32_t width, uint32_t height )
{
	m_spRenderContext.Release();

	Helium::Renderer::ContextInitParameters initParameters;
	initParameters.pWindow = m_hWnd;
	initParameters.bFullscreen = false;
	initParameters.bVsync = false;
	initParameters.displayWidth = width;
	initParameters.displayHeight = height;

	Helium::Renderer* pRenderer = Helium::Renderer::GetInstance();
	HELIUM_ASSERT( pRenderer );
	m_spRenderContext = pRenderer->CreateSubContext( initParameters );

	return ( m_spRenderContext != NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DeviceManager::ResizeDevice( uint32_t width, uint32_t height )
{
	m_spRenderContext.Release();
	sm_spMainRenderContext.Release();

	Helium::Renderer::ContextInitParameters initParameters;
	initParameters.pWindow = m_hWnd;
	initParameters.bFullscreen = false;
	initParameters.bVsync = false;
	initParameters.displayWidth = width;
	initParameters.displayHeight = height;

	Helium::Renderer* pRenderer = Helium::Renderer::GetInstance();
	HELIUM_ASSERT( pRenderer );
	pRenderer->ResetMainContext( initParameters );

	m_spRenderContext = pRenderer->GetMainContext();
	sm_spMainRenderContext = m_spRenderContext;

	return ( m_spRenderContext != NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DeviceManager::Resize( uint32_t width, uint32_t height )
{
	if ( m_UsingSwapchain )
	{
		return ResizeSwapChain( width, height );
	}

	// this will currently only be used in 'unique' mode
	HELIUM_ASSERT( m_Unique );
	return ResizeDevice( width, height );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DeviceManager::Swap()
{
	m_spRenderContext->Swap();

	return true;
}

bool DeviceManager::TestDeviceReady()
{
	Helium::Renderer* pRenderer = Helium::Renderer::GetInstance();
	HELIUM_ASSERT( pRenderer );

	Helium::Renderer::EStatus rendererStatus = pRenderer->GetStatus();
	if ( rendererStatus == Helium::Renderer::STATUS_READY )
	{
		return true;
	}

	if ( rendererStatus == Helium::Renderer::STATUS_NOT_RESET )
	{
		rendererStatus = pRenderer->Reset();
		if ( rendererStatus == Helium::Renderer::STATUS_READY )
		{
			return true;
		}
	}

	return false;
}
