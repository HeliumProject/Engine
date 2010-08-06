#include "Precompile.h"
#include "Editor/Graph/Graph.h"

#include <wx/clipbrd.h>
#include <wx/xml/xml.h>
#include <wx/dataobj.h>
#include <wx/sstream.h>
#include <wx/metafile.h>

#include <list>
#include <stack>
#include <set>

#include "Editor/Graph/ShaderFrame.h"
#include "Editor/Graph/Node.h"
#include "Editor/Graph/NodeDefinition.h"
#include "Editor/Graph/Clipboard.h"
#include "Editor/Graph/ProjectNotebook.h"
#include "Editor/Graph/Group.h"
#include "Editor/Graph/TypeDialog.h"
#include "Editor/Graph/Debug.h"

class URAddNode: public UndoRedo
{
public:
	URAddNode(Graph *graph, Node *node): m_graph(graph), m_node(node) {}

	virtual void Undo()    { m_graph->DoDelNode(m_node); }
	virtual void Redo()    { m_graph->DoAddNode(m_node); }

	virtual void Destroy() { DESTROY(m_node); }

private:
	Graph *m_graph;
	Node  *m_node;
};

class URAddEdge: public UndoRedo
{
public:
	URAddEdge(Graph *graph, OutputPort *source, InputPort *target)
		: m_graph(graph), m_source(source), m_target(target) {}

	virtual void Undo() { m_graph->DoDelEdge(m_source, m_target); }
	virtual void Redo() { m_graph->DoAddEdge(m_source, m_target); }

private:
	Graph      *m_graph;
	OutputPort *m_source;
	InputPort  *m_target;
};

class URMoveShapes: public UndoRedo
{
public:
	URMoveShapes(Graph *graph, std::map<Shape *, wxPoint> *newpos): m_graph(graph), m_newpos(newpos)
	{
		for (List<Shape *>::Iterator i = m_graph->ChildIterator(); !i; i++)
		{
			Shape *child = *i;
			m_oldpos.insert(std::pair<Shape *, wxPoint>(child, child->GetPosition()));
		}
	}

	virtual ~URMoveShapes() { DESTROY(m_newpos); }

	virtual void Undo()
	{
		for (std::map<Shape *, wxPoint>::iterator i = m_oldpos.begin(); i != m_oldpos.end(); ++i)
		{
			i->first->SetPosition(i->second);
		}
		m_graph->Reposition();
	}

	virtual void Redo()
	{
		for (std::map<Shape *, wxPoint>::iterator i = m_newpos->begin(); i != m_newpos->end(); ++i)
		{
			i->first->SetPosition(i->second);
		}
		m_graph->Reposition();
	}

private:
	Graph *m_graph;
	std::map<Shape *, wxPoint> *m_newpos, m_oldpos;
};

class URDelEdges: public UndoRedo
{
public:
	URDelEdges(Graph *graph, List<Port *> *ports): m_graph(graph)
	{
		for (List<Port *>::Iterator i = ports->Iterate(); !i; i++)
		{
			if ((*i)->NumSources() > 0)
			{
				List<Shape *>::Iterator i2 = (*i)->SourceIterator();
				if ((*i)->NumSources() > 1)
				{
					i2++;
				}
				m_edges.Add(Edge(*i2, *i));
			}
			for (List<Shape *>::Iterator i2 = (*i)->TargetIterator(); !i2; i2++)
			{
				m_edges.Add(Edge(*i, *i2));
			}
		}
		DESTROY(ports);
	}

	virtual void Undo()
	{
		m_graph->BeginUpdate();
		for (List<Edge>::Iterator i = m_edges.Iterate(); !i; i++)
		{
			m_graph->DoAddEdge((*i).m_source, (*i).m_target);
		}
		m_graph->EndUpdate();
	}

	virtual void Redo()
	{
		m_graph->BeginUpdate();
		for (List<Edge>::Iterator i = m_edges.Iterate(); !i; i++)
		{
			m_graph->DoDelEdge((*i).m_source, (*i).m_target);
		}
		m_graph->EndUpdate();
	}

private:
	struct Edge
	{
		Shape *m_source;
		Shape *m_target;
		Edge(Shape *source, Shape *target): m_source(source), m_target(target) {}
		bool operator< (const Edge& other) const
		{
			return (m_source < other.m_source) || (m_source == other.m_source && m_target < other.m_target);
		}
	};
	Graph *m_graph;
	List<Edge> m_edges;
};

