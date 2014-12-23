#include "PipelinePch.h"
#include "PipelineTrayIcon.h"
#include "MenuItem.h"

#include "ApplicationUI/FileIconsTable.h"

#include "resource.h"

#include <shellapi.h>

using namespace Helium;

PipelineTrayIcon::PipelineTrayIcon() 
	: m_Menu( NULL )
	, m_UpdateMenuItem( NULL )
	, m_BusyCount( 0 )
	, m_IsMenuShowing( false )
{
	// Connect Events
	Connect( wxID_ANY, wxEVT_TASKBAR_CLICK, wxTaskBarIconEventHandler( PipelineTrayIcon::OnTrayIconClick ), NULL, this );
	Connect( wxID_ANY, wxEVT_TASKBAR_LEFT_UP, wxTaskBarIconEventHandler( PipelineTrayIcon::OnTrayIconClick ), NULL, this );
	Connect( EventIDs::Exit, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( PipelineTrayIcon::OnMenuExit ), NULL, this );
	Connect( EventIDs::Help, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( PipelineTrayIcon::OnMenuHelp ), NULL, this );
	Connect( EventIDs::Refresh, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( PipelineTrayIcon::OnMenuRefresh ), NULL, this );
	Connect( EventIDs::Reload, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( PipelineTrayIcon::OnMenuReload ), NULL, this );
	Connect( EventIDs::Add, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( PipelineTrayIcon::OnMenuAdd ), NULL, this );
}

PipelineTrayIcon::~PipelineTrayIcon() 
{ 
	// Disconnect Events
	Disconnect( wxID_ANY, wxEVT_TASKBAR_CLICK, wxTaskBarIconEventHandler( PipelineTrayIcon::OnTrayIconClick ), NULL, this );
	Disconnect( wxID_ANY, wxEVT_TASKBAR_LEFT_UP, wxTaskBarIconEventHandler( PipelineTrayIcon::OnTrayIconClick ), NULL, this );
	Disconnect( EventIDs::Exit, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( PipelineTrayIcon::OnMenuExit ), NULL, this );
	Disconnect( EventIDs::Help, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( PipelineTrayIcon::OnMenuHelp ), NULL, this );
	Disconnect( EventIDs::Refresh, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( PipelineTrayIcon::OnMenuRefresh ), NULL, this );
	Disconnect( EventIDs::Reload, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( PipelineTrayIcon::OnMenuReload ), NULL, this );
	Disconnect( EventIDs::Add, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( PipelineTrayIcon::OnMenuAdd ), NULL, this );

	// Dynamically added
	Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( PipelineTrayIcon::OnMenuShortcut ), NULL, this );
}

void PipelineTrayIcon::Initialize()
{
	wxIcon appIcon;
	appIcon.CopyFromBitmap( wxArtProvider::GetBitmap( ArtIDs::Editor::Helium, wxART_OTHER, wxSize( 32, 32 ) ) );
	SetIcon( appIcon );

	Refresh( true );
}

void PipelineTrayIcon::Cleanup()
{
	m_MenuItems.clear();

	RemoveIcon();

	delete m_Menu;
	m_Menu = NULL;
}

void PipelineTrayIcon::OnTrayIconClick( wxTaskBarIconEvent& evt ) 
{ 
	if ( m_Menu )
	{
		m_IsMenuShowing = true;

		PopupMenu( m_Menu );

		m_IsMenuShowing = false;
	}
}

void PipelineTrayIcon::OnMenuExit( wxCommandEvent& evt )
{
	if ( wxYES == wxMessageBox( wxT( "Are you sure you would like to exit pipeline?" ), wxT( "Exit Pipeline?" ), wxYES_NO | wxCENTER | wxICON_QUESTION ) )
	{
		wxExit();
	}
}

void PipelineTrayIcon::OnMenuHelp( wxCommandEvent& evt )
{
	Helium::String about;
	about += 
		"Helium pipeline runs in the background, building game assets and providing them to tools and game instances.\n";

	wxMessageDialog dialog(
		NULL,
		*about,
		wxT( "About Helium Pipeline" ),
		wxOK | wxICON_INFORMATION );

	dialog.ShowModal();
}

void PipelineTrayIcon::OnMenuReload( wxCommandEvent& evt )
{
	wxBusyCursor wait;

	Refresh( true );
}

void PipelineTrayIcon::OnMenuRefresh( wxCommandEvent& evt )
{
	wxBusyCursor wait;

	Refresh( false );
}

