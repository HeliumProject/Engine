#pragma once

#include "ApplicationUI.h"
#include "ApplicationUI/ArtProvider.h"

#include "Platform/Types.h"
#include "Foundation/FilePath.h"

#include <map>

#include <wx/animate.h>
#include <wx/artprov.h>
#include <wx/imaglist.h>
#include <wx/generic/dirctrlg.h>

namespace Helium
{
    // The global fileicons table
    HELIUM_APPLICATION_UI_API class FileIconsTable& GlobalFileIconsTable();

    class HELIUM_APPLICATION_UI_API FileIconsTable //: public wxFileIconsTable
    {
    public:
        //enum iconId_Type
        //{
        //    folder,
        //    folder_open,
        //    computer,
        //    drive,
        //    cdrom,
        //    floppy,
        //    removeable,
        //    file,
        //    executable,
        //    FIRST_ID = wxFileIconsTable::executable,
        //};

        FileIconsTable();
        virtual ~FileIconsTable();

        int GetIconIDFromPath( const Helium::FilePath& path );
        int GetIconID( const wxString& extension, const wxString& mime = wxEmptyString );

        // which is wxIMAGE_LIST_NORMAL, wxIMAGE_LIST_SMALL, or wxIMAGE_LIST_STATE
        wxImageList* GetImageList( int which = wxIMAGE_LIST_SMALL );
        wxImageList* GetSmallImageList() { return GetImageList( wxIMAGE_LIST_SMALL ); }

    private:
        // delayed initialization - wait until first use after wxArtProvider created
        void Create();

    private:
        class FileIconEntry : public wxObject
        {
        public:
            int id;

            FileIconEntry( int i )
            {
                id = i;
            }        
        };

        typedef std::map< wxString, const wxChar* > M_ExtensionArtIDs;
        M_ExtensionArtIDs m_ExtensionArtIDs;

        wxHashTable* m_HashTable;
        wxImageList* m_NormalImageList;
        wxImageList* m_SmallImageList;
        wxImageList* m_StateImageList;
    };
}