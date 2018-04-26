#pragma once

#include <wx/wx.h>
#include <wx/generic/dirctrlg.h> // for wxTheFileIconsTable

#include "SortTreeCtrl.h"
#include "Platform/Types.h"

namespace Helium
{
    namespace Editor
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

        class FileTreeItemData : public wxTreeItemData
        {
        public:

            wxString  m_Path;
            bool      m_IsFolder;

            FileTreeItemData( const wxString& path, const bool isFolder = true )
                : m_Path( path ), m_IsFolder( isFolder )
            {}

        };


        /////////////////////////////////////////////////////////////////////////////

        class FileTreeCtrl : public SortTreeCtrl
        {
        public:

            FileTreeCtrl();
            FileTreeCtrl( wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTR_HAS_BUTTONS, const wxValidator& validator = wxDefaultValidator, const wxString& name = wxT( "listCtrl" ) );
            virtual ~FileTreeCtrl();

            virtual int OnCompareItems( const wxTreeItemId& lhsItem, const wxTreeItemId& rhsItem );

            //virtual void CreateImageList( int32_t iconSize = 16 );

        private:

            int32_t m_IconSize;

            // Required so that OnCompareItems will be called
            DECLARE_DYNAMIC_CLASS( FileTreeCtrl )
        };

    } // namespace Editor
}