class URDelNodes: public UndoRedo
{
public:
	URDelNodes(Graph *graph, List<Shape *> *nodes): m_graph(graph), m_nodes(nodes)
	{
		for (List<Shape *>::Iterator i = m_nodes->Iterate(); !i; i++)
		{
			Node *node = (Node *)*i;
			for (List<InputPort *>::Iterator i2 = node->InputIterator(); !i2; i2++)
			{
				if ((*i2)->NumSources() > 0)
				{
					List<Shape *>::Iterator i3 = (*i2)->SourceIterator();
					if ((*i2)->NumSources() > 1)
					{
						i3++;
					}
					m_edges.Add(Edge(*i3, *i2));
				}
			}
			for (List<OutputPort *>::Iterator i2 = node->OutputIterator(); !i2; i2++)
			{
				for (List<Shape *>::Iterator i3 = (*i2)->TargetIterator(); !i3; i3++)
				{
					m_edges.Add(Edge(*i2, *i3));
				}
			}
		}
	}

	virtual ~URDelNodes() { DESTROY(m_nodes); }

	virtual void Undo()
	{
		m_graph->BeginUpdate();
		for (List<Shape *>::Iterator i = m_nodes->Iterate(); !i; i++)
		{
			m_graph->DoAddNode((Node *)*i);
		}
		for (List<Edge>::Iterator i = m_edges.Iterate(); !i; i++)
		{
			m_graph->DoAddEdge((*i).m_source, (*i).m_target);
		}
		m_graph->EndUpdate();
	}

	virtual void Redo()
	{
		m_graph->BeginUpdate();
		for (List<Edge>::Iterator i = m_edges.Iterate(); !i; i++)
		{
			m_graph->DoDelEdge((*i).m_source, (*i).m_target);
		}
		for (List<Shape *>::Iterator i = m_nodes->Iterate(); !i; i++)
		{
			m_graph->DoDelNode((Node *)*i);
		}
		m_graph->EndUpdate();
	}

private:
	struct Edge
	{
		Shape *m_source;
		Shape *m_target;
		Edge(Shape *source, Shape *target): m_source(source), m_target(target) {}
		bool operator< (const Edge& other) const
		{
			return (m_source < other.m_source) || (m_source == other.m_source && m_target < other.m_target);
		}
	};
	Graph *m_graph;
	List<Shape *>* m_nodes;
	List<Edge> m_edges;
};

class URPaste: public UndoRedo
{
public:
	URPaste(Graph *graph, List<Shape *> *nodes): m_graph(graph), m_nodes(nodes) {}

	virtual ~URPaste() { DESTROY(m_nodes); }

	virtual void Undo()
	{
		m_graph->BeginUpdate();
		m_graph->ClearSelection();
		for (List<Shape *>::Iterator i = m_nodes->Iterate(); !i; i++)
		{
			m_graph->DoDelNode((Node *)*i);
		}
		m_graph->EndUpdate();
	}

	virtual void Redo()
	{
		m_graph->BeginUpdate();
		m_graph->ClearSelection();
		for (List<Shape *>::Iterator i = m_nodes->Iterate(); !i; i++)
		{
			m_graph->DoAddNode((Node *)*i);
			m_graph->Select(*i);
		}
		m_graph->EndUpdate();
	}

private:
	Graph *m_graph;
	List<Shape *>* m_nodes;
};

class URGroupNodes: public UndoRedo
{
public:
	URGroupNodes(Graph *graph, List<Shape *> *shapes): m_graph(graph), m_shapes(shapes)
	{
	}

	~URGroupNodes() { DESTROY(m_shapes); }

