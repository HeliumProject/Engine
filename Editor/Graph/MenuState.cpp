#include "Precompile.h"
#include "Graph/MenuState.h"

#include "Graph/Debug.h"

MenuState::MenuState(wxMenuBar *menu)
	: m_MenuBar(menu)
	, m_States(0)
{
}

MenuState::~MenuState()
{
	if (m_States != 0)
	{
		DESTROY(m_States);
	}
}

void
MenuState::Add(int first, int last)
{
	if (m_States == 0)
	{
		if (last == 0)
		{
			last = first;
		}
		if (first > last)
		{
			int temp = first;
			first = last;
			last = temp;
		}
		Range range;
		range.first = first;
		range.last = last;
		m_Ranges.push_back(range);
	}
}

void
MenuState::Done()
{
	const size_t length = m_Ranges.size();
	int count = 0;
	for (size_t index = 0; index < length; index++)
	{
		Range range = m_Ranges[index];
		count += range.last - range.first + 1;
	}
	m_States = NEWARRAY(bool, count);
}

void
MenuState::Get()
{
	if (m_States == 0)
	{
		Done();
	}
	const size_t length = m_Ranges.size();
	int count = 0;
	for (size_t index = 0; index < length; index++)
	{
		Range range = m_Ranges[index];
		for (int id = range.first; id <= range.last; id++)
		{
			m_States[count++] = m_MenuBar->IsEnabled(id);
		}
	}
}

void
MenuState::Set()
{
	if (m_States == 0)
	{
		Done();
	}
	const size_t length = m_Ranges.size();
	int count = 0;
	for (size_t index = 0; index < length; index++)
	{
		Range range = m_Ranges[index];
		for (int id = range.first; id <= range.last; id++)
		{
			m_MenuBar->Enable(id, m_States[count++]);
		}
	}
}

void
MenuState::Enable(int itemid, bool enable)
{
	const size_t length = m_Ranges.size();
	int count = 0;
	for (size_t index = 0; index < length; index++)
	{
		Range range = m_Ranges[index];
		if (itemid >= range.first && itemid <= range.last)
		{
			m_MenuBar->Enable(itemid, enable);
			break;
		}
	}
}

void
MenuState::Enable(const wxString& label, bool enable)
{
	int slash = label.Find('/');
	if (slash != wxNOT_FOUND)
	{
		wxString first = label.Left(slash);
		wxString second = label.Mid(slash + 1);
		int id = m_MenuBar->FindMenuItem(first, second);
		if (id != wxNOT_FOUND)
		{
			Enable(id, enable);
		}
	}
}

MenuState *
MenuState::Clone()
{
	MenuState *clone = NEW(MenuState, (m_MenuBar));
	const size_t length = m_Ranges.size();
	int count = 0;
	for (size_t index = 0; index < length; index++)
	{
		Range range = m_Ranges[index];
		clone->Add(range.first, range.last);
		count += range.last - range.first + 1;
	}
	if (m_States != 0)
	{
		clone->m_States = NEWARRAY(bool, count);
		for (int index = 0; index < count; index++)
		{
			clone->m_States[index] = m_States[index];
		}
	}
	return clone;
}
