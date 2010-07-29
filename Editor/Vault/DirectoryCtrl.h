#pragma once

#include "Platform/Compiler.h"

#include <wx/dirctrl.h>

namespace Editor
{
    namespace DirectoryCtrlIconTypes
    {
        enum DirectoryCtrlIconType
        {
            Start = 0,

            // These must line up with iconId_Type in dirctrlg.h in wxWidgets (terrible).
            Folder = Start,
            FolderOpen,
            Computer,
            Drive,
            CDRom,
            Floppy,
            Removeable,
            File,
            Executable,

            // Add our own icons here
            Error,

            Count,
        };
    }

    typedef DirectoryCtrlIconTypes::DirectoryCtrlIconType DirectoryCtrlIconType;

    class DirectoryCtrl : public wxGenericDirCtrl
    {
    public:
        DirectoryCtrl( const tstring& rootDirectory );
        DirectoryCtrl( wxWindow *parent,
            const wxWindowID id = wxID_ANY,
            const wxString &dir = wxDirDialogDefaultFolderStr,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxDIRCTRL_3D_INTERNAL|wxSUNKEN_BORDER,
            const wxString& filter = wxEmptyString,
            int defaultFilter = 0,
            const wxString& name = wxTreeCtrlNameStr,
            const tstring& rootDirectory = TXT( "" ) );

        virtual ~DirectoryCtrl();

        virtual void SetupSections() HELIUM_OVERRIDE;
        virtual void OnItemCollapsing( wxTreeEvent& e );

    protected:
        virtual wxTreeCtrl* CreateTreeCtrl(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long treeStyle) HELIUM_OVERRIDE;

    private:
        tstring m_RootDirectory;
        wxImageList m_ImageList;
        wxTreeItemId m_Root;

    public:
        DECLARE_EVENT_TABLE();
    };
}