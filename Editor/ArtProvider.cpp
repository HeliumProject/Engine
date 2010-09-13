#include "Precompile.h"
#include "ArtProvider.h"

#include "Platform/Exception.h"
#include "Foundation/Container/Insert.h"
#include "Foundation/Log.h"

#include <sstream>

#include <wx/hash.h>
#include <wx/image.h>
#include <wx/stdpaths.h>

using namespace Helium;
using namespace Helium::Editor;
using Helium::Insert; 

wxSize ArtProvider::DefaultIconSize( 24, 24 );

///////////////////////////////////////////////////////////////////////////////
// Cache class - stores already requested bitmaps
//
// Copied from wxWidgets src\common\artprov.cpp
///////////////////////////////////////////////////////////////////////////////
namespace Helium
{
    namespace Editor
    {
        WX_DECLARE_STRING_HASH_MAP_WITH_DECL( wxBitmap, ArtProviderBitmapsHash, class );

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
}

///////////////////////////////////////////////////////////////////////////////
IconArtFile::IconArtFile( const tstring& filename )
    : m_Filename( filename )
{
}

///////////////////////////////////////////////////////////////////////////////
IconArtFile& IconArtFile::AddOverlay( const tchar* filename, OverlayQuadrants::OverlayQuadrant quadrant )
{
    m_Overlays[quadrant] = filename;
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
void IconArtFile::CalculatePlacement( wxImage &target_image, const wxImage &source_image, OverlayQuadrants::OverlayQuadrant quadrant, int &x, int &y )
{
    switch ( quadrant )
    {
    default:
    case OverlayQuadrants::BottomRight:
        x = target_image.GetWidth() - source_image.GetWidth();
        y = target_image.GetHeight() - source_image.GetHeight();
        break;

    case OverlayQuadrants::TopLeft:
        x = 0;
        y = 0;
        break;

    case OverlayQuadrants::TopRight:
        x = target_image.GetWidth() - source_image.GetWidth();
        y = 0;
        break;

    case OverlayQuadrants::BottomLeft:
        x = 0;
        y = target_image.GetHeight() - source_image.GetHeight();
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////
void IconArtFile::Paste( wxImage &target_image, const wxImage &source_image, int &x, int &y, bool blendAlpha )
{

    // add offset to adjust for negative insertion point
    int offset_x = 0;
    int offset_y = 0;

    // actual width and height that will be copied from source to target
    int target_width = source_image.GetWidth();
    int target_height = source_image.GetHeight();

    if ( x < 0 )
    {
        offset_x = -x;
        target_width += x;
    }
    if ( y < 0 )
    {
        offset_y = -y;
        target_height += y;
    }

    // actual insertion point onto the target image
    int target_x = x + offset_x;
    int target_y = y + offset_y;

    if (target_x+target_width > target_image.GetWidth() )
    {
        target_width = target_image.GetWidth() - target_x;
    }

    if (target_y+target_height > target_image.GetHeight() )
    {
        target_height = target_image.GetHeight() - target_y;
    }

    if ( target_width < 1 || target_height < 1 )
    {
        return;
    }

    ///////////////////////////////////////////
    // Masking
    bool maskSource = false;
    unsigned char source_mask_r = 0;
    unsigned char source_mask_g = 0;
    unsigned char source_mask_b = 0;

    // if the source image has a mask and either the target image does NOT 
    // or the target image's mask is different than the source
    // then use the source mask when pasting onto the target 
    if ( source_image.HasMask()
        && ( !target_image.HasMask()
           || target_image.GetMaskRed() != source_image.GetMaskRed()
           || target_image.GetMaskGreen() != source_image.GetMaskGreen()
           || target_image.GetMaskBlue() != source_image.GetMaskBlue() ) )
    {
        maskSource = true;
        source_mask_r = source_image.GetMaskRed();
        source_mask_g = source_image.GetMaskGreen();
        source_mask_b = source_image.GetMaskBlue();
    }


    ///////////////////////////////////////////
    // Alpha blending
    bool hasAlpha = false;

    unsigned char* source_alpha_data = NULL;
    int source_alpha_step = 0;
    
    unsigned char* target_alpha_data = NULL;
    int target_alpha_step = 0;

    if ( source_image.HasAlpha() )
    {
        if ( !target_image.HasAlpha() )
        {
            target_image.InitAlpha();
        }

        source_alpha_data = source_image.GetAlpha() + offset_x + offset_y*source_image.GetWidth();
        source_alpha_step = source_image.GetWidth();

        target_alpha_data = target_image.GetAlpha() + (target_x) + (target_y)*target_image.GetWidth();
        target_alpha_step = target_image.GetWidth();

        hasAlpha = true;
    }

    // can't blend alpha if there is none
    blendAlpha &= hasAlpha;

    unsigned char* source_data = source_image.GetData() + offset_x*3 + offset_y*3*source_image.GetWidth();
    int source_data_step = source_image.GetWidth()*3;

    unsigned char* target_data = target_image.GetData() + (target_x)*3 + (target_y)*3*target_image.GetWidth();
    int target_data_step = target_image.GetWidth()*3;

    for ( int height_index = 0; height_index < target_height; ++height_index )
    {
        for ( int width_index = 0; width_index < target_width; ++width_index )
        {
            int r_index = width_index*3;
            int g_index = width_index*3 + 1;
            int b_index = width_index*3 + 2;

            // paste the pixel if either:
            // - not masking the source
            // - are masking, and the source pixel is NOT masked
            if ( !maskSource
                || !(  ( source_data[r_index] == source_mask_r )
                    && ( source_data[g_index] == source_mask_g )
                    && ( source_data[b_index] == source_mask_b ) ) )
            {
                if ( blendAlpha )
                {
                    // value of 255 means that the pixel is 100% opaque
                    float source_alpha = ( source_alpha_data[width_index] / 255.f );
                    float inverse_source_alpha = 1 - source_alpha;

                    target_data[r_index] = ( inverse_source_alpha * target_data[r_index] ) + ( source_data[r_index] * source_alpha );
                    target_data[g_index] = ( inverse_source_alpha * target_data[g_index] ) + ( source_data[g_index] * source_alpha );
                    target_data[b_index] = ( inverse_source_alpha * target_data[b_index] ) + ( source_data[b_index] * source_alpha );

                    // merge alpha channels
                    target_alpha_data[width_index] = ( target_alpha_data[width_index] + source_alpha_data[width_index] ) > 255
                        ? 255
                        : target_alpha_data[width_index] + source_alpha_data[width_index];
                }
                else
                {
                    if ( hasAlpha )
                    {
                        target_alpha_data[width_index] = source_alpha_data[width_index];
                    }

                    memcpy( target_data+r_index, source_data+r_index, 3 );
                }
            }
        }

        if ( hasAlpha )
        {
            source_alpha_data += source_alpha_step;
            target_alpha_data += target_alpha_step;
        }

        source_data += source_data_step;
        target_data += target_data_step;
    }
}

///////////////////////////////////////////////////////////////////////////////
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
    HELIUM_ASSERT(!m_ArtProviderCache);

    m_ArtProviderCache = new ArtProviderCache();
    m_ArtProviderCache->Clear();

    m_ArtIDToFilename.clear();

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Unknown, IconArtFile( TXT( "core/unknown.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Null, IconArtFile( TXT( "core/unknown.png" ) ) ) );

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Helium, IconArtFile( TXT( "editor/editor.png" ) ) ) );

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::File, IconArtFile( TXT( "editor/file.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Folder, IconArtFile( TXT( "editor/dir.png" ) ) ) );

    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_ADD_BOOKMARK, IconArtFile( TXT( "core/unknown.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_CDROM, IconArtFile( TXT( "core/unknown.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_COPY, IconArtFile( TXT( "core/copy.png" ) ) ) );
    
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_CROSS_MARK, IconArtFile( TXT( "core/unknown.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_CUT, IconArtFile( TXT( "core/cut.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_DELETE, IconArtFile( TXT( "core/del.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_DEL_BOOKMARK, IconArtFile( TXT( "core/unknown.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_ERROR, IconArtFile( TXT( "core/error.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_EXECUTABLE_FILE, IconArtFile( TXT( "core/unknown.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_FILE_OPEN, IconArtFile( TXT( "core/open.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_FILE_SAVE, IconArtFile( TXT( "core/save.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_FILE_SAVE_AS, IconArtFile( TXT( "core/saveas.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_FIND, IconArtFile( TXT( "editor/find.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_FIND_AND_REPLACE, IconArtFile( TXT( "core/unknown.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_FLOPPY, IconArtFile( TXT( "core/unknown.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_FOLDER, IconArtFile( TXT( "editor/dir.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_FOLDER_OPEN, IconArtFile( TXT( "core/unknown.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_GO_BACK, IconArtFile( TXT( "editor/prev.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_GO_DIR_UP, IconArtFile( TXT( "core/unknown.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_GO_DOWN, IconArtFile( TXT( "core/unknown.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_GO_FORWARD, IconArtFile( TXT( "editor/next.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_GO_HOME, IconArtFile( TXT( "core/unknown.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_GO_TO_PARENT, IconArtFile( TXT( "core/unknown.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_GO_UP, IconArtFile( TXT( "core/unknown.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_HARDDISK, IconArtFile( TXT( "core/unknown.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_HELP, IconArtFile( TXT( "core/unknown.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_HELP_BOOK, IconArtFile( TXT( "core/unknown.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_HELP_FOLDER, IconArtFile( TXT( "core/unknown.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_HELP_PAGE, IconArtFile( TXT( "core/unknown.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_HELP_SETTINGS, IconArtFile( TXT( "core/unknown.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_HELP_SIDE_PANEL, IconArtFile( TXT( "core/unknown.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_INFORMATION, IconArtFile( TXT( "core/info.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_LIST_VIEW, IconArtFile( TXT( "core/unknown.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_MISSING_IMAGE, IconArtFile( TXT( "core/unknown.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_NEW, IconArtFile( TXT( "core/unknown.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_NEW_DIR, IconArtFile( TXT( "core/unknown.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_NORMAL_FILE, IconArtFile( TXT( "editor/file.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_PASTE, IconArtFile( TXT( "core/paste.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_PRINT, IconArtFile( TXT( "core/unknown.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_QUESTION, IconArtFile( TXT( "core/unknown.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_QUIT, IconArtFile( TXT( "core/unknown.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_REDO, IconArtFile( TXT( "core/redo.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_REMOVABLE, IconArtFile( TXT( "core/unknown.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_REPORT_VIEW, IconArtFile( TXT( "core/unknown.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_TICK_MARK, IconArtFile( TXT( "core/unknown.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_TIP, IconArtFile( TXT( "core/unknown.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_UNDO, IconArtFile( TXT( "core/undo.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_WARNING, IconArtFile( TXT( "core/warning.png" ) ) ) );

    // Overlays
    {
        // Verbs
        m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Verbs::Create, IconArtFile( TXT( "overlay/create.png" ) ) ) );
        m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Verbs::Add, IconArtFile( TXT( "overlay/add.png" ) ) ) );
        m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Verbs::Edit, IconArtFile( TXT( "overlay/edit.png" ) ) ) );
        m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Verbs::Delete, IconArtFile( TXT( "overlay/del.png" ) ) ) );
        m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Verbs::Select, IconArtFile( TXT( "overlay/sel.png" ) ) ) );
        m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Verbs::Refresh, IconArtFile( TXT( "overlay/refresh.png" ) ) ) );
        m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Verbs::Find, IconArtFile( TXT( "overlay/find.png" ) ) ) );

        // Statuses
        m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Statuses::Lockable, IconArtFile( TXT( "overlay/lockable.png" ) ) ) );
        m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Statuses::Locked, IconArtFile( TXT( "overlay/locked.png" ) ) ) );
        m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Statuses::Busy, IconArtFile( TXT( "overlay/busy.png" ) ) ) );
        m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Statuses::Information, IconArtFile( TXT( "overlay/info.png" ) ) ) );
        m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Statuses::Warning, IconArtFile( TXT( "overlay/warning.png" ) ) ) );
        m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Statuses::Error, IconArtFile( TXT( "overlay/error.png" ) ) ) );
        m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Statuses::Binary, IconArtFile( TXT( "overlay/binary.png" ) ) ) );
        m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Statuses::Text, IconArtFile( TXT( "overlay/txt.png" ) ) ) );
    }

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Open, IconArtFile( TXT( "core/open.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Save, IconArtFile( TXT( "core/save.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::SaveAs, IconArtFile( TXT( "core/saveas.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::SaveAll, IconArtFile( TXT( "core/saveall.png" ) ) ) );

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::SelectAll, IconArtFile( TXT( "core/selectall.png" ) ) ) );

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Cut, IconArtFile( TXT( "core/cut.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Copy, IconArtFile( TXT( "core/copy.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Paste, IconArtFile( TXT( "core/paste.png" ) ) ) );

    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Create, IconArtFile( TXT( "core/unknown.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Add, IconArtFile( TXT( "core/unknown.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Delete, IconArtFile( TXT( "core/unknown.png" ) ) ) );

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Undo, IconArtFile( TXT( "core/undo.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Redo, IconArtFile( TXT( "core/redo.png" ) ) ) );

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Options, IconArtFile( TXT( "core/option.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Settings, IconArtFile( TXT( "core/option.png" ) ) ) );

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Find, IconArtFile( TXT( "editor/find.png" ) ) ) );

    // RevisionControl
    {
        m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::Unknown, IconArtFile( TXT( "rev/unknown.png" ) ) ) );
        m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::Active, IconArtFile( TXT( "rev/active.png" ) ) ) );
        //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::CheckedOutByMe, IconArtFile( TXT( "core/unknown.png" ) ) ) );
        //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::CheckedOutBySomeoneElse, IconArtFile( TXT( "core/unknown.png" ) ) ) );
        m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::OutOfDate, IconArtFile( TXT( "rev/outofdate.png" ) ) ) );
        m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::CheckOut, IconArtFile( TXT( "rev/chout.png" ) ) ) );
        m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::CheckIn, IconArtFile( TXT( "rev/chin.png" ) ) ) );
        m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::Submit, IconArtFile( TXT( "rev/submit.png" ) ) ) );
        m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::Revert, IconArtFile( TXT( "rev/revert.png" ) ) ) );
        m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::Merge, IconArtFile( TXT( "rev/merge.png" ) ) ) );
        m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::History, IconArtFile( TXT( "rev/history.png" ) ) ) );
        //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::File, IconArtFile( TXT( "core/unknown.png" ) ) ) );
        m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::Changelist, IconArtFile( TXT( "rev/changelist.png" ) ) ) );
        //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::CreateChangelist, IconArtFile( TXT( "core/unknown.png" ) ) ) );
        m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::User, IconArtFile( TXT( "rev/user.png" ) ) ) );
    }

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Scene, IconArtFile( TXT( "editor/scene.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Entity, IconArtFile( TXT( "editor/entity.png" ) ) ) );

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::FrameOrigin, IconArtFile( TXT( "editor/frmorigin.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::FrameSelected, IconArtFile( TXT( "editor/framesel.png" ) ) ) );

    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::PreviousView, IconArtFile( TXT( "core/unknown.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::NextView, IconArtFile( TXT( "core/unknown.png" ) ) ) );

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::HighlightMode, IconArtFile( TXT( "editor/hilite.png" ) ) ) );
    
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::PerspectiveCamera, IconArtFile( TXT( "editor/perspective.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::FrontOrthoCamera, IconArtFile( TXT( "editor/front.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::SideOrthoCamera, IconArtFile( TXT( "editor/side.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::TopOrthoCamera, IconArtFile( TXT( "editor/top.png" ) ) ) );

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::ShowAxes, IconArtFile( TXT( "editor/axes.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::ShowGrid, IconArtFile( TXT( "editor/grid.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::ShowBounds, IconArtFile( TXT( "editor/bounds.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::ShowStatistics, IconArtFile( TXT( "editor/stats.png" ) ) ) );

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::FrustumCulling, IconArtFile( TXT( "editor/frustum.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::BackfaceCulling, IconArtFile( TXT( "editor/backculling.png" ) ) ) );

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::ShadingWireframe, IconArtFile( TXT( "editor/wire.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::ShadingMaterial, IconArtFile( TXT( "editor/materials.png" ) ) ) );

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::ColorModeScene, IconArtFile( TXT( "editor/cmscene.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::ColorModeLayer, IconArtFile( TXT( "editor/cmlayers.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::ColorModeNodeType, IconArtFile( TXT( "editor/cmtype.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::ColorModeScale, IconArtFile( TXT( "core/unknown.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::ColorModeScaleGradient, IconArtFile( TXT( "core/unknown.png" ) ) ) );

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::SelectTool, IconArtFile( TXT( "core/sel.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::TranslateTool, IconArtFile( TXT( "editor/translate.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RotateTool, IconArtFile( TXT( "editor/rotate.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::ScaleTool, IconArtFile( TXT( "editor/scale.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::DuplicateTool, IconArtFile( TXT( "editor/duplicate.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::EntityPlacementTool, IconArtFile( TXT( "editor/entity.png" ) ) ) );

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Camera, IconArtFile( TXT( "editor/cam.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Locator, IconArtFile( TXT( "editor/locator.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Volume, IconArtFile( TXT( "editor/vol.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Curve, IconArtFile( TXT( "core/unknown.png" ) ) ) );

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::ProjectFolder, IconArtFile( TXT( "editor/pdir.png" ) ) ) );

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::CreateNewLayer, IconArtFile( TXT( "editor/layers.png" ) ).AddOverlay( TXT( "overlay/create.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::CreateNewLayerFromSelection, IconArtFile( TXT( "editor/newsel.png" ) ) ) ); 
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::DeleteSelectedLayers, IconArtFile( TXT( "editor/layers.png" ) ).AddOverlay( TXT( "overlay/del.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::AddSelectionToLayers, IconArtFile( TXT( "core/unknown.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RemoveSelectionFromLayers, IconArtFile( TXT( "core/unknown.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::SelectLayerMembers, IconArtFile( TXT( "editor/layermember.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::SelectLayers, IconArtFile( TXT( "editor/layers.png" ) ) ) );

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::TaskWindow, IconArtFile( TXT( "editor/task.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::EditorIcon, IconArtFile( TXT( "editor/editor.png" ) ) ) );

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Play, IconArtFile( TXT( "editor/play.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Pause, IconArtFile( TXT( "editor/pause.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Stop, IconArtFile( TXT( "editor/stop.png" ) ) ) );
    
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Vault, IconArtFile( TXT( "editor/vault.png" ) ) ) );
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
        if ( findFilename == m_ArtIDToFilename.end() || findFilename->second.m_Filename.empty() )
        {
            findFilename = m_ArtIDToFilename.find( ArtIDs::Unknown );
        }

        if ( findFilename != m_ArtIDToFilename.end() && !findFilename->second.m_Filename.empty() )
        {
            const tstring& icon = findFilename->second.m_Filename;

            int width = size.GetWidth() > 0 ? size.GetWidth() : DefaultIconSize.GetWidth();
            int height = size.GetHeight() > 0 ? size.GetHeight() : DefaultIconSize.GetHeight();

            Helium::Path exePath( wxStandardPaths::Get().GetExecutablePath().c_str() );

            tstringstream strm;
            strm << exePath.Directory() << TXT( "Icons/" ) << width << TXT( 'x' ) << height << TXT( '/' ) << icon;
            Helium::Path imageFile( strm.str() );
            if ( !imageFile.Exists() || imageFile.Size() <= 0 )
            {
                HELIUM_BREAK();
            }

            wxImage image( imageFile.Get().c_str(), wxBITMAP_TYPE_PNG );
            HELIUM_ASSERT( image.Ok() );
            if ( image.GetWidth() != width || image.GetHeight() != height )
            {
                image.Rescale( width, height );
            }

            if ( !findFilename->second.m_Overlays.empty() )
            {
                int overlayWidth = image.GetWidth() / 2;
                int overlayHeight = image.GetHeight() / 2;

                for ( M_OverlayQuadrants::const_iterator itr = findFilename->second.m_Overlays.begin(),
                    end = findFilename->second.m_Overlays.end(); 
                    itr != end;
                    ++itr )
                {
                    tstringstream overlayStrm;
                    overlayStrm << exePath.Directory() << TXT( "Icons/" ) << overlayWidth << TXT( 'x' ) << overlayHeight << TXT( '/' ) << itr->second;
                    Helium::Path overlayImageFile( overlayStrm.str() );
                    if ( !overlayImageFile.Exists() || overlayImageFile.Size() <= 0 )
                    {
                        HELIUM_BREAK();
                    }      

                    wxImage overlayImage( overlayImageFile.Get().c_str(), wxBITMAP_TYPE_PNG );
                    HELIUM_ASSERT( overlayImage.Ok() );
                    if ( image.GetWidth() != width || image.GetHeight() != height )
                    {
                        image.Rescale( overlayWidth, overlayHeight );
                    }

                    int x = 0;
                    int y = 0;
                    IconArtFile::CalculatePlacement( image, overlayImage, itr->first, x, y );
                    IconArtFile::Paste( image, overlayImage, x, y, true );
                }
            }

            bitmap = wxBitmap( image );

            m_ArtProviderCache->PutBitmap( hashId, bitmap );
        }
    }

    return bitmap;
}