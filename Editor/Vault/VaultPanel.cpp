#include "EditorPch.h"

#include "VaultPanel.h"
#include "ApplicationUI/ArtProvider.h"

#include "ListResultsView.h"
#include "ThumbnailView.h"

#include "Editor/App.h"
#include "Editor/Controls/MenuButton.h"
#include "Editor/Vault/VaultSettings.h"

using namespace Helium;
using namespace Helium::Editor;

VaultPanel::VaultPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
	: VaultPanelGenerated( parent, id, pos, size, style )
	, m_VaultSettings( NULL )
	, m_ThumbnailView( NULL )
	, m_CurrentView( NULL )
	, m_CurrentViewMode( VaultViewMode::None )
{
	// TODO: Remove this block of code if/when wxFormBuilder supports wxArtProvider
	{
		Freeze();

		m_OptionsButton->SetBitmap( wxArtProvider::GetBitmap( ArtIDs::Actions::Options, wxART_OTHER, wxSize(16, 16) ) );

		m_ListResultsView = new ListResultsView( m_ResultsPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
		m_ListResultsView->Hide();

		m_ThumbnailView = new ThumbnailView( m_ResultsPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
		m_ThumbnailView->Hide();

		SetViewMode( VaultViewMode::Details );

		Layout();
		Thaw();
	}

	{
		m_OptionsMenu = new wxMenu();

		wxMenuItem* detailsMenuItem = new wxMenuItem(
			m_OptionsMenu,
			VaultMenu::ViewResultDetails,
			VaultMenu::Label( VaultMenu::ViewResultDetails ),
			VaultMenu::Label( VaultMenu::ViewResultDetails ),
			wxITEM_RADIO );
		m_OptionsMenu->Append( detailsMenuItem );
		Connect( VaultMenu::ViewResultDetails, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( VaultPanel::OnOptionsMenuSelect ), NULL, this );

		wxMenuItem* listMenuItem = new wxMenuItem(
			m_OptionsMenu,
			VaultMenu::ViewResultList,
			VaultMenu::Label( VaultMenu::ViewResultList ),
			VaultMenu::Label( VaultMenu::ViewResultList ),
			wxITEM_RADIO );
		m_OptionsMenu->Append( listMenuItem );
		Connect( VaultMenu::ViewResultList, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( VaultPanel::OnOptionsMenuSelect ), NULL, this );

		wxMenuItem* smallMenuItem = new wxMenuItem(
			m_OptionsMenu,
			VaultMenu::ViewThumbnailsSmall,
			VaultMenu::Label( VaultMenu::ViewThumbnailsSmall ) + std::string( " " ) + VaultThumbnailsSizes::Label( VaultThumbnailsSizes::Small ),
			VaultMenu::Label( VaultMenu::ViewThumbnailsSmall ),
			wxITEM_RADIO );
		m_OptionsMenu->Append( smallMenuItem );
		Connect( VaultMenu::ViewThumbnailsSmall, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( VaultPanel::OnOptionsMenuSelect ), NULL, this );

		wxMenuItem* mediumMenuItem = new wxMenuItem( 
			m_OptionsMenu, 
			VaultMenu::ViewThumbnailsMedium, 
			VaultMenu::Label( VaultMenu::ViewThumbnailsMedium ) + std::string( " " ) + VaultThumbnailsSizes::Label( VaultThumbnailsSizes::Medium ),
			VaultMenu::Label( VaultMenu::ViewThumbnailsMedium ),
			wxITEM_RADIO );
		m_OptionsMenu->Append( mediumMenuItem );
		Connect( VaultMenu::ViewThumbnailsMedium, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( VaultPanel::OnOptionsMenuSelect ), NULL, this );

		wxMenuItem* largeMenuItem = new wxMenuItem(
			m_OptionsMenu,
			VaultMenu::ViewThumbnailsLarge,
			VaultMenu::Label( VaultMenu::ViewThumbnailsLarge ) + std::string( " " ) + VaultThumbnailsSizes::Label( VaultThumbnailsSizes::Large ),
			VaultMenu::Label( VaultMenu::ViewThumbnailsLarge ),
			wxITEM_RADIO );
		m_OptionsMenu->Append( largeMenuItem );
		Connect( VaultMenu::ViewThumbnailsLarge, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( VaultPanel::OnOptionsMenuSelect ), NULL, this );
	}

	m_OptionsButton->SetContextMenu( m_OptionsMenu );
	m_OptionsButton->SetHoldDelay( 0.0f );
	m_OptionsButton->Connect( wxEVT_MENU_OPEN, wxMenuEventHandler( VaultPanel::OnOptionsMenuOpen ), NULL, this );
	m_OptionsButton->Connect( wxEVT_MENU_CLOSE, wxMenuEventHandler( VaultPanel::OnOptionsMenuClose ), NULL, this );
	m_OptionsButton->Enable( true );

	// settings
	m_VaultSettings = wxGetApp().GetSettingsManager()->GetSettings< VaultSettings >();
	if ( m_VaultSettings )
	{
		SetViewMode( m_VaultSettings->m_VaultViewMode );
		m_CurrentThumbnailSize = m_VaultSettings->m_ThumbnailSize;
		Clamp( m_CurrentThumbnailSize, VaultThumbnailsSizes::Small, VaultThumbnailsSizes::Large );
		//m_VaultSettings->m_WindowSettings->ApplyToWindow( this, m_FrameManager, true );
	}

	m_VaultSearch.AddSearchResultsAvailableListener( Editor::SearchResultsAvailableSignature::Delegate( this, &VaultPanel::OnSearchResultsAvailable ) );
}

VaultPanel::~VaultPanel()
{
	Disconnect( VaultMenu::ViewResultDetails, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( VaultPanel::OnOptionsMenuSelect ), NULL, this );
	Disconnect( VaultMenu::ViewResultList, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( VaultPanel::OnOptionsMenuSelect ), NULL, this );
	Disconnect( VaultMenu::ViewThumbnailsSmall, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( VaultPanel::OnOptionsMenuSelect ), NULL, this );
	Disconnect( VaultMenu::ViewThumbnailsMedium, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( VaultPanel::OnOptionsMenuSelect ), NULL, this );
	Disconnect( VaultMenu::ViewThumbnailsLarge, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( VaultPanel::OnOptionsMenuSelect ), NULL, this );

	m_OptionsButton->Disconnect( wxEVT_MENU_OPEN, wxMenuEventHandler( VaultPanel::OnOptionsMenuOpen ), NULL, this );
	m_OptionsButton->Disconnect( wxEVT_MENU_CLOSE, wxMenuEventHandler( VaultPanel::OnOptionsMenuClose ), NULL, this );

	m_VaultSettings = NULL;

	m_VaultSearch.AddSearchResultsAvailableListener( Editor::SearchResultsAvailableSignature::Delegate( this, &VaultPanel::OnSearchResultsAvailable ) );

	delete m_ThumbnailView;
}

///////////////////////////////////////////////////////////////////////////////
// Sets the string in the NavBar and starts the search query.
void VaultPanel::Search( const std::string& queryString )
{
	wxBusyCursor bc;
	if ( queryString.empty() )
	{
		return;
	}

	if ( wxGetApp().GetFrame()->GetProject().empty() )
	{
		wxMessageBox( wxT( "You must have a project loaded to use the vault." ), TXT( "Project Not Loaded" ), wxCENTER | wxICON_WARNING | wxOK, this );
		return;
	}

	m_SearchCtrl->Clear();
	m_SearchCtrl->SetValue( queryString.c_str() );

	// parse the query string
	VaultSearchQueryPtr query = new VaultSearchQuery();

	std::string errors;
	if ( !query->SetQueryString( queryString, errors ) )
	{
		wxMessageBox( errors.c_str(), TXT( "Search Errors" ), wxCENTER | wxICON_WARNING | wxOK, this );
		return;
	}

	m_VaultSearch.SetProject( wxGetApp().GetFrame()->GetProject() );
	m_VaultSearch.StartSearchThread( query );
}

///////////////////////////////////////////////////////////////////////////////
void VaultPanel::SetViewMode( VaultViewMode view )
{
	if ( view != m_CurrentViewMode )
	{
		// Detach
		if ( m_CurrentView )
		{
			m_CurrentView->Hide();
			m_ResultsPanel->GetSizer()->Detach( m_CurrentView );
			m_CurrentView = NULL;
		}

		// Switch
		m_CurrentViewMode = view;
		switch ( m_CurrentViewMode )
		{
		default:
		case VaultViewMode::Details:
			{
				m_ListResultsView->ShowDetails( true );
				m_CurrentView = m_ListResultsView;
			}
			break;

		case VaultViewMode::List:
			{
				m_ListResultsView->ShowDetails( false );
				m_CurrentView = m_ListResultsView;
			}
			break;

		case VaultViewMode::ThumbnailsSmall:
			{
				m_CurrentThumbnailSize = VaultThumbnailsSizes::Small;
				m_ThumbnailView->SetZoom( VaultThumbnailsSizes::Small );
				m_CurrentView = m_ThumbnailView;
			}
			break;
		case VaultViewMode::ThumbnailsMedium:
			{
				m_CurrentThumbnailSize = VaultThumbnailsSizes::Medium;
				m_ThumbnailView->SetZoom( VaultThumbnailsSizes::Medium );
				m_CurrentView = m_ThumbnailView;
			}
			break;
		case VaultViewMode::ThumbnailsLarge:
			{
				m_CurrentThumbnailSize = VaultThumbnailsSizes::Large;
				m_ThumbnailView->SetZoom( VaultThumbnailsSizes::Large );
				m_CurrentView = m_ThumbnailView;
			}
			break;
		case VaultViewMode::ThumbnailsCustom:
			{
				m_ThumbnailView->SetZoom( m_CurrentThumbnailSize );
				m_CurrentView = m_ThumbnailView;
			}
			break;
		}

		// Attach
		if ( m_CurrentView )
		{
			m_CurrentView->Show();
			m_CurrentView->Layout();

			m_ResultsPanel->GetSizer()->Add( m_CurrentView, 1, wxALL | wxEXPAND, 0 );
			GetSizer()->FitInside( this );
			Layout();
		}

		// Redo any existing search to force a re-render
		StartSearchFromField();
	}
}

///////////////////////////////////////////////////////////////////////////////
VaultViewMode VaultPanel::GetViewMode() const
{
	return m_CurrentViewMode;
}

void VaultPanel::SetResults( VaultSearchResults* results )
{
	switch ( m_CurrentViewMode )
	{
	default:
	case VaultViewMode::Details:
	case VaultViewMode::List:
		m_ListResultsView->SetResults( results );
		break;

	case VaultViewMode::ThumbnailsSmall:
	case VaultViewMode::ThumbnailsMedium:
	case VaultViewMode::ThumbnailsLarge:
	case VaultViewMode::ThumbnailsCustom:
		m_ThumbnailView->SetResults( results );
		break;
	}
}

void VaultPanel::ClearResults()
{
	switch ( m_CurrentViewMode )
	{
	default:
	case VaultViewMode::Details:
	case VaultViewMode::List:
		m_ListResultsView->ClearResults();
		break;

	case VaultViewMode::ThumbnailsSmall:
	case VaultViewMode::ThumbnailsMedium:
	case VaultViewMode::ThumbnailsLarge:
	case VaultViewMode::ThumbnailsCustom:
		m_ThumbnailView->ClearResults();
		break;
	}
}

void VaultPanel::SelectPath( const Helium::FilePath& path )
{
	switch ( m_CurrentViewMode )
	{
	default:
	case VaultViewMode::Details:
	case VaultViewMode::List:
		m_ListResultsView->SelectPath( path );
		break;

	case VaultViewMode::ThumbnailsSmall:
	case VaultViewMode::ThumbnailsMedium:
	case VaultViewMode::ThumbnailsLarge:
	case VaultViewMode::ThumbnailsCustom:
		m_ThumbnailView->SelectPath( path );
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////
uint32_t VaultPanel::GetSelectedPaths( std::set< Helium::FilePath >& paths )
{
	switch ( m_CurrentViewMode )
	{
	default:
	case VaultViewMode::Details:
		HELIUM_BREAK();
		break;

	case VaultViewMode::List:
		HELIUM_BREAK();
		break;

	case VaultViewMode::ThumbnailsSmall:
	case VaultViewMode::ThumbnailsMedium:
	case VaultViewMode::ThumbnailsLarge:
	case VaultViewMode::ThumbnailsCustom:
		m_ThumbnailView->GetSelectedPaths( paths );
		break;
	}

	return static_cast< uint32_t >( paths.size() );
}

///////////////////////////////////////////////////////////////////////////////
void VaultPanel::SaveSettings()
{
	if ( m_VaultSettings )
	{
		m_VaultSettings->m_VaultViewMode = m_CurrentViewMode;
		m_VaultSettings->m_ThumbnailSize = m_CurrentThumbnailSize;
		Clamp( m_VaultSettings->m_ThumbnailSize, VaultThumbnailsSizes::Small, VaultThumbnailsSizes::Large );
	}
}

///////////////////////////////////////////////////////////////////////////////
void VaultPanel::OnSearchResultsAvailable( const Editor::SearchResultsAvailableArgs& args )
{
	SetResults( args.m_SearchResults );
}

///////////////////////////////////////////////////////////////////////////////
void VaultPanel::StartSearchFromField()
{
	wxString queryString = m_SearchCtrl->GetLineText(0);
	queryString.Trim(true);  // trim white-space right 
	queryString.Trim(false); // trim white-space left

	Search( std::string( queryString.c_str() ) );
}

///////////////////////////////////////////////////////////////////////////////
void VaultPanel::OnSearchCancelButtonClick( wxCommandEvent& event )
{
	m_VaultSearch.StopSearchThreadAndWait();
	event.Skip(false);
}

void VaultPanel::OnSearchGoButtonClick( wxCommandEvent& event )
{
	StartSearchFromField();
	event.Skip(false);
}

void VaultPanel::OnSearchTextEnter( wxCommandEvent& event )
{
	StartSearchFromField();
	event.Skip(false);
}

void VaultPanel::OnOptionsMenuOpen( wxMenuEvent& event )
{
	event.Skip();
	if ( event.GetMenu() == m_OptionsMenu )
	{
		for ( wxMenuItemList::iterator itr = event.GetMenu()->GetMenuItems().begin(), end = event.GetMenu()->GetMenuItems().end(); itr != end; ++itr )
		{
			(*itr)->Check( false );
		}

		HELIUM_ASSERT( m_CurrentViewMode != VaultViewMode::None );

		int id = -1;
		switch ( m_CurrentViewMode )
		{
		default:
		case VaultViewMode::List:
			id = VaultMenu::ViewResultList;
			break;
		case VaultViewMode::Details:
			id = VaultMenu::ViewResultDetails;
			break;
		case VaultViewMode::ThumbnailsSmall:
			id = VaultMenu::ViewThumbnailsSmall;
			break;
		case VaultViewMode::ThumbnailsMedium:
			id = VaultMenu::ViewThumbnailsMedium;
			break;
		case VaultViewMode::ThumbnailsLarge:
			id = VaultMenu::ViewThumbnailsLarge;
			break;
		}

		event.GetMenu()->Check( id, true );
	}
}

void VaultPanel::OnOptionsMenuClose( wxMenuEvent& event )
{
	m_SearchCtrl->SetFocus();
	event.Skip();
}

void VaultPanel::OnOptionsMenuSelect( wxCommandEvent& event )
{
	event.Skip();

	VaultViewMode id = VaultViewMode::Details;

	switch( event.GetId() )
	{
	default:
	case VaultMenu::ViewResultDetails:
		id = VaultViewMode::Details;
		break;

	case VaultMenu::ViewResultList:
		id = VaultViewMode::List;
		break;

	case VaultMenu::ViewThumbnailsSmall:
		id = VaultViewMode::ThumbnailsSmall;
		break;

	case VaultMenu::ViewThumbnailsMedium:
		id = VaultViewMode::ThumbnailsMedium;
		break;

	case VaultMenu::ViewThumbnailsLarge:
		id = VaultViewMode::ThumbnailsLarge;
		break;
	};

	SetViewMode( id );
}

void VaultPanel::OnClose( wxCloseEvent& event )
{
	SaveSettings();
}
