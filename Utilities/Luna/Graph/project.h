#ifndef __PROJECT_H__
#define __PROJECT_H__

#include <wx/wx.h>
#include <wx/notebook.h>

#include "luautil.h"
#include "menustate.h"
#include "stream.h"
#include "panel.h"

class Project: public wxNotebook
{
public:
	enum CompileStatus
	{
		CompileSuccess, CompileInvalidPage, CompileError
	};

	static void New(wxWindow *parent, MenuState *state, const wxString& type);
	static void Load(wxWindow *parent, MenuState *state, const wxString& filename);
	static void Destroy();

	static Project *GetProject();
	wxString        GetType() const   { return m_type; }
	wxString        GetTarget() const { return m_target; }
	Panel *         GetPanel() const;
	Panel *         GetPanel(wxWindow *child) const;

	void Configure();

	void Save(const wxString& filename);
	int Compile(int index, const wxString& output);

	static wxString GetProjectType(const wxString& filename);
	static const wxArrayString GetProjectTypes();

	void LoadConfig();
	void SaveConfig();

	void OnPageChanging(wxNotebookEvent& evt);
	void OnPageChanged(wxNotebookEvent& evt);

private:
	Project(wxWindow *parent, const wxString& type);
	~Project();
	void LoadBase64(wxInputStream& is);
	void LoadBinary(LuaInputStream& lis);
	void LoadBinary20(LuaInputStream& lis);
	void LoadBinary21(LuaInputStream& lis);
	void LoadXML(wxInputStream& is);
	wxString m_type;
	wxString m_target;

DECLARE_NO_COPY_CLASS(Project)
DECLARE_EVENT_TABLE()
};

#endif /* __PROJECT_H__ */
