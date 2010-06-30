//////////////////////////////////////
// MDIFrameApp.h

#ifndef MDIFRAMEAPP_H
#define MDIFRAMEAPP_H

#include "MainMDIFrm.h"


class CMDIFrameApp : public CWinApp
{
public:
    CMDIFrameApp();
    virtual ~CMDIFrameApp() {}
	virtual BOOL InitInstance();
	CMainMDIFrame& GetMDIFrame() { return m_MainMDIFrame; }

  static const TCHAR* REGISTRY_KEY;

private:
    CMainMDIFrame m_MainMDIFrame;

};


// returns a reference to the CMDIFrameApp object
inline CMDIFrameApp& GetMDIApp() { return *((CMDIFrameApp*)GetApp()); }


#endif // MDIFRAMEAPP_H
