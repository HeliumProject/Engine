#ifndef __PANEL_H__
#define __PANEL_H__

#include <wx/wx.h>
#include <wx/splitter.h>
#include <wx/treectrl.h>
#include <wx/notebook.h>
#include <wx/config.h>

#include "propgrid.h"
#include "graph.h"
#include "nodelib.h"
#include "graph.h"
#include "menustate.h"
#include "code.h"
#include "report.h"
#include "undo.h"
#include "clipboard.h"
#include "stream.h"

class Panel: public wxPanel
{
public:
	struct Layout
	{
		int vsash, hsash1, hsash2;
	};

	Panel(wxWindow *parent, MenuState *menu);
	~Panel();

	void LoadBinary20(LuaInputStream& lis);
	void LoadBinary21(LuaInputStream& lis);

	void   AddToTree(const wxString& name);
	void   LoadLibrary(const wxString& filename);
	void   AddNode(const wxString& type, bool deletable = true);
	Layout GetLayout();
	void   SetLayout(const Layout& layout);
	void   GetMenu();
	void   SetMenu();

	// Getters for the controls.
	PropertyGrid *GetPropertyGridCtrl() const { return m_Props; }
	wxTextCtrl *  GetHelpCtrl() const         { return m_Help; }
	Code *        GetCodeCtrl() const         { return m_Code; }
	wxTreeCtrl *  GetTreeCtrl() const         { return m_Tree; }
	wxNotebook *  GetPropHelpCtrl() const     { return m_Ph; }
	wxNotebook *  GetGraphCodeCtrl() const    { return m_Gc; }
	Graph *       GetGraphCtrl() const        { return m_Graph; }
	Report *      GetReportCtrl() const       { return m_Report; }
	MenuState *   GetMenuStateCtrl() const    { return m_State; }
	Undoer *      GetUndoerCtrl() const       { return m_Undoer; }

	void OnTreeItemActivated(wxTreeEvent& evt);
	void OnTreeSelChanged(wxTreeEvent& evt);
	void OnItemActivated(wxListEvent& evt);
	void OnPropertyChanging(wxPropertyGridEvent& evt);
	void OnPropertyChanged(wxPropertyGridEvent& evt);

private:
	wxString BuildNodeName(wxTreeItemId item);
	wxString BuildNodePath(wxTreeItemId item);
	void AddToGraph(wxTreeItemId item);

	wxSplitterWindow	*m_VSplitter, *m_HSplitter1, *m_HSplitter2;
	PropertyGrid		*m_Props;
	wxTextCtrl			*m_Help;
	Code				*m_Code;
	wxTreeCtrl			*m_Tree;
	wxNotebook			*m_Ph, *m_Gc;
	Graph				*m_Graph;
	Report				*m_Report;
	MenuState			*m_State;
	Undoer				*m_Undoer;

DECLARE_NO_COPY_CLASS(Panel)
DECLARE_EVENT_TABLE()
};

#endif // __PANEL_H__
