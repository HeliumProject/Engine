#include "Precompile.h"
#include "Graph/Method.h"

#include <wx/tokenzr.h>
#include <wx/sstream.h>

#include "Graph/Node.h"
#include "Graph/Property.h"
#include "Graph/XML.h"
#include "Graph/Debug.h"
#include "Graph/Panel.h"

#include "Platform/String.h"

wxXmlNode *
Method::Serialize() const
{
	wxXmlNode *root = NEW(wxXmlNode, (wxXML_ELEMENT_NODE, wxT("method")));
	root->AddProperty(wxT("type"), GetClassName());
	return root;
}

void
Method::Deserialize(const wxXmlNode& root)
{
}

wxString
MethodAny::GetType(const OutputPort *output) const
{
	Node *node = static_cast<Node *>(output->GetParent());
	if (node->NumInputs() == 0)
	{
		return wxT("?");
	}
	return (*node->InputIterator())->GetType();
}

MethodList::MethodList(std::set<wxString> *choices)
	: Method()
{
	if (choices != NULL)
	{
		for (std::set<wxString>::const_iterator i = choices->begin(); i != choices->end(); ++i)
		{
			m_choices.insert(*i);
		}
	}
}

wxXmlNode *
MethodList::Serialize() const
{
	wxXmlNode *root = Method::Serialize();
	wxString list;
	for (std::set<wxString>::const_iterator i = m_choices.begin(); i != m_choices.end(); ++i)
	{
		if (!list.IsEmpty())
		{
			list.Append(wxT(", "));
		}
		list.Append(*i);
	}
	root->AddChild(NEW(wxXmlNode, (wxXML_TEXT_NODE, wxT(""), list)));
	return root;
}

void
MethodList::Deserialize(const wxXmlNode& root)
{
	m_choices.clear();
	wxStringTokenizer tkz(root.GetNodeContent(), wxT(","));
	while (tkz.HasMoreTokens())
	{
		wxString token = tkz.GetNextToken();
		m_choices.insert(token.Trim().Trim(false));
	}
	Method::Deserialize(root);
}

MethodRegex::MethodRegex(std::vector<wxString> *patterns)
	: Method()
{
	if (patterns != NULL)
	{
		for (std::vector<wxString>::const_iterator i = patterns->begin(); i != patterns->end(); ++i)
		{
			m_patterns.push_back(*i);
		}
	}
	Compile();
}

MethodRegex::~MethodRegex()
{
	for (std::vector<wxRegEx *>::iterator i = m_regex.begin(); i != m_regex.end(); ++i)
	{
		DESTROY(*i);
	}
}

wxXmlNode *
MethodRegex::Serialize() const
{
	wxXmlNode *root = Method::Serialize();
	wxString patterns;
	for (std::vector<wxString>::const_iterator i = m_patterns.begin(); i != m_patterns.end(); ++i)
	{
		if (!patterns.IsEmpty())
		{
			patterns.Append(wxT("|"));
		}
		patterns.Append(*i);
	}
	root->AddChild(NEW(wxXmlNode, (wxXML_TEXT_NODE, wxT(""), patterns)));
	return root;
}

void
MethodRegex::Deserialize(const wxXmlNode& root)
{
	m_patterns.clear();
	wxStringTokenizer tkz(root.GetNodeContent(), wxT("|"));
	while (tkz.HasMoreTokens())
	{
		wxString token = tkz.GetNextToken();
		m_patterns.push_back(token.Trim().Trim(false));
	}
	Method::Deserialize(root);
	Compile();
}

void
MethodRegex::Compile()
{
	m_regex.clear();
	for (std::vector<wxString>::const_iterator i = m_patterns.begin(); i != m_patterns.end(); ++i)
	{
		wxRegEx *regex = NEW(wxRegEx, ());
		regex->Compile(*i);
		m_regex.push_back(regex);
	}
}

bool
MethodRegex::Matches(const wxString& what) const
{
	for (std::vector<wxRegEx *>::const_iterator i = m_regex.begin(); i != m_regex.end(); ++i)
	{
		if ((*i)->Matches(what))
		{
			return true;
		}
	}
	return false;
}

bool
MethodFixed::InputConstraints(const Node *node) const
{
	for (List<InputPort *>::Iterator i = node->InputIterator(); !i; i++)
	{
		if ((*i)->GetType() != m_what)
		{
			return false;
		}
	}
	return true;
}

wxXmlNode *
MethodFixed::Serialize() const
{
	wxXmlNode *root = Method::Serialize();
	root->AddChild(NEW(wxXmlNode, (wxXML_TEXT_NODE, wxT(""), m_what)));
	return root;
}

