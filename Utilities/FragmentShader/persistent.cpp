#include "persistent.h"

#include "xml.h"

#include "debug.h"

std::map<wxString, Persistent::Creator> Persistent::m_Creators;
std::map<wxString, Persistent *> Persistent::m_Objects;

void
Persistent::RegisterClass(const wxString& classname, Creator creator)
{
	m_Creators.insert(std::pair<wxString, Creator>(classname, creator));
}

wxXmlNode *
Persistent::SerializeObject(const Persistent& obj)
{
	wxXmlNode *root = obj.Serialize();
	return root;
}

Persistent *
Persistent::GetObjectByUID(const wxString& uid)
{
	std::map<wxString, Persistent *>::iterator it = m_Objects.find(uid);
	if (it != m_Objects.end())
	{
		Debug::Printf(TXT("%s(\"%s\") = %p\n"), __FUNCTION__, uid.c_str(), it->second);
		return it->second;
	}
	Debug::Printf(TXT("%s(\"%s\") = NULL\n"), __FUNCTION__, uid.c_str());
	return (Persistent *)NULL;
}

void
Persistent::CreateObjects(const wxXmlNode& root)
{
	wxString uid;
	if (root.GetPropVal(wxT("uid"), &uid))
	{
		if (m_Objects.find(uid) == m_Objects.end())
		{
			wxString classname = root.GetName();
			std::map<wxString, Creator>::iterator it = m_Creators.find(classname);
			if (it == m_Creators.end())
			{
				THROW(TXT("Class \"%s\" not found."), root.GetName().c_str());
			}
			Persistent *obj = (it->second)(root);
			m_Objects.insert(std::pair<wxString, Persistent *>(uid, obj));
		}
	}
	wxXmlNode *child = root.GetChildren();
	while (child != NULL)
	{
		CreateObjects(*child);
		child = child->GetNext();
	}
}

Persistent *
Persistent::DeserializeObject(const wxXmlNode& root, Persistent *obj)
{
	m_Objects.clear();
	wxString uid;
	if (!root.GetPropVal(wxT("uid"), &uid))
	{
		return (Persistent *)NULL;
	}
	if (obj != NULL)
	{
		m_Objects.insert(std::pair<wxString, Persistent *>(uid, obj));
		Debug::Printf(TXT("%s(%p, %p) -> object %s created as %p\n"), __FUNCTION__, &root, obj, uid.c_str(), obj);
	}
	CreateObjects(root);
	obj = GetObjectByUID(uid);
	obj->Deserialize(root);
	return obj;
}
