#include "Precompile.h"
#include "FileIconsTable.h"

#include "Platform/Exception.h"
#include "Foundation/Container/Insert.h" 
#include "Foundation/File/Directory.h"
#include "Foundation/File/Path.h"
#include "Foundation/Log.h"

#include "Editor/ArtProvider.h"

#include <sstream>

#include <wx/wx.h>
#include <wx/hash.h>
#include <wx/image.h>
#include <wx/stdpaths.h>
#include <wx/mimetype.h>
#include <wx/log.h>
#include <wx/module.h>

using namespace Helium;
using namespace Helium::Editor;
using Helium::Insert; 


///////////////////////////////////////////////////////////////////////////////
// GlobalFileIconsTable (using wxModule FileIconsTableModule)
namespace Helium
{
    namespace Editor
    {
        ///////////////////////////////////////////////////////////////////////
        // global instance of a FileIconsTable
        static FileIconsTable* g_GlobalFileIconsTable = NULL;

        FileIconsTable& GlobalFileIconsTable()
        {
            if ( !g_GlobalFileIconsTable )
            {
                throw Helium::Exception( TXT( "GlobalFileIconsTable is not initialized!" ) );
            }

            return *g_GlobalFileIconsTable;
        }

        ///////////////////////////////////////////////////////////////////////
        // A module to allow icons table cleanup
        class FileIconsTableModule: public wxModule
        {
        public:
            DECLARE_DYNAMIC_CLASS(FileIconsTableModule)

            FileIconsTableModule()
            {
            }

            bool OnInit()
            {
                g_GlobalFileIconsTable = new FileIconsTable;
                return true;
            }

            void OnExit()
            {
                if ( g_GlobalFileIconsTable )
                {
                    delete g_GlobalFileIconsTable;
                    g_GlobalFileIconsTable = NULL;
                }
            }
        };
        IMPLEMENT_DYNAMIC_CLASS(FileIconsTableModule, wxModule)
    }
}

///////////////////////////////////////////////////////////////////////////////
FileIconsTable::FileIconsTable()
: m_HashTable( NULL )
, m_NormalImageList( NULL )
, m_SmallImageList( NULL )
, m_StateImageList( NULL )
{
}

FileIconsTable::~FileIconsTable()
{
    if ( m_HashTable )
    {
        WX_CLEAR_HASH_TABLE(*m_HashTable);
        delete m_HashTable;
    }

    if ( m_NormalImageList )
    {
        delete m_NormalImageList;
    }

    if ( m_SmallImageList )
    {
        delete m_SmallImageList;
    }

    if ( m_StateImageList )
    {
        delete m_StateImageList;
    }
}

