#include "Platform/Windows/Windows.h"

#include "ArtProvider.h"

#include "Platform/Exception.h"
#include "Foundation/Container/Insert.h"
#include "Foundation/Log.h"

#include <sstream>

#include <wx/hash.h>
#include <wx/image.h>
#include <wx/stdpaths.h>

using namespace Nocturnal;
using Nocturnal::Insert; 

wxSize ArtProvider::DefaultImageSize( 16, 16 );

class ArtProviderBitmapEntry : public wxObject
{
public:
    wxBitmap m_Bitmap;

    ArtProviderBitmapEntry( wxImage image )
    {
        m_Bitmap = wxBitmap( image );
    }        
};


ArtProvider::ArtProvider()
: m_HashTable( NULL )
{
}

ArtProvider::~ArtProvider()
{
    if ( m_HashTable )
    {
        WX_CLEAR_HASH_TABLE(*m_HashTable);
        delete m_HashTable;
    }

    m_ArtIDToFilename.clear();
}

void ArtProvider::Create()
{
    NOC_ASSERT(!m_HashTable);

    m_HashTable = new wxHashTable(wxKEY_STRING);

    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_ADD_BOOKMARK, TXT( "" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_CDROM, TXT( "devices/media-optical.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_COPY, TXT( "actions/edit-copy.png" ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_CROSS_MARK, TXT( "" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_CUT, TXT( "actions/edit-cut.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_DELETE, TXT( "actions/edit-delete.png" ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_DEL_BOOKMARK, TXT( "" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_ERROR, TXT( "status/dialog-error.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_EXECUTABLE_FILE, TXT( "mimetypes/application-x-executable.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_FILE_OPEN, TXT( "actions/document-open.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_FILE_SAVE, TXT( "actions/document-save.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_FILE_SAVE_AS, TXT( "actions/document-save-as.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_FIND, TXT( "actions/edit-find.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_FIND_AND_REPLACE, TXT( "actions/edit-find-replace.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_FLOPPY, TXT( "devices/media-floppy.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_FOLDER, TXT( "places/folder.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_FOLDER_OPEN, TXT( "status/folder-open.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_GO_BACK, TXT( "actions/go-previous.png" ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_GO_DIR_UP, TXT( "" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_GO_DOWN, TXT( "actions/go-down.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_GO_FORWARD, TXT( "actions/go-next.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_GO_HOME, TXT( "actions/go-home.png" ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_GO_TO_PARENT, TXT( "" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_GO_UP, TXT( "actions/go-up.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_HARDDISK, TXT( "devices/drive-harddisk.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_HELP, TXT( "apps/help-browser.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_HELP_BOOK, TXT( "apps/help-browser.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_HELP_FOLDER, TXT( "apps/help-browser.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_HELP_PAGE, TXT( "apps/help-browser.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_HELP_SETTINGS, TXT( "apps/help-browser.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_HELP_SIDE_PANEL, TXT( "apps/help-browser.png" ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_INFORMATION, TXT( "" ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_LIST_VIEW, TXT( "" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_MISSING_IMAGE, TXT( "status/image-missing.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_NEW, TXT( "actions/document-new.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_NEW_DIR, TXT( "actions/folder-new.png" ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_NORMAL_FILE, TXT( "" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_PASTE, TXT( "actions/edit-paste.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_PRINT, TXT( "actions/document-print.png" ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_QUESTION, TXT( "" ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_QUIT, TXT( "" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_REDO, TXT( "actions/edit-redo.png" ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_REMOVABLE, TXT( "" ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_REPORT_VIEW, TXT( "" ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_TICK_MARK, TXT( "" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_TIP, TXT( "status/dialog-information.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_UNDO, TXT( "actions/edit-undo.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_WARNING, TXT( "status/dialog-warning.png" ) ) );


    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Nocturnal::ArtIDs::Unknown, TXT( "emotes/face-devil-grin.png" ) ) );

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Nocturnal::ArtIDs::PerspectiveCamera, TXT( "" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Nocturnal::ArtIDs::FrontOrthoCamera, TXT( "" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Nocturnal::ArtIDs::SideOrthoCamera, TXT( "" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Nocturnal::ArtIDs::TopOrthoCamera, TXT( "" ) ) );

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Nocturnal::ArtIDs::Select, TXT( "" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Nocturnal::ArtIDs::Translate, TXT( "" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Nocturnal::ArtIDs::Rotate, TXT( "" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Nocturnal::ArtIDs::Scale, TXT( "" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Nocturnal::ArtIDs::Duplicate, TXT( "" ) ) );
}

wxBitmap ArtProvider::CreateBitmap( const wxArtID& artId, const wxArtClient& artClient, const wxSize& size )
{
    if ( !m_HashTable )
    {
        Create();
    }

    ArtProviderBitmapEntry *entry = (ArtProviderBitmapEntry*) m_HashTable->Get( artId );
    if ( entry )
    {
        return ( entry->m_Bitmap );
    }

    M_ArtIDToFilename::iterator findFilename = m_ArtIDToFilename.find( artId );
    if ( findFilename == m_ArtIDToFilename.end() || findFilename->second.empty() )
    {
        findFilename = m_ArtIDToFilename.find( Nocturnal::ArtIDs::Unknown );
    }

    if ( findFilename != m_ArtIDToFilename.end() && !findFilename->second.empty() )
    {
        const tstring& icon = findFilename->second;

        int width = size.GetWidth() > 0 ? size.GetWidth() : DefaultImageSize.GetWidth();
        int height = size.GetHeight() > 0 ? size.GetHeight() : DefaultImageSize.GetHeight();

        Nocturnal::Path exePath( wxStandardPaths::Get().GetExecutablePath().c_str() );

        tstringstream strm;
        strm << exePath.Directory() << TXT( "Icons/" ) << width << TXT( 'x' ) << height << TXT( '/' ) << icon;
        Nocturnal::Path imageFile( strm.str() );

        if ( imageFile.Exists() && imageFile.Size() > 0 )
        {
            wxImage image( imageFile.Get().c_str(), wxBITMAP_TYPE_PNG );
            if ( image.Ok() )
            {
                m_HashTable->Put( artId, new ArtProviderBitmapEntry( image ) );
                return wxBitmap( image );
            }
        }
    }

    return wxNullBitmap;//return __super::CreateBitmap( artId, artClient, size );
}