	virtual void Undo()
	{
		wxPoint p0 = m_group->GetPosition();
		m_graph->BeginUpdate();
		m_graph->ClearSelection();
		m_graph->DeleteChild(m_group);
		for (List<Shape *>::Iterator i = m_shapes->Iterate(); !i; i++)
		{
			m_group->DeleteChild(*i);
			m_graph->Shape::AddChild(*i);
			static_cast<Node *>(*i)->EvalSize();
			m_graph->Select(*i);
			wxPoint pos = (*i)->GetPosition();
			(*i)->SetPosition(pos.x - p0.x, pos.y - p0.y);
		}
		DESTROY(m_group);
		m_graph->EndUpdate();
	}

	virtual void Redo()
	{
		m_graph->BeginUpdate();
		for (List<Shape *>::Iterator i = m_shapes->Iterate(); !i; i++)
		{
			m_graph->DeleteChild(*i);
		}
		m_group = NEW(Group, (m_shapes));
		m_graph->Shape::AddChild(m_group);
		m_graph->ClearSelection();
		m_graph->Select(m_group);
		m_graph->EndUpdate();
	}

	virtual void Destroy() { DESTROY(m_group); }

private:
	Graph *m_graph;
	List<Shape *> *m_shapes;
	Group *m_group;
};

class URUngroupNodes: public UndoRedo
{
public:
	URUngroupNodes(Graph *graph, Group *group): m_graph(graph), m_group(group)
	{
		m_shapes = NEW(List<Shape *>, ());
		for (List<Shape *>::Iterator i = m_group->ChildIterator(); !i; i++)
		{
			m_shapes->Add(*i);
		}
	}

	~URUngroupNodes() { DESTROY(m_shapes); }

	virtual void Undo()
	{
		m_graph->BeginUpdate();
		for (List<Shape *>::Iterator i = m_shapes->Iterate(); !i; i++)
		{
			m_graph->DeleteChild(*i);
		}
		m_group = NEW(Group, (m_shapes));
		m_graph->Shape::AddChild(m_group);
		m_graph->ClearSelection();
		m_graph->Select(m_group);
		m_graph->EndUpdate();
	}

	virtual void Redo()
	{
		wxPoint p0 = m_group->GetPosition();
		m_graph->BeginUpdate();
		m_graph->ClearSelection();
		m_graph->DeleteChild(m_group);
		for (List<Shape *>::Iterator i = m_shapes->Iterate(); !i; i++)
		{
			m_group->DeleteChild(*i);
			m_graph->Shape::AddChild(*i);
			static_cast<Node *>(*i)->EvalSize();
			m_graph->Select(*i);
			wxPoint pos = (*i)->GetPosition();
			(*i)->SetPosition(pos.x - p0.x, pos.y - p0.y);
		}
		DESTROY(m_group);
		m_graph->EndUpdate();
	}

	virtual void Destroy() { DESTROY(m_group); }

private:
	Graph *m_graph;
	List<Shape *> *m_shapes;
	Group *m_group;
};

BEGIN_EVENT_TABLE(Graph, Canvas)
	EVT_LEFT_DOWN(Graph::OnLeftDown)
	EVT_MOTION(Graph::OnMotion)
//	EVT_LEAVE_WINDOW(Graph::OnLeave)
	EVT_KEY_DOWN(Graph::OnKeyDown)
//	EVT_KEY_UP(Graph::OnKeyUp)
//	EVT_RIGHT_DOWN(Graph::OnRightDown)
END_EVENT_TABLE()

Graph::Graph(wxWindow *parent)
	: Shape()
	, Canvas(parent)
	, m_last_selected(NULL)
	, m_type(wxT(""))
	, m_update(false)
	, m_under(NULL)
{
}

void
Graph::Paint(wxDC& dc)
{
	DrawConnections(dc);
	Draw(dc);
	for (List<Shape *>::Iterator i = m_selected.Iterate(); !i; i++)
	{
		wxRect border = (*i)->GetBBox();
		border.Inflate(5, 5);
		dc.SetPen(wxPen(wxColor(0, 0, 255), 3));
		dc.SetBrush(*wxTRANSPARENT_BRUSH);
		dc.DrawRoundedRectangle(border, 5);
	}
	if (m_under != NULL)
	{
		wxRect border = m_under->GetBBox();
		border.Inflate(5, 5);
		dc.SetPen(*wxCYAN_PEN);
		dc.SetBrush(*wxTRANSPARENT_BRUSH);
		dc.DrawRoundedRectangle(border, 5);
	}
}

