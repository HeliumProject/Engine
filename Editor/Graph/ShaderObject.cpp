#include "EditorPch.h"
#include "Editor/Graph/ShaderObject.h"

#include "Editor/Graph/Method.h"
#include "Editor/Graph/XML.h"
#include "Editor/Graph/Debug.h"

void
ShaderObject::PushLua(lua_State *L) const
{
	void *ud = lua_newuserdata(L, sizeof(ShaderObject *));
	*(const ShaderObject **)ud = this;
	lua_newtable(L);
	lua_pushcfunction(L, GetLuaIndex());
	lua_setfield(L, -2, "__index");
	lua_setmetatable(L, -2);
}

int
ShaderObject::LuaIndex(lua_State *L)
{
	ShaderObject *obj = *(ShaderObject **)lua_touserdata(L, 1);
	size_t len;
	const char *key = luaL_checklstring(L, 2, &len);
	if (len == 3 && !strcmp(key, "uid"))
	{
		lua_pushvalue(L, 1);
		lua_pushcclosure(L, LuaUID, 1);
		return 1;
	}
	return 0;
}

int
ShaderObject::LuaUID(lua_State *L)
{
	void *pointer = *(void **)lua_touserdata(L, lua_upvalueindex(1));
	wxString uid = wxString::Format(wxT("%p"), pointer);

    std::string temp;
    Helium::ConvertString( tstring( uid ), temp );

	lua_pushlstring(L, temp.c_str(), temp.length());
	return 1;
}

ShaderObject::~ShaderObject()
{
	for (std::map<wxString, Method *>::iterator i = m_methods.begin(); i != m_methods.end(); ++i)
	{
		DESTROY(i->second);
	}
}

void
ShaderObject::AddMethod(const wxString& name, const wxString& target, Method *method)
{
	wxString key = name;
	key.Append(wxT(':')).Append(target);
	m_methods.insert(std::pair<wxString, Method *>(key, method));
}

Method *
ShaderObject::GetMethod(const wxString& name, const wxString& target) const
{
	wxString key = name;
	key.Append(wxT(':')).Append(target);
	std::map<wxString, Method *>::const_iterator i = m_methods.find(key);
	if (i != m_methods.end())
	{
		return i->second;
	}
	key = name;
	key.Append(wxT(":*"));
	i = m_methods.find(key);
	if (i != m_methods.end())
	{
		return i->second;
	}
	return NULL;
}

wxXmlNode *
ShaderObject::Serialize() const
{
	wxXmlNode *root = Shape::Serialize();
	root->AddChild(SerializeMethods());
	return root;
}

void
ShaderObject::Deserialize(const wxXmlNode& root)
{
	Shape::Deserialize(root);
	DeserializeMethods(*XML::FindChild(root, wxT("methods")));
}

wxXmlNode *
ShaderObject::SerializeMethods() const
{
	wxXmlNode *root = NEW(wxXmlNode, (wxXML_ELEMENT_NODE, wxT("methods")));
	for (std::map<wxString, Method *>::const_iterator i = m_methods.begin(); i != m_methods.end(); ++i)
	{
		wxString key = i->first;
		int colon = key.Find(wxT(':'));
		wxString name = key.Left(colon);
		wxString target = key.Right(key.Len() - colon - 1);
		wxXmlNode *child = i->second->Serialize();
		child->AddAttribute(wxT("name"), name);
		child->AddAttribute(wxT("target"), target);
		root->AddChild(child);
	}
	return root;
}

void
ShaderObject::DeserializeMethods(const wxXmlNode& root)
{
	wxXmlNode *child = root.GetChildren();
	while (child != NULL)
	{
		if (child->GetName() == wxT("method"))
		{
			wxString name = XML::GetStringAttribute(*child, wxT("name"));
			wxString target = XML::GetStringAttribute(*child, wxT("target"));
			wxString type = XML::GetStringAttribute(*child, wxT("type"));
			Method *method = DeserializeMethod(*child);
			AddMethod(name, target, method);
		}
		child = child->GetNext();
	}
}

