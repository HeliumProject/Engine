#include "Precompile.h"
#include "Editor/Graph/Shape.h"

#include <set>

#include "Editor/Graph/Debug.h"
#include "Editor/Graph/ProjectNotebook.h"
#include "Editor/Graph/XML.h"

Shape::Shape()
	: m_parent((Shape *)NULL)
	, m_bbox(0, 0, 0, 0)
{
	//AddMember(new Member(this, wxT("selected"), &m_IsSelected));
	//AddMember(new Member(this, wxT("highlighted"), &m_IsHighlighted));
	//AddMember(new Member(wxT("modified"), &m_IsModified));
	AddMember(NEW(Member, (this, wxT("bbox"), &m_bbox)));
}

Shape::~Shape()
{
	for (List<Shape *>::Iterator i = m_children.Iterate(); !i; i++)
	{
		DESTROY(*i);
	}
	m_children.Clear();
	for (List<Member *>::Iterator i = m_members.Iterate(); !i; i++)
	{
		DESTROY(*i);
	}
	m_members.Clear();
	for (List<Property *>::Iterator i = m_props.Iterate(); !i; i++)
	{
		DESTROY(*i);
	}
	m_props.Clear();
}

Shape *
Shape::IsInside(const wxPoint& pt)
{
	for (List<Shape *>::Iterator i = m_children.Iterate(); !i; i++)
	{
		Shape *found = (*i)->IsInside(pt);
		if (found != NULL)
		{
			return found;
		}
	}
	return m_bbox.Contains(pt) ? this : (Shape *)NULL;
}


void
Shape::SetPosition(int x, int y)
{
  Project* project = Project::GetProject();

	int dx = x - m_bbox.GetX();
	int dy = y - m_bbox.GetY();
	for (List<Shape *>::Iterator i = m_children.Iterate(); !i; i++)
	{
		Shape *child = *i;
		wxPoint pos = child->GetPosition();
		pos.x += dx;
		pos.y += dy;
		child->SetPosition(pos);
	}
	m_bbox.SetX(x);
	m_bbox.SetY(y);
}

void
Shape::Draw(wxDC& dc) const
{
	for (List<Shape *>::Iterator i = m_children.Iterate(); !i; i++)
	{
		(*i)->Draw(dc);
	}
}

void
Shape::DrawConnections(wxDC& dc) const
{
	dc.SetPen(*wxBLACK_PEN);
	for (List<Shape *>::Iterator i = m_sources.Iterate(); !i; i++)
	{
		wxPoint points[4];
		points[2] = points[3] = GetConnPoint();
		points[0] = points[1] = (*i)->GetConnPoint();
		points[1].x += 20;
		points[2].x -= 20;
		dc.DrawSpline(4, points);
		dc.SetPen(*wxRED_PEN);
	}
	for (List<Shape *>::Iterator i = m_children.Iterate(); !i; i++)
	{
		(*i)->DrawConnections(dc);
	}
}

void
Shape::Redraw()
{
	if (m_parent != NULL)
	{
		m_parent->Redraw();
	}
}

void
Shape::AddChild(Shape *child)
{
	child->SetParent(this);
	m_children.Add(child);
}

wxRect
Shape::GetChildrenBBox() const
{
	wxRect bbox;
	for (List<Shape *>::Iterator i = m_children.Iterate(); !i; i++)
	{
		bbox.Union((*i)->GetBBox());
	}
	return bbox;
}

Member *
Shape::AddMember(Member *member)
{
	member->SetOwner(this);
	m_members.Add(member);
	m_members_by_id.insert(std::pair<wxString, Member *>(member->GetId(), member));
	return member;
}

void
Shape::DeleteMember(Member *member)
{
	for (std::map<wxString, Member *>::iterator i = m_members_by_id.begin(); i != m_members_by_id.end(); ++i)
	{
		if (i->second == member)
		{
			m_members_by_id.erase(i);
			break;
		}
	}
	m_members.Delete(member);
}

Member *
Shape::FindMember(const wxString& id) const
{
	std::map<wxString, Member *>::const_iterator i = m_members_by_id.find(id);
	if (i != m_members_by_id.end())
	{
		return i->second;
	}
	return NULL;
}

Member *
Shape::GetMember(const wxString& id) const
{
	static int value = 0;
	static Member dummy(NULL, wxT("?"), &value);
	Member *member = FindMember(id);
	if (member != NULL)
	{
		return member;
	}
	return &dummy;
}

Property *
Shape::AddProperty(Property *prop)
{
	prop->SetOwner(this);
	m_props.Add(prop);
	m_members_by_id.insert(std::pair<wxString, Property *>(prop->GetId(), prop));
	return prop;
}

void
Shape::MemberChanged(Member *member)
{
	Redraw();
}

void
Shape::ShowProperties(PropertyGrid *grid)
{
	for (List<Property *>::Iterator i = m_props.Iterate(); !i; i++)
	{
		(*i)->Show(grid);
	}
}