void
Graph::BeginUpdate()
{
	if (!IsFrozen())
	{
		m_update = false;
	}
	Freeze();
}

void
Graph::EndUpdate()
{
	Thaw();
	if (!IsFrozen() && m_update)
	{
		GenerateCode();
		Canvas::Redraw(true);
	}
}

static void
Visit(Node *n, std::list<Node *> *L, std::set<Node *> *visited)
{
	// if n has not been visited yet then
	std::set<Node *>::iterator i = visited->find(n);
	if (i == visited->end())
	{
		visited->insert(n);
		// for each node m with an edge from n to m do
		for (List<InputPort *>::Iterator i = n->InputIterator(); !i; i++)
		{
			if ((*i)->NumSources() > 0)
			{
				List<Shape *>::Iterator i2 = (*i)->SourceIterator();
				if ((*i)->NumSources() > 1)
				{
					i2++;
				}
				Node *m = dynamic_cast<Node *>((*i2)->GetParent());
				if (m != NULL)
				{
					Visit(m, L, visited);
				}
			}
		}
		// add n to L
		L->push_back(n);
	}
}

bool
Graph::GenerateCode()
{
	// L <- Empty list that will contain the sorted nodes
	std::list<Node *> L;
	// S <- Set of all nodes
	std::set<Node *> S, visited;
	std::stack<Shape *> stack;
	stack.push(this);
	while (stack.size() != 0)
	{
		Shape *shape = stack.top();
		stack.pop();
		Node *node = dynamic_cast<Node *>(shape);
		if (node != NULL)
		{
			S.insert(node);
		}
		for (List<Shape *>::Iterator i = shape->ChildIterator(); !i; i++)
		{
			stack.push(*i);
		}
	}
	// for each node n in S do
	for (std::set<Node *>::iterator i = S.begin(); i != S.end(); ++i)
	{
		// visit(n)
		Visit(*i, &L, &visited);
	}
	// Generate code
	Panel* panel = Project::GetProject()->GetPanel(this);
	CodeTextCtrl *codectrl = panel->GetCodeCtrl();
	Report *reportctrl = panel->GetReportCtrl();
	codectrl->Freeze();
	reportctrl->Freeze();
	codectrl->Clear();
	reportctrl->Clear();
	for (std::list<Node *>::iterator i = L.begin(); i != L.end(); ++i)
	{
		(*i)->GenerateCode(codectrl, reportctrl);
	}
	if ((reportctrl->NumErrors() + reportctrl->NumWarnings() + reportctrl->NumInfos()) == 0)
	{
		reportctrl->Info(wxT("Everything ok!"));
	}
	reportctrl->Refresh(false);
	codectrl->Refresh(false);
	reportctrl->Thaw();
	codectrl->Thaw();
	return reportctrl->NumErrors() == 0;
}

bool
Graph::IsCyclic()
{
	std::set<Node *> nodes;
	std::stack<Shape *> stack;
	stack.push(this);
	while (stack.size() != 0)
	{
		Shape *shape = stack.top();
		stack.pop();
		if (dynamic_cast<Node *>(shape) != NULL && dynamic_cast<Group *>(shape) == NULL)
		{
			nodes.insert((Node *)shape);
		}
		for (List<Shape *>::Iterator i = shape->ChildIterator(); !i; i++)
		{
			stack.push(*i);
		}
	}
	std::map<Node *, size_t> degree;
	for (std::set<Node *>::iterator i = nodes.begin(); i != nodes.end(); ++i)
	{
		size_t d = 0;
		for (List<InputPort *>::Iterator i2 = (*i)->InputIterator(); !i2; i2++)
		{
			d += (*i2)->NumSources();
		}
		degree.insert(std::pair<Node *, size_t>(*i, d));
	}
	std::list<Node *> queue;
	for (std::map<Node *, size_t>::iterator i = degree.begin(); i != degree.end(); ++i)
	{
		if (i->second == 0)
		{
			queue.push_back(i->first);
		}
	}
	size_t count = 0;
	while (queue.size() != 0)
	{
		Node *node = queue.front();
		queue.pop_front();
		count++;
		for (List<OutputPort *>::Iterator i = node->OutputIterator(); !i; i++)
		{
			for (List<Shape *>::Iterator i2 = (*i)->TargetIterator(); !i2; i2++)
			{
				Node *other = (Node *)(*i2)->GetParent();
				std::map<Node *, size_t>::iterator i3 = degree.find(other);
				size_t d = i3->second;
				if (d != 0)
				{
					d = d - 1;
					if (d == 0)
					{
						queue.push_back(other);
					}
					i3->second = d;
				}
			}
		}
	}
	return count != nodes.size();
}

