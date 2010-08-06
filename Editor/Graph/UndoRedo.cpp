#include "Precompile.h"
#include "Editor/Graph/UndoRedo.h"

#include "Editor/Graph/ProjectNotebook.h"
#include "Editor/Graph/Debug.h"

UndoRedo::~UndoRedo()
{
	if (m_active)
	{
		Destroy();
	}
}

void
UndoRedo::SetActive(bool active)
{
	m_active = active;
}

void
URGroup::AddUR(UndoRedo *ur)
{
	m_items.push_back(ur);
}

void
URGroup::Undo()
{
	size_t count = m_items.size();
	for (size_t i = 0; i < count; i++)
	{
		m_items[i]->Undo();
	}
}

void
URGroup::Redo()
{
	size_t count = m_items.size();
	for (size_t i = 0; i < count; i++)
	{
		m_items[i]->Redo();
	}
}

void
URGroup::SetActive(bool active)
{
	UndoRedo::SetActive(active);
	size_t count = m_items.size();
	for (size_t i = 0; i < count; i++)
	{
		m_items[i]->SetActive(active);
	}
}

void
URGroup::Destroy()
{
	size_t count = m_items.size();
	for (size_t i = 0; i < count; i++)
	{
		DESTROY(m_items[i]);
	}
	UndoRedo::Destroy();
}

void
URNeg::SetActive(bool active)
{
	UndoRedo::SetActive(active);
	m_ur->SetActive(active);
}

void URNeg::Destroy()
{
	DESTROY(m_ur);
}

Undoer::Undoer()
	: m_index(0)
{
	Panel *panel = Project::GetProject()->GetPanel();
	if (panel != NULL)
	{
		MenuState *state = panel->GetMenuStateCtrl();
		state->Enable(wxID_UNDO, false);
		state->Enable(wxID_REDO, false);
	}
}

Undoer::~Undoer()
{
	Clear();
}

void
Undoer::Add(UndoRedo *ur)
{
	size_t size = m_list.size();
	for (size_t i = m_index; i < size; i++)
	{
		UndoRedo *ur = m_list.back();
		DESTROY(ur);
		m_list.pop_back();
	}
	m_list.push_back(ur);
	m_index++;
	ur->Redo();
	ur->SetActive(true);
	MenuState *state = Project::GetProject()->GetPanel()->GetMenuStateCtrl();
	state->Enable(wxID_UNDO, true);
	state->Enable(wxID_REDO, false);
}

void
Undoer::Undo()
{
	if (m_index == 0)
	{
		THROW(TXT("Nothing to undo"));
	}
	UndoRedo *ur = m_list[--m_index];
	ur->Undo();
	ur->SetActive(false);
	MenuState *state = Project::GetProject()->GetPanel()->GetMenuStateCtrl();
	state->Enable(wxID_UNDO, m_index != 0);
	state->Enable(wxID_REDO, true);
}

void
Undoer::Redo()
{
	size_t size = m_list.size();
	if (m_index == size)
	{
		THROW(TXT("Nothing to redo"));
	}
	UndoRedo *ur = m_list[m_index++];
	ur->Redo();
	ur->SetActive(true);
	MenuState *state = Project::GetProject()->GetPanel()->GetMenuStateCtrl();
	state->Enable(wxID_UNDO, true);
	state->Enable(wxID_REDO, m_index != size);
}

void
Undoer::Clear()
{
	size_t size = m_list.size();
	for (size_t i = 0; i < size; i++)
	{
		UndoRedo *ur = m_list[i];
		DESTROY(ur);
	}
	m_list.clear();
	m_index = 0;
	/*Project *project = Project::GetProject();
	if (project != NULL)
	{
		Panel *panel = project->GetPanel();
		if (panel != NULL)
		{
			MenuState *state = panel->GetMenuStateCtrl();
			state->Enable(wxID_UNDO, false);
			state->Enable(wxID_REDO, false);
		}
	}*/
}