void FileIconsTable::Create()
{
    HELIUM_ASSERT(!m_NormalImageList && !m_SmallImageList && !m_StateImageList && !m_HashTable);

    m_HashTable = new wxHashTable(wxKEY_STRING);

    ///////////////////////////////////////////////////////////////////////////
    {
        m_NormalImageList = new wxImageList(32, 32);
    }

    ///////////////////////////////////////////////////////////////////////////
    {
        tchar_t module[MAX_PATH];
        ::GetModuleFileName( 0, module, MAX_PATH );

        Helium::Path exePath( module );
        Helium::Path iconFolder( exePath.Directory() + TXT( "Icons/16x16/mimetypes/" ) );

        std::set< Helium::Path > artFiles;
        Helium::Directory::GetFiles( iconFolder, artFiles, true );

        int numImages = 0;
        for ( std::set< Helium::Path >::const_iterator itr = artFiles.begin(), end = artFiles.end(); itr != end; ++itr )
        {
            if ( (*itr).Extension() == TXT( "png" ) )
            {
                ++numImages;
            }
        }
        
        numImages += wxFileIconsTable::executable + 1;

        m_SmallImageList = new wxImageList( 16, 16, true, numImages );

        // folder:
        m_SmallImageList->Add( wxArtProvider::GetBitmap(wxART_FOLDER,
            wxART_CMN_DIALOG, wxSize(16, 16)) );

        // folder_open
        m_SmallImageList->Add( wxArtProvider::GetBitmap(wxART_FOLDER_OPEN,
            wxART_CMN_DIALOG, wxSize(16, 16)) );

        // computer
        m_SmallImageList->Add( wxArtProvider::GetBitmap(wxART_HARDDISK,
            wxART_CMN_DIALOG, wxSize(16, 16)) );

        // drive
        m_SmallImageList->Add( wxArtProvider::GetBitmap(wxART_HARDDISK,
            wxART_CMN_DIALOG, wxSize(16, 16)) );

        // cdrom
        m_SmallImageList->Add( wxArtProvider::GetBitmap(wxART_CDROM,
            wxART_CMN_DIALOG, wxSize(16, 16)) );

        // floppy
        m_SmallImageList->Add( wxArtProvider::GetBitmap(wxART_FLOPPY,
            wxART_CMN_DIALOG, wxSize(16, 16)) );

        // removeable
        m_SmallImageList->Add( wxArtProvider::GetBitmap(wxART_REMOVABLE,
            wxART_CMN_DIALOG, wxSize(16, 16)) );

        // file
        m_SmallImageList->Add( wxArtProvider::GetBitmap(wxART_NORMAL_FILE,
            wxART_CMN_DIALOG, wxSize(16, 16)) );

        // executable
        if ( GetIconID( wxEmptyString, TXT( "application/x-executable" ) ) == wxFileIconsTable::file )
        {
            m_SmallImageList->Add( wxArtProvider::GetBitmap(wxART_EXECUTABLE_FILE,
                wxART_CMN_DIALOG,
                wxSize(16, 16)) );
            delete m_HashTable->Get( TXT( "exe" ) );
            m_HashTable->Delete( TXT( "exe" ) );
            m_HashTable->Put( TXT( "exe" ), new FileIconEntry( wxFileIconsTable::executable ) );
        }

        // other file types
        int id;

        id = m_SmallImageList->Add( wxArtProvider::GetBitmap( ArtIDs::MimeTypes::Binary,
            wxART_OTHER,
            wxSize(16, 16)) );
        m_HashTable->Put( TXT( "bin" ), new FileIconEntry( id ) );

        id = m_SmallImageList->Add( wxArtProvider::GetBitmap( ArtIDs::MimeTypes::Binary,
            wxART_OTHER,
            wxSize(16, 16)) );
        m_HashTable->Put( TXT( "dat" ), new FileIconEntry( id ) );

        id = m_SmallImageList->Add( wxArtProvider::GetBitmap( ArtIDs::MimeTypes::ReflectBinary,
            wxART_OTHER,
            wxSize(16, 16)) );
        m_HashTable->Put( TXT( "hrb" ), new FileIconEntry( id ) );

        id = m_SmallImageList->Add( wxArtProvider::GetBitmap( ArtIDs::MimeTypes::Text,
            wxART_OTHER,
            wxSize(16, 16)) );
        m_HashTable->Put( TXT( "txt" ), new FileIconEntry( id ) );
    }

    ///////////////////////////////////////////////////////////////////////////
    {
        m_StateImageList = new wxImageList(16, 16);
    }
}

wxImageList* FileIconsTable::GetImageList( int which )
{
    wxImageList* imageList = NULL;
    if ( which == wxIMAGE_LIST_NORMAL )
    {
        imageList = m_NormalImageList;
    }
    else if ( which == wxIMAGE_LIST_SMALL )
    {
        imageList = m_SmallImageList;
    }
    else if ( which == wxIMAGE_LIST_STATE )
    {
        imageList = m_StateImageList;
    }
    else
    {
        return NULL;
    }

    if ( !imageList )
    {
        Create();
    }

    return imageList;
}

#if wxUSE_MIMETYPE && wxUSE_IMAGE && (!defined(__WXMSW__) || wxUSE_WXDIB)
// VS: we don't need this function w/o wxMimeTypesManager because we'll only have
//     one icon and we won't resize it
static wxBitmap CreateAntialiasedBitmap(const wxImage& img)
{
    const unsigned int size = 16;

    wxImage smallimg (size, size);
    unsigned char *p1, *p2, *ps;
    unsigned char mr = img.GetMaskRed(),
        mg = img.GetMaskGreen(),
        mb = img.GetMaskBlue();

    unsigned x, y;
    unsigned sr, sg, sb, smask;

    p1 = img.GetData(), p2 = img.GetData() + 3 * size*2, ps = smallimg.GetData();
    smallimg.SetMaskColour(mr, mr, mr);

    for (y = 0; y < size; y++)
    {
        for (x = 0; x < size; x++)
        {
            sr = sg = sb = smask = 0;
            if (p1[0] != mr || p1[1] != mg || p1[2] != mb)
                sr += p1[0], sg += p1[1], sb += p1[2];
            else smask++;
            p1 += 3;
            if (p1[0] != mr || p1[1] != mg || p1[2] != mb)
                sr += p1[0], sg += p1[1], sb += p1[2];
            else smask++;
            p1 += 3;
            if (p2[0] != mr || p2[1] != mg || p2[2] != mb)
                sr += p2[0], sg += p2[1], sb += p2[2];
            else smask++;
            p2 += 3;
            if (p2[0] != mr || p2[1] != mg || p2[2] != mb)
                sr += p2[0], sg += p2[1], sb += p2[2];
            else smask++;
            p2 += 3;

            if (smask > 2)
                ps[0] = ps[1] = ps[2] = mr;
            else
            {
                ps[0] = (unsigned char)(sr >> 2);
                ps[1] = (unsigned char)(sg >> 2);
                ps[2] = (unsigned char)(sb >> 2);
            }
            ps += 3;
        }
        p1 += size*2 * 3, p2 += size*2 * 3;
    }

    return wxBitmap(smallimg);
}
#endif // wxUSE_MIMETYPE

