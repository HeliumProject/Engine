#include "Precompile.h"
#include "Editor/Graph/Property.h"

#include "Editor/Graph/Method.h"
#include "Editor/Graph/XML.h"
#include "Editor/Graph/Debug.h"

Property::~Property()
{
	if (m_validator != NULL)
	{
		DESTROY(m_validator);
	}
	if (m_onchanged != NULL)
	{
		DESTROY(m_onchanged);
	}
}

void
Property::OnChanged()
{
	if (m_onchanged != NULL)
	{
		m_onchanged->OnChanged(this);
	}
}

wxXmlNode *
Property::Serialize() const
{
	wxXmlNode *root = Member::Serialize();
	wxXmlProperty *type = XML::FindAttribute(*root, wxT("type"));
	type->SetValue(GetClassName());
	root->SetName(wxT("property"));
	if (m_validator != NULL)
	{
		wxXmlNode *n = m_validator->Serialize();
		n->AddAttribute(wxT("name"), wxT("validator"));
		root->AddChild(n);
	}
	if (m_onchanged != NULL)
	{
		wxXmlNode *n = m_onchanged->Serialize();
		n->AddAttribute(wxT("name"), wxT("on-changed"));
		root->AddChild(n);
	}
	return root;
}

void
Property::Deserialize(const wxXmlNode& root)
{
	wxXmlProperty *prop = XML::FindAttribute(root, wxT("type"));
	wxString type = prop->GetValue();
	if (type == wxT("list"))
	{
		prop->SetValue(wxT("int"));
	}
	else if (type == wxT("text") || type == wxT("memo") || type == wxT("file"))
	{
		prop->SetValue(wxT("string"));
	}
	Member::Deserialize(root);
	wxXmlNode *method = XML::FindChild(root, wxT("method"));
	if (method != NULL)
	{
		wxString name = XML::GetStringAttribute(*method, wxT("name"));
		if (name == wxT("validator"))
		{
			SetValidator(DeserializeMethod(*method));
		}
		else if (name == wxT("on-changed"))
		{
			SetOnChanged(DeserializeMethod(*method));
		}
	}
	prop->SetValue(type);
}

ListProperty::ListProperty(Shape *owner, const wxString& name, std::vector<Item> *items, const wxString& id)
	: Property(owner, name, &m_int, id)
	, m_int(0)
{
	int count = (int)items->size();
	for (int i = 0; i < count; i++)
	{
		m_labels.Add((*items)[i].m_Label, i);
		m_values.Add((*items)[i].m_Value, i);
	}
}

void
ListProperty::Show(PropertyGrid *props)
{
	wxPGProperty* id = props->Append(NEW(wxEnumProperty, (m_name, wxPG_LABEL, m_labels)), this);
	props->SetPropertyValue(id, m_int);
}

wxXmlNode *
ListProperty::Serialize() const
{
	wxXmlNode *root = Property::Serialize();
	wxXmlNode *options = NEW(wxXmlNode, (wxXML_ELEMENT_NODE, wxT("options")));
	for (unsigned int i = 0; i < m_labels.GetCount(); i++)
	{
		const wxString label = m_labels.GetLabel(i);
		const wxString value = m_values.GetLabel(i);
		wxXmlNode *option = NEW(wxXmlNode, (wxXML_ELEMENT_NODE, wxT("option")));
		option->AddChild(NEW(wxXmlNode, (wxXML_TEXT_NODE, wxT(""), label)));
		option->AddAttribute(NEW(wxXmlProperty, (wxT("value"), value)));
		/*if (i == (unsigned int)m_int)
		{
			child->AddProperty(NEW(wxXmlProperty, (wxT("selected"), wxT("true"))));
		}*/
		options->AddChild(option);
	}
	root->AddChild(options);
	return root;
}

void
ListProperty::Deserialize(const wxXmlNode& root)
{
	Property::Deserialize(root);
	wxXmlNode *child = root.GetChildren();
	while (child != NULL)
	{
		if (child->GetName() == wxT("options"))
		{
			wxXmlNode *option = child->GetChildren();
			int count = 0;
			while (option != NULL)
			{
				wxString label = option->GetNodeContent();
				m_labels.Add(label, count);
				m_values.Add(XML::GetStringAttribute(*option, wxT("value"), label), count);
				/*if (XML::GetBoolAttribute(*child, wxT("selected"), wxT("false")))
				{
					m_int = count;
				}*/
				count++;
				option = option->GetNext();
			}
		}
		child = child->GetNext();
	}
}

void
BoolProperty::Show(PropertyGrid *props)
{
	wxPGProperty* id = props->Append(NEW(wxBoolProperty, (m_name, wxPG_LABEL, m_bool)), this);
	props->SetPropertyAttribute(id, wxPG_BOOL_USE_CHECKBOX, (long)1, wxPG_RECURSE);
}

void
FileProperty::Show(PropertyGrid *props)
{
	wxPGProperty* id = props->Append(NEW(wxFileProperty, (m_name, wxPG_LABEL, m_string)), this);
	props->SetPropertyAttribute(id, wxPG_FILE_WILDCARD, m_wildcard);
}

Property *
DeserializeProperty(const wxXmlNode &root)
{
	Property *prop = NULL;
	wxString type = XML::GetStringAttribute(root, wxT("type"));
	wxString name = XML::GetStringAttribute(root, wxT("name"));
	if (type == wxT("text"))
	{
		prop = NEW(TextProperty, ());
	}
	else if (type == wxT("memo"))
	{
		prop = NEW(MemoProperty, ());
	}
	/*else if (type == wxT("int"))
	{
		prop = NEW(IntProperty, ());
	}
	else if (type == wxT("double"))
	{
		prop = NEW(DoubleProperty, ());
	}*/
	else if (type == wxT("boolean"))
	{
		prop = NEW(BoolProperty, ());
	}
	else if (type == wxT("file"))
	{
		prop = NEW(FileProperty, ());
	}
	else if (type == wxT("color"))
	{
		prop = NEW(ColorProperty, ());
	}
	else if (type == wxT("list"))
	{
		prop = NEW(ListProperty, ());
	}
  if(prop)
  {
	  prop->Deserialize(root);
  }
	//prop->FromString(root.GetNodeContent());
	return prop;
}
