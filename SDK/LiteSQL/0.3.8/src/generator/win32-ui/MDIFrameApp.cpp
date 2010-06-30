//////////////////////////////////////
// MDIFrameApp.cpp

#include "MDIFrameApp.h"


CMDIFrameApp::CMDIFrameApp()
{
}

BOOL CMDIFrameApp::InitInstance()
{
    //Create the Window
	if (!m_MainMDIFrame.Create())
		return FALSE;	// End the application if the window creation fails

	return TRUE;
}


