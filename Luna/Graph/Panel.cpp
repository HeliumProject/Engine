#include "Precompile.h"
#include "Graph/Panel.h"

#include <wx/tokenzr.h>
#include <wx/propgrid/advprops.h>
#include <wx/xml/xml.h>

#include "Graph/LuaUtilities.h"
#include "Graph/CodeTextCtrl.h"
#include "Graph/Debug.h"

wxString g_FragmentShaderLibPath;
lua_State *g_FragmentShaderLuaState;

class wxTreeItemPath: public wxTreeItemData
{
	public:
		wxTreeItemPath(wxString& path)
			: wxTreeItemData()
			, path(path)
		{
		}

		wxString path;
};

BEGIN_EVENT_TABLE(Panel, wxPanel)
	EVT_TREE_ITEM_ACTIVATED(wxID_ANY, Panel::OnTreeItemActivated)
//	EVT_TREE_KEY_DOWN(wxID_ANY, Panel::OnTreeKeyDown)
	EVT_TREE_SEL_CHANGED(wxID_ANY, Panel::OnTreeSelChanged)
//	EVT_TREE_ITEM_RIGHT_CLICK(wxID_ANY, Panel::OnTreeRightClick)
	EVT_LIST_ITEM_ACTIVATED(wxID_ANY, Panel::OnItemActivated)
	EVT_PG_CHANGING(wxID_ANY, Panel::OnPropertyChanging)
	EVT_PG_CHANGED(wxID_ANY, Panel::OnPropertyChanged)
END_EVENT_TABLE()

Panel::Panel(wxWindow *parent, MenuState *state)
	: wxPanel(parent, wxID_ANY)
	, m_State(state)
{
	m_Undoer = NEW(Undoer, ());
	// The splitters for the panels.
	m_VSplitter = NEW(wxSplitterWindow, (this, wxID_ANY));
	m_HSplitter1 = NEW(wxSplitterWindow, (m_VSplitter, wxID_ANY));
	m_HSplitter2 = NEW(wxSplitterWindow, (m_HSplitter1, wxID_ANY));
	// The tree control that holds the nodes. It stays at the top of hsplitter1 (top-left of the frame.)
	m_Tree = NEW(wxTreeCtrl, (m_HSplitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS | wxTR_HIDE_ROOT | wxTR_LINES_AT_ROOT));
	// The error report. It stays in between hsplitter1 and hsplitter2 (middle-left of the frame.)
	m_Report = NEW(Report, (m_HSplitter2, wxID_ANY));
	// The properties/help notebook. It stays at the bottom of hsplitter2 (bottom-left of the frame.)
	m_Ph = NEW(wxNotebook, (m_HSplitter2, wxID_ANY));
	// Add the property grid to it.
	m_Props = NEW(PropertyGrid, (m_Ph, m_Undoer));
	m_Ph->AddPage(m_Props, wxT("Properties"), true);
	// Add the help area to it.
	m_Help = NEW(wxTextCtrl, (m_Ph, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH));
	m_Ph->AddPage(m_Help, wxT("Help"));
	// The graph/code notebook. It stays at the right of vsplitter (right side of the frame.)
	m_Gc = NEW(wxNotebook, (m_VSplitter, wxID_ANY));
	// Instantiate the code window.
	m_Code = NEW(CodeTextCtrl, (m_Gc, wxID_ANY));
	// Add the graph to it.
	m_Graph = NEW(Graph, (m_Gc));
	m_Gc->AddPage(m_Graph, wxT("Graph"), true);
	// Add the code control to it.
	m_Gc->AddPage(m_Code, wxT("Code"));
	// Split everything.
	m_HSplitter2->SplitHorizontally(m_Report, m_Ph);
	m_HSplitter1->SplitHorizontally(m_Tree, m_HSplitter2);
	m_VSplitter->SplitVertically(m_HSplitter1, m_Gc);
	m_VSplitter->SetMinimumPaneSize(20);
	m_HSplitter1->SetMinimumPaneSize(20);
	m_HSplitter2->SetMinimumPaneSize(20);
	// The sizer.
	wxBoxSizer *sizer = NEW(wxBoxSizer, (wxHORIZONTAL));
	sizer->Add(m_VSplitter, 1, wxEXPAND, 0);
	SetSizer(sizer);
	sizer->SetSizeHints(this);
}

Panel::~Panel()
{
	DESTROY(m_Undoer);
	DESTROY(m_State);
}

