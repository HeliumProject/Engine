#pragma once

#include "MenuItem.h"

#include <wx/taskbar.h>

namespace Helium
{
	namespace EventIDs
	{
		enum EventID
		{
			First = wxID_HIGHEST + 1,
			Exit = First,
			Help,
			Refresh,
			Reload,
			Update,
			Add,
			Last = Add,
		};
	}

	class PipelineTrayIcon : public wxTaskBarIcon
	{
	public:
		PipelineTrayIcon();
		virtual ~PipelineTrayIcon();

		void Initialize();    
		void Cleanup();

		bool IsMenuShowing() const
		{
			return m_IsMenuShowing;
		}

		void BeginBusy();
		void EndBusy();
		void Refresh( bool reload );

	protected:
		void OnTrayIconClick( wxTaskBarIconEvent& evt );
		void OnMenuExit( wxCommandEvent& evt );
		void OnMenuHelp( wxCommandEvent& evt );
		void OnMenuRefresh( wxCommandEvent& evt );
		void OnMenuReload( wxCommandEvent& evt );
		void OnMenuShortcut( wxCommandEvent& evt );
		void OnMenuAdd( wxCommandEvent& evt );
		void OnMenuRemove( wxCommandEvent& evt );
		void OnMenuEdit( wxCommandEvent& evt );

	private:
		void DetectAndSetIcon( MenuItem& menuItem, wxMenuItem* actualMenuItem );
		void CreateProjectsMenu( wxMenu* parentMenu );

	private:
		std::map< Helium::String, std::pair< uint32_t, std::vector< MenuItem > > > m_MenuItems;
		wxMenu* m_Menu;
		wxMenuItem* m_UpdateMenuItem;
		int m_BusyCount;
		bool m_IsMenuShowing;
	};
}
