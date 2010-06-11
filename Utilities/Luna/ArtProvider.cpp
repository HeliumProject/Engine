#include "Precompile.h"
#include "ArtProvider.h"

#include "Foundation/File/File.h"

#include <sstream>

#include <wx/image.h>
#include <wx/stdpaths.h>

using namespace Luna;

wxBitmap ArtProvider::CreateBitmap( const wxArtID& artId, const wxArtClient& artClient, const wxSize& size )
{
    Nocturnal::Path exePath( wxStandardPaths::Get().GetExecutablePath().c_str() );

    std::string icon;
    if ( artId == wxART_FILE_OPEN )
        icon = "actions/document-open.png";
    else if ( artId == wxART_NEW )
        icon = "actions/document-new.png";
    else if ( artId == wxART_FIND )
        icon = "actions/edit-find.png";
    else if ( artId == wxART_FIND_AND_REPLACE )
        icon = "actions/edit-find-replace.png";
    else if ( artId == wxART_CUT )
        icon = "actions/edit-cut.png";
    else if ( artId == wxART_PASTE )
        icon = "actions/edit-paste.png";
    else if ( artId == wxART_COPY )
        icon = "actions/edit-copy.png";
    else if ( artId == wxART_UNDO )
        icon = "actions/edit-undo.png";
    else if ( artId == wxART_REDO )
        icon = "actions/edit-redo.png";
    else if ( artId == wxART_DELETE )
        icon = "actions/edit-delete.png";
    else if ( artId == wxART_ERROR )
        icon = "status/dialog-error.png";
    else if ( artId == wxART_FILE_SAVE )
        icon = "actions/document-save.png";
    else if ( artId == wxART_FILE_SAVE_AS )
        icon = "actions/document-save-as.png";
    else if ( artId == wxART_FOLDER )
        icon = "places/folder.png";
    else if ( artId == wxART_FOLDER_OPEN )
        icon = "status/folder-open.png";
    else if ( artId == wxART_HELP )
        icon = "apps/help-browser.png";

    if ( !icon.empty() )
    {
        std::stringstream strm;
        strm << exePath.Directory() << "Icons/" << size.x << 'x' << size.y << '/' << icon;
        Nocturnal::File imageFile( strm.str() );

        if ( imageFile.Exists() )
        {
            wxImage image( imageFile.GetPath().c_str(), wxBITMAP_TYPE_PNG );
            if ( image.Ok() )
            {
                return wxBitmap( image );
            }
        }
        else
        {
            wxImage image( exePath.Directory() + "Icons/32x32/" + icon, wxBITMAP_TYPE_PNG );
            if ( image.Ok() )
            {
                int x = size.GetWidth() > 0 ? size.GetWidth() : 32;
                int y = size.GetHeight() > 0 ? size.GetHeight() : 32;
                return wxBitmap( image.Rescale( x, y ) );
            }
        }
    }

    return wxNullBitmap;
}