void PipelineTrayIcon::OnMenuShortcut( wxCommandEvent& evt )
{
	wxMenu* projectMenu = wxDynamicCast( evt.GetEventObject(), wxMenu );

	if ( projectMenu && projectMenu->FindItem( evt.GetId() ) )
	{
		wxMenuItem* menuItemMenuItem = projectMenu->FindItem( evt.GetId() ); 
		MenuItem* menuItem = static_cast< MenuItemRefData* >( menuItemMenuItem->GetRefData() )->m_MenuItem;

		BeginBusy();
		{
			menuItem->Execute();
		}
		EndBusy();
	}
}

void PipelineTrayIcon::OnMenuAdd( wxCommandEvent& evt )
{
	wxFileDialog dlg ( NULL, "Open Eshell Project file", wxEmptyString, "eshell.xml", "*.xml", wxFD_OPEN | wxFD_FILE_MUST_EXIST );
	if ( wxID_OK == dlg.ShowModal() )
	{
		//m_Application->AddProject( tstring( dlg.GetPath().c_str() ) );
		Refresh( true );
	}
}

void PipelineTrayIcon::OnMenuRemove( wxCommandEvent& evt )
{
	wxMenu* projectMenu = wxDynamicCast( evt.GetEventObject(), wxMenu );

	if ( projectMenu && projectMenu->FindItem( evt.GetId() ) )
	{
		//wxMenuItem* menuItem = projectMenu->FindItem( evt.GetId() ); 
		//StringRefData* refData = static_cast< StringRefData* >( menuItem->GetRefData() );
		//m_Application->RemoveProject( refData->m_Value );
		Refresh( true );
	}
}

void PipelineTrayIcon::OnMenuEdit( wxCommandEvent& evt )
{
// 	wxMenu* projectMenu = wxDynamicCast( evt.GetEventObject(), wxMenu );
// 
// 	if ( projectMenu && projectMenu->FindItem( evt.GetId() ) )
// 	{
// 		wxMenuItem* menuItem = projectMenu->FindItem( evt.GetId() ); 
// 		StringRefData* refData = static_cast< StringRefData* >( menuItem->GetRefData() );
// 		EShellMenu::ExecuteCommand( tstring ( wxT("cmd.exe /c start \"\" \"") ) + refData->m_Value + wxT("\""), false );
// 	}
}

void PipelineTrayIcon::BeginBusy()
{
	if ( ++m_BusyCount > 1 )
		return;

	// Disconnect events
	Disconnect( wxID_ANY, wxEVT_TASKBAR_CLICK, wxTaskBarIconEventHandler( PipelineTrayIcon::OnTrayIconClick ), NULL, this );
	Disconnect( wxID_ANY, wxEVT_TASKBAR_LEFT_UP, wxTaskBarIconEventHandler( PipelineTrayIcon::OnTrayIconClick ), NULL, this );
	Disconnect( EventIDs::Reload, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( PipelineTrayIcon::OnMenuReload ), NULL, this );

	// Clear the current menu and change the icon to notify the user that things are happening
	//SetIcon( wxICON( BUSY_ICON ), wxString( m_Application->m_Title.c_str() ) + " Refreshing..." );
}

void PipelineTrayIcon::EndBusy()
{  
	if ( --m_BusyCount > 0 )
		return;

	assert( m_BusyCount == 0 );

	// Re-connect events
	Connect( wxID_ANY, wxEVT_TASKBAR_CLICK, wxTaskBarIconEventHandler( PipelineTrayIcon::OnTrayIconClick ), NULL, this );
	Connect( wxID_ANY, wxEVT_TASKBAR_LEFT_UP, wxTaskBarIconEventHandler( PipelineTrayIcon::OnTrayIconClick ), NULL, this );
	Connect( EventIDs::Reload, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( PipelineTrayIcon::OnMenuReload ), NULL, this );

	// Set the icon back
	//SetIcon( m_Application->IsUpdateAvailable() ? wxICON( UPDATE_ICON ) : wxICON( LOGO_ICON ), m_Application->m_Title.c_str() );
}

