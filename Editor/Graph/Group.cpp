#include "Precompile.h"
#include "Editor/Graph/Group.h"

#include "Editor/Graph/XML.h"

Group::Group()
	: Node()
{
	Member *name = FindMember(wxT("Name"));
	DeleteMember(name);
	DESTROY(name);
}

Group::Group(List<Shape *> *nodes)
	: Node()
{
	for (List<Shape *>::Iterator i = nodes->Iterate(); !i; i++)
	{
		if (dynamic_cast<Node *>(*i) != NULL)
		{
			m_children.Add(*i);
		}
	}
	AddPorts();
	Member *name = FindMember(wxT("Name"));
	DeleteMember(name);
	DESTROY(name);
	AddProperty(NEW(TextProperty, (this, wxT("Name"))));
	SetPosition(GetTopLeft(nodes));
	EvalSize();
}

Shape *
Group::IsInside(const wxPoint& pt)
{
	for (List<InputPort *>::Iterator i = m_inputs.Iterate(); !i; i++)
	{
		if ((*i)->IsInside(pt))
		{
			return *i;
		}
	}
	for (List<OutputPort *>::Iterator i = m_outputs.Iterate(); !i; i++)
	{
		if ((*i)->IsInside(pt))
		{
			return *i;
		}
	}
	return m_bbox.Contains(pt) ? this : (Shape *)NULL;
}

void
Group::DrawConnections(wxDC& dc) const
{
	for (List<InputPort *>::Iterator input = m_inputs.Iterate(); !input; input++)
	{
		dc.SetPen(*wxBLACK_PEN);
		for (List<Shape *>::Iterator i = (*input)->SourceIterator(); !i; i++)
		{
			wxPoint points[4];
			points[2] = points[3] = (*input)->GetConnPoint();
			points[0] = points[1] = (*i)->GetConnPoint();
			points[1].x += 20;
			points[2].x -= 20;
			dc.DrawSpline(4, points);
			dc.SetPen(*wxRED_PEN);
		}
	}
}

void
Group::GenerateCode(CodeTextCtrl *codectrl, Report *reportctrl) const
{
#if 0
	codectrl->Append(wxT("/*\n===========================================\nMembers:\n"));
	for (List<Member *>::Iterator i = MemberIterator(); !i; i++)
	{
		codectrl->Append(wxT("\t")).Append((*i)->GetName()).Append(wxT("\t")).Append((*i)->ToString()).Append(wxT("\n"));
	}
	codectrl->Append(wxT("Properties:\n"));
	for (List<Property *>::Iterator i = PropertyIterator(); !i; i++)
	{
		codectrl->Append(wxT("\t")).Append((*i)->GetName()).Append(wxT("\t")).Append((*i)->ToString()).Append(wxT("\n"));
	}
	codectrl->Append(wxT("Inputs:\n"));
	for (List<InputPort *>::Iterator i = InputIterator(); !i; i++)
	{
		codectrl->Append(wxT("\t")).Append((*i)->GetMember(wxT("Name"))->GetString()).Append(wxT("\n"));
	}
	codectrl->Append(wxT("Outputs:\n"));
	for (List<OutputPort *>::Iterator i = OutputIterator(); !i; i++)
	{
		codectrl->Append(wxT("\t")).Append((*i)->GetMember(wxT("Name"))->GetString()).Append(wxT("\n"));
	}
	codectrl->Append(wxT("===========================================\n*/\n"));
#endif
}

bool
Group::CheckInputConstraits() const
{
	for (List<Shape *>::Iterator i = m_children.Iterate(); !i; i++)
	{
		Node *node = dynamic_cast<Node *>(*i);
		if (!node->CheckInputConstraints())
		{
			return false;
		}
	}
	return true;
}

void
Group::Deserialize(const wxXmlNode& root)
{
	Node::Deserialize(root);
	AddPorts();
	EvalSize();
}

void
Group::DeserializeChildren(const wxXmlNode& root)
{
	wxXmlNode *child = root.GetChildren();
	while (child != NULL)
	{
		wxString uid = XML::GetStringAttribute(*child, wxT("uid"));
		Serialized *obj = Serialized::GetObjectByUID(uid);
		obj->Deserialize(*child);
		Shape *shape = static_cast<Shape *>(obj);
		shape->SetParent(this);
		m_children.Add(shape);
		child = child->GetNext();
	}
}

void
Group::AddPorts()
{
	for (List<Shape *>::Iterator i = m_children.Iterate(); !i; i++)
	{
		Node *node = dynamic_cast<Node *>(*i);
		for (List<InputPort *>::Iterator input = node->InputIterator(); !input; input++)
		{
			if ((*input)->NumSources() == 0)
			{
				m_inputs.Add(*input);
			}
			else
			{
				for (List<Shape *>::Iterator output = (*input)->SourceIterator(); !output; output++)
				{
					if (!m_children.Contains((*output)->GetParent()))
					{
						m_inputs.Add(*input);
					}
				}
			}
		}
		for (List<OutputPort *>::Iterator output = node->OutputIterator(); !output; output++)
		{
			if ((*output)->NumTargets() == 0)
			{
				m_outputs.Add(*output);
			}
			else
			{
				for (List<Shape *>::Iterator input = (*output)->TargetIterator(); !input; input++)
				{
					if (!m_children.Contains((*input)->GetParent()))
					{
						m_outputs.Add(*output);
					}
				}
			}
		}
	}
}

wxPoint
Group::GetTopLeft(List<Shape *> *shapes) const
{
	int min_x = 0, min_y = 0;
	List<Shape *>::Iterator i = shapes == NULL ? ChildIterator() : shapes->Iterate();
	if (!i)
	{
		wxPoint pos = (*i)->GetPosition();
		min_x = pos.x, min_y = pos.y;
		for (i++; !i; i++)
		{
			wxPoint pos = (*i)->GetPosition();
			if (pos.x < min_x)
			{
				min_x = pos.x;
			}
			if (pos.y < min_y)
			{
				min_y = pos.y;
			}
		}
	}
	return wxPoint(min_x, min_y);
}