void
MethodFixed::Deserialize(const wxXmlNode& root)
{
	m_what = root.GetNodeContent();
	Method::Deserialize(root);
}

bool
MethodEqual::InputConstraints(const Node *node) const
{
	if (node->NumInputs() > 1)
	{
		List<InputPort *>::Iterator i = node->InputIterator();
		wxString type = (*i)->GetType();
		i++;
		for (; !i; i++)
		{
			if ((*i)->GetType() != type)
			{
				return false;
			}
		}
	}
	return true;
}

wxString
MethodSameAs::GetType(const OutputPort *output) const
{
	Node *node = static_cast<Node *>(output->GetParent());
	for (List<InputPort *>::Iterator i = node->InputIterator(); !i; i++)
	{
		InputPort *input = *i;
		if (input->GetLuaId() == m_id)
		{
			return input->GetType();
		}
	}
	return wxT("?");
}

wxXmlNode *
MethodSameAs::Serialize() const
{
	wxXmlNode *root = Method::Serialize();
	root->AddChild(NEW(wxXmlNode, (wxXML_TEXT_NODE, wxT(""), m_id)));
	return root;
}

void
MethodSameAs::Deserialize(const wxXmlNode& root)
{
	m_id = root.GetNodeContent();
	Method::Deserialize(root);
}

MethodLua::MethodLua(const wxString& source)
	: Method()
	, m_source(source)
{
    std::string temp;
    Platform::ConvertString( m_source.c_str(), temp );

	LuaUtilities::LoadBuffer(g_FragmentShaderLuaState, temp.c_str(), temp.length(), "method");
	m_lua_ref = luaL_ref(g_FragmentShaderLuaState, LUA_REGISTRYINDEX);
}

MethodLua::~MethodLua()
{
	luaL_unref(g_FragmentShaderLuaState, LUA_REGISTRYINDEX, m_lua_ref);
}

bool
MethodLua::CheckType(const InputPort *input) const
{
	Call(static_cast<Node *>(input->GetParent()));
	bool ok = lua_toboolean(g_FragmentShaderLuaState, -1) ? true : false;
	lua_pop(g_FragmentShaderLuaState, 1);
	return ok;
}

wxString
MethodLua::GetType(const OutputPort *output) const
{
	Call(static_cast<Node *>(output->GetParent()));
    tstring temp;
    Platform::ConvertString( lua_tostring(g_FragmentShaderLuaState, -1), temp );
    wxString type = temp;
	lua_pop(g_FragmentShaderLuaState, 1);
	return type;
}

bool
MethodLua::InputConstraints(const Node *node) const
{
	Call(node);
	bool ok = lua_toboolean(g_FragmentShaderLuaState, -1) ? true : false;
	lua_pop(g_FragmentShaderLuaState, 1);
	return ok;
}

bool
MethodLua::Validate(const Property *prop) const
{
	Call(dynamic_cast<Node *>(prop->GetOwner()));
	bool ok = lua_toboolean(g_FragmentShaderLuaState, -1) ? true : false;
	lua_pop(g_FragmentShaderLuaState, 1);
	return ok;
}

void
MethodLua::OnChanged(const Property *prop) const
{
	Call(dynamic_cast<Node *>(prop->GetOwner()));
	lua_pop(g_FragmentShaderLuaState, 1);
}

wxString
MethodLua::GenerateCode(const Node *node) const
{
	Call(node);
	tstring temp;
    Platform::ConvertString( lua_tostring(g_FragmentShaderLuaState, -1), temp );
    wxString code = temp;
	lua_pop(g_FragmentShaderLuaState, 1);
	return code;
}

wxXmlNode *
MethodLua::Serialize() const
{
	wxXmlNode *root = Method::Serialize();
	//root->AddChild(new wxXmlNode(wxXML_CDATA_SECTION_NODE, wxT(""), m_source));
	root->AddChild(NEW(wxXmlNode, (wxXML_TEXT_NODE, wxT(""), m_source)));
	return root;
}

void
MethodLua::Deserialize(const wxXmlNode& root)
{
	m_source = root.GetNodeContent();

    std::string temp;
    Platform::ConvertString( m_source.c_str(), temp );
    LuaUtilities::LoadBuffer(g_FragmentShaderLuaState, temp.c_str(), temp.length(), "method");
	m_lua_ref = luaL_ref(g_FragmentShaderLuaState, LUA_REGISTRYINDEX);
	Method::Deserialize(root);
}