void
Graph::AddNode(Node *node)
{
	URAddNode *ur = NEW(URAddNode, (this, node));
	Project::GetProject()->GetPanel()->GetUndoerCtrl()->Add(ur);
}

void
Graph::ClearSelection()
{
	BeginUpdate();
	m_selected.Clear();
	UpdateMenuAndProps();
	EndUpdate();
}

void
Graph::Select(Shape *shape)
{
	BeginUpdate();
	m_selected.Add(shape);
	UpdateMenuAndProps();
	EndUpdate();
}

void
Graph::Center(Shape *shape)
{
	wxRect bbox = shape->GetBBox();
	Canvas::Center((bbox.GetLeft() + bbox.GetRight()) / 2, (bbox.GetTop() + bbox.GetBottom()) / 2);
}

void
Graph::Clear()
{
	BeginUpdate();
	ClearSelection();
	for (List<Shape *>::Iterator i = Shape::m_children.Iterate(); !i; i++)
	{
		DESTROY(*i);
	}
	Shape::m_children.Clear();
	EndUpdate();
}

bool
Graph::Compile(const wxString& output)
{
	if (GenerateCode())
	{
		wxString code = Project::GetProject()->GetPanel(this)->GetCodeCtrl()->GetValue();
		FILE *file = _tfopen(output.c_str(), TXT("wb" ));
		fwrite(code.c_str(), 1, code.Len(), file);
		fclose(file);
		return true;
	}
	//_unlink(output.c_str());
	return false;
}

wxXmlNode *
Graph::SerializeSelected() const
{
	Clipboard clip;
	for (List<Shape *>::Iterator i = m_selected.Iterate(); !i; i++)
	{
		clip.AddChild(*i);
	}
	return clip.Serialize();
}

void
Graph::Reposition()
{
	BeginUpdate();
	wxPoint tl = GetTopLeft();
	for (List<Shape *>::Iterator i = Shape::m_children.Iterate(); !i; i++)
	{
		Shape *shape = *i;
		wxPoint pos = shape->GetPosition();
		shape->SetPosition(pos.x - tl.x, pos.y - tl.y);
	}
	EndUpdate();
}

void
Graph::Cut()
{
	// Copy the shapes to the clipboard.
	Copy();
	// Delete the selected shapes.
	List<Shape *> *nodes = NEW(List<Shape *>, ());
	for (List<Shape *>::Iterator i = m_selected.Iterate(); !i; i++)
	{
		if (dynamic_cast<Node *>(*i) != NULL)
		{
			nodes->Add(*i);
		}
	}
	if (nodes->Size() != 0)
	{
		Project::GetProject()->GetPanel()->GetUndoerCtrl()->Add(NEW(URDelNodes, (this, nodes)));
	}
}

void
Graph::Copy()
{
	// Generate XML.
	wxXmlDocument doc;
	doc.SetRoot(SerializeSelected());
	wxStringOutputStream sos;
	doc.Save(sos);
	wxString xml = sos.GetString();
	// Create a text object and copy it to the clipboard.
	wxTextDataObject *tdo = NEW(wxTextDataObject, (xml));
	if (wxTheClipboard->Open())
	{
		wxTheClipboard->SetData(tdo);
		wxTheClipboard->Close();
	}
}

