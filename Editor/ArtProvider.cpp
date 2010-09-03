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

wxSize ArtProvider::DefaultImageSize( 16, 16 );

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

    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_ADD_BOOKMARK, TXT( "core/unknown.png" ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_CDROM, TXT( "core/unknown.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_COPY, TXT( "core/copy.png" ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_CROSS_MARK, TXT( "core/unknown.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_CUT, TXT( "core/cut.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_DELETE, TXT( "core/del.png" ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_DEL_BOOKMARK, TXT( "core/unknown.png" ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_ERROR, TXT( "core/unknown.png" ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_EXECUTABLE_FILE, TXT( "core/unknown.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_FILE_OPEN, TXT( "core/open.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_FILE_SAVE, TXT( "core/save.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_FILE_SAVE_AS, TXT( "core/saveas.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_FIND, TXT( "editor/find.png" ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_FIND_AND_REPLACE, TXT( "core/unknown.png" ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_FLOPPY, TXT( "core/unknown.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_FOLDER, TXT( "editor/dir.png" ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_FOLDER_OPEN, TXT( "core/unknown.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_GO_BACK, TXT( "editor/prev.png" ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_GO_DIR_UP, TXT( "core/unknown.png" ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_GO_DOWN, TXT( "core/unknown.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_GO_FORWARD, TXT( "editor/next.png" ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_GO_HOME, TXT( "core/unknown.png" ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_GO_TO_PARENT, TXT( "core/unknown.png" ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_GO_UP, TXT( "core/unknown.png" ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_HARDDISK, TXT( "core/unknown.png" ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_HELP, TXT( "core/unknown.png" ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_HELP_BOOK, TXT( "core/unknown.png" ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_HELP_FOLDER, TXT( "core/unknown.png" ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_HELP_PAGE, TXT( "core/unknown.png" ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_HELP_SETTINGS, TXT( "core/unknown.png" ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_HELP_SIDE_PANEL, TXT( "core/unknown.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_INFORMATION, TXT( "core/info.png" ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_LIST_VIEW, TXT( "core/unknown.png" ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_MISSING_IMAGE, TXT( "core/unknown.png" ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_NEW, TXT( "core/unknown.png" ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_NEW_DIR, TXT( "core/unknown.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_NORMAL_FILE, TXT( "editor/file.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_PASTE, TXT( "core/paste.png" ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_PRINT, TXT( "core/unknown.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_QUESTION, TXT( "core/unknown.png" ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_QUIT, TXT( "core/unknown.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_REDO, TXT( "core/redo.png" ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_REMOVABLE, TXT( "core/unknown.png" ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_REPORT_VIEW, TXT( "core/unknown.png" ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_TICK_MARK, TXT( "core/unknown.png" ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_TIP, TXT( "core/unknown.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_UNDO, TXT( "core/undo.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_WARNING, TXT( "core/warning.png" ) ) );

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Unknown, TXT( "core/unknown.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Null, TXT( "" ) ) );

    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::File, TXT( "core/unknown.png" ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Folder, TXT( "core/unknown" ) ) );

    // Overlays
    {
        //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Verbs::Create, TXT( "core/unknown.png" ) ) );
        //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Verbs::Add, TXT( "core/unknown.png" ) ) );
        //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Verbs::Delete, TXT( "core/unknown.png" ) ) );
        //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Verbs::Select, TXT( "core/unknown.png" ) ) );
        //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Verbs::Refresh, TXT( "core/unknown.png" ) ) );
        //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Verbs::Find, TXT( "core/unknown.png" ) ) );

        //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Statuses::Lockable, TXT( "core/unknown.png" ) ) );
        //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Statuses::Locked, TXT( "core/unknown.png" ) ) );
        //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Statuses::Busy, TXT( "core/unknown.png" ) ) );
        //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Statuses::Information, TXT( "core/unknown.png" ) ) );
        //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Statuses::Warning, TXT( "core/unknown.png" ) ) );
        //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Statuses::Error, TXT( "core/unknown.png" ) ) );
        //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Statuses::Binary, TXT( "core/unknown.png" ) ) );
        //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Statuses::Text, TXT( "core/unknown.png" ) ) );
    }

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Open, TXT( "core/open.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Save, TXT( "core/save.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::SaveAs, TXT( "core/saveas.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::SaveAll, TXT( "core/saveall.png" ) ) );

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::SelectAll, TXT( "core/selectall.png" ) ) );

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Cut, TXT( "core/cut.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Copy, TXT( "core/copy.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Paste, TXT( "core/paste.png" ) ) );

    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Create, TXT( "core/unknown.png" ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Add, TXT( "core/unknown.png" ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Delete, TXT( "core/unknown.png" ) ) );

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Undo, TXT( "core/undo.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Redo, TXT( "core/redo.png" ) ) );

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Options, TXT( "core/option.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Settings, TXT( "core/option.png" ) ) );

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Find, TXT( "editor/find.png" ) ) );

    // RevisionControl
    {
        m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::Unknown, TXT( "rev/unknown.png" ) ) );
        m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::Active, TXT( "rev/active.png" ) ) );
        //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::CheckedOutByMe, TXT( "core/unknown.png" ) ) );
        //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::CheckedOutBySomeoneElse, TXT( "core/unknown.png" ) ) );
        m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::OutOfDate, TXT( "rev/outofdate.png" ) ) );
        m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::CheckOut, TXT( "rev/chout.png" ) ) );
        m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::CheckIn, TXT( "rev/chin.png" ) ) );
        m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::Submit, TXT( "rev/submit.png" ) ) );
        m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::Revert, TXT( "rev/revert.png" ) ) );
        m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::Merge, TXT( "rev/merge.png" ) ) );
        m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::History, TXT( "rev/history.png" ) ) );
        //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::File, TXT( "core/unknown.png" ) ) );
        m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::Changelist, TXT( "rev/changelist.png" ) ) );
        //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::CreateChangelist, TXT( "core/unknown.png" ) ) );
        m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::User, TXT( "rev/user.png" ) ) );
    }

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Scene, TXT( "editor/scene.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Entity, TXT( "editor/entity.png" ) ) );

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::FrameOrigin, TXT( "editor/frmorigin.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::FrameSelected, TXT( "editor/framesel.png" ) ) );

    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::PreviousView, TXT( "core/unknown.png" ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::NextView, TXT( "core/unknown.png" ) ) );

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::HighlightMode, TXT( "editor/hilite.png" ) ) );
    
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::PerspectiveCamera, TXT( "editor/perspective.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::FrontOrthoCamera, TXT( "editor/front.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::SideOrthoCamera, TXT( "editor/side.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::TopOrthoCamera, TXT( "editor/top.png" ) ) );

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::ShowAxes, TXT( "editor/axes.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::ShowGrid, TXT( "editor/grid.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::ShowBounds, TXT( "editor/bounds.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::ShowStatistics, TXT( "editor/stats.png" ) ) );

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::FrustumCulling, TXT( "editor/frustum.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::BackfaceCulling, TXT( "editor/backculling.png" ) ) );

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::ShadingWireframe, TXT( "editor/wire.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::ShadingMaterial, TXT( "editor/materials.png" ) ) );

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::ColorModeScene, TXT( "editor/cmscene.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::ColorModeLayer, TXT( "editor/cmlayers.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::ColorModeNodeType, TXT( "editor/cmtype.png" ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::ColorModeScale, TXT( "core/unknown.png" ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::ColorModeScaleGradient, TXT( "core/unknown.png" ) ) );

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::SelectTool, TXT( "core/sel.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::TranslateTool, TXT( "editor/translate.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RotateTool, TXT( "editor/rotate.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::ScaleTool, TXT( "editor/scale.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::DuplicateTool, TXT( "editor/duplicate.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::EntityPlacementTool, TXT( "editor/entity.png" ) ) );

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Camera, TXT( "editor/cam.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Locator, TXT( "editor/locator.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Volume, TXT( "editor/vol.png" ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Curve, TXT( "core/unknown.png" ) ) );

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::ProjectFolder, TXT( "editor/pdir.png" ) ) );

    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::CreateNewLayer, TXT( "core/unknown.png" ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::CreateNewLayerFromSelection, TXT( "core/unknown.png" ) ) ); 
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::DeleteSelectedLayers, TXT( "editor/dellayer.png" ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::AddSelectionToLayers, TXT( "core/unknown.png" ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RemoveSelectionFromLayers, TXT( "core/unknown.png" ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::SelectLayerMembers, TXT( "core/unknown.png" ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::SelectLayers, TXT( "core/unknown.png" ) ) );

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::TaskWindow, TXT( "editor/task.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::EditorIcon, TXT( "editor/editor.png" ) ) );

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Play, TXT( "editor/play.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Pause, TXT( "editor/pause.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Stop, TXT( "editor/stop.png" ) ) );
    
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Vault, TXT( "editor/vault.png" ) ) );
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
            findFilename = m_ArtIDToFilename.find( ArtIDs::Unknown );
        }

        if ( findFilename != m_ArtIDToFilename.end() && !findFilename->second.empty() )
        {
            const tstring& icon = findFilename->second;

            int width = size.GetWidth() > 0 ? size.GetWidth() : DefaultImageSize.GetWidth();
            int height = size.GetHeight() > 0 ? size.GetHeight() : DefaultImageSize.GetHeight();

            Helium::Path exePath( wxStandardPaths::Get().GetExecutablePath().c_str() );

            tstringstream strm;
            strm << exePath.Directory() << TXT( "Icons/" ) << width << TXT( 'x' ) << height << TXT( '/' ) << icon;
            Helium::Path imageFile( strm.str() );

            if ( !imageFile.Exists() || imageFile.Size() <= 0 )
            {
                HELIUM_BREAK();
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
