#include "WindowingPch.h"
#include "Windowing/WindowManager.h"

#include "Windowing/Window.h"

using namespace Helium;

/// Constructor.
WindowManager::WindowManager()
: m_hInstance( NULL )
, m_nCmdShow( 0 )
, m_windowClassAtom( 0 )
{
}

/// Destructor.
WindowManager::~WindowManager()
{
}

/// Initialize this manager.
///
/// @param[in] hInstance  Handle to the application instance.
/// @param[in] nCmdShow   Flags specifying how the application window should be shown (passed in from WinMain()).
///
/// @return  True if window manager initialization was successful, false if not.
///
/// @see Shutdown()
bool WindowManager::Initialize( HINSTANCE hInstance, int nCmdShow )
{
	HELIUM_ASSERT( hInstance );

	// Register the default window class.
	WNDCLASSEXW windowClass;
	windowClass.cbSize = sizeof( windowClass );
	windowClass.style = 0;
	windowClass.lpfnWndProc = WindowProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = hInstance;
	windowClass.hIcon = NULL;
	windowClass.hCursor = NULL;
	windowClass.hbrBackground = NULL;
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = L"HeliumWindowClass";
	windowClass.hIconSm = NULL;

	m_windowClassAtom = RegisterClassEx( &windowClass );
	HELIUM_ASSERT( m_windowClassAtom != 0 );
	if( m_windowClassAtom == 0 )
	{
		HELIUM_TRACE( TraceLevels::Error, TXT( "WindowManager::Initialize(): Failed to register default window class.\n" ) );

		return false;
	}

	// Store the application instance and window show flags.
	m_hInstance = hInstance;
	m_nCmdShow = nCmdShow;

	return true;
}

/// @copydoc WindowManager::Cleanup()
void WindowManager::Cleanup()
{
	if( m_windowClassAtom )
	{
		HELIUM_ASSERT( m_hInstance );
		HELIUM_VERIFY( UnregisterClass(
			reinterpret_cast< LPCTSTR >( static_cast< uintptr_t >( m_windowClassAtom ) ),
			m_hInstance ) );
		m_windowClassAtom = 0;
	}

	m_hInstance = NULL;
	m_nCmdShow = 0;
}

/// @copydoc WindowManager::Update()
bool WindowManager::Update()
{
	bool bQuit = false;

	MSG message;
	while( !bQuit && PeekMessage( &message, NULL, 0, 0, PM_REMOVE ) )
	{
		if( message.message == WM_QUIT )
		{
			bQuit = true;
		}

		TranslateMessage( &message );
		DispatchMessage( &message );
	}

	return !bQuit;
}

/// @copydoc WindowManager::RequestQuit()
void WindowManager::RequestQuit()
{
	PostQuitMessage( 0 );
}

/// @copydoc WindowManager::Create()
Window* WindowManager::Create( Window::Parameters& rParameters )
{
	HELIUM_ASSERT( m_hInstance );
	HELIUM_ASSERT( m_windowClassAtom );

	// Validate window creation parameters.
	uint32_t width = rParameters.width;
	if( width == 0 )
	{
		HELIUM_TRACE(
			TraceLevels::Warning,
			TXT( "WindowManager::Create(): Zero width specified.  Actual window will have a width of 1.\n" ) );

		width = 1;
	}

	uint32_t height = rParameters.height;
	if( height == 0 )
	{
		HELIUM_TRACE(
			TraceLevels::Warning,
			TXT( "WindowManager::Create(): Zero height specified.  Actual window will have a height of 1.\n" ) );

		height = 1;
	}

	// Create the window itself.
	DWORD style, exStyle;
	int xy;
	if( rParameters.bFullscreen )
	{
		style = WS_POPUP;
		exStyle = WS_EX_TOPMOST;
		xy = 0;
	}
	else
	{
		style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
		exStyle = 0;
		xy = CW_USEDEFAULT;
	}

	RECT windowRect;
	windowRect.left = 0;
	windowRect.top = 0;
	windowRect.right = static_cast< LONG >( width );
	windowRect.bottom = static_cast< LONG >( height );
	HELIUM_VERIFY( AdjustWindowRectEx( &windowRect, style, FALSE, exStyle ) );

	Window* pWindow = new Window;
	HELIUM_ASSERT( pWindow );

	HELIUM_TCHAR_TO_WIDE( rParameters.pTitle, convertedTitle );
	HWND hWnd = CreateWindowEx(
		exStyle,
		reinterpret_cast< LPCTSTR >( static_cast< uintptr_t >( m_windowClassAtom ) ),
		( convertedTitle ? convertedTitle : L"" ),
		style,
		xy,
		xy,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL,
		NULL,
		m_hInstance,
		pWindow );
	HELIUM_ASSERT( hWnd );
	if( !hWnd )
	{
		HELIUM_TRACE( TraceLevels::Error, TXT( "WindowManager::Create(): Window creation failed.\n" ) );

		delete pWindow;

		return NULL;
	}

	pWindow->Set( hWnd, rParameters.pTitle, width, height, rParameters.bFullscreen );

	ShowWindow( hWnd, m_nCmdShow );
	UpdateWindow( hWnd );

	return pWindow;
}

/// Window message procedure callback.
///
/// @param[in] hWnd    Window handle.
/// @param[in] msg     Message ID.
/// @param[in] wParam  Additional message information (dependent on the message sent).
/// @param[in] lParam  Additional message information (dependent on the message sent).
///
/// @return  Result of the message processing (dependent on the message sent).
LRESULT CALLBACK WindowManager::WindowProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	HELIUM_ASSERT( hWnd );

	switch( msg )
	{
	case WM_CREATE:
		{
			// Window pointer is passed in the lpCreateParams member of the CREATESTRUCT structure provided in
			// lParam, so set it as the user data pointer in the window itself so we can easily retrieve it from any
			// future messages.
			CREATESTRUCT* pCreateStruct = reinterpret_cast< CREATESTRUCT* >( lParam );
			HELIUM_ASSERT( pCreateStruct );

			Window* pWindow = static_cast< Window* >( pCreateStruct->lpCreateParams );
			HELIUM_ASSERT( pWindow );

			SetWindowLongPtr( hWnd, GWLP_USERDATA, reinterpret_cast< LONG_PTR >( pWindow ) );

			return 0;
		}

	case WM_CLOSE:
		{
			Window* pWindow = reinterpret_cast< Window* >( GetWindowLongPtr( hWnd, GWLP_USERDATA ) );
			HELIUM_ASSERT( pWindow );

			pWindow->Destroy();

			return 0;
		}

	case WM_DESTROY:
		{
			// Window is being destroyed, so clear reference to Window object and destroy it.
			Window* pWindow = reinterpret_cast< Window* >( GetWindowLongPtr( hWnd, GWLP_USERDATA ) );
			HELIUM_ASSERT( pWindow );
			SetWindowLongPtr( hWnd, GWLP_USERDATA, 0 );

			const Delegate<Window*>& rOnDestroyed = pWindow->GetOnDestroyed();
			if( rOnDestroyed.Valid() )
			{
				rOnDestroyed.Invoke( pWindow );
			}

			delete pWindow;

			return 0;
		}
	}

	return DefWindowProc( hWnd, msg, wParam, lParam );
}
