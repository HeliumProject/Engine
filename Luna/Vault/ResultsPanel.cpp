#include "Precompile.h"
#include "ResultsPanel.h"
#include "ThumbnailView.h"
#include "SearchResults.h"

using namespace Luna;

BEGIN_EVENT_TABLE( Luna::ResultsPanel, ResultsPanelGenerated )
END_EVENT_TABLE()

ResultsPanel::ResultsPanel( const tstring& rootDirectory, VaultFrame* browserFrame )
: ResultsPanelGenerated( browserFrame, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL )
, m_RootDirectory( rootDirectory )
, m_CurrentMode( ViewModes::Invalid )
, m_CurrentView( NULL )
, m_VaultSearchPanel( new VaultSearchPanel( browserFrame, this ) )
, m_VaultFrame( browserFrame )
{
    wxSizer* sizer = new wxBoxSizer( wxVERTICAL );
    SetSizer( sizer );

    Nocturnal::Path thumbnailPath( m_RootDirectory + TXT( "/.thumbnails/" ) );
    m_ThumbnailView = new ThumbnailView( thumbnailPath.Get(), browserFrame, this );
    m_ThumbnailView->AddSelectionChangedListener( ThumbnailSelectionSignature::Delegate( this, &ResultsPanel::OnThumbnailSelectionChanged ) );
    m_ThumbnailView->AddHighlightChangedListener( ThumbnailHighlightSignature::Delegate( this, &ResultsPanel::OnThumbnailHighlightChanged ) );

    m_ThumbnailView->Hide();
    m_VaultSearchPanel->Hide();

    SetViewMode( ViewModes::Thumbnail );
}

ResultsPanel::~ResultsPanel()
{
    m_ThumbnailView->RemoveSelectionChangedListener( ThumbnailSelectionSignature::Delegate( this, &ResultsPanel::OnThumbnailSelectionChanged ) );
    m_ThumbnailView->RemoveHighlightChangedListener( ThumbnailHighlightSignature::Delegate( this, &ResultsPanel::OnThumbnailHighlightChanged ) );
    delete m_ThumbnailView;

    delete m_VaultSearchPanel;
}

void ResultsPanel::SetViewMode( ViewMode view )
{
    if ( view != m_CurrentMode )
    {
        // Detach
        if ( m_CurrentView )
        {
            m_CurrentView->Hide();
            GetSizer()->Detach( m_CurrentView );
            m_CurrentView = NULL;
        }

        // Switch
        m_CurrentMode = view;
        switch ( m_CurrentMode )
        {
        case ViewModes::Thumbnail:
            m_CurrentView = m_ThumbnailView;
            break;

        case ViewModes::AdvancedSearch:
            m_CurrentView = m_VaultSearchPanel;
            break;

        default:
            m_CurrentView = NULL;
            break;
        }

        // Attach
        if ( m_CurrentView )
        {
            m_CurrentView->Show();
            m_CurrentView->Layout();

            GetSizer()->Add( m_CurrentView, 1, wxALL | wxEXPAND, 0 );
            GetSizer()->FitInside( this );
            Layout();
        }
    }
}

ViewMode ResultsPanel::GetViewMode() const
{
    return m_CurrentMode;
}

void ResultsPanel::SetThumbnailSize( u16 zoom )
{
    m_ThumbnailView->SetZoom( zoom );
}

void ResultsPanel::SetThumbnailSize( ThumbnailSize zoom )
{
    m_ThumbnailView->SetZoom( zoom );
}

void ResultsPanel::SetResults( SearchResults* results )
{
    ResultChangeArgs args;

    switch ( m_CurrentMode )
    {
    case ViewModes::Thumbnail:
        m_ThumbnailView->SetResults( results );
        std::set< Nocturnal::Path > paths;
        m_ThumbnailView->GetSelectedPaths( paths );
        args.m_NumSelected = paths.size();
        args.m_HighlightPath = m_ThumbnailView->GetHighlightedPath();
        break;
    }

    m_ResultsChanged.Raise( args );
}

void ResultsPanel::ClearResults()
{
    switch ( m_CurrentMode )
    {
    case ViewModes::Thumbnail:
        m_ThumbnailView->ClearResults();
        break;
    }

    ResultChangeArgs args;
    m_ResultsChanged.Raise( args );
}

void ResultsPanel::SelectPath( const tstring& path )
{
    switch ( m_CurrentMode )
    {
    case ViewModes::Thumbnail:
        m_ThumbnailView->SelectPath( path );
        break;
    }
}

u32 ResultsPanel::GetSelectedPaths( std::set< Nocturnal::Path >& paths )
{
    switch ( m_CurrentMode )
    {
    case ViewModes::Thumbnail:
        m_ThumbnailView->GetSelectedPaths( paths );
        break;
    }
    return static_cast< u32 >( paths.size() );
}

u32 ResultsPanel::GetNumFiles() const
{
    const SearchResults* results = NULL;

    switch ( m_CurrentMode )
    {
    case ViewModes::Thumbnail:
        results = m_ThumbnailView->GetResults();
        break;
    }

    u32 numFiles = 0;
    if ( results )
    {
        const std::map< u64, Nocturnal::Path >& paths = results->GetPathsMap();
        for( std::map< u64, Nocturnal::Path >::const_iterator itr = paths.begin(), end = paths.end(); itr != end; ++itr )
        {
            if ( !(*itr).second.IsDirectory() )
            {
                ++numFiles;
            }
        }
    }
    return numFiles;
}

u32 ResultsPanel::GetNumFolders() const
{
    const SearchResults* results = NULL;

    switch ( m_CurrentMode )
    {
    case ViewModes::Thumbnail:
        results = m_ThumbnailView->GetResults();
        break;
    }

    u32 numFolders = 0;
    if ( results )
    {
        const std::map< u64, Nocturnal::Path >& paths = results->GetPathsMap();
        for( std::map< u64, Nocturnal::Path >::const_iterator itr = paths.begin(), end = paths.end(); itr != end; ++itr )
        {
            if ( (*itr).second.IsDirectory() )
            {
                ++numFolders;
            }
        }
    }
    return numFolders;
}

void ResultsPanel::OnThumbnailSelectionChanged( const ThumbnailSelectionArgs& args )
{
    ResultChangeArgs combinedArgs( args.m_NumSelected, m_ThumbnailView->GetHighlightedPath() );
    m_ResultsChanged.Raise( combinedArgs );
}

void ResultsPanel::OnThumbnailHighlightChanged( const ThumbnailHighlightArgs& args )
{
    std::set< Nocturnal::Path > paths;
    m_ThumbnailView->GetSelectedPaths( paths );
    ResultChangeArgs combinedArgs( paths.size(), args.m_HighlightPath );
    m_ResultsChanged.Raise( combinedArgs );
}
