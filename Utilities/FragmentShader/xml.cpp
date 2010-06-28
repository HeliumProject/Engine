#include "xml.h"

#include "debug.h"

namespace XML
{
	wxXmlNode *
	FindChild(const wxXmlNode& node, const wxString& name)
	{
		wxXmlNode *child = node.GetChildren();
		while (child != NULL)
		{
			if (child->GetName() == name)
			{
				return child;
			}
			child = child->GetNext();
		}
		return NULL;
	}

	void
	DeleteChild(wxXmlNode& node, const wxString& name)
	{
		wxXmlNode *child = FindChild(node, name);
		if (child != NULL)
		{
			node.RemoveChild(child);
			DESTROY(child);
		}
	}

	void
	DeleteChild(wxXmlNode& node, wxXmlNodeType type)
	{
		wxXmlNode *child = node.GetChildren();
		while (child != NULL)
		{
			if (child->GetType() == type)
			{
				node.RemoveChild(child);
				DESTROY(child);
				return;
			}
			child = child->GetNext();
		}
	}

	wxXmlProperty *
	FindAttribute(const wxXmlNode& node, const wxString& name)
	{
		wxXmlProperty *prop = node.GetProperties();
		while (prop != NULL)
		{
			if (prop->GetName() == name)
			{
				return prop;
			}
			prop = prop->GetNext();
		}
		return NULL;
	}

	void
	DeleteAttribute(wxXmlNode& node, const wxString& name)
	{
		wxXmlProperty *prop = node.GetProperties();
		wxXmlProperty *prev = NULL;
		while (prop != NULL)
		{
			if (prop->GetName() == name)
			{
				if (prev == NULL)
				{
					node.SetProperties(prop->GetNext());
				}
				else
				{
					prev->SetNext(prop->GetNext());
				}
				DESTROY(prop);
				return;
			}
			prop = prop->GetNext();
		}
	}

	wxString
	GetStringAttribute(const wxXmlNode& node, const wxString& name, const wxString& def)
	{
		wxString value;
		if (!node.GetPropVal(name, &value))
		{
			if (def == wxEmptyString)
			{
				THROW(TXT("Couldn't find attribute \"%s\" in element <%s>."), name.c_str(), node.GetName().c_str());
			}
			value = def;
		}
		return value;
	}

	bool
	GetBoolAttribute(const wxXmlNode& node, const wxString& name, const wxString& def)
	{
		wxString value = GetStringAttribute(node, name, def);
		if (value == wxT("true"))
		{
			return true;
		}
		else if (value == wxT("false"))
		{
			return false;
		}
		else
		{
			THROW(TXT("Attribute \"%s\" in element <%s> is not a boolean."), name.c_str(), node.GetName().c_str());
		}
	}

	int
	GetIntAttribute(const wxXmlNode& node, const wxString& name, const wxString& def)
	{
		wxString value = GetStringAttribute(node, name, def);
		long l;
		if (!value.ToLong(&l, 0))
		{
			THROW(TXT("Attribute \"%s\" in element <%s> is not an integer."), name.c_str(), node.GetName().c_str());
		}
		return l;
	}
};