void Panel::LoadBinary20(LuaInputStream& lis)
{
	m_Graph->BeginUpdate();
	std::vector<std::pair<Node *, std::vector<wxString>>> nodes;
	int node_count = lis.ReadInteger();
	for (int i = 0; i < node_count; i++)
	{
		wxString node_type = lis.ReadString();
		Debug::Printf(TXT("Loading node %s (%d)\n"), node_type, i);
		Node *node = NodeLibrary::Create(node_type);
		if (node == NULL)
		{
			THROW(TXT("Unknown node %s\n"), node_type.c_str());
		}
		std::vector<wxString> ports;
		int member_count = lis.ReadInteger();
		Debug::Printf(TXT("  Node has %d members\n"), member_count);
		for (int i = 0; i < member_count; i++)
		{
			wxVariant key = lis.Read();
			wxVariant value = lis.Read();
			if (key.GetType() == wxT("string"))
			{
				wxString name = key.GetString();
				Debug::Printf(TXT("  Reading member %s\n"), name.c_str());
				if (name == wxT("cost"))
				{
					//int cost = value.GetType() == wxT("long") ? (int)value.GetLong() : (int)value.GetDouble();
					//Property *prop = NEW(IntProperty, (node, wxT("Cost")));
					//prop->SetValue(cost);
					//node->AddProperty(prop);
				}
				else if (name == wxT("x0"))
				{
					wxPoint pos = node->GetPosition();
					pos.y = (int)(1.5 * (value.GetType() == wxT("long") ? (double)value.GetLong() : value.GetDouble()));
					node->SetPosition(pos);
				}
				else if (name == wxT("y0"))
				{
					wxPoint pos = node->GetPosition();
					pos.x = (int)(3.0 * (value.GetType() == wxT("long") ? (double)value.GetLong() : value.GetDouble()));
					node->SetPosition(pos);
				}
				else if (name == wxT("fill"))
				{
					int color = value.GetType() == wxT("long") ? (int)value.GetLong() : (int)value.GetDouble();
					node->GetMember(wxT("Fill_color"))->SetValue(wxColor(color >> 16, color >> 8, color));
				}
				else if (name == wxT("deletable"))
				{
					node->GetMember(wxT("deletable"))->SetValue(value.GetBool());
				}
				else if (name == wxT("name"))
				{
					node->GetMember(wxT("Name"))->SetValue(value.GetString());
				}
				else if (name == wxT("comment"))
				{
					node->GetMember(wxT("Comment"))->SetValue(value.GetString());
				}
				else if (name.Right(4) == wxT("_ndx"))
				{
					node->GetMember(name)->SetValue(value.GetLong() - 1);
				}
				else if (name == wxT("x1") || name == wxT("y1") || name == wxT("color"))
				{
					// Ignore.
				}
				else
				{
					Member *member = node->FindMember(name);
					if (member != NULL)
					{
						member->FromString(value.GetString());
					}
					else
					{
						Debug::Printf(TXT("Member %s:%s not handled in binary file.\n"), node_type.c_str(), name.c_str());
					}
				}
			}
		}

    if (node_type == wxT("Cg/Arithmetic Operators/Two Operand"))
    {
      wxString name = node->GetMember(wxT("Name"))->GetString();
      if (name == wxT("Add"))
      {
        node->GetMember(wxT("operator_ndx"))->SetValue(0);
      }
      else if (name == wxT("Subtract"))
      {
        node->GetMember(wxT("operator_ndx"))->SetValue(1);
      }
      else if (name == wxT("Multiply"))
      {
        node->GetMember(wxT("operator_ndx"))->SetValue(2);
      }
      else if (name == wxT("Divide"))
      {
        node->GetMember(wxT("operator_ndx"))->SetValue(3);
      }
    }

		int input_count = lis.ReadInteger();
		Debug::Printf(TXT("  Node has %d input ports\n"), input_count);
    List<InputPort *>::Iterator inputs = node->InputIterator();
		for (int i = 0; i < input_count; i++)
		{
			wxString port_name = lis.ReadString();
			Debug::Printf(TXT("    Read input port %s (%d)\n"), port_name.c_str(), i);
			ports.push_back(port_name);
      if (!inputs)
      {
        (*inputs)->GetMember(wxT("Name"))->SetValue(port_name);
        inputs++;
      }
		}
		int output_count = lis.ReadInteger();
		Debug::Printf(TXT("  Node has %d output ports\n"), output_count);
    List<OutputPort *>::Iterator outputs = node->OutputIterator();
		for (int i = 0; i < output_count; i++)
		{
			wxString port_name = lis.ReadString();
			Debug::Printf(TXT("    Read output port %s (%d)\n"), port_name.c_str(), i);
			ports.push_back(port_name);
      if (!outputs)
      {
        (*outputs)->GetMember(wxT("Name"))->SetValue(port_name);
        outputs++;
      }
		}
		nodes.push_back(std::pair<Node *, std::vector<wxString>>(node, ports));
		node->EvalSize();
		m_Graph->AddNode(node);
	}
	int edges_count = lis.ReadInteger();
	Debug::Printf(TXT("  Graph has %d edges\n"), edges_count);
	for (int i = 0; i < edges_count; i++)
	{
		int source_node = lis.ReadInteger() - 1;
		int source_port = lis.ReadInteger() - 1;
		Debug::Printf(TXT("    Source is node %2d, port %d\n"), source_node, source_port);
		int target_node = lis.ReadInteger() - 1;
		int target_port = lis.ReadInteger() - 1;
		Debug::Printf(TXT("    Target is node %2d, port %d\n"), target_node, target_port);
		bool debug = lis.ReadBoolean();
    if (debug)
    {
      Debug::Printf(TXT("    Edge is a debug edge\n"));
    }
		Shape *source = NULL, *target = NULL;
		{
			std::pair<Node *, std::vector<wxString>> pair = nodes[source_node];
			for (List<OutputPort *>::Iterator i = pair.first->OutputIterator(); !i; i++)
			{
				OutputPort *shape = *i;
        Debug::Printf(TXT("      Looking for source %s (%s)\n"), pair.second[source_port].c_str(), (*i)->GetMember(wxT("Name"))->GetString().c_str());
				if (shape->GetMember(wxT("Name"))->GetString() == pair.second[source_port])
				{
					Debug::Printf(TXT("        Source is %s\n"), shape->GetMember(wxT("Name"))->GetString().c_str());
					source = shape;
          break;
				}
			}
			if (source == NULL)
			{
				Debug::Printf(TXT("        Source not found\n"));
			}
		}
		{
			std::pair<Node *, std::vector<wxString>> pair = nodes[target_node];
			for (List<InputPort *>::Iterator i = pair.first->InputIterator(); !i; i++)
			{
				InputPort *shape = *i;
        Debug::Printf(TXT("      Looking for target %s (%s)\n"), pair.second[target_port].c_str(), (*i)->GetMember(wxT("Name"))->GetString().c_str());
				if (shape->GetMember(wxT("Name"))->GetString() == pair.second[target_port])
				{
					Debug::Printf(TXT("        Target is %s\n"), shape->GetMember(wxT("Name"))->GetString().c_str());
					target = shape;
          break;
				}
			}
			if (target == NULL)
			{
				Debug::Printf(TXT("        Target not found\n"));
			}
		}
    if (source == NULL)
    {
      Node* node = nodes[source_node].first;
      if (node->NumOutputs() != 0)
      {
        source = *node->OutputIterator();
      }
    }
    if (target == NULL)
    {
      Node* node = nodes[target_node].first;
      for (List<InputPort *>::Iterator i = node->InputIterator(); !i; ++i)
      {
        if ((*i)->GetSource() == NULL)
        {
          target = *i;
          break;
        }
      }
    }
		if (source != NULL && target != NULL)
		{
			m_Graph->DoAddEdge(source, target);
		}
	}
	m_Graph->EndUpdate();
}

