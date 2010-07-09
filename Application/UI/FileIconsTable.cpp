#include "Platform/Windows/Windows.h"

#include "FileIconsTable.h"

#include "Platform/Exception.h"
#include "Foundation/Container/Insert.h" 
#include "Foundation/File/Directory.h"
#include "Foundation/File/Path.h"
#include "Foundation/Log.h"

#include "Application/UI/ArtProvider.h"

#include <sstream>

#include <wx/hash.h>
#include <wx/image.h>
#include <wx/stdpaths.h>
#include <wx/mimetype.h>
#include <wx/log.h>
#include <wx/module.h>

using namespace Nocturnal;
using Nocturnal::Insert; 

int g_InitCount = 0;


#if wxUSE_DIRDLG || wxUSE_FILEDLG

// ----------------------------------------------------------------------------
// FileIconsTable icons
// ----------------------------------------------------------------------------

#ifndef __WXGTK24__
/* Computer (c) Julian Smart */
static const char * file_icons_tbl_computer_xpm[] = {
/* columns rows colors chars-per-pixel */
"16 16 42 1",
"r c #4E7FD0",
"$ c #7198D9",
"; c #DCE6F6",
"q c #FFFFFF",
"u c #4A7CCE",
"# c #779DDB",
"w c #95B2E3",
"y c #7FA2DD",
"f c #3263B4",
"= c #EAF0FA",
"< c #B1C7EB",
"% c #6992D7",
"9 c #D9E4F5",
"o c #9BB7E5",
"6 c #F7F9FD",
", c #BED0EE",
"3 c #F0F5FC",
"1 c #A8C0E8",
"  c None",
"0 c #FDFEFF",
"4 c #C4D5F0",
"@ c #81A4DD",
"e c #4377CD",
"- c #E2EAF8",
"i c #9FB9E5",
"> c #CCDAF2",
"+ c #89A9DF",
"s c #5584D1",
"t c #5D89D3",
": c #D2DFF4",
"5 c #FAFCFE",
"2 c #F5F8FD",
"8 c #DFE8F7",
"& c #5E8AD4",
"X c #638ED5",
"a c #CEDCF2",
"p c #90AFE2",
"d c #2F5DA9",
"* c #5282D0",
"7 c #E5EDF9",
". c #A2BCE6",
"O c #8CACE0",
/* pixels */
"                ",
"  .XXXXXXXXXXX  ",
"  oXO++@#$%&*X  ",
"  oX=-;:>,<1%X  ",
"  oX23=-;:4,$X  ",
"  oX5633789:@X  ",
"  oX05623=78+X  ",
"  oXqq05623=OX  ",
"  oX,,,,,<<<$X  ",
"  wXXXXXXXXXXe  ",
"  XrtX%$$y@+O,, ",
"  uyiiiiiiiii@< ",
" ouiiiiiiiiiip<a",
" rustX%$$y@+Ow,,",
" dfffffffffffffd",
"                "
};
#endif // GTK+ < 2.4

// ----------------------------------------------------------------------------
// FileIconsTable & friends
// ----------------------------------------------------------------------------

namespace Nocturnal
{
    // global instance of a FileIconsTable
    static FileIconsTable* g_GlobalFileIconsTable = NULL;
    FileIconsTable& GlobalFileIconsTable()
    {
        if ( !g_GlobalFileIconsTable )
        {
            throw Nocturnal::Exception( TXT( "GlobalFileIconsTable is not initialized!" ) );
        }

        return *g_GlobalFileIconsTable;
    }

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

    class FileIconEntry : public wxObject
    {
    public:
        int id;

        FileIconEntry( int i )
        {
            id = i;
        }        
    };
}

FileIconsTable::FileIconsTable()
: m_HashTable( NULL )
, m_NormalImageList( NULL )
, m_SmallImageList( NULL )
, m_StateImageList( NULL )
{
}

FileIconsTable::~FileIconsTable()
{
    if (m_HashTable)
    {
        WX_CLEAR_HASH_TABLE(*m_HashTable);
        delete m_HashTable;
    }

    if (m_NormalImageList)
    {
        delete m_NormalImageList;
    }

    if (m_SmallImageList)
    {
        delete m_SmallImageList;
    }

    if (m_StateImageList)
    {
        delete m_StateImageList;
    }
}

