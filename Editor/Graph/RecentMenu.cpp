#include "Precompile.h"
#include "Editor/Graph/RecentMenu.h"

#include <wx/filename.h>

#include "Editor/Graph/Debug.h"

RecentMenu::RecentMenu(int firstid, int lastid, const wxString& empty)
	: wxMenu()
	, m_FirstId(firstid)
	, m_LastId(lastid)
	, m_Empty(empty)
{
	if (m_FirstId > m_LastId)
	{
		int id = m_FirstId;
		m_FirstId = m_LastId;
		m_LastId = id;
	}
	Append(m_FirstId, m_Empty);
	Enable(m_FirstId, false);
}

void
RecentMenu::Put(const wxString& path)
{
	// Get the file name.
	wxFileName filename(path);
	if (!filename.FileExists())
	{
		return;
	}
	wxString name = filename.GetName();
	// Check if the only item is Empty.
	size_t count = GetMenuItemCount();
	if (count == 1)
	{
		wxMenuItem *item = FindItemByPosition(0);
		if (!IsEnabled(item->GetId()))
		{
			// It is, remove it.
			Destroy(item);
			count--;
		}
	}
	// Check if path already exists.
	for (size_t index = 0; index < count; index++)
	{
		wxMenuItem *item = FindItemByPosition(index);
		wxString other = item->GetHelp();
		if (other == path)
		{
			// It does, move to top and return.
			int id = item->GetId();
			Delete(item);
			Prepend(id, name, path);
			return;
		}
	}
	// Check if there's still an available id to add the new item.
	int id = wxNOT_FOUND;
	for (int index = m_FirstId; index <= m_LastId; index++)
	{
		wxMenuItem *item = FindItem(index);
		if (item == NULL)
		{
			id = index;
			break;
		}
	}
	// There wasn't, delete the last entry.
	if (id == wxNOT_FOUND)
	{
		wxMenuItem *item = FindItemByPosition(m_LastId - m_FirstId);
		id = item->GetId();
		Delete(item);
	}
	// Prepend the new item.
	Prepend(id, name, path);
}

bool
RecentMenu::Get(int id, wxString& path)
{
	wxMenuItem *item = FindItem(id);
	if (item != NULL)
	{
		path = item->GetHelp();
		return true;
	}
	return false;
}

void
RecentMenu::Remove(const wxString& path)
{
	// Look for path.
	size_t count = GetMenuItemCount();
	for (size_t index = 0; index < count; index++)
	{
		wxMenuItem *item = FindItemByPosition(index);
		wxString other = item->GetHelp();
		if (other == path)
		{
			// Found, remove it from menu.
			Delete(item);
			// If menu is empty, create the Empty entry.
			if (count == 1)
			{
				Append(m_FirstId, m_Empty);
				Enable(m_FirstId, false);
			}
			return;
		}
	}
	// Not found, just return.
}

void
RecentMenu::Load(wxConfigBase& config, const wxString& prefix)
{
	// Empty the menu.
	while (GetMenuItemCount() != 0)
	{
		Delete(FindItemByPosition(0));
	}
	// Get number of entries.
	long last;
	config.Read(prefix, &last);
	// Add paths.
	const int total = m_LastId - m_FirstId + 1;
	if (last > total)
	{
		last = total;
	}
	for (int index = last - 1; index >= 0; index--)
	{
		wxString key, path;
		key.Printf(wxT("%s_%d"), prefix, index);
		config.Read(key, &path);
		Put(path);
	}
	// Add the Empty entry if the menu is empty.
	if (GetMenuItemCount() == 0)
	{
		Append(m_FirstId, m_Empty);
		Enable(m_FirstId, false);
	}
}

void
RecentMenu::Save(wxConfigBase& config, const wxString& prefix)
{
	// Check if the only item is Empty.
	size_t count = GetMenuItemCount();
	if (count == 1)
	{
		wxMenuItem *item = FindItemByPosition(0);
		if (!IsEnabled(item->GetId()))
		{
			// It is, return.
			return;
		}
	}
	// There are valid entries in the menu.
	config.Write(prefix, (long)count);
	for (size_t index = 0; index < count; index++)
	{
		wxString key;
		key.Printf(wxT("%s_%d"), prefix, index);
		wxMenuItem *item = FindItemByPosition(index);
		wxString path = item->GetHelp();
		config.Write(key, path);
	}
}