void
Graph::Paste()
{
	BeginUpdate();
	if (wxTheClipboard->Open())
	{
		if (wxTheClipboard->IsSupported(wxDataFormat(wxDF_TEXT)))
		{
			wxTextDataObject tdo;
			wxTheClipboard->GetData(tdo);
			wxString xml(tdo.GetText());
			wxStringInputStream sis(xml);
			wxXmlDocument doc; // TODO Check for errors.
			if (doc.Load(sis))
			{
				wxXmlNode *root = doc.GetRoot();
				Clipboard *clip = (Clipboard *)Serialized::DeserializeObject(*root);
				List<Shape *> *nodes = NEW(List<Shape *>, ());
				for (List<Shape *>::Iterator i = clip->ChildIterator(); !i; i++)
				{
					Shape *shape = *i;
					wxPoint pos = shape->GetPosition();
					pos.x += 16;
					pos.y += 16;
					shape->SetPosition(pos);
					nodes->Add(shape);
				}
				DESTROY(clip);
				URPaste *ur = NEW(URPaste, (this, nodes));
				Project::GetProject()->GetPanel()->GetUndoerCtrl()->Add(ur);
			}
		}
		wxTheClipboard->Close();
	}
	EndUpdate();
}

void
Graph::Duplicate()
{
	wxXmlNode *root = SerializeSelected();
	Clipboard *clip = (Clipboard *)Serialized::DeserializeObject(*root);
	List<Shape *> *nodes = NEW(List<Shape *>, ());
	for (List<Shape *>::Iterator i = clip->ChildIterator(); !i; i++)
	{
		Shape *shape = *i;
		wxPoint pos = shape->GetPosition();
		pos.x += 16;
		pos.y += 16;
		shape->SetPosition(pos);
		nodes->Add(shape);
	}
	DESTROY(clip);
	DESTROY(root);
	URPaste *ur = NEW(URPaste, (this, nodes));
	Project::GetProject()->GetPanel()->GetUndoerCtrl()->Add(ur);
}

void
Graph::GroupNodes()
{
	List<Shape *> *shapes = NEW(List<Shape *>, ());
	for (List<Shape *>::Iterator i = m_selected.Iterate(); !i; i++)
	{
		shapes->Add(*i);
	}
	m_selected.Clear();
	URGroupNodes *ur = NEW(URGroupNodes, (this, shapes));
	Project::GetProject()->GetPanel()->GetUndoerCtrl()->Add(ur);
}

void
Graph::UngroupNodes()
{
	URUngroupNodes *ur = NEW(URUngroupNodes, (this, dynamic_cast<Group *>(*m_selected.Iterate())));
	Project::GetProject()->GetPanel()->GetUndoerCtrl()->Add(ur);
}

void
Graph::SaveGroup()
{
	Group *group = static_cast<Group *>(*m_selected.Iterate());
	wxString type = group->GetMember(wxT("Name"))->GetString();
	if (type.Len() == 0)
	{
		wxMessageBox(wxT("Please name your new node in its property sheet. The name will be used as the node's type under the \"User Defined\" folder."), wxT("Error"), wxOK | wxICON_EXCLAMATION, Project::GetProject()->GetPanel());
	}
	else
	{
		wxXmlNode *root = group->Serialize();
		root->AddProperty(wxT("type"), type);
		if (NodeLibrary::SaveGroup(root))
		{
			wxMessageBox(wxT("Node saved."), wxT("Info"), wxOK | wxICON_INFORMATION, Project::GetProject()->GetPanel());
		}
		else
		{
			wxMessageBox(wxT("There's already a node with this type."), wxT("Error"), wxOK | wxICON_EXCLAMATION, Project::GetProject()->GetPanel());
		}
	}
}

/*void
Graph::CopyAsMetafile()
{
	//m_Graph->CopyAsMetafile();
#ifdef _WIN32
	wxMetafileDC mdc;
	Paint(mdc);
	wxMetafile *meta = mdc.Close();
	meta->SetClipboard();
	DESTROY(meta);
#endif
}*/

