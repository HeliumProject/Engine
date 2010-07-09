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


///////////////////////////////////////////////////////////////////////////////
// Cache class - stores already requested bitmaps
//
// Copied from wxWidgets src\common\artprov.cpp
///////////////////////////////////////////////////////////////////////////////
namespace Nocturnal
{
    WX_DECLARE_EXPORTED_STRING_HASH_MAP(wxBitmap, ArtProviderBitmapsHash);

    class ArtProviderCache
    {
    public:
        bool GetBitmap(const wxString& full_id, wxBitmap* bmp)
        {
            ArtProviderBitmapsHash::iterator entry = m_bitmapsHash.find(full_id);
            if ( entry == m_bitmapsHash.end() )
            {
                return false;
            }
            else
            {
                *bmp = entry->second;
                return true;
            }
        }

        void PutBitmap(const wxString& full_id, const wxBitmap& bmp)
        {
            m_bitmapsHash[full_id] = bmp;
        }

        void Clear()
        {
            m_bitmapsHash.clear();
        }

        static wxString ConstructHashID(const wxArtID& id,
            const wxArtClient& client,
            const wxSize& size)
        {
            wxString str;
            str.Printf(wxT("%s-%s-%i-%i"), id.c_str(), client.c_str(), size.x, size.y);
            return str;
        }


    private:
        ArtProviderBitmapsHash m_bitmapsHash;
    };
}


ArtProvider::ArtProvider()
: m_ArtProviderCache( NULL )
{
}

ArtProvider::~ArtProvider()
{
    if ( m_ArtProviderCache )
    {
        m_ArtProviderCache->Clear();
        delete m_ArtProviderCache;
        m_ArtProviderCache = NULL;
    }

    m_ArtIDToFilename.clear();
}

