#include "nodelib.h"

#include <wx/tokenzr.h>

#include "node.h"
#include "persistent.h"
#include "util.h"
#include "method.h"
#include "xml.h"
#include "main.h"
#include "project.h"

#include "debug.h"

NodeDef::NodeDef(const wxString& path, const wxXmlNode *root)
	: m_Path(path)
	, m_Root(root)
{
	m_Type = XML::GetStringAttribute(*root, wxT("type"));
	m_Description = XML::FindChild(*root, wxT("description"));
	m_Tooltip = XML::FindChild(*root, wxT("tool-tip"));
	if (m_Tooltip == NULL)
	{
		m_Tooltip = m_Description;
	}
	m_NodeType = m_Path;
	m_NodeType.Append(wxT('/')).Append(m_Type);
}

Node *
NodeDef::NewInstance()
{
	Node *node = NEW(Node, ());
	ParseNode(node);
	return node;
}

void
NodeDef::ParseNode(Node *node)
{
	wxString type = XML::GetStringAttribute(*m_Root, wxT("type"));
	wxString name = XML::GetStringAttribute(*m_Root, wxT("name"), m_Type);
	node->GetMember(wxT("path"))->SetValue(m_NodeType);
	node->GetMember(wxT("type"))->SetValue(type);
	node->GetMember(wxT("Name"))->SetValue(name);
	//node->GetMember(wxT("id"))->SetString(Util::ToID(name));
	wxXmlNode *child = m_Root->GetChildren();
	while (child != 0)
	{
		wxString name = child->GetName();
		if (name == wxT("description"))
		{
			ParseDescription(node, *child);
		}
		else if (name == wxT("tool-tip"))
		{
			ParseToolTip(node, *child);
		}
		else if (name == wxT("ports"))
		{
			ParsePorts(node, *child);
		}
		else if (name == wxT("properties"))
		{
			ParseProperties(node, *child);
		}
		else if (name == wxT("code-generation"))
		{
			ParseCodeGeneration(node, *child);
		}
		else
		{
			Debug::Printf("Invalid element <%s> in <node>.\n", name.c_str());
		}
		child = child->GetNext();
	}
}

void
NodeDef::ParseDescription(Node *node, const wxXmlNode& root)
{
	wxString description = root.GetNodeContent().Trim();
	node->GetMember(wxT("description"))->SetValue(description);
}

void
NodeDef::ParseToolTip(Node *node, const wxXmlNode& root)
{
	wxString tool_tip = root.GetNodeContent().Trim();
	node->GetMember(wxT("tooltip"))->SetValue(tool_tip);
}

void
NodeDef::ParsePorts(Node *node, const wxXmlNode& root)
{
	wxXmlNode *child = root.GetChildren();
	while (child != 0)
	{
		wxString name = child->GetName();
		if (name == wxT("input"))
		{
			ParseInput(node, *child);
		}
		else if (name == wxT("output"))
		{
			ParseOutput(node, *child);
		}
		else if (name == wxT("input-constraints"))
		{
			ParseInputConstraints(node, *child);
		}
		else
		{
			Debug::Printf("Invalid element <%s> in <ports>.\n", name.c_str());
		}
		child = child->GetNext();
	}
}

void
NodeDef::ParseInput(Node *node, const wxXmlNode& root)
{
	InputPort *input = NEW(InputPort, ());
	node->AddInput(input);
	wxString name = XML::GetStringAttribute(root, wxT("name"));
	input->GetMember(wxT("Name"))->SetValue(name);
	input->GetMember(wxT("id"))->SetValue(Util::ToID(name));
	input->GetMember(wxT("required"))->SetValue(XML::GetBoolAttribute(root, wxT("required"), wxT("true")));
	wxXmlNode *child = root.GetChildren();
	while (child != 0)
	{
		wxString name = child->GetName();
		if (name == wxT("check-type"))
		{
			ParseCheckType(input, *child);
		}
		else
		{
			Debug::Printf("Invalid element <%s> in <input>.\n", name.c_str());
		}
		child = child->GetNext();
	}
}

void
NodeDef::ParseCheckType(InputPort *input, const wxXmlNode& root)
{
	Method *method = ParseMethod(root, wxT("any"));
	wxString target = XML::GetStringAttribute(root, wxT("target"), wxT("*"));
	input->AddMethod(root.GetName(), target, method);
}

void
NodeDef::ParseOutput(Node *node, const wxXmlNode& root)
{
	OutputPort *output = NEW(OutputPort, ());
	node->AddOutput(output);
	wxString name = XML::GetStringAttribute(root, wxT("name"));
	output->GetMember(wxT("Name"))->SetValue(name);
	output->GetMember(wxT("id"))->SetValue(Util::ToID(name));
	wxXmlNode *child = root.GetChildren();
	while (child != 0)
	{
		wxString name = child->GetName();
		if (name == wxT("get-type"))
		{
			ParseGetType(output, *child);
		}
		else
		{
			Debug::Printf("\tInvalid element <%s> in <output>.\n", name.c_str());
		}
		child = child->GetNext();
	}
}

