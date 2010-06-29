#pragma once

class RecentMenu: public wxMenu
{
public:
	RecentMenu(int firstid, int lastid, const wxString& empty = wxT("Empty"));
	void Put(const wxString& path);
	bool Get(int id, wxString& path);
	void Remove(const wxString& path);
	void Load(wxConfigBase& config, const wxString& prefix);
	void Save(wxConfigBase& config, const wxString& prefix);

private:
	int m_FirstId, m_LastId;
	wxString m_Empty;
};