void
MethodLua::Call(const ShaderObject *obj) const
{
	lua_getglobal(g_FragmentShaderLuaState, "node");
	int ref;
	if (!lua_isnil(g_FragmentShaderLuaState, -1))
	{
		ref = luaL_ref(g_FragmentShaderLuaState, LUA_REGISTRYINDEX);
	}
	else
	{
		ref = LUA_NOREF;
	}
	obj->PushLua(g_FragmentShaderLuaState);
	lua_setglobal(g_FragmentShaderLuaState, "node");
	lua_rawgeti(g_FragmentShaderLuaState, LUA_REGISTRYINDEX, m_lua_ref);

    std::string temp;
    Platform::ConvertString( obj->GetMember(wxT("Name"))->GetString().c_str(), temp);
    LuaUtilities::Call(g_FragmentShaderLuaState, 0, 1, temp.c_str());
	if (ref != LUA_NOREF)
	{
		lua_rawgeti(g_FragmentShaderLuaState, LUA_REGISTRYINDEX, ref);
		lua_setglobal(g_FragmentShaderLuaState, "node");
		luaL_unref(g_FragmentShaderLuaState, LUA_REGISTRYINDEX, ref);
	}
}

wxXmlNode *
MethodVarParse::Serialize() const
{
	wxXmlNode *root = Method::Serialize();
	root->AddChild(NEW(wxXmlNode, (wxXML_TEXT_NODE, wxT(""), m_pattern)));
	return root;
}

void
MethodVarParse::Deserialize(const wxXmlNode& root)
{
	m_pattern = root.GetNodeContent();
	m_source = Compile(m_pattern);
	wxXmlNode *child = root.GetChildren();
	while (child != NULL)
	{
		if (child->GetType() == wxXML_TEXT_NODE || child->GetType() == wxXML_CDATA_SECTION_NODE)
		{
			break;
		}
		child = child->GetNext();
	}
	child->SetContent(m_source);
	MethodLua::Deserialize(root);
	child->SetContent(m_pattern);
}

wxString
MethodVarParse::Compile(const wxString& pattern)
{
	wxString source = wxT("return '");
	size_t size = pattern.Len();
	for (size_t i = 0; i < size; i++)
	{
		wxChar k = pattern[i];
		switch (k)
		{
		case 9:
		case 10:
		case 13:
			break;
		case wxT('\\'):
			if (i == (size - 1))
			{
				source.Append(wxT('\\'));
			}
			else
			{
				switch (pattern[i + 1])
				{
				case wxT('t'):
					source.Append(wxT("\\t"));
					break;
				case wxT('n'):
					source.Append(wxT("\\n"));
					break;
				case wxT('\\'):
					source.Append(wxT("\\\\"));
					break;
				case wxT('$'):
					source.Append(wxT('$'));
					break;
				default:
					source.Append(wxT("\\\\")).Append(pattern[i + 1]);
					break;
				}
				i++;
			}
			break;
		case wxT('$'):
			if (i == (size - 1))
			{
				source.Append(wxT('$'));
			}
			else if (pattern[i + 1] == wxT('{'))
			{
				source.Append(wxT("'.."));
				int braces = 1;
				for (i += 2; i < size; i++)
				{
					wxChar k = pattern[i];
					if (k == wxT('{'))
					{
						braces++;
					}
					else if (k == wxT('}'))
					{
						if (--braces == 0)
						{
							break;
						}
					}
					source.Append(pattern[i]);
				}
				//if (i < (size - 1) && pattern[i] == wxT('}'))
				//{
				//	i++;
				//}
				source.Append(wxT("..'"));
			}
			else
			{
				source.Append(wxT('$')).Append(pattern[i + 1]);
			}
			break;
		default:
			source.Append(k);
			break;
		}
	}
	source.Append(wxT("';"));
	return source;
}

Method *
DeserializeMethod(const wxXmlNode& root)
{
	Method *method;
	wxString type = XML::GetStringAttribute(root, wxT("type"));
	if (type == wxT("any"))
	{
		method = NEW(MethodAny, ());
	}
	else if (type == wxT("list"))
	{
		method = NEW(MethodList, ());
	}
	else if (type == wxT("regex"))
	{
		method = NEW(MethodRegex, ());
	}
	else if (type == wxT("fixed"))
	{
		method = NEW(MethodFixed, ());
	}
	else if (type == wxT("equal"))
	{
		method = NEW(MethodEqual, ());
	}
	else if (type == wxT("same-as"))
	{
		method = NEW(MethodSameAs, ());
	}
	else if (type == wxT("lua"))
	{
		/*static bool first = true;
		if (first)
		{
			first = false;
		}
		else
		{
			BREAK(true);
		}*/
		method = NEW(MethodLua, ());
	}
	else if (type == wxT("var-parse"))
	{
		method = NEW(MethodVarParse, ());
	}
	method->Deserialize(root);
	return method;
}
