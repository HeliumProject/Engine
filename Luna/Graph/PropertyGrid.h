#pragma once

#include "Graph/UndoRedo.h"

class Graph;
class Property;

class PropertyGrid: public wxPropertyGrid
{
public:
	PropertyGrid(wxWindow *parent, Undoer *undoer)
		: wxPropertyGrid(parent), m_undoer(undoer) {}

	virtual void Clear();
	wxPGProperty* Append(wxPGProperty *pgprop, Property *prop);

	void OnPropertyChanging(Graph *graph, wxPropertyGridEvent& evt);
	void OnPropertyChanged(Graph *graph, wxPropertyGridEvent& evt);

private:
	std::map<wxPGProperty*, Property *> m_map;
	Undoer *m_undoer;
};

