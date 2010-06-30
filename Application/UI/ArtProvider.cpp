#include "ArtProvider.h"

#include "Foundation/File/Path.h"

#include <sstream>

#include <wx/image.h>
#include <wx/stdpaths.h>

using namespace Nocturnal;

wxBitmap ArtProvider::CreateBitmap( const wxArtID& artId, const wxArtClient& artClient, const wxSize& size )
{
    Nocturnal::Path exePath( wxStandardPaths::Get().GetExecutablePath().c_str() );

    tstring icon;
    if ( artId == wxART_FILE_OPEN )
    {
        icon = TXT( "actions/document-open.png" );
    }
    else if ( artId == wxART_NEW )
    {
        icon = TXT( "actions/document-new.png" );
    }
    else if ( artId == wxART_FIND )
    {
        icon = TXT( "actions/edit-find.png" );
    }
    else if ( artId == wxART_FIND_AND_REPLACE )
    {
        icon = TXT( "actions/edit-find-replace.png" );
    }
    else if ( artId == wxART_CUT )
    {
        icon = TXT( "actions/edit-cut.png" );
    }
    else if ( artId == wxART_PASTE )
    {
        icon = TXT( "actions/edit-paste.png" );
    }
    else if ( artId == wxART_COPY )
    {
        icon = TXT( "actions/edit-copy.png" );
    }
    else if ( artId == wxART_UNDO )
    {
        icon = TXT( "actions/edit-undo.png" );
    }
    else if ( artId == wxART_REDO )
    {
        icon = TXT( "actions/edit-redo.png" );
    }
    else if ( artId == wxART_DELETE )
    {
        icon = TXT( "actions/edit-delete.png" );
    }
    else if ( artId == wxART_ERROR )
    {
        icon = TXT( "status/dialog-error.png" );
    }
    else if ( artId == wxART_FILE_SAVE )
    {
        icon = TXT( "actions/document-save.png" );
    }
    else if ( artId == wxART_FILE_SAVE_AS )
    {
        icon = TXT( "actions/document-save-as.png" );
    }
    else if ( artId == wxART_FOLDER )
    {
        icon = TXT( "places/folder.png" );
    }
    else if ( artId == wxART_FOLDER_OPEN )
    {
        icon = TXT( "status/folder-open.png" );
    }
    else if ( artId == wxART_HELP )
    {
        icon = TXT( "apps/help-browser.png" );
    }

    if ( !icon.empty() )
    {
        tstringstream strm;
        strm << exePath.Directory() << TXT( "Icons/" ) << size.x << TXT( 'x' ) << size.y << TXT( '/' ) << icon;
        Nocturnal::Path imageFile( strm.str() );

        if ( imageFile.Exists() )
        {
            wxImage image( imageFile.c_str(), wxBITMAP_TYPE_PNG );
            if ( image.Ok() )
            {
                return wxBitmap( image );
            }
        }
        else
        {
            wxImage image( exePath.Directory() + TXT( "Icons/32x32/" ) + icon, wxBITMAP_TYPE_PNG );
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