void
Graph::OnLeftDown(wxMouseEvent& evt)
{
	BeginUpdate();
	do
	{
		// Convert the screen coordinates to canvas coordinates.
		int x, y;
		MouseToCanvas(evt.GetX(), evt.GetY(), &x, &y);
		// Get the shape under the cursor.
		Shape *shape = IsInside(x, y);
		if (evt.ShiftDown()) // Add/remove shape from the selected list or connect ports.
		{
			if (shape != NULL) // There's a shape under the cursor!
			{
				if (m_selected.Size() == 1) // There's just one shape selected.
				{
					// Check if we have the right combination of ports.
					Shape *selected = *m_selected.Iterate();
					OutputPort *source = dynamic_cast<OutputPort *>(selected);
					InputPort *target = dynamic_cast<InputPort *>(shape);
					if (source == NULL || target == NULL)
					{
						source = dynamic_cast<OutputPort *>(shape);
						target = dynamic_cast<InputPort *>(selected);
					}
					if (source != NULL && target != NULL)
					{
						// We do, check if the edge will make the graph cyclic.
						DoAddEdge(source, target);
						bool is_cyclic = IsCyclic();
						DoDelEdge(source, target);
						if (is_cyclic)
						{
							// Yes, tell the user.
							wxMessageBox(wxT("This edge would make the graph cyclic!"), wxT("Error"), wxOK, this);
						}
						else
						{
							// No, add the edge.
							Project::GetProject()->GetPanel()->GetUndoerCtrl()->Add(NEW(URAddEdge, (this, source, target)));
						}
						break;
					}
				}
				if (m_selected.Contains(shape))
				{
					// Shape was selected, de-select it.
					m_selected.Delete(shape);
				}
				else
				{
					// Shape wasn't selected, select it.
					m_selected.Add(shape);
				}
				break;
			}
			// There isn't a shape under the cursor, move selected shapes.
			// Find top-left corner of the selection.
			wxPoint tl = GetTopLeft(&m_selected);
			// Map to hold new shapes positions.
			std::map<Shape *, wxPoint> *newpos = new std::map<Shape *, wxPoint>();
			//Debug::DumpHeap("new", newpos, sizeof(std::map<Shape *, wxPoint>), __FILE__, __LINE__ - 1);
			// Compute new positions.
			int dx = x - tl.x;
			int dy = y - tl.y;
			for (List<Shape *>::Iterator i = m_selected.Iterate(); !i; i++)
			{
				if (dynamic_cast<Port *>(*i) == NULL)
				{
					Shape *shape = *i;
					wxPoint pos = shape->GetPosition();
					newpos->insert(std::pair<Shape *, wxPoint>(shape, wxPoint(pos.x + dx, pos.y + dy)));
				}
			}
			// Do the move.
			Project::GetProject()->GetPanel()->GetUndoerCtrl()->Add(NEW(URMoveShapes, (this, newpos)));
			break;
		}
		if (evt.AltDown() && m_type != wxT(""))
		{
			Node *node = NodeLibrary::Create(m_type);
			node->SetPosition(x, y);
			AddNode(node);
			break;
		}
		// Select only the shape under the cursor.
		// De-select all selected shapes.
		m_selected.Clear();
		if (shape != NULL)
		{
			// Select the shape if there's one.
			m_selected.Add(shape);
			break;
		}
	} while (false);
	EndUpdate();
	UpdateMenuAndProps();
	evt.Skip();
}

void
Graph::OnMotion(wxMouseEvent& evt)
{
	int x, y;
	MouseToCanvas(evt.GetX(), evt.GetY(), &x, &y);
	Debug::Printf( TXT("%s %d, %d -> %d, %d\n"), __FUNCTION__, evt.GetX(), evt.GetY(), x, y);
	Shape *under = IsInside(x, y);
	if (under != m_under)
	{
		BeginUpdate();
		m_under = under;
		EndUpdate();
	}
	evt.Skip();
}

