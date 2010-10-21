#include "Precompile.h"
#include "Editor/Graph/Serialized.h"

#include "Editor/Graph/XML.h"
#include "Editor/Graph/Debug.h"

std::map<wxString, Serialized::Creator> Serialized::m_Creators;
std::map<wxString, Serialized *> Serialized::m_Objects;

void
Serialized::RegisterSerializedClass(const wxString& classname, Creator creator)
{
	m_Creators.insert(std::pair<wxString, Creator>(classname, creator));
}

wxXmlNode *
Serialized::SerializeObject(const Serialized& obj)
{
	wxXmlNode *root = obj.Serialize();
	return root;
}

Serialized* Serialized::GetObjectByUID(const wxString& uid)
{
	std::map<wxString, Serialized *>::iterator it = m_Objects.find(uid);
	if (it != m_Objects.end())
	{
		Debug::Printf(TXT("%s(\"%s\") = %p\n"), __FUNCTION__, uid.c_str(), it->second);
		return it->second;
	}
	Debug::Printf(TXT("%s(\"%s\") = NULL\n"), __FUNCTION__, uid.c_str());
	return (Serialized *)NULL;
}

void
Serialized::CreateObjects(const wxXmlNode& root)
{
	wxString uid;
	if (root.GetAttribute(wxT("uid"), &uid))
	{
		if (m_Objects.find(uid) == m_Objects.end())
		{
			wxString classname = root.GetName();
			std::map<wxString, Creator>::iterator it = m_Creators.find(classname);
			if (it == m_Creators.end())
			{
				THROW(TXT("Class \"%s\" not found."), root.GetName().c_str());
			}
			Serialized *obj = (it->second)(root);
			m_Objects.insert(std::pair<wxString, Serialized *>(uid, obj));
		}
	}
	wxXmlNode *child = root.GetChildren();
	while (child != NULL)
	{
		CreateObjects(*child);
		child = child->GetNext();
	}
}

Serialized *
Serialized::DeserializeObject(const wxXmlNode& root, Serialized *obj)
{
	m_Objects.clear();
	wxString uid;
	if (!root.GetAttribute(wxT("uid"), &uid))
	{
		return (Serialized *)NULL;
	}
	if (obj != NULL)
	{
		m_Objects.insert(std::pair<wxString, Serialized *>(uid, obj));
		Debug::Printf(TXT("%s(%p, %p) -> object %s created as %p\n"), __FUNCTION__, &root, obj, uid.c_str(), obj);
	}
	CreateObjects(root);
	obj = GetObjectByUID(uid);
	obj->Deserialize(root);
	return obj;
}
