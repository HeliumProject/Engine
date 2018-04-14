#include "EditorPch.h"

#include "ListResultsView.h"
#include "VaultSearch.h"

#include "wx/generic/dirctrlg.h" // for wxTheFileIconsTable

using namespace Helium::Editor;

std::string DetailsColumn::FilePath( const Helium::FilePath& path )
{
    return path.Get();
}

std::string DetailsColumn::Filename( const Helium::FilePath& path )
{
    return path.Filename().Get();
}

std::string DetailsColumn::Directory( const Helium::FilePath& path )
{
    return path.Directory().Get();
}

std::string DetailsColumn::Size( const Helium::FilePath& path )
{
	Status status;
	status.Read( path.Get().c_str() );
	int64_t size = status.m_Size;

    std::stringstream printSize;
    if ( size == 0 )
    {
      printSize << "0 KB";
    }
    else if ( size <= 1024 )
    {
      printSize << "1 KB";
    }
    else
    {
      size = size / 1024;
      printSize << size << " KB";
    }

    return printSize.str().c_str();
}

ListResultsView::ListResultsView( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
: wxPanel( parent, id, pos, size, style )
, m_ShowDetails( false )
, m_ListCtrl( NULL )
{
  wxSizer* sizer = new wxBoxSizer( wxVERTICAL );

  m_ListCtrl = new SortableListView( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT );
  sizer->Add( m_ListCtrl, 1, wxALL|wxEXPAND, 5 );

  wxImageList *imageList = wxTheFileIconsTable->GetSmallImageList();
  m_ListCtrl->SetImageList( imageList, wxIMAGE_LIST_SMALL );

  m_DetailsColumns[DetailsColumnTypes::FilePath] = DetailsColumn( DetailsColumnTypes::FilePath, "FilePath", 600, DetailsColumn::FilePath );
  //m_DetailsColumns[DetailsColumnTypes::Filename] = DetailsColumn( DetailsColumnTypes::Filename, "Name", 200, DetailsColumn::Filename );
  //m_DetailsColumns[DetailsColumnTypes::Directory] = DetailsColumn( DetailsColumnTypes::Directory, "Directory", 250, DetailsColumn::Directory );
  //m_DetailsColumns[DetailsColumnTypes::Size] = DetailsColumn( DetailsColumnTypes::Size, "Size", 50, DetailsColumn::Size );

  m_DetailsColumnList.clear();
  m_DetailsColumnList.push_back( DetailsColumnTypes::FilePath );
  //m_DetailsColumnList.push_back( DetailsColumnTypes::Filename );
  //m_DetailsColumnList.push_back( DetailsColumnTypes::Directory );
  //m_DetailsColumnList.push_back( DetailsColumnTypes::Size );

  InitResults();

  m_ListCtrl->Show();

  SetSizer( sizer );
  Layout();
  //sizer->Fit( this );
}

ListResultsView::~ListResultsView()
{
    m_ListCtrl->ClearAll();
    m_ListCtrl->Destroy();
    m_ListCtrl = NULL;

    m_Results = NULL;
    m_DetailsColumns.clear();
    m_DetailsColumnList.clear();
}

///////////////////////////////////////////////////////////////////////////////
void ListResultsView::InitResults()
{
    wxBusyCursor bc;

    m_Results = NULL;
    m_ListCtrl->Freeze();
    m_ListCtrl->EnableSorting( false );
        
    m_ListCtrl->ClearAll();
    m_CurrentFileIndex = 0;

    int colIndex = 0;
    for ( V_DetailsColumnType::const_iterator colItr = m_DetailsColumnList.begin(),
        colEndItr = m_DetailsColumnList.end();
        colItr != colEndItr; ++colItr, ++colIndex )
    {
        m_ListCtrl->InsertColumn( colIndex, m_DetailsColumns[*colItr].Name().c_str(), wxLC_ALIGN_LEFT );
        m_ListCtrl->SetColumnWidth( colIndex, m_DetailsColumns[*colItr].Width() );
    }

    m_ListCtrl->EnableSorting( true );
    m_ListCtrl->Thaw();
}

///////////////////////////////////////////////////////////////////////////////
void ListResultsView::SetResults( VaultSearchResults* results )
{
    if ( m_Results.Ptr() != results )
    {
        m_Results = results;
        InitResults();

        m_ListCtrl->Freeze();
        {
            m_ListCtrl->EnableSorting( false );

#ifdef TRACKER_REFACTOR
            const std::set< TrackedFile >& foundFiles = results->GetResults();
            for ( std::set< TrackedFile >::const_iterator itr = foundFiles.begin(), end = foundFiles.end(); itr != end; ++itr )
            {
                const TrackedFile& foundFile = (*itr);
                FilePath path( foundFile.mPath.value() );

                // File Icon
                int32_t imageIndex = wxFileIconsTable::file;
                std::string fileExtension = path.Extension();
                if ( !fileExtension.empty() )
                {
                    imageIndex = wxTheFileIconsTable->GetIconID( fileExtension.c_str() );
                }

                // Basename
                wxString buf;
                buf.Printf( wxT( "%s" ), path.Get().c_str() );
                int32_t rowIndex = m_ListCtrl->InsertItem( m_CurrentFileIndex, buf, imageIndex );
                HELIUM_ASSERT( rowIndex != -1 );
                m_ListCtrl->SetItemData( rowIndex, m_CurrentFileIndex );

                if ( foundFile.mBroken.value() )
                {
                    m_ListCtrl->SetItemTextColour( rowIndex, *wxRED );
                }

                //////////////////////
                // insert the data index and file info pointer into the m_FileInfoIndexTable
                //m_FileInfoIndexTable[m_CurrentFileIndex] = (*insertSet.first);
                ++m_CurrentFileIndex;

                //////////////////////
                // populate the rest of the row
                if ( m_ShowDetails )
                {
                    int colIndex = 0;
                    for ( V_DetailsColumnType::const_iterator colItr = m_DetailsColumnList.begin(),
                        colEndItr = m_DetailsColumnList.end();
                        colItr != colEndItr; ++colItr, ++colIndex )
                    {
                        m_ListCtrl->SetItem( rowIndex, colIndex, m_DetailsColumns[*colItr].Data( path ).c_str() );
                    }
                }
            }
#endif
            m_ListCtrl->EnableSorting( true );
        }
        m_ListCtrl->Thaw();
    }
}

///////////////////////////////////////////////////////////////////////////////
void ListResultsView::ClearResults()
{
    SetResults( NULL );
}

///////////////////////////////////////////////////////////////////////////////
const VaultSearchResults* ListResultsView::GetResults() const
{
    return m_Results;
}

///////////////////////////////////////////////////////////////////////////////
void ListResultsView::SelectPath( const Helium::FilePath& path )
{
}

///////////////////////////////////////////////////////////////////////////////
void ListResultsView::GetSelectedPaths( std::set< Helium::FilePath >& paths )
{
}

///////////////////////////////////////////////////////////////////////////////
std::string ListResultsView::GetHighlightedPath() const
{
    std::string holding;
    return holding;
}

///////////////////////////////////////////////////////////////////////////////
void ListResultsView::ShowDetails( bool showDetails )
{
    if ( showDetails != m_ShowDetails )
    {
        m_ShowDetails = showDetails;
    }
}

///////////////////////////////////////////////////////////////////////////////
VaultSortMethod ListResultsView::GetSortMethod() const
{
    return m_SortMethod;
}

///////////////////////////////////////////////////////////////////////////////
void ListResultsView::Sort( VaultSortMethod method, uint32_t sortOptions )
{
    // Only sort if we are being forced to, or if the sort method is actually changing
    if ( method != m_SortMethod || ( sortOptions & VaultSortOptions::Force ) )
    {
        wxBusyCursor busyCursor;

        m_SortMethod = method;

        switch( m_SortMethod )
        {
        default:
        case VaultSortMethods::AlphabeticalByName:
            m_ListCtrl->SortItems( 0 );
            break;

        case VaultSortMethods::AlphabeticalByType:
            //m_ListCtrl->SortItems( 1 );
            break;
        };

        // Only refresh if the option is set
        if ( sortOptions & VaultSortOptions::Refresh )
        {
            Refresh();
        }
    }
}