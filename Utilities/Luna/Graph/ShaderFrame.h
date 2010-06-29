#pragma once

#include "Graph/RecentMenu.h"

#define wxID_RECENT			(wxID_HIGHEST + 0)
// wxID_HIGHEST + 1 through + 20 are reserved for entries in the recent menu.
#define wxID_TYPES			(wxID_HIGHEST + 21)
// wxID_HIGHEST + 21 through + 120 are reserved for entries in project types.
#define wxID_VIEWERS		(wxID_HIGHEST + 121)
// wxID_HIGHEST + 121 through + 220 are reserved for entries in the viewers menu.
#define wxID_COPYMETAFILE	(wxID_HIGHEST + 221)
#define wxID_RELOADLIBS		(wxID_HIGHEST + 222)
#define wxID_GROUP			(wxID_HIGHEST + 223)
#define wxID_UNGROUP		(wxID_HIGHEST + 224)
#define wxID_CONFIGURE		(wxID_HIGHEST + 225)
#define wxID_SAVEGROUP		(wxID_HIGHEST + 226)

class Shader: public wxFrame
{
public:
	Shader();
	~Shader();

	// Open the given project.
	void Open(const wxString& file);
	int Compile(int index, const wxString& output);

	void OnClose(wxCloseEvent& evt);
	void OnNew(wxCommandEvent& evt);
	void OnRecent(wxCommandEvent& evt);
	void OnViewer(wxCommandEvent& evt);
	void OnAbout(wxCommandEvent& evt);
	void OnCommand(wxCommandEvent& evt);
	void OnTimer(wxTimerEvent& evt);

private:
	void BuildMenu();
	void AddStatusBar();
	void LoadConfig();
	void SaveConfig();
	void NewProject(const wxString& type);
	void Open();
	int  Save();
	int  SaveAs();
	int  ConfirmSave();
	void EnableMenu(const wxString& label, bool enable = true);
	// The menu bar and submenus.
	wxMenuBar		*m_MenuBar;
	wxMenu			*m_ProjectsMenu;
	RecentMenu		*m_RecentMenu;
	wxMenu			*m_FileMenu;
	wxMenu			*m_EditMenu;
	wxMenu			*m_ToolsMenu;
	wxMenu			*m_ViewersMenu;
	wxMenu			*m_HelpMenu;
	// The status bar.
	wxStatusBar		*m_StatusBar;
	// The current loaded file and path.
	wxString		m_FileName, m_FilePath;
	// The timer to update the app title.
	wxTimer			m_Timer;

	DECLARE_NO_COPY_CLASS(Shader)
	DECLARE_EVENT_TABLE()
};