void FileIconsTable::Create()
{
    NOC_ASSERT(!m_NormalImageList && !m_SmallImageList && !m_StateImageList && !m_HashTable);

    m_HashTable = new wxHashTable(wxKEY_STRING);

    ///////////////////////////////////////////////////////////////////////////
    {
        m_NormalImageList = new wxImageList(32, 32);
    }

    ///////////////////////////////////////////////////////////////////////////
    {
        tchar module[MAX_PATH];
        ::GetModuleFileName( 0, module, MAX_PATH );

        Nocturnal::Path exePath( module );
        Nocturnal::Path iconFolder( exePath.Directory() + TXT( "Icons/16x16/" ) );

        std::set< Nocturnal::Path > artFiles;
        Nocturnal::Directory::GetFiles( iconFolder, artFiles, TXT( "*.png" ), true );

        int numImages = (int)artFiles.size();
        if ( numImages <= 0 )
        {
            numImages = 0;
        }
        numImages += 10;

        m_SmallImageList = new wxImageList( 16, 16, true, numImages );

        // folder:
        m_SmallImageList->Add(wxArtProvider::GetBitmap(wxART_FOLDER,
            wxART_CMN_DIALOG,
            wxSize(16, 16)));
        // folder_open
        m_SmallImageList->Add(wxArtProvider::GetBitmap(wxART_FOLDER_OPEN,
            wxART_CMN_DIALOG,
            wxSize(16, 16)));
        // computer
#ifdef __WXGTK24__
        // GTK24 uses this icon in the file open dialog
        m_SmallImageList->Add(wxArtProvider::GetBitmap(wxART_HARDDISK,
            wxART_CMN_DIALOG,
            wxSize(16, 16)));
#else
        m_SmallImageList->Add(wxIcon(file_icons_tbl_computer_xpm));
#endif
        // drive
        m_SmallImageList->Add(wxArtProvider::GetBitmap(wxART_HARDDISK,
            wxART_CMN_DIALOG,
            wxSize(16, 16)));
        // cdrom
        m_SmallImageList->Add(wxArtProvider::GetBitmap(wxART_CDROM,
            wxART_CMN_DIALOG,
            wxSize(16, 16)));
        // floppy
        m_SmallImageList->Add(wxArtProvider::GetBitmap(wxART_FLOPPY,
            wxART_CMN_DIALOG,
            wxSize(16, 16)));
        // removeable
        m_SmallImageList->Add(wxArtProvider::GetBitmap(wxART_REMOVABLE,
            wxART_CMN_DIALOG,
            wxSize(16, 16)));
        // file
        m_SmallImageList->Add(wxArtProvider::GetBitmap(wxART_NORMAL_FILE,
            wxART_CMN_DIALOG,
            wxSize(16, 16)));
        // executable
        if (GetIconID(wxEmptyString, _T("application/x-executable")) == wxFileIconsTable::file)
        {
            m_SmallImageList->Add(wxArtProvider::GetBitmap(wxART_EXECUTABLE_FILE,
                wxART_CMN_DIALOG,
                wxSize(16, 16)));
            delete m_HashTable->Get(_T("exe"));
            m_HashTable->Delete(_T("exe"));
            m_HashTable->Put(_T("exe"), new FileIconEntry( wxFileIconsTable::executable ) );
        }
        /* else put into list by GetIconID
        (KDE defines application/x-executable for *.exe and has nice icon)
        */

        std::set< Nocturnal::Path >::const_iterator fileItr = artFiles.begin();
        std::set< Nocturnal::Path >::const_iterator fileEnd = artFiles.end();
        for ( ; fileItr != fileEnd; ++fileItr )
        {
            const Nocturnal::Path& filePath = (*fileItr);


            if ( !filePath.Exists() || filePath.Size() <= 0 )
            {
                Log::Warning( TXT( "Unable to load empty image file %s\n" ), filePath.Get().c_str() );
                continue;
            }

            wxImage image;
            if ( !image.LoadFile( filePath.Get().c_str(), wxBITMAP_TYPE_PNG ) )
            {
                Log::Warning( TXT( "Unable to load GUI image %s\n" ), filePath.Get().c_str() );
                continue;
            }
            else
            {
                wxString token = filePath.Filename();
                // strip: iconFolder
                size_t findDot = token.find( TXT( "." ) );
                if ( findDot != wxString::npos )
                    token.erase( findDot );

                // The file was loaded, add it to the image list and store a mapping of file name to index
                int index = m_SmallImageList->Add( wxBitmap( image ) );
                m_HashTable->Put( token, new FileIconEntry( index ) );
            }
        }
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

// This function is currently not unused anymore
#if 0
// finds empty borders and return non-empty area of image:
static wxImage CutEmptyBorders(const wxImage& img)
{
    unsigned char mr = img.GetMaskRed(),
                  mg = img.GetMaskGreen(),
                  mb = img.GetMaskBlue();
    unsigned char *dt = img.GetData(), *dttmp;
    unsigned w = img.GetWidth(), h = img.GetHeight();

    unsigned top, bottom, left, right, i;
    bool empt;

#define MK_DTTMP(x,y)      dttmp = dt + ((x + y * w) * 3)
#define NOEMPTY_PIX(empt)  if (dttmp[0] != mr || dttmp[1] != mg || dttmp[2] != mb) {empt = false; break;}

    for (empt = true, top = 0; empt && top < h; top++)
    {
        MK_DTTMP(0, top);
        for (i = 0; i < w; i++, dttmp+=3)
            NOEMPTY_PIX(empt)
    }
    for (empt = true, bottom = h-1; empt && bottom > top; bottom--)
    {
        MK_DTTMP(0, bottom);
        for (i = 0; i < w; i++, dttmp+=3)
            NOEMPTY_PIX(empt)
    }
    for (empt = true, left = 0; empt && left < w; left++)
    {
        MK_DTTMP(left, 0);
        for (i = 0; i < h; i++, dttmp+=3*w)
            NOEMPTY_PIX(empt)
    }
    for (empt = true, right = w-1; empt && right > left; right--)
    {
        MK_DTTMP(right, 0);
        for (i = 0; i < h; i++, dttmp+=3*w)
            NOEMPTY_PIX(empt)
    }
    top--, left--, bottom++, right++;

    return img.GetSubImage(wxRect(left, top, right - left + 1, bottom - top + 1));
}
#endif // #if 0

#endif // wxUSE_MIMETYPE

int FileIconsTable::GetIconID(const wxString& extension, const wxString& mime)
{
    if (!m_SmallImageList)
        Create();

#if wxUSE_MIMETYPE
    if (!extension.empty())
    {
        FileIconEntry *entry = (FileIconEntry*) m_HashTable->Get(extension);
        if (entry) return (entry -> id);
    }

    wxFileType *ft = (mime.empty()) ?
                   wxTheMimeTypesManager -> GetFileTypeFromExtension(extension) :
                   wxTheMimeTypesManager -> GetFileTypeFromMimeType(mime);

    wxIconLocation iconLoc;
    wxIcon ic;
    {
        wxLogNull logNull;
        if ( ft && ft->GetIcon(&iconLoc) )
        {
            ic = wxIcon( iconLoc );
        }
    }

    delete ft;

    if ( !ic.Ok() )
    {
        int newid = wxFileIconsTable::file;
        m_HashTable->Put(extension, new FileIconEntry(newid));
        return newid;
    }

    wxBitmap bmp;
    bmp.CopyFromIcon(ic);

    if ( !bmp.Ok() )
    {
        int newid = wxFileIconsTable::file;
        m_HashTable->Put(extension, new FileIconEntry(newid));
        return newid;
    }

    const unsigned int size = 16;

    int id = m_SmallImageList->GetImageCount();
    if ((bmp.GetWidth() == (int) size) && (bmp.GetHeight() == (int) size))
    {
        m_SmallImageList->Add(bmp);
    }
#if wxUSE_IMAGE && (!defined(__WXMSW__) || wxUSE_WXDIB)
    else
    {
        wxImage img = bmp.ConvertToImage();

        if ((img.GetWidth() != size*2) || (img.GetHeight() != size*2))
//            m_SmallImageList->Add(CreateAntialiasedBitmap(CutEmptyBorders(img).Rescale(size*2, size*2)));
            m_SmallImageList->Add(CreateAntialiasedBitmap(img.Rescale(size*2, size*2)));
        else
            m_SmallImageList->Add(CreateAntialiasedBitmap(img));
    }
#endif // wxUSE_IMAGE

    m_HashTable->Put(extension, new FileIconEntry(id));
    return id;

#else // !wxUSE_MIMETYPE

    wxUnusedVar(mime);
    if (extension == wxT("exe"))
        return executable;
    else
        return file;
#endif // wxUSE_MIMETYPE/!wxUSE_MIMETYPE
}

#endif // wxUSE_DIRDLG || wxUSE_FILEDLG
