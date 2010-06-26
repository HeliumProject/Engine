#include "member.h"

#include <wx/tokenzr.h>

#include "shape.h"
#include "persistent.h"
#include "util.h"
#include "xml.h"

#include "debug.h"

Member::Member(Shape *owner, const wxString& name, int *value, const wxString& id)
	: m_owner(owner)
	, m_name(name)
	, m_type(Int)
	, m_int(value)
	, m_id(id)
	, m_cdata(false)
{
	BuildId();
}

Member::Member(Shape *owner, const wxString& name, bool *value, const wxString& id)
	: m_owner(owner)
	, m_name(name)
	, m_type(Bool)
	, m_bool(value)
	, m_id(id)
	, m_cdata(false)
{
	BuildId();
}

Member::Member(Shape *owner, const wxString& name, double *value, const wxString& id)
	: m_owner(owner)
	, m_name(name)
	, m_type(Double)
	, m_double(value)
	, m_id(id)
	, m_cdata(false)
{
	BuildId();
}

Member::Member(Shape *owner, const wxString& name, wxString *value, const wxString& id)
	: m_owner(owner)
	, m_name(name)
	, m_type(String)
	, m_string(value)
	, m_id(id)
	, m_cdata(false)
{
	BuildId();
}

Member::Member(Shape *owner, const wxString& name, wxColor *value, const wxString& id)
	: m_owner(owner)
	, m_name(name)
	, m_type(Color)
	, m_color(value)
	, m_id(id)
	, m_cdata(false)
{
	BuildId();
}

Member::Member(Shape *owner, const wxString& name, wxRect *value, const wxString& id)
	: m_owner(owner)
	, m_name(name)
	, m_type(Rect)
	, m_rect(value)
	, m_id(id)
	, m_cdata(false)
{
	BuildId();
}

wxString
Member::GetValue() const
{
	return ToString();
}

void
Member::SetValue(const int value)
{
	if (m_type == Int && *m_int != value)
	{
		*m_int = value;
		Updated();
	}
}

void
Member::SetValue(const bool value)
{
	if (m_type == Bool && *m_bool != value)
	{
		*m_bool = value;
		Updated();
	}
}

void
Member::SetValue(const double value)
{
	if (m_type == Double && *m_double != value)
	{
		*m_double = value;
		Updated();
	}
}

void
Member::SetValue(const wxString& value)
{
	if (m_type == String && *m_string != value)
	{
		*m_string = value;
		Updated();
	}
}

void
Member::SetValue(const wxColor& value)
{
	if (m_type == Color && *m_color != value)
	{
		*m_color = value;
		Updated();
	}
}

void
Member::SetValue(const wxRect& value)
{
	if (m_type == Rect && *m_rect != value)
	{
		*m_rect = value;
		Updated();
	}
}

void
Member::SetValue(const wxVariant& value)
{
	wxString type = value.GetType();
	if (type == wxT("bool"))
	{
		SetValue(value.GetBool());
	}
	else if (type == wxT("double"))
	{
		SetValue(value.GetDouble());
	}
	else if (type == wxT("long"))
	{
		SetValue((int)value.GetLong());
	}
	else if (type == wxT("string"))
	{
		SetValue(value.GetString());
	}
	else if (type == wxT("wxColour"))
	{
		wxColour color;
		color << value;
		SetValue(color);
	}
}

void
Member::FromString(const wxString& value)
{
	long l;
	unsigned long u;
	double d;
	wxColor color;
	wxRect rect;
	switch (m_type)
	{
	case Int:
		value.ToLong(&l, 0);
		SetValue((int)l);
		break;
	case Bool:
		SetValue(value == wxT("true"));
		break;
	case Double:
		value.ToDouble(&d);
		SetValue(d);
		break;
	case String:
		SetValue(value);
		break;
	case Color:
		value.ToULong(&u, 0);
		color.Set(u >> 24, u >> 16, u >> 8, u);
		SetValue(color);
		break;
	case Rect:
		{
			wxStringTokenizer tkz(value, wxT(" "));
			tkz.GetNextToken().Trim(false).ToLong(&l, 0);
			rect.SetLeft(l);
			tkz.GetNextToken().Trim(false).ToLong(&l, 0);
			rect.SetTop(l);
			tkz.GetNextToken().Trim(false).ToLong(&l, 0);
			rect.SetRight(l);
			tkz.GetNextToken().Trim(false).ToLong(&l, 0);
			rect.SetBottom(l);
			SetValue(rect);
		}
		break;
	}
}

