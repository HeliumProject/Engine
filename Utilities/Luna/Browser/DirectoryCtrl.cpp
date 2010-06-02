#include "Precompile.h"
#include "DirectoryCtrl.h"

#include "FileSystem/FileSystem.h"
#include "Finder/Finder.h"
#include "UIToolKit/ImageManager.h"
#include "UIToolKit/SortTreeCtrl.h"

using namespace Luna;

BEGIN_EVENT_TABLE( DirectoryCtrl, wxGenericDirCtrl )
EVT_TREE_ITEM_COLLAPSING( wxID_ANY, DirectoryCtrl::OnItemCollapsing )
END_EVENT_TABLE()

///////////////////////////////////////////////////////////////////////////////
DirectoryCtrl::DirectoryCtrl( const std::string& rootDirectory )
: m_RootDirectory( rootDirectory )
{
}

///////////////////////////////////////////////////////////////////////////////
DirectoryCtrl::DirectoryCtrl
(
 const std::string& rootDirectory,
 wxWindow *parent, const wxWindowID id,
 const wxString &dir,
 const wxPoint& pos,
 const wxSize& size,
 long style,
 const wxString& filter,
 int defaultFilter,
 const wxString& name
 )
 : wxGenericDirCtrl( parent, id, dir, pos, size, style, filter, defaultFilter, name )
 , m_RootDirectory( rootDirectory )
{
  m_ImageList.Create( 16, 16, true, DirectoryCtrlIconTypes::Count );
  
  // Guarantee that the icon indices are the same (would still need to check if icons were added though)
  NOC_ASSERT( DirectoryCtrlIconTypes::Folder == wxFileIconsTable::folder );
  NOC_ASSERT( DirectoryCtrlIconTypes::FolderOpen == wxFileIconsTable::folder_open );
  NOC_ASSERT( DirectoryCtrlIconTypes::Computer == wxFileIconsTable::computer );
  NOC_ASSERT( DirectoryCtrlIconTypes::Drive == wxFileIconsTable::drive );
  NOC_ASSERT( DirectoryCtrlIconTypes::CDRom == wxFileIconsTable::cdrom );
  NOC_ASSERT( DirectoryCtrlIconTypes::Floppy == wxFileIconsTable::floppy );
  NOC_ASSERT( DirectoryCtrlIconTypes::File == wxFileIconsTable::file );
  NOC_ASSERT( DirectoryCtrlIconTypes::Executable == wxFileIconsTable::executable );
  
  for ( u32 type = DirectoryCtrlIconTypes::Start; type != DirectoryCtrlIconTypes::Count; ++type )
  {
    switch ( (DirectoryCtrlIconType) type )
    {
      case DirectoryCtrlIconTypes::Folder:
        m_ImageList.Add( UIToolKit::GlobalImageManager().GetBitmap( "ms_folder_closed.png" ) );
        break;

      case DirectoryCtrlIconTypes::FolderOpen:
        m_ImageList.Add( UIToolKit::GlobalImageManager().GetBitmap( "ms_folder_open.png" ) );
        break;

      case DirectoryCtrlIconTypes::InsomniacMoon:
        m_ImageList.Add( UIToolKit::GlobalImageManager().GetBitmap( "moon_16.png" ) );
        break;

      // We should not be using these anyway - if we are, setup a proper icon for them above
      case DirectoryCtrlIconTypes::Computer:
      case DirectoryCtrlIconTypes::Drive:
      case DirectoryCtrlIconTypes::CDRom:
      case DirectoryCtrlIconTypes::Floppy:
      case DirectoryCtrlIconTypes::Removeable:
      case DirectoryCtrlIconTypes::File:
      case DirectoryCtrlIconTypes::Executable:
      case DirectoryCtrlIconTypes::Error:
      default:
        m_ImageList.Add( UIToolKit::GlobalImageManager().GetBitmap( "error_16.png" ) );
        break;
    }
  }

  GetTreeCtrl()->SetImageList( &m_ImageList );
  GetTreeCtrl()->SetStateImageList( &m_ImageList );
}

///////////////////////////////////////////////////////////////////////////////
DirectoryCtrl::~DirectoryCtrl()
{
  m_ImageList.RemoveAll();
}

///////////////////////////////////////////////////////////////////////////////
void DirectoryCtrl::SetupSections()
{
  std::string rootPath = m_RootDirectory;

  std::string win32RootPath;
  FileSystem::Win32Name( rootPath, win32RootPath, true );
  m_Root = AddSection( win32RootPath, rootPath, DirectoryCtrlIconTypes::InsomniacMoon );
  if ( !GetTreeCtrl()->IsExpanded( m_Root ) )
  {
    GetTreeCtrl()->Toggle( m_Root );
  }
}

///////////////////////////////////////////////////////////////////////////////
void DirectoryCtrl::OnItemCollapsing( wxTreeEvent& e )
{
  if ( e.GetItem() == m_Root )
  {
    e.Veto();
  }
  else
  {
    e.Skip();
  }
}

///////////////////////////////////////////////////////////////////////////////
wxTreeCtrl* DirectoryCtrl::CreateTreeCtrl(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long treeStyle)
{
  return reinterpret_cast<wxTreeCtrl*>( new UIToolKit::SortTreeCtrl( parent, id, pos, size, treeStyle ) );
}