void PipelineTrayIcon::Refresh( bool reload )
{
	BeginBusy();
	{  
		if ( reload )
		{
			m_MenuItems.clear();
		}

		if ( !m_Menu )
		{
			m_Menu = new wxMenu();
			m_Menu->AppendSeparator();
			wxMenuItem* refreshMenuItem = new wxMenuItem( m_Menu, EventIDs::Reload, wxT( "Refresh" ), wxEmptyString, wxITEM_NORMAL );
			refreshMenuItem->SetBitmap( wxIcon( "REFRESH_ICON", wxBITMAP_TYPE_ICO_RESOURCE, 16, 16 ) );
			m_Menu->Append( refreshMenuItem );

			m_UpdateMenuItem = new wxMenuItem( m_Menu, EventIDs::Update, wxT( "Update" ), wxEmptyString, wxITEM_NORMAL );
			m_Menu->Append( m_UpdateMenuItem );

			m_Menu->Append( new wxMenuItem( m_Menu, EventIDs::Add, wxString("Add..."), wxEmptyString, wxITEM_NORMAL ) );
			m_Menu->Append( new wxMenuItem( m_Menu, EventIDs::Help, wxString("Help"), wxEmptyString, wxITEM_NORMAL ) );
			m_Menu->Append( new wxMenuItem( m_Menu, EventIDs::Exit, wxString( wxT("Exit Pipeline Menu") ) , wxEmptyString, wxITEM_NORMAL ) );
		}
		else
		{
			wxMenuItemList list = m_Menu->GetMenuItems();
			for ( wxMenuItemList::const_iterator itr = list.begin(), end = list.end(); itr != end; ++itr )
			{
				if ( (*itr)->GetId() < EventIDs::First || (*itr)->GetId() > EventIDs::Last )
				{
					m_Menu->Remove( *itr );
					delete *itr;
				}
			}
			m_Menu->PrependSeparator();
		}

		if ( !m_MenuItems.empty() )
		{
			CreateProjectsMenu( m_Menu );
		}

// 		m_UpdateMenuItem->Enable( update );
// 		if ( update )
// 		{
// 			m_UpdateMenuItem->SetBitmap( wxIcon( "UPDATE_ICON", wxBITMAP_TYPE_ICO_RESOURCE, 16, 16 ) );
// 		}
	}
	EndBusy();
}

void PipelineTrayIcon::DetectAndSetIcon( MenuItem& menuItem, wxMenuItem* actualMenuItem )
{
// 	wxFileName name ( menuItem.m_Icon );
// 
// 	if ( menuItem.m_Icon.empty() )
// 	{
// 		actualMenuItem->SetBitmap( wxIcon( "PROMPT_ICON", wxBITMAP_TYPE_ICO_RESOURCE, 16, 16 ) );
// 	}
// 	else
// 	{
// 		wxImageHandler* handler = wxImage::FindHandler( name.GetExt() );
// 		if ( handler )
// 		{
// 			wxBitmap bitmap;
// 			bitmap.LoadFile( menuItem.m_Icon, wxBITMAP_TYPE_ANY );
// 			if ( bitmap.IsOk() )
// 			{
// 				actualMenuItem->SetBitmap( bitmap );
// 			}
// 		}
// 		else
// 		{
// 			// get the icon data from the shell associations
// 			SHFILEINFO info;
// 			ZeroMemory( &info, sizeof( info ) );
// 			SHGetFileInfo( menuItem.m_Icon.c_str(), 0, &info, sizeof( info ), SHGFI_ICON | SHGFI_SMALLICON );
// 			if ( info.hIcon )
// 			{
// 				wxIcon icon;
// 				icon.SetHICON( info.hIcon );
// 				icon.SetSize( 16, 16 );
// 				actualMenuItem->SetBitmap( icon );
// 			}
// 		}
// 	}
}

