#include "Precompile.h"

#include "ListResultsView.h"
#include "VaultSearch.h"

#include "wx/generic/dirctrlg.h" // for wxTheFileIconsTable

using namespace Helium::Editor;

//int wxCALLBACK MyCompareFunction(long item1, long item2, wxIntPtr WXUNUSED(sortData))
//{
//    // inverse the order
//    if (item1 < item2)
//        return -1;
//    if (item1 > item2)
//        return 1;
//
//    return 0;
//}

///////////////////////////////////////////////////////////////////////////////
tstring DetailsColumn::Path( const Helium::Path& path )
{
    return path.Get();
}


///////////////////////////////////////////////////////////////////////////////
tstring DetailsColumn::Filename( const Helium::Path& path )
{
    return path.Filename();
}

///////////////////////////////////////////////////////////////////////////////
tstring DetailsColumn::Directory( const Helium::Path& path )
{
    return path.Directory();
}

///////////////////////////////////////////////////////////////////////////////
tstring DetailsColumn::Size( const Helium::Path& path )
{
    i64 size = path.Size();

    tstringstream printSize;
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


///////////////////////////////////////////////////////////////////////////////
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

  if ( m_DetailsColumns.empty() )
  {
      m_DetailsColumns[DetailsColumnTypes::Path] = DetailsColumn( DetailsColumnTypes::Path, TXT( "Path" ), 600, DetailsColumn::Path );
      //m_DetailsColumns[DetailsColumnTypes::Filename] = DetailsColumn( DetailsColumnTypes::Filename, TXT( "Name" ), 200, DetailsColumn::Filename );
      //m_DetailsColumns[DetailsColumnTypes::Directory] = DetailsColumn( DetailsColumnTypes::Directory, TXT( "Directory" ), 250, DetailsColumn::Directory );
      //m_DetailsColumns[DetailsColumnTypes::Size] = DetailsColumn( DetailsColumnTypes::Size, TXT( "Size" ), 50, DetailsColumn::Size );
  }

  m_DetailsColumnList.clear();
  m_DetailsColumnList.push_back( DetailsColumnTypes::Path );
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

            const std::map< u64, Helium::Path >& foundFiles = results->GetPathsMap();
            for ( std::map< u64, Helium::Path >::const_iterator itr = foundFiles.begin(), end = foundFiles.end(); itr != end; ++itr )
            {
                //AddResult( file );
                const Helium::Path& foundFile = itr->second;

                // File Icon
                i32 imageIndex = wxFileIconsTable::file;
                tstring fileExtension = foundFile.Extension();
                if ( !fileExtension.empty() )
                {
                    imageIndex = wxTheFileIconsTable->GetIconID( fileExtension.c_str() );
                }

                // Basename
                wxString buf;
                buf.Printf( wxT( "%s" ), foundFile.Get().c_str() );
                i32 rowIndex = m_ListCtrl->InsertItem( m_CurrentFileIndex, buf, imageIndex );
                if ( rowIndex == -1 )
                {
                    // FIXME: report error?
                    continue;
                }
                m_ListCtrl->SetItemData( rowIndex, m_CurrentFileIndex );

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
                        m_ListCtrl->SetItem( rowIndex, colIndex, m_DetailsColumns[*colItr].Data( foundFile ).c_str() );
                    }
                }
            }
            
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
void ListResultsView::SelectPath( const tstring& path )
{
}

///////////////////////////////////////////////////////////////////////////////
void ListResultsView::GetSelectedPaths( std::set< Helium::Path >& paths )
{
}

///////////////////////////////////////////////////////////////////////////////
tstring ListResultsView::GetHighlightedPath() const
{
    tstring holding;
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
void ListResultsView::Sort( VaultSortMethod method, u32 sortOptions )
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