int FileIconsTable::GetIconIDFromPath( const Helium::Path& path )
{ 
    wxString extension = path.FullExtension();
    if ( extension.empty() )
    {
        return wxFileIconsTable::file;
    }

    FileIconEntry *entry = (FileIconEntry*) m_HashTable->Get( extension );
    if ( entry )
    {
        return entry->id;
    }

    // try just the end extension
    if ( _tcsicmp( path.Extension().c_str(), extension.c_str() ) != 0 )
    {
        extension = path.Extension();
        if ( extension.empty() )
        {
            return wxFileIconsTable::file;
        }

        FileIconEntry *entry = (FileIconEntry*) m_HashTable->Get( extension );
        if ( entry )
        {
            return entry->id;
        }
    }

    return wxFileIconsTable::file;
}

int FileIconsTable::GetIconID( const wxString& extension, const wxString& mime )
{
    if ( !m_SmallImageList )
    {
        Create();
    }

#if wxUSE_MIMETYPE
    if ( !extension.empty() )
    {
        FileIconEntry *entry = (FileIconEntry*) m_HashTable->Get(extension);
        if ( entry )
        {
            return (entry->id);
        }
    }

    wxFileType *ft = NULL;
    if ( !mime.empty() )
    {
        wxTheMimeTypesManager->GetFileTypeFromMimeType( mime );
    }
    else if ( !extension.empty() )
    {
        wxTheMimeTypesManager->GetFileTypeFromExtension( extension );
    }

    wxIconLocation iconLoc;
    wxIcon ic;
    if ( ft )
    {
        wxLogNull logNull;
        if ( ft && ft->GetIcon(&iconLoc) )
        {
            ic = wxIcon( iconLoc );
        }
        delete ft;
    }

    if ( !ic.Ok() )
    {
        int newid = wxFileIconsTable::file;
        if ( !extension.empty() )
        {
            m_HashTable->Put(extension, new FileIconEntry(newid) );
        }
        return newid;
    }

    wxBitmap bmp;
    bmp.CopyFromIcon( ic );
    if ( !bmp.Ok() )
    {
        int newid = wxFileIconsTable::file;
        if ( !extension.empty() )
        {
            m_HashTable->Put(extension, new FileIconEntry(newid) );
        }
        return newid;
    }

    const unsigned int size = 16;

    int id = m_SmallImageList->GetImageCount();
    if ( (bmp.GetWidth() == (int) size) && (bmp.GetHeight() == (int) size) )
    {
        m_SmallImageList->Add(bmp);
    }
#if wxUSE_IMAGE && (!defined(__WXMSW__) || wxUSE_WXDIB)
    else
    {
        wxImage img = bmp.ConvertToImage();

        if ((img.GetWidth() != size*2) || (img.GetHeight() != size*2))
        {
            //m_SmallImageList->Add(CreateAntialiasedBitmap(CutEmptyBorders(img).Rescale(size*2, size*2)) );
            m_SmallImageList->Add( CreateAntialiasedBitmap( img.Rescale( size*2, size*2 ) ) );
        }
        else
        {
            m_SmallImageList->Add(CreateAntialiasedBitmap(img) );
        }
    }
#endif // wxUSE_IMAGE

    if ( !extension.empty() )
    {
        m_HashTable->Put(extension, new FileIconEntry(id) );
    }
    return id;
#else // !wxUSE_MIMETYPE

    wxUnusedVar(mime);
    if (extension == wxT("exe"))
    {
        return executable;
    }
    else
    {
        return file;
    }
#endif // wxUSE_MIMETYPE/!wxUSE_MIMETYPE
}