void ArtProvider::Create()
{
    NOC_ASSERT(!m_ArtProviderCache);

    m_ArtProviderCache = new ArtProviderCache();
    m_ArtProviderCache->Clear();

    m_ArtIDToFilename.clear();

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_ADD_BOOKMARK, TXT( "actions/bookmark_add.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_CDROM, TXT( "devices/cdrom_unmount.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_COPY, TXT( "actions/editcopy.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_CROSS_MARK, TXT( "actions/edit_add.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_CUT, TXT( "actions/editcut.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_DELETE, TXT( "actions/edit_remove.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_DEL_BOOKMARK, TXT( "actions/edit_remove.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_ERROR, TXT( "actions/messagebox_critical.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_EXECUTABLE_FILE, TXT( "filesystems/exec.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_FILE_OPEN, TXT( "filesystems/folder_open.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_FILE_SAVE, TXT( "actions/filesave.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_FILE_SAVE_AS, TXT( "actions/filesaveas.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_FIND, TXT( "actions/filefind.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_FIND_AND_REPLACE, TXT( "actions/filefind.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_FLOPPY, TXT( "devices/floppy_mount.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_FOLDER, TXT( "filesystem/folder.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_FOLDER_OPEN, TXT( "filesystems/folder_open.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_GO_BACK, TXT( "actions/previous.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_GO_DIR_UP, TXT( "actions/up.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_GO_DOWN, TXT( "actions/down.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_GO_FORWARD, TXT( "actions/forward.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_GO_HOME, TXT( "actions/home.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_GO_TO_PARENT, TXT( "actions/home.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_GO_UP, TXT( "actions/up.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_HARDDISK, TXT( "filesystems/hd.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_HELP, TXT( "apps/help.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_HELP_BOOK, TXT( "apps/help.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_HELP_FOLDER, TXT( "apps/help.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_HELP_PAGE, TXT( "apps/help.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_HELP_SETTINGS, TXT( "apps/help.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_HELP_SIDE_PANEL, TXT( "apps/help.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_INFORMATION, TXT( "actions/messagebox_info.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_LIST_VIEW, TXT( "actions/view_text.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_MISSING_IMAGE, TXT( "apps/kblackbox.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_NEW, TXT( "actions/edit_add.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_NEW_DIR, TXT( "actions/folder_new.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_NORMAL_FILE, TXT( "mimetypes/mime-empty.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_PASTE, TXT( "actions/editpaste.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_PRINT, TXT( "devices/printer.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_QUESTION, TXT( "actions/messagebox_question.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_QUIT, TXT( "actions/exit.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_REDO, TXT( "actions/redo.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_REMOVABLE, TXT( "actions/exit.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_REPORT_VIEW, TXT( "actions/view_multicolumn.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_TICK_MARK, TXT( "actions/ok.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_TIP, TXT( "actions/messagebox_info.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_UNDO, TXT( "actions/undo.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_WARNING, TXT( "actions/messagebox_warning.png" ) ) );


    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Nocturnal::ArtIDs::Unknown, TXT( "nocturnal/unknown.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Nocturnal::ArtIDs::Null, TXT( "" ) ) );

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Nocturnal::ArtIDs::PerspectiveCamera, TXT( "devices/camera.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Nocturnal::ArtIDs::FrontOrthoCamera, TXT( "devices/camera.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Nocturnal::ArtIDs::SideOrthoCamera, TXT( "devices/camera.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Nocturnal::ArtIDs::TopOrthoCamera, TXT( "devices/camera.png" ) ) );


    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Nocturnal::ArtIDs::Select, TXT( "" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Nocturnal::ArtIDs::Translate, TXT( "apps/amsn4.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Nocturnal::ArtIDs::Rotate, TXT( "" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Nocturnal::ArtIDs::Scale, TXT( "apps/amsn8.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Nocturnal::ArtIDs::Duplicate, TXT( "" ) ) );

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Nocturnal::ArtIDs::Locator, TXT( "apps/galeon.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Nocturnal::ArtIDs::Volume, TXT( "apps/3d.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Nocturnal::ArtIDs::Entity, TXT( "apps/katuberling.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Nocturnal::ArtIDs::Curve, TXT( "" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Nocturnal::ArtIDs::CurveEdit, TXT( "" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Nocturnal::ArtIDs::NavMesh, TXT( "apps/gnome_apps.png" ) ) );

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Nocturnal::ArtIDs::NewCollection, TXT( "actions/folder_new.png" ) ) );

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Nocturnal::ArtIDs::RCSCheckedOutByMe, TXT( "actions/apply.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Nocturnal::ArtIDs::RCSCheckedOutBySomeoneElse, TXT( "actions/lock.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Nocturnal::ArtIDs::RCSOutOfDate, TXT( "actions/messagebox_warning.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Nocturnal::ArtIDs::RCSUnknown, TXT( "" ) ) );

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Nocturnal::ArtIDs::TaskWindow, TXT( "actions/openterm.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Nocturnal::ArtIDs::SceneEditor, TXT( "apps/browser.png" ) ) );
}

wxBitmap ArtProvider::CreateBitmap( const wxArtID& artId, const wxArtClient& artClient, const wxSize& size )
{
    if ( !m_ArtProviderCache )
    {
        Create();
    }

    wxString hashId = ArtProviderCache::ConstructHashID( artId, artClient, size );

    wxBitmap bitmap = wxNullBitmap;
    if ( !m_ArtProviderCache->GetBitmap( hashId, &bitmap ) )
    {
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

            if ( !imageFile.Exists() || imageFile.Size() <= 0 )
            {
                NOC_BREAK();
            }

            wxImage image( imageFile.Get().c_str(), wxBITMAP_TYPE_PNG );
            if ( image.Ok() )
            {
                if ( image.GetWidth() != width || image.GetHeight() != height )
                {
                    image.Rescale( width, height );
                }

                bitmap = wxBitmap( image );
                
                m_ArtProviderCache->PutBitmap( hashId, bitmap );
            }
        }
    }

    return bitmap;
}
