
#include "OisPch.h"
#include "OisSystem.h"

#include "Dependencies/ois/includes/OIS.h"

using namespace Helium;

static int g_OisInitCount = 0;
static OIS::InputManager *g_InputSystem = 0;
static OIS::Keyboard *g_Keyboard = 0;
static OIS::Mouse *g_Mouse = 0;

#define MAX_KEY_STATES (256)
char g_PreviousFrameKeyStates[MAX_KEY_STATES];

void Input::Initialize(void *hWindow, bool bExclusive)
{
	if (!g_OisInitCount++)
	{
		for (int i =0; i < MAX_KEY_STATES; ++i)
		{
			g_PreviousFrameKeyStates[i] = 0;
		}

		HELIUM_ASSERT(!g_InputSystem);

		// Setup basic variables
		OIS::ParamList paramList;

		size_t windowHnd = *reinterpret_cast<size_t*>(hWindow);
		std::ostringstream windowHndStr;
 
		// Fill parameter list
		windowHndStr << windowHnd;
		paramList.insert( std::make_pair( std::string( "WINDOW" ), windowHndStr.str() ) );
 
		if (!bExclusive)
		{
#if defined OIS_WIN32_PLATFORM
			paramList.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND" )));
			paramList.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
			paramList.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
			paramList.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));
#elif defined OIS_LINUX_PLATFORM
			paramList.insert(std::make_pair(std::string("x11_mouse_grab"), std::string("false")));
			paramList.insert(std::make_pair(std::string("x11_mouse_hide"), std::string("false")));
			paramList.insert(std::make_pair(std::string("x11_keyboard_grab"), std::string("false")));
			paramList.insert(std::make_pair(std::string("XAutoRepeatOn"), std::string("true")));
#endif
		}

		// Create inputsystem
		g_InputSystem = OIS::InputManager::createInputSystem( paramList );
		HELIUM_ASSERT(g_InputSystem);


		g_Keyboard = static_cast<OIS::Keyboard*>(g_InputSystem->createInputObject( OIS::OISKeyboard, false ));
		HELIUM_ASSERT(g_Keyboard);

		g_Mouse = static_cast<OIS::Mouse*>(g_InputSystem->createInputObject( OIS::OISMouse, false ));
		HELIUM_ASSERT(g_Mouse);
	}
}

void Input::Cleanup()
{
	--g_OisInitCount;
	if (!g_OisInitCount)
	{
		HELIUM_ASSERT(g_InputSystem);
		g_InputSystem->destroyInputSystem( g_InputSystem );
		//mInputSystem->destroyInputSystem();
		g_InputSystem = 0;
	}
}

void Input::SetWindowSize(int x, int y)
{    
	const OIS::MouseState &mouseState = g_Mouse->getMouseState();
	mouseState.width  = x;
	mouseState.height = y;
}

void Input::Capture()
{
	g_Keyboard->copyKeyStates(g_PreviousFrameKeyStates);

	HELIUM_ASSERT(g_Keyboard);
	g_Keyboard->capture();

	HELIUM_ASSERT(g_Mouse);
	g_Mouse->capture();
}

bool Input::IsKeyDown(Input::KeyCode keyCode)
{
	HELIUM_ASSERT(g_Keyboard);
	return g_Keyboard->isKeyDown(static_cast<OIS::KeyCode>(keyCode));
}

bool Input::WasKeyPressedThisFrame(Input::KeyCode keyCode)
{
	HELIUM_ASSERT(g_Keyboard);
	return !g_PreviousFrameKeyStates[keyCode] && g_Keyboard->isKeyDown(static_cast<OIS::KeyCode>(keyCode));
}

bool Input::IsModifierDown(Input::KeyboardModifier keyCode)
{
	HELIUM_ASSERT(g_Keyboard);
	return g_Keyboard->isModifierDown(static_cast< OIS::Keyboard::Modifier >(keyCode));
}

Point Input::GetMousePos()
{
	HELIUM_ASSERT(g_Mouse);
	return Point( g_Mouse->getMouseState().X.abs, g_Mouse->getMouseState().X.abs);
}

Simd::Vector2 Input::GetMousePosNormalized()
{
	HELIUM_ASSERT(g_Mouse);
	return Simd::Vector2( 
		(static_cast<float>(g_Mouse->getMouseState().X.abs) / static_cast<float>(g_Mouse->getMouseState().width) - 0.5f) * 2.0f, 
		(static_cast<float>(g_Mouse->getMouseState().Y.abs) / static_cast<float>(g_Mouse->getMouseState().height) - 0.5f) * 2.0f
		);
}

Simd::Vector2 Input::GetMousePosDelta()
{
	return Simd::Vector2(
		static_cast<float>(g_Mouse->getMouseState().X.rel),
		static_cast<float>(g_Mouse->getMouseState().Y.rel));
}