void Panel::LoadBinary21(LuaInputStream& lis)
{
	LoadBinary20(lis);
}

void Panel::LoadLibrary(const wxString& filename)
{
	wxString path(g_FragmentShaderLibPath);
	path.Append(wxT("nodes\\")).Append(filename);
	wxArrayString nodes = NodeLibrary::Add(path);
	for (size_t i = 0; i < nodes.GetCount(); i++)
	{
		AddToTree(nodes[i]);
	}
}

void Panel::AddNode(const wxString& type, bool deletable)
{
	Node *node = NodeLibrary::Create(type);
	if (node != NULL)
	{
		node->GetMember(wxT("deletable"))->SetValue(deletable);
		m_Graph->AddNode(node);
	}
}

Panel::Layout Panel::GetLayout()
{
	Layout layout;
	layout.vsash = m_VSplitter->GetSashPosition();
	layout.hsash1 = m_HSplitter1->GetSashPosition();
	layout.hsash2 = m_HSplitter2->GetSashPosition();
	return layout;
}

void Panel::SetLayout(const Panel::Layout& layout)
{
	m_VSplitter->SetSashPosition(layout.vsash);
	m_HSplitter1->SetSashPosition(layout.hsash1);
	m_HSplitter2->SetSashPosition(layout.hsash2);
}

