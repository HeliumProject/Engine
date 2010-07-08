#include "Precompile.h"
#include "FileTreeCtrl.h"
#include "Foundation/String/Natural.h"

#include <wx/wx.h>
#include <wx/artprov.h>

namespace Luna
{

    // Required so that OnCompareItems will be called
    IMPLEMENT_DYNAMIC_CLASS( FileTreeCtrl, SortTreeCtrl );

    ///////////////////////////////////////////////////////////////////////////////
    // Default constructor - required by IMPLEMENT_DYNAMIC_CLASS
    // 
    FileTreeCtrl::FileTreeCtrl()
    {
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Constructor
    // 
    FileTreeCtrl::FileTreeCtrl( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator, const wxString& name )
        : SortTreeCtrl( parent, id, pos, size, style, validator, name )
        , m_IconSize( 16 )
    {
        //CreateImageList();
        wxImageList *imageList = wxTheFileIconsTable->GetSmallImageList();
        SetImageList( imageList ); //, wxIMAGE_LIST_SMALL );
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Destructor
    // 
    FileTreeCtrl::~FileTreeCtrl()
    {
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Callback to compare two tree items.  If item1 should be sorted before item2,
    // a negative number is returned.  If item1 and item2 are equal according to
    // the sorting algorithm, zero is returned.  Finally, if item1 should be sorted
    // after item2, a positive number is returned.
    // 
    int FileTreeCtrl::OnCompareItems( const wxTreeItemId& lhsItem, const wxTreeItemId& rhsItem )
    {
        FileTreeItemData* lhsData = reinterpret_cast<FileTreeItemData*>( GetItemData( lhsItem ) );
        FileTreeItemData* rhsData = reinterpret_cast<FileTreeItemData*>( GetItemData( rhsItem ) );

        if ( lhsData->m_IsFolder != rhsData->m_IsFolder )
        {
            return lhsData->m_IsFolder ? -1 : 1 ;
        }
        else
        {
            return __super::OnCompareItems( lhsItem, rhsItem );
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Create's the image list for the file tree view
    //
    //void FileTreeCtrl::CreateImageList( i32 iconSize )
    //{
    //  if ( iconSize == -1 )
    //  {
    //    SetImageList( NULL );
    //    return;
    //  }
    //  else if ( iconSize == 0 )
    //  {
    //    iconSize = m_IconSize;
    //  }
    //  else
    //  {
    //    m_IconSize = iconSize;
    //  }

    //  const wxSize imageSize( m_IconSize, m_IconSize );

    //  // Make an image list containing small icons
    //  wxImageList* treeItemIcons = new wxImageList( m_IconSize, m_IconSize, true );

    //  wxBusyCursor wait;

    //  // corresponds to FileTreeCtrlIcon enum
    //  wxIcon icons[5];
    //  icons[0] = wxIcon( wxArtProvider::GetIcon( wxART_NORMAL_FILE, wxART_OTHER, imageSize ) );  // File
    //  icons[1] = wxIcon( wxArtProvider::GetIcon( wxART_NORMAL_FILE, wxART_OTHER, imageSize ) );  // FileSelected
    //  icons[2] = wxIcon( wxArtProvider::GetIcon( wxART_FOLDER, wxART_OTHER, imageSize ) );       // Folder
    //  icons[3] = wxIcon( wxArtProvider::GetIcon( wxART_FOLDER, wxART_OTHER, imageSize ) );       // FolderSelected
    //  icons[4] = wxIcon( wxArtProvider::GetIcon( wxART_FOLDER_OPEN, wxART_OTHER, imageSize ) );  // FolderSelected

    //  for ( size_t i = 0; i < WXSIZEOF(icons); i++ )
    //  {
    //    int sizeOrig = icons[i].GetWidth();
    //    if ( iconSize == sizeOrig )
    //    {
    //      treeItemIcons->Add(icons[i]);
    //    }
    //    else
    //    {
    //      treeItemIcons->Add(wxBitmap(wxBitmap(icons[i]).ConvertToImage().Rescale( m_IconSize, m_IconSize )));
    //    }
    //  }

    //  AssignImageList( treeItemIcons );
    //}
}