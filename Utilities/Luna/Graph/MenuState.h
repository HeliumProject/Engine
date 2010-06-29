#pragma once

#include <wx/menu.h>

#include <vector>

class MenuState
{
public:
	MenuState(wxMenuBar *menu);
	~MenuState();
	void Add(int first, int last = 0);
	void Get();
	void Set();
	void Enable(int itemid, bool enable = true);
	void Enable(const wxString& label, bool enable = true);
	MenuState *Clone();

private:
	void Done();

	struct Range
	{
		int first, last;
	};

	wxMenuBar			*m_MenuBar;
	std::vector<Range>	m_Ranges;
	bool				*m_States;
};
