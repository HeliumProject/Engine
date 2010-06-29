#ifndef __GRAPH_H__
#define __GRAPH_H__

#include <wx/wx.h>
#include <wx/notebook.h>

#include "canvas.h"
#include "shape.h"
#include "node.h"
#include "propgrid.h"
#include "util.h"
#include "menustate.h"
#include "undo.h"
#include "report.h"
#include "code.h"

#include "debug.h"

class Graph: public Canvas, public Shape
{
public:
	Graph(wxWindow *parent);
	virtual ~Graph() {}

	// Paints the graph.
	virtual void Paint(wxDC& dc);
	// Flags the beginning of an update group.
	virtual void Redraw() { if (IsFrozen()) { m_update = true; } else { Refresh(true); } }
	void BeginUpdate();
	// Flags the ending of an update group.
	void EndUpdate();
	// Generate code and populate the report window.
	bool GenerateCode();

	// Sets the current node type for later insertion.
	void SetCurrentType(const wxString& type = wxT("")) { m_type = type; }
	// Adds a node to the graph.
	void AddNode(Node *node);
	// Selects the shape.
	void ClearSelection();
	void Select(Shape *shape);
	void Center(Shape *shape);
	void Clear();
	bool Compile(const wxString& output);
	// Serializes the selected shapes only.
	wxXmlNode *SerializeSelected() const;

	// True if the graph is cyclic.
	bool IsCyclic();
	// Reposition all nodes so the origin is (0, 0)
	void Reposition();

	void Cut();
	void Copy();
	void Paste();
	void Duplicate();
	void GroupNodes();
	void UngroupNodes();
	void SaveGroup();
	//void CopyAsMetafile();

	void OnLeftDown(wxMouseEvent &evt);
	void OnMotion(wxMouseEvent& evt);
	void OnKeyDown(wxKeyEvent& evt);

	virtual wxString   GetClassName() const          { return wxT("graph"); }
	static Persistent *Create(const wxXmlNode& root) { return (Graph*)NULL; }

	void DoAddNode(Node *node);
	void DoDelNode(Node *node);
	void DoAddEdge(Shape *source, Shape *target);
	void DoDelEdge(Shape *source, Shape *target);

	virtual void Deserialize(const wxXmlNode& root);

private:
	DECLARE_NO_COPY_CLASS(Graph)
	DECLARE_EVENT_TABLE()

	// Get top-left coordinates of all children.
	wxPoint GetTopLeft(List<Shape *> *shapes = NULL) const;
	// Update menu and properties according to graph state.
	void UpdateMenuAndProps();

	// The shape under the mouse cursor.
	Shape *m_under;
	// The previously selected shape.
	Shape *m_last_selected;
	// The set of selected shapes.
	List<Shape *> m_selected;
	// Are we in an update group?
	//int m_updating;
	// The currently selected node type.
	wxString m_type;
	// True to force the graph to be updated.
	bool m_update;
};

#endif // __GRAPH_H__