void PipelineTrayIcon::CreateProjectsMenu( wxMenu* parentMenu )
{
// 	std::vector< MenuItem* > favoriteShortcuts;
// 
// 	std::map< tstring, std::pair< uint32_t, std::vector< MenuItem > > >::reverse_iterator projItr = m_MenuItems.rbegin();
// 	std::map< tstring, std::pair< uint32_t, std::vector< MenuItem > > >::reverse_iterator projEnd = m_MenuItems.rend();
// 	for ( ; projItr != projEnd; ++projItr )
// 	{
// 		const tstring& title = projItr->first;
// 		
// 		std::vector< MenuItem >& menuItems = projItr->second.second;
// 		if ( menuItems.empty() )
// 		{
// 			continue;
// 		}
// 
// 		wxMenu* projectMenu = new wxMenu();
// 
// 		typedef std::map< tstring, wxMenu* > M_SubMenues;
// 		M_SubMenues subMenus;
// 
// 		std::vector< MenuItem >::iterator menuItemItr = menuItems.begin();
// 		std::vector< MenuItem >::iterator menuItemEnd = menuItems.end();
// 		for ( ; menuItemItr != menuItemEnd; ++menuItemItr )
// 		{
// 			MenuItem& menuItem = (*menuItemItr);
// 
// 			wxMenuItem* actualMenuItem = new wxMenuItem( projectMenu, wxID_ANY,
// 				wxString( menuItem.m_Name.c_str() ),
// 				wxString( menuItem.m_Description.c_str() ),
// 				wxITEM_NORMAL );
// 
// 			if ( menuItem.m_Disable )
// 			{
// 				wxString name( "INVALID: " );
// 				name += menuItem.m_Name.c_str();
// 				actualMenuItem->SetText( name );
// 				actualMenuItem->Enable( false );
// 			}
// 
// 			DetectAndSetIcon( menuItem, actualMenuItem );
// 
// 			actualMenuItem->SetRefData( new MenuItemRefData( &menuItem ) );
// 
// 			if ( menuItem.m_Folder.empty() )
// 			{
// 				projectMenu->Append( actualMenuItem );
// 			}
// 			else
// 			{
// 				wxMenu* menuToInsert = new wxMenu();
// 				std::pair< M_SubMenues::iterator, bool > insertedSubMenu = subMenus.insert( M_SubMenues::value_type( menuItem.m_Folder, menuToInsert ) );
// 				insertedSubMenu.first->second->Append( actualMenuItem );
// 				if ( !insertedSubMenu.second )
// 				{
// 					// New menu was not inserted, you have to clean it up
// 					delete menuToInsert;
// 				}
// 			}
// 
// 			Connect( actualMenuItem->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( TrayIcon::OnMenuShortcut ), NULL, this );
// 
// // 			if ( m_Application->IsFavorite( menuItem.m_Command ) )
// // 			{
// // 				favoriteShortcuts.push_back( &menuItem );
// // 			}
// 		}
// 
// 		M_SubMenues::reverse_iterator subMenuItr = subMenus.rbegin();
// 		M_SubMenues::reverse_iterator subMenuEnd = subMenus.rend();
// 		for ( ; subMenuItr != subMenuEnd; ++subMenuItr )
// 		{
// 			wxMenuItem* subMenuItem = new wxMenuItem( projectMenu, wxID_ANY,
// 				wxString( subMenuItr->first.c_str() ),
// 				wxString( subMenuItr->first.c_str() ),
// 				wxITEM_NORMAL,
// 				subMenuItr->second );
// 
// 			subMenuItem->SetBitmap( wxIcon( "FOLDER_OPEN_ICON", wxBITMAP_TYPE_ICO_RESOURCE, 16, 16 ) );
// 			projectMenu->Prepend( subMenuItem );
// 		}
// 
// 		wxMenuItem* projectItem = parentMenu->Prepend( wxID_ANY, title, projectMenu );
// 		projectItem->SetBitmap( wxIcon( "FOLDER_OPEN_ICON", wxBITMAP_TYPE_ICO_RESOURCE, 16, 16 ) );
// 
// 		projectMenu->AppendSeparator();
// 
// 		//wxMenuItem* remove = projectMenu->Append( wxID_ANY, "Remove" );
// 		//remove->SetRefData( new StringRefData( m_Projects[ projItr->second.first ].m_File ) );
// 		//Connect( remove->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( TrayIcon::OnMenuRemove ), NULL, this );
// 
// 		//wxMenuItem* edit = projectMenu->Append( wxID_ANY, "Edit" );
// 		//edit->SetRefData( new StringRefData( m_Projects[ projItr->second.first ].m_File ) );
// 		//Connect( edit->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( TrayIcon::OnMenuEdit ), NULL, this );
// 	}
// 
// 	if ( !favoriteShortcuts.empty() )
// 	{
// 		parentMenu->PrependSeparator();
// 
// 		std::vector< MenuItem* >::reverse_iterator favItr = favoriteShortcuts.rbegin();
// 		std::vector< MenuItem* >::reverse_iterator favEnd = favoriteShortcuts.rend();
// 		for( ; favItr != favEnd; ++favItr )
// 		{
// 			MenuItem* menuItem = (*favItr);
// 
// 			wxMenuItem* favoritesMenuItem = new wxMenuItem( parentMenu, wxID_ANY,
// 				wxString( menuItem->m_FavoriteName.c_str() ),
// 				wxString( menuItem->m_Description.c_str() ),
// 				wxITEM_NORMAL );
// 
// 			DetectAndSetIcon( *menuItem, favoritesMenuItem );
// 
// 			favoritesMenuItem->SetRefData( new MenuItemRefData( menuItem ) );
// 
// 			parentMenu->Prepend( favoritesMenuItem );
// 
// 			Connect( favoritesMenuItem->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( TrayIcon::OnMenuShortcut ), NULL, this );
// 		}
// 	}
}