wxXmlNode *
Shape::Serialize() const
{
	wxXmlNode *root = NEW(wxXmlNode, (wxXML_ELEMENT_NODE, GetClassName()));
	root->AddProperty(wxT("uid"), wxString::Format(wxT("%p"), this));
	root->AddChild(SerializeMembers());
	root->AddChild(SerializeChildren());
	root->AddChild(SerializeConnections());
	return root;
}

void
Shape::Deserialize(const wxXmlNode& root)
{
	for (List<Property *>::Iterator i = m_props.Iterate(); !i; i++)
	{
		std::map<wxString, Member *>::iterator i2 = m_members_by_id.find((*i)->GetId());
		if (i2 != m_members_by_id.end())
		{
			m_members_by_id.erase((*i)->GetId());
		}
		DESTROY(*i);
	}
	m_props.Clear();
	if (root.GetName() != GetClassName())
	{
		THROW(TXT("Expecting element <%s>, found <%s>."), GetClassName().c_str(), root.GetName().c_str());
	}
	wxXmlNode *child = root.GetChildren();
	while (child != NULL)
	{
		wxString name = child->GetName();
		if (name == wxT("children"))
		{
			DeserializeChildren(*child);
		}
		else if (name == wxT("properties"))
		{
			DeserializeMembers(*child);
		}
		else if (name == wxT("connections"))
		{
			DeserializeConnections(*child);
		}
		child = child->GetNext();
	}
}

wxXmlNode *
Shape::SerializeChildren() const
{
	wxXmlNode *children = NEW(wxXmlNode, (wxXML_ELEMENT_NODE, wxT("children")));
	for (List<Shape *>::Iterator i = m_children.Iterate(); !i; i++)
	{
		children->AddChild((*i)->Serialize());
	}
	return children;
}

wxXmlNode *
Shape::SerializeMembers() const
{
	wxXmlNode *members = NEW(wxXmlNode, (wxXML_ELEMENT_NODE, wxT("properties")));
	for (List<Member *>::Iterator i = m_members.Iterate(); !i; i++)
	{
		members->AddChild((*i)->Serialize());
	}
	for (List<Property *>::Iterator i = m_props.Iterate(); !i; i++)
	{
		members->AddChild((*i)->Serialize());
	}
	return members;
}

wxXmlNode *
Shape::SerializeConnections() const
{
	wxXmlNode *conn = NEW(wxXmlNode, (wxXML_ELEMENT_NODE, wxT("connections")));
	for (List<Shape *>::Iterator i = m_sources.Iterate(); !i; i++)
	{
		wxXmlNode *source = NEW(wxXmlNode, (wxXML_ELEMENT_NODE, wxT("source")));
		source->AddProperty(wxT("ref"), wxString::Format(wxT("%p"), *i));
		conn->AddChild(source);
	}
	for (List<Shape *>::Iterator i = m_targets.Iterate(); !i; i++)
	{
		wxXmlNode *target = NEW(wxXmlNode, (wxXML_ELEMENT_NODE, wxT("target")));
		target->AddProperty(wxT("ref"), wxString::Format(wxT("%p"), *i));
		conn->AddChild(target);
	}
	return conn;
}

void
Shape::DeserializeChildren(const wxXmlNode& root)
{
	wxXmlNode *child = root.GetChildren();
	while (child != NULL)
	{
		wxString uid = XML::GetStringAttribute(*child, wxT("uid"));
		Serialized *obj = Serialized::GetObjectByUID(uid);
		obj->Deserialize(*child);
		AddChild(static_cast<Shape *>(obj));
		child = child->GetNext();
	}
}

void
Shape::DeserializeMembers(const wxXmlNode& root)
{
	wxXmlNode *child = root.GetChildren();
	while (child != NULL)
	{
		wxString name = child->GetName();
		if (name == wxT("member"))
		{
			wxString id;
			if (!child->GetPropVal(wxT("id"), &id))
			{
				id = Util::ToID(XML::GetStringAttribute(*child, wxT("name")));
			}
			std::map<wxString, Member *>::iterator i = m_members_by_id.find(id);
			if (i == m_members_by_id.end())
			{
				THROW(TXT("Member \"%s\" not found for element <%s>."), id.c_str(), root.GetParent()->GetName().c_str());
			}
			i->second->Deserialize(*child);
		}
		else if (name == wxT("property"))
		{
			AddProperty(DeserializeProperty(*child));
		}
		else
		{
			THROW(TXT("Element <%s> cannot be a subelement of <%s>."), name.c_str(), root.GetName().c_str());
		}
		child = child->GetNext();
	}
}

void
Shape::DeserializeConnections(const wxXmlNode& root)
{
	wxXmlNode *child = root.GetChildren();
	while (child != NULL)
	{
		wxString uid = XML::GetStringAttribute(*child, wxT("ref"));
		Serialized *obj = Serialized::GetObjectByUID(uid);
		if (obj != NULL)
		{
			wxString name = child->GetName();
			if (name == wxT("source"))
			{
				AddSource(static_cast<Shape *>(obj));
			}
			else if (name == wxT("target"))
			{
				AddTarget(static_cast<Shape *>(obj));
			}
			else
			{
				THROW(TXT("Element <%s> cannot be a subelement of <%s>."), name.c_str(), root.GetName().c_str());
			}
		}
		child = child->GetNext();
	}
}