wxString
Member::ToString() const
{
	wxString value;
	switch (m_type)
	{
	case Int:
		value.Printf( wxT( "%d" ), *m_int);
		break;
	case Bool:
		value = *m_bool ? wxT( "true" ) : wxT( "false" );
		break;
	case Double:
		value.Printf( wxT( "%g" ), *m_double);
		break;
	case String:
		value = *m_string;
		break;
	case Color:
		value.Printf( wxT( "0x%.2x%.2x%.2x%.2x" ), m_color->Red(), m_color->Green(), m_color->Blue(), m_color->Alpha());
		break;
	case Rect:
		value.Printf(wxT( "%d, %d, %d, %d" ),m_rect->GetLeft(), m_rect->GetTop(), m_rect->GetRight(), m_rect->GetBottom());
		break;
	}
	return value;
}

wxXmlNode *
Member::Serialize() const
{
	wxXmlNode *root = NEW(wxXmlNode, (wxXML_ELEMENT_NODE, GetClassName()));
	root->AddProperty(wxT("name"), m_name);
	if (m_id != m_name)
	{
		root->AddProperty(wxT("id"), m_id);
	}
	wxString type;
	switch (m_type)
	{
	case Int:
		type = wxT("int");
		break;
	case Bool:
		type = wxT("bool");
		break;
	case Double:
		type = wxT("double");
		break;
	case String:
		type = wxT("string");
		break;
	case Color:
		type = wxT("color");
		break;
	case Rect:
		type = wxT("rect");
		break;
	}
	root->AddProperty(wxT("type"), type);
	wxXmlNode *value = new wxXmlNode(root, wxXML_ELEMENT_NODE, wxT("value"));
	/*if (m_cdata)
	{
		value->AddChild(new wxXmlNode(wxXML_CDATA_SECTION_NODE, wxT(""), ToString()));
	}
	else*/
	{
		value->AddChild(NEW(wxXmlNode, (wxXML_TEXT_NODE, wxT(""), ToString())));
	}
	root->AddChild(value);
	return root;
}

void
Member::Deserialize(const wxXmlNode& root)
{
	m_name = XML::GetStringAttribute(root, wxT("name"));
	m_id = XML::GetStringAttribute(root, wxT("id"), Util::ToID(m_name));
	wxString type = XML::GetStringAttribute(root, wxT("type"));
	if (type == wxT("int"))
	{
		m_type = Int;
	}
	else if (type == wxT("bool"))
	{
		m_type = Bool;
	}
	else if (type == wxT("double"))
	{
		m_type = Double;
	}
	else if (type == wxT("string"))
	{
		m_type = String;
	}
	else if (type == wxT("color"))
	{
		m_type = Color;
	}
	else if (type == wxT("rect"))
	{
		m_type = Rect;
	}
	wxXmlNode *value = XML::FindChild(root, wxT("value"));
	if (value != NULL)
	{
		FromString(value->GetNodeContent());
	}
	else
	{
		FromString(root.GetNodeContent());
	}
	m_cdata = false;
	wxXmlNode *child = root.GetChildren();
	while (child != NULL)
	{
		if (child->GetType() == wxXML_CDATA_SECTION_NODE)
		{
			m_cdata = true;
			break;
		}
		child = child->GetNext();
	}
}

void
Member::BuildId()
{
	if (m_id.IsEmpty())
	{
		m_id = Util::ToID(m_name);
	}
}

void
Member::Updated()
{
	if (m_owner != NULL)
	{
		m_owner->Redraw();
	}
	OnChanged();
}