void
NodeDef::ParseGetType(OutputPort *output, const wxXmlNode& root)
{
	Method *method = ParseMethod(root, wxT("fixed"));
	wxString target = XML::GetStringAttribute(root, wxT("target"), wxT("*"));
	output->AddMethod(root.GetName(), target, method);
}

void
NodeDef::ParseInputConstraints(Node *node, const wxXmlNode& root)
{
	Method *method = ParseMethod(root, wxT("equal"));
	wxString target = XML::GetStringAttribute(root, wxT("target"), wxT("*"));
	node->AddMethod(root.GetName(), target, method);
}

void
NodeDef::ParseProperties(Node *node, const wxXmlNode& root)
{
	wxXmlNode *child = root.GetChildren();
	while (child != 0)
	{
		wxString name = child->GetName();
		if (name == wxT("default-properties"))
		{
			ParseDefaultProperties(node, *child);
		}
		else if (name == wxT("property"))
		{
			ParseProperty(node, *child);
		}
		child = child->GetNext();
	}
}

void
NodeDef::ParseDefaultProperties(Node *node, const wxXmlNode& root)
{
	node->AddProperty(NEW(MemoProperty, (node, wxT("Comment"))));
	node->AddProperty(NEW(ColorProperty, (node, wxT("Fill color"))))->SetValue(wxColor(255, 255, 255));
}

void
NodeDef::ParseProperty(Node *node, const wxXmlNode& root)
{
	Property *prop = DeserializeProperty(root);
	wxXmlNode *validator = XML::FindChild(root, wxT("validator"));
	if (validator != NULL)
	{
		prop->SetValidator(ParseMethod(*validator, wxT("")));
	}
	wxXmlNode *onchanged = XML::FindChild(root, wxT("on-changed"));
	if (onchanged != NULL)
	{
		wxXmlProperty *p = XML::FindAttribute(*onchanged, wxT("method"));
		if (p == NULL)
		{
			onchanged->AddProperty(wxT("method"), wxT("lua"));
		}
		else
		{
			p->SetValue(wxT("lua"));
		}
		prop->SetOnChanged(ParseMethod(*onchanged, wxT("")));
	}
	prop->SetOwner(node);
	node->AddProperty(prop);
}

void
NodeDef::ParseCodeGeneration(Node *node, const wxXmlNode& root)
{
	ParseCode(node, root);
}

void
NodeDef::ParseCode(Node *node, const wxXmlNode& root)
{
	wxXmlNode *child = root.GetChildren();
	while (child != NULL)
	{
		if (child->GetName() == wxT("code"))
		{
			Method *method = ParseMethod(*child, wxT("var-parse"));
			wxString target = XML::GetStringAttribute(*child, wxT("target"), wxT("*"));
			node->AddMethod(child->GetName(), target, method);
		}
		child = child->GetNext();
	}
}

Method *
NodeDef::ParseMethod(const wxXmlNode& root, const wxString& dftl)
{
	wxXmlProperty *prop = XML::FindAttribute(root, wxT("method"));
	prop->SetName(wxT("type"));
	Method *method = DeserializeMethod(root);
	prop->SetName(wxT("method"));
	return method;
}

namespace NodeLib
{
	static std::vector<wxXmlNode *> m_Libraries;
	static std::map<wxString, wxXmlNode *> m_Nodes;
	static std::map<wxString, wxString *> m_Aka;

	wxArrayString
	Add(const wxString& file)
	{
		wxArrayString nodes;
		wxXmlDocument xml;
		if (!xml.Load(file))
		{
			THROW("Could not load library %s.", file.c_str());
		}
		wxXmlNode *root = xml.DetachRoot();
		if (root->GetName() != wxT("library"))
		{
			THROW("Root node must be <library>.");
		}
		m_Libraries.push_back(root);
		wxString path = XML::GetStringAttribute(*root, wxT("path"));
		wxXmlNode *child = root->GetChildren();
		while (child != NULL)
		{
			wxString name = child->GetName();
			if (name == wxT("node"))
			{
				wxXmlProperty *deprecated = XML::FindAttribute(*child, wxT("deprecated"));
				wxString type = path;
				type.Append(wxT('/')).Append(XML::GetStringAttribute(*child, wxT("type")));
				if (deprecated != NULL && deprecated->GetValue() == wxT("true"))
				{
					goto again;
				}
				nodes.Add(type);
				std::map<wxString, wxXmlNode *>::iterator i = m_Nodes.find(type);
				if (i != m_Nodes.end())
				{
					goto again;
				}
				m_Nodes.insert(std::pair<wxString, wxXmlNode *>(type, child));
				// HACK to mantain compatibility with old node names
				int lp = type.Find(wxT('('), true);
				if (lp != wxNOT_FOUND && lp != 0)
				{
					wxString aka = type.Left(lp - 1);
					std::map<wxString, wxString *>::iterator i = m_Aka.find(aka);
					if (i == m_Aka.end())
					{
						m_Aka.insert(std::pair<wxString, wxString *>(aka, NEW(wxString,(type))));
					}
				}
			}
			else if (name == wxT("group"))
			{
				wxString type = path;
				type.Append(wxT('/')).Append(XML::GetStringAttribute(*child, wxT("type")));
				nodes.Add(type);
				std::map<wxString, wxXmlNode *>::iterator i = m_Nodes.find(type);
				if (i != m_Nodes.end())
				{
					goto again;
				}
				m_Nodes.insert(std::pair<wxString, wxXmlNode *>(type, child));
			}
			else
			{
				THROW("Libraries can only have node definitions.");
			}
			again:
			child = child->GetNext();
		}
		return nodes;
	}

