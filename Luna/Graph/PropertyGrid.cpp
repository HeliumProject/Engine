#include "Precompile.h"
#include "Graph/PropertyGrid.h"

#include <wx/variant.h>

#include "Graph/Property.h"
#include "Graph/Method.h"
#include "Graph/Graph.h"
#include "Graph/Debug.h"

class URChangeValue: public UndoRedo
{
public:
	URChangeValue(Graph *graph, Property *prop, const wxString& oldvalue, const wxString& newvalue)
		: m_graph(graph), m_prop(prop), m_oldvalue(oldvalue), m_newvalue(newvalue) {}

	virtual void Undo() { m_graph->BeginUpdate(); m_prop->FromString(m_oldvalue); m_graph->EndUpdate(); }
	virtual void Redo() { m_graph->BeginUpdate(); m_prop->FromString(m_newvalue); m_graph->EndUpdate(); }

private:
	Graph    *m_graph;
	Property *m_prop;
	wxString  m_oldvalue;
	wxString  m_newvalue;
};

void
PropertyGrid::Clear()
{
	m_map.clear();
	wxPropertyGrid::Clear();
}

wxPGProperty*
PropertyGrid::Append(wxPGProperty *pgprop, Property *prop)
{
	wxPGProperty* id = wxPropertyGrid::Append(pgprop);
	m_map.insert(std::pair<wxPGProperty*, Property *>(id, prop));
	return id;
}

void
PropertyGrid::OnPropertyChanging(Graph *graph, wxPropertyGridEvent& evt)
{
	wxPGProperty* id = evt.GetProperty();
	std::map<wxPGProperty*, Property *>::iterator i = m_map.find(id);
	if (i != m_map.end())
	{
		Property *prop = i->second;
		wxString old_value_str = prop->ToString();
		wxVariant old_value = GetPropertyValue(id);
		prop->SetValue(evt.GetValue());
		Method *method = prop->GetValidator();
		if (method != NULL && !method->Validate(prop))
		{
			evt.Veto();
			evt.SetValidationFailureBehavior(wxPG_VFB_BEEP);
			wxTextCtrl *editor = GetEditorTextCtrl();
			if (editor != NULL)
			{
				editor->SetValue(old_value_str);
			}
		}
		prop->SetValue(old_value);
	}
}

void
PropertyGrid::OnPropertyChanged(Graph *graph, wxPropertyGridEvent& evt)
{
	graph->BeginUpdate();
	wxPGProperty* id = evt.GetProperty();
	std::map<wxPGProperty*, Property *>::iterator i = m_map.find(id);
	if (i != m_map.end())
	{
		Property *prop = i->second;
		wxString old_value = prop->ToString();
		prop->SetValue(GetPropertyValue(id));
		wxString new_value = prop->ToString();
		prop->FromString(old_value);
		m_undoer->Add(NEW(URChangeValue, (graph, prop, old_value, new_value)));
	}
	graph->EndUpdate();
}
