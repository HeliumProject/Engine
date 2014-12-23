#pragma once

#include "Foundation/String.h"

#include <wx/object.h>

namespace Helium
{
	class MenuItem
	{
	public:
		MenuItem();
		virtual ~MenuItem();

		virtual bool Execute();

	public:
		String m_Name;             // Display name
		String m_FavoriteName;     // Display name for when the shortcut is in the favorites menu
		String m_Icon;             // Optional MenuIcon
		String m_Description;      // Mouse over description
		String m_Folder;           // Optional name of sub menu
		String m_Command;          // Command complete with Arguments

		bool m_Disable;				// true if we think the shortcut will not load
		String m_DisableReason;    // the reason this might fail
	};

	class MenuItemRefData : public wxObjectRefData
	{
	public:
		MenuItemRefData( MenuItem* item )
			: m_MenuItem( item )
		{

		}

		MenuItem* m_MenuItem;
	};
}