void Panel::GetMenu()
{
	m_State->Get();
}

void Panel::SetMenu()
{
	m_State->Set();
}

void Panel::OnTreeItemActivated(wxTreeEvent& evt)
{
	wxTreeItemId item = m_Tree->GetSelection();
	if (m_Tree->GetChildrenCount(item) == 0)
	{
		AddToGraph(item);
	}
}

void Panel::OnTreeSelChanged(wxTreeEvent& evt)
{
	/*wxTreeItemId item = evt.GetItem();
	if (m_Tree->GetChildrenCount(item) == 0)
	{
		LuaUtilities::PushSelf(g_FragmentShaderLuaState, this);
		lua_getfield(g_FragmentShaderLuaState, -1, "get_node_help");
		lua_pushstring(g_FragmentShaderLuaState, BuildNodePath(item).c_str());
		LuaUtilities::Call(g_FragmentShaderLuaState, 1, 1);
		const tchar *text = luaL_optstring(g_FragmentShaderLuaState, -1, "");
		m_Help->SetValue(text);
		lua_pop(g_FragmentShaderLuaState, 2);
		m_Ph->ChangeSelection(1);
	}*/
	wxTreeItemId item = evt.GetItem();
	if (m_Tree->GetChildrenCount(item) == 0)
	{
		wxString type = BuildNodePath(item);
		m_Graph->SetCurrentType(type);
		m_Help->SetValue(NodeLibrary::GetDescription(type));
		m_Ph->SetSelection(1);
	}
	else
	{
		m_Graph->SetCurrentType();
	}
}

void Panel::OnItemActivated(wxListEvent& evt)
{
  union ShapeData
  {
    Shape*  m_shape;
    long    m_data;
  };

  ShapeData dShape;
	dShape.m_data =  evt.GetData();

	if (dShape.m_shape != NULL)
	{
		m_Graph->Center(dShape.m_shape);
		m_Graph->ClearSelection();
		m_Graph->Select(dShape.m_shape);
	}
}

void Panel::OnPropertyChanging(wxPropertyGridEvent& evt)
{
	// TODO This should be in PropertyGrid, but we need the reference to the graph.
	m_Props->OnPropertyChanging(m_Graph, evt);
}

void Panel::OnPropertyChanged(wxPropertyGridEvent& evt)
{
	// TODO This should be in PropertyGrid, but we need the reference to the grap.
	m_Props->OnPropertyChanged(m_Graph, evt);
}

void Panel::AddToTree(const wxString& name)
{
	wxTreeItemId node = m_Tree->GetRootItem();
	if (!node.IsOk())
	{
		node = m_Tree->AddRoot(wxT("Nodes"));
	}
	wxTreeItemId item = 0;
	wxStringTokenizer parts(name, wxT("/"));
	while (parts.HasMoreTokens())
	{
		wxString part = parts.GetNextToken();
		wxTreeItemIdValue cookie;
		wxTreeItemId child = m_Tree->GetFirstChild(node, cookie);
		while (child.IsOk())
		{
			wxString text = m_Tree->GetItemText(child);
			if (text == part)
			{
				break;
			}
			child = m_Tree->GetNextChild(node, cookie);
		}
		if (child.IsOk())
		{
			node = child;
		}
		else
		{
			item = m_Tree->AppendItem(node, part);
			Debug::Printf(TXT("\tAdded node \"%s\".\n"), m_Tree->GetItemText(item));
			m_Tree->SortChildren(node);
			node = item;
		}
	}
	/*if (item != (wxTreeItemId)0)
	{
		//m_Tree->SetItemData(item, nd);
	}*/
}

wxString Panel::BuildNodeName(wxTreeItemId item)
{
	wxTreeItemId root = m_Tree->GetRootItem();
	wxString name = m_Tree->GetItemText(item);
	for (item = m_Tree->GetItemParent(item); item != root; item = m_Tree->GetItemParent(item))
	{
		name.Prepend(wxT("/"));
		name.Prepend(m_Tree->GetItemText(item));
	}
	return name;
}

wxString Panel::BuildNodePath(wxTreeItemId item)
{
	wxTreeItemPath *path = (wxTreeItemPath *)m_Tree->GetItemData(item);
	if (path == NULL)
	{
		return BuildNodeName(item);
	}
	else
	{
		return path->path;
	}
}

void Panel::AddToGraph(wxTreeItemId item)
{
	wxString type = BuildNodePath(item);
	AddNode(type);
}
