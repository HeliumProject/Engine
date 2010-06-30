////////////////////////////////////////////////////
// MainMDIfrm.cpp  - definitions for the CMainMDIFrame class

#include "mainMDIfrm.h"
#include "LitesqlMDIChild.h"
#include "resource.h"
#include "mdiframeapp.h"

const TCHAR* CMDIFrameApp::REGISTRY_KEY=_T("litesl-generator");

CMainMDIFrame::CMainMDIFrame()
{
	// Set the registry key name, and load the initial window position
	// Use a registry key name like "CompanyName\\Application"
	LoadRegistrySettings(CMDIFrameApp::REGISTRY_KEY);
}

CMainMDIFrame::~CMainMDIFrame()
{
}

void CMainMDIFrame::OnFileOpen()
{
	TCHAR szFilePathName[_MAX_PATH] = _T("");
	OPENFILENAME ofn = {0};
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = m_hWnd;
	ofn.lpstrFile = szFilePathName;
	ofn.nMaxFile = _MAX_PATH;
	ofn.lpstrTitle = _T("Open File");

	// Bring up the dialog, and open the file
	::GetOpenFileName(&ofn);
  
  AddMDIChild(new CSimpleMDIChild(ofn.lpstrFile)); // CMDIFrame::RemoveMDIChild deletes this pointer
	
  // TODO:
	// Add your own code here. Refer to the tutorial for additional information 
}

void CMainMDIFrame::OnFileSave()
{ 
  CSimpleMDIChild* pActive = (CSimpleMDIChild*)GetActiveMDIChild();
  if (pActive!=NULL)
  {
    pActive->getDocument()->Save();
  }
  else
  {
   // TODO:
  }
}

void CMainMDIFrame::OnFileSaveAs()
{
	TCHAR szFilePathName[_MAX_PATH] = _T("");
	OPENFILENAME ofn = {0};
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = m_hWnd;
	ofn.lpstrFile = szFilePathName;
	ofn.nMaxFile = _MAX_PATH;
	ofn.lpstrTitle = _T("Open File");

	// Bring up the dialog, and open the file
	::GetOpenFileName(&ofn);
  
  CSimpleMDIChild* pActive = (CSimpleMDIChild*)GetActiveMDIChild();
  if (pActive!=NULL)
  {
    pActive->getDocument()->SaveAs(ofn.lpstrFile);
  }
  else
  {
   // TODO:
  }
}


void CMainMDIFrame::OnFilePrint()
{
	// Bring up a dialog to choose the printer
	PRINTDLG pd = {0};
	pd.lStructSize = sizeof( pd );
	pd.Flags = PD_RETURNDC;

	// Retrieve the printer DC
	PrintDlg( &pd );
	
	// TODO:
	// Add your own code here. Refer to the tutorial for additional information 
}


void CMainMDIFrame::OnInitialUpdate()
{
	TRACE(_T("MDI Frame started \n"));
	//The frame is now created.
	//Place any additional startup code here.
}

BOOL CMainMDIFrame::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case IDM_FILE_NEW:
		AddMDIChild(new CSimpleMDIChild); // CMDIFrame::RemoveMDIChild deletes this pointer
		return TRUE;
	case IDM_FILE_CLOSE:          // Close the active MDI window
		GetActiveMDIChild()->SendMessage(WM_CLOSE, 0, 0);
		return TRUE;
	case IDM_FILE_OPEN:
		OnFileOpen();
		return TRUE;
	case IDM_FILE_SAVE:
		OnFileSave();
    return TRUE;
	case IDM_FILE_SAVEAS:
		OnFileSaveAs();
    return TRUE;
	case IDM_FILE_PRINT:
		OnFilePrint();
		return TRUE;
	case IDW_VIEW_STATUSBAR:
		OnViewStatusbar();

		break;
	case IDW_VIEW_TOOLBAR:
		OnViewToolbar();
		break;
	case IDW_WINDOW_ARRANGE:
		::PostMessage (GetView()->GetHwnd(), WM_MDIICONARRANGE, 0L, 0L) ;
		break;
	case IDW_WINDOW_CASCADE:
		::PostMessage (GetView()->GetHwnd(), WM_MDICASCADE, 0L, 0L) ;
		break;
	case IDW_WINDOW_CLOSEALL:
		RemoveAllMDIChildren();
		break;
	case IDW_WINDOW_TILE:
		::PostMessage (GetView()->GetHwnd(), WM_MDITILE, 0L, 0L) ;
		break;
	default:    // Pass to active child...
		{
			if (GetActiveMDIChild())
				GetActiveMDIChild()->SendMessage(WM_COMMAND, wParam, lParam);
		}
		break ;
	case IDM_FILE_EXIT:
		::PostMessage(m_hWnd, WM_CLOSE, 0, 0);
		return TRUE;
	case IDM_HELP_ABOUT:
		OnHelp();
		return TRUE;
	}
	return FALSE;
}

void CMainMDIFrame::OnCreate()
{
	// OnCreate controls the way the frame is created.
	// Overriding CFrame::Oncreate is optional.
	// The default for the following variables is TRUE

	// m_bShowIndicatorStatus = FALSE;	// Don't show statusbar indicators
	// m_bShowMenuStatus = FALSE;		// Don't show toolbar or menu status
	// m_bUseRebar = FALSE;				// Don't use rebars
	// m_bUseThemes = FALSE;            // Don't use themes
	// m_bUseToolbar = FALSE;			// Don't use a toolbar

	// call the base class function
	CMDIFrame::OnCreate();
}

void CMainMDIFrame::SetupToolbar()
{
	// Define the resource IDs for the toolbar
	AddToolbarButton( IDM_FILE_NEW   );
	AddToolbarButton( IDM_FILE_OPEN  );
	AddToolbarButton( IDM_FILE_SAVE  );
	AddToolbarButton( 0 );				// Separator
	AddToolbarButton( IDM_EDIT_CUT   );
	AddToolbarButton( IDM_EDIT_COPY  );
	AddToolbarButton( IDM_EDIT_PASTE );
	AddToolbarButton( 0 );				// Separator
	AddToolbarButton( IDM_FILE_PRINT );
	AddToolbarButton( 0 );				// Separator
	AddToolbarButton( IDM_HELP_ABOUT );
}

LRESULT CMainMDIFrame::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
//	switch (uMsg)
//	{
//		Add case statements for each messages to be handled here
//	}

//	pass unhandled messages on for default processing
	return WndProcDefault(uMsg, wParam, lParam);
}