void
Graph::OnKeyDown(wxKeyEvent& evt)
{
	if (evt.GetKeyCode() == WXK_DELETE)
	{
		List<Shape *> *nodes = NEW(List<Shape *>, ());
		for (List<Shape *>::Iterator i = m_selected.Iterate(); !i; i++)
		{
			if (dynamic_cast<Node *>(*i) != NULL)
			{
				nodes->Add(*i);
			}
		}
		if (nodes->Size() != 0)
		{
			Project::GetProject()->GetPanel()->GetUndoerCtrl()->Add(NEW(URDelNodes, (this, nodes)));
		}
		else
		{
			DESTROY(nodes);
			List<Port *> *ports = NEW(List<Port *>, ());
			for (List<Shape *>::Iterator i = m_selected.Iterate(); !i; i++)
			{
				Port *port = dynamic_cast<Port *>(*i);
				if (port != NULL)
				{
					ports->Add(port);
				}
			}
			if (ports->Size() != 0)
			{
				Project::GetProject()->GetPanel()->GetUndoerCtrl()->Add(NEW(URDelEdges, (this, ports)));
			}
			else
			{
				DESTROY(ports);
			}
		}
	}
	evt.Skip();
}

void
Graph::DoAddNode(Node *node)
{
	BeginUpdate();
	Shape::AddChild(node);
	Reposition();
	Redraw();
	EndUpdate();
}

void
Graph::DoDelNode(Node *node)
{
	BeginUpdate();
	if (m_selected.Contains(node))
	{
		m_selected.Delete(node);
	}
	DeleteChild(node);
	wxPoint tl = GetTopLeft();
	wxPoint pos = node->GetPosition();
	node->SetPosition(pos.x - tl.x, pos.y - tl.y);
	Reposition();
	Redraw();
	EndUpdate();
}

void
Graph::DoAddEdge(Shape *source, Shape *target)
{
	BeginUpdate();
	if (target->NumSources() < 2)
	{
		source->AddTarget(target);
		target->AddSource(source);
		Redraw();
	}
	EndUpdate();
}

void
Graph::DoDelEdge(Shape *source, Shape *target)
{
	BeginUpdate();
	source->DeleteTarget(target);
	target->DeleteSource(source);
	Redraw();
	EndUpdate();
}

void
Graph::Deserialize(const wxXmlNode& root)
{
	Shape::Deserialize(root);
	Reposition();
}

wxPoint
Graph::GetTopLeft(List<Shape *> *shapes) const
{
	int min_x = 0, min_y = 0;
	List<Shape *>::Iterator i = shapes == NULL ? ChildIterator() : shapes->Iterate();
	if (!i)
	{
		wxPoint pos = (*i)->GetPosition();
		min_x = pos.x, min_y = pos.y;
		for (i++; !i; i++)
		{
			wxPoint pos = (*i)->GetPosition();
			if (pos.x < min_x)
			{
				min_x = pos.x;
			}
			if (pos.y < min_y)
			{
				min_y = pos.y;
			}
		}
	}
	return wxPoint(min_x, min_y);
}

void
Graph::UpdateMenuAndProps()
{
	// Show the properties of the only selected shape.
	PropertyGrid *props = Project::GetProject()->GetPanel()->GetPropertyGridCtrl();
	props->Freeze();
	props->Clear();
	if (m_selected.Size() == 1)
	{
		Shape *shape = *m_selected.Iterate();
		shape->ShowProperties(props);
		Project::GetProject()->GetPanel()->GetPropHelpCtrl()->SetSelection(0);
		m_last_selected = shape;
		Project::GetProject()->GetPanel()->GetHelpCtrl()->SetValue(shape->GetMember(wxT("description"))->GetString());
	}
	props->Refresh(false);
	props->Thaw();
	int selnodes = 0;
	for (List<Shape *>::Iterator i = m_selected.Iterate(); !i; i++)
	{
		if (dynamic_cast<Node *>(*i) != NULL)
		{
			selnodes++;
		}
	}
	MenuState *menustate = Project::GetProject()->GetPanel()->GetMenuStateCtrl();
	menustate->Enable(wxID_COPY, selnodes > 0);
	menustate->Enable(wxID_CUT, selnodes > 0);
	menustate->Enable(wxID_DUPLICATE, selnodes > 0);
	menustate->Enable(wxID_GROUP, selnodes == m_selected.Size());
	bool one_group = selnodes == 1 && dynamic_cast<Group *>(*m_selected.Iterate()) != NULL;
	menustate->Enable(wxID_UNGROUP, one_group);
	menustate->Enable(wxID_SAVEGROUP, one_group);
}
