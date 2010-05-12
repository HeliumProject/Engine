#include "Precompile.h"
#include "ResultsPanel.h"
#include "ThumbnailView.h"
#include "SearchResults.h"

using namespace Luna;

BEGIN_EVENT_TABLE( Luna::ResultsPanel, ResultsPanelGenerated )
END_EVENT_TABLE()

ResultsPanel::ResultsPanel( BrowserFrame* browserFrame )
: ResultsPanelGenerated( browserFrame, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL )
, m_CurrentMode( ViewModes::Invalid )
, m_CurrentView( NULL )
, m_ThumbnailView( new ThumbnailView( browserFrame, this ) )
, m_BrowserSearchPanel( new BrowserSearchPanel( browserFrame, this ) )
, m_BrowserFrame( browserFrame )
{
  wxSizer* sizer = new wxBoxSizer( wxVERTICAL );
  SetSizer( sizer );

  m_ThumbnailView->AddSelectionChangedListener( ThumbnailSelectionSignature::Delegate( this, &ResultsPanel::OnThumbnailSelectionChanged ) );
  m_ThumbnailView->AddHighlightChangedListener( ThumbnailHighlightSignature::Delegate( this, &ResultsPanel::OnThumbnailHighlightChanged ) );

  m_ThumbnailView->Hide();
  m_BrowserSearchPanel->Hide();

  SetViewMode( ViewModes::Thumbnail );
}

ResultsPanel::~ResultsPanel()
{
  m_ThumbnailView->RemoveSelectionChangedListener( ThumbnailSelectionSignature::Delegate( this, &ResultsPanel::OnThumbnailSelectionChanged ) );
  m_ThumbnailView->RemoveHighlightChangedListener( ThumbnailHighlightSignature::Delegate( this, &ResultsPanel::OnThumbnailHighlightChanged ) );
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
      m_CurrentView = m_BrowserSearchPanel;
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
  V_string unused;
  ResultChangeArgs args;

  switch ( m_CurrentMode )
  {
  case ViewModes::Thumbnail:
    m_ThumbnailView->SetResults( results );
    args.m_NumSelected = m_ThumbnailView->GetSelectedPaths( unused );
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

void ResultsPanel::SelectPath( const std::string& path )
{
  switch ( m_CurrentMode )
  {
  case ViewModes::Thumbnail:
    m_ThumbnailView->SelectPath( path );
    break;
  }
}

u32 ResultsPanel::GetSelectedPaths( V_string& paths, bool useForwardSlashes )
{
  switch ( m_CurrentMode )
  {
  case ViewModes::Thumbnail:
    m_ThumbnailView->GetSelectedPaths( paths, useForwardSlashes );
    break;
  }
  return static_cast< u32 >( paths.size() );
}

void ResultsPanel::GetSelectedFilesAndFolders( Asset::V_AssetFiles& files, Asset::V_AssetFolders& folders )
{
  switch ( m_CurrentMode )
  {
  case ViewModes::Thumbnail:
    m_ThumbnailView->GetSelectedFilesAndFolders( files, folders );
    break;
  }
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
    numFiles = static_cast< u32 >( results->GetFiles().size() );
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
    numFolders = static_cast< u32 >( results->GetFolders().size() );
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
  V_string unused;
  ResultChangeArgs combinedArgs( m_ThumbnailView->GetSelectedPaths( unused ), args.m_HighlightPath );
  m_ResultsChanged.Raise( combinedArgs );
}
