#pragma once

#include "API.h"
#include "SortTreeCtrl.h"

#include "Platform/Types.h"

#include <wx/wx.h>
#include "wx/generic/dirctrlg.h" // for wxTheFileIconsTable

namespace UIToolKit
{

  /////////////////////////////////////////////////////////////////////////////  
  
  namespace FileTreeCtrlIcons
  {
    enum FileTreeCtrlIcon
    {
      NoIcon        = -1,
      File          =  0,
      FileSelected,
      Folder,
      FolderSelected,
      FolderOpened
    };
  }
  typedef FileTreeCtrlIcons::FileTreeCtrlIcon FileTreeCtrlIcon;


  /////////////////////////////////////////////////////////////////////////////

  class UITOOLKIT_API FileTreeItemData : public wxTreeItemData
  {
  public:

    wxString  m_Path;
    bool      m_IsFolder;

    FileTreeItemData( const wxString& path, const bool isFolder = true )
      : m_Path( path ), m_IsFolder( isFolder )
    {}

  };


  /////////////////////////////////////////////////////////////////////////////

  class UITOOLKIT_API FileTreeCtrl : public SortTreeCtrl
  {
  public:

    FileTreeCtrl();
    FileTreeCtrl( wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTR_HAS_BUTTONS, const wxValidator& validator = wxDefaultValidator, const wxString& name = "listCtrl" );
    virtual ~FileTreeCtrl();

    virtual int OnCompareItems( const wxTreeItemId& lhsItem, const wxTreeItemId& rhsItem );

    //virtual void CreateImageList( i32 iconSize = 16 );

  private:
    
    i32 m_IconSize;

    // Required so that OnCompareItems will be called
    DECLARE_DYNAMIC_CLASS( FileTreeCtrl )
  };

} // namespace UIToolKit