	Node *
	Create(wxString type)
	{
		for (int count = 0; count < 2; count++)
		{
			std::map<wxString, wxXmlNode *>::iterator i = m_Nodes.find(type);
			if (i != m_Nodes.end())
			{
				wxXmlNode *root = i->second;
				if (root->GetName() == wxT("node"))
				{
					NodeDef nd(type, i->second);
					return nd.NewInstance();
				}
				return (Node *)Persistent::DeserializeObject(*root);
			}
			std::map<wxString, wxString *>::iterator i2 = m_Aka.find(type);
			if (i2 == m_Aka.end())
			{
				break;
			}
			type = *i2->second;
		}
		return NULL;
	}

	wxString
	GetDescription(const wxString& type)
	{
		/*std::map<wxString, NodeDef *>::iterator i = m_Nodes.find(type);
		if (i == m_Nodes.end())
		{
			return wxT("");
		}
		return i->second->GetDescription();*/
		std::map<wxString, wxXmlNode *>::iterator i = m_Nodes.find(type);
		if (i != m_Nodes.end())
		{
			wxXmlNode *root = i->second;
			wxXmlNode *desc = XML::FindChild(*root, wxT("description"));
			if (desc != NULL)
			{
				return desc->GetNodeContent();
			}
		}
		return wxT("");
	}

	bool
	SaveGroup(wxXmlNode *root)
	{
		// Check if the node already exists.
		wxString type = XML::FindAttribute(*root, wxT("type"))->GetValue();
		wxString path = wxT("User Defined");
		path.Append(wxT("/")).Append(type);
		std::map<wxString, wxXmlNode *>::iterator i = m_Nodes.find(path);
		if (i != m_Nodes.end())
		{
			return false;
		}
		// Add the node to the nodes tree.
		Project::GetProject()->GetPanel()->AddToTree(path);
		// Add the node to the groups map so it can be created.
		m_Nodes.insert(std::pair<wxString, wxXmlNode *>(path, root));
    /*
		// Add the node to the user-defined XML.
		wxXmlNode *user = NULL;
		for (std::vector<wxXmlNode *>::iterator i = m_Libraries.begin(); i != m_Libraries.end(); ++i)
		{
			if (XML::FindAttribute(**i, wxT("path"))->GetValue() == wxT("User Defined"))
			{
				user = *i;
				break;
			}
		}
		user->AddChild(root);
		// Save the user-defined XML.
		wxString file(g_LibPath);
		file.Append(wxT("nodes\\user.xml"));
		wxXmlDocument doc;
		doc.SetRoot(user);
		doc.Save(file);
		doc.DetachRoot();
    */

		return true;
	}

	void
	Clear()
	{
    /*
		wxXmlNode *user = NULL;
		for (std::vector<wxXmlNode *>::iterator i = m_Libraries.begin(); i != m_Libraries.end(); ++i)
		{
			if (XML::FindAttribute(**i, wxT("path"))->GetValue() == wxT("User Defined"))
			{
				user = *i;
				break;
			}
		}
		// Save the user-defined XML.
		wxString file(g_LibPath);
		file.Append(wxT("nodes\\user.xml"));
		wxXmlDocument doc;
		doc.SetRoot(user);
		doc.Save(file);
		doc.DetachRoot();
    */

		// Cleanup.
		for (std::vector<wxXmlNode *>::iterator i = m_Libraries.begin(); i != m_Libraries.end(); ++i)
		{
			DESTROY(*i);
		}
		m_Libraries.clear();
		m_Nodes.clear();
		for (std::map<wxString, wxString *>::iterator i = m_Aka.begin(); i != m_Aka.end(); ++i)
		{
			DESTROY(i->second);
		}
		m_Aka.clear();
	}
};
