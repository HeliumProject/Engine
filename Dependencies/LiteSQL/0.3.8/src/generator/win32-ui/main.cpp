///////////////////////////////////////
// main.cpp

#include "MDIFrameApp.h"


INT WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
	// Start Win32++
    CMDIFrameApp MyApp;

	// Run the application
	return MyApp.Run();
}
