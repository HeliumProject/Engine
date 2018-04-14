#include "EditorPch.h"
#include "ArtProvider.h"

#include "Platform/Exception.h"
#include "Foundation/Log.h"

#include <sstream>

#include <wx/hash.h>
#include <wx/image.h>
#include <wx/stdpaths.h>

using namespace Helium;
using namespace Helium::Editor;

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
IconArtFile::IconArtFile( const std::string& filename )
: m_Filename( filename )
{
}

///////////////////////////////////////////////////////////////////////////////
IconArtFile& IconArtFile::AddOverlay( const char* filename, OverlayQuadrants::OverlayQuadrant quadrant )
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
        HELIUM_BREAK();
        break;

    case OverlayQuadrants::TopLeft:
        x = 0;
        y = 0;
        break;

    case OverlayQuadrants::TopCenter:
        x = ( target_image.GetWidth() / 2 ) - ( source_image.GetWidth() / 2 );
        y = 0;
        break;

    case OverlayQuadrants::TopRight:
        x = target_image.GetWidth() - source_image.GetWidth();
        y = 0;
        break;

    case OverlayQuadrants::CenterLeft:
        x = 0;
        y = ( target_image.GetHeight() / 2 ) - ( source_image.GetHeight() / 2 );
        break;

    case OverlayQuadrants::Centered:
        x = ( target_image.GetWidth() / 2 ) - ( source_image.GetWidth() / 2 );
        y = ( target_image.GetHeight() / 2 ) - ( source_image.GetHeight() / 2 );
        break;

    case OverlayQuadrants::CenterRight:
        x = target_image.GetWidth() - source_image.GetWidth();
        y = ( target_image.GetHeight() / 2 ) - ( source_image.GetHeight() / 2 );
        break;

    case OverlayQuadrants::BottomLeft:
        x = 0;
        y = target_image.GetHeight() - source_image.GetHeight();
        break;

    case OverlayQuadrants::BottomCenter:
        x = ( target_image.GetWidth() / 2 ) - ( source_image.GetWidth() / 2 );
        y = target_image.GetHeight() - source_image.GetHeight();
        break;

    case OverlayQuadrants::BottomRight:
        x = target_image.GetWidth() - source_image.GetWidth();
        y = target_image.GetHeight() - source_image.GetHeight();
        break;
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


    //
    // wxART_* IDs
    //
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_COPY,          IconArtFile( "actions/copy.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_CUT,           IconArtFile( "actions/cut.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_DELETE,        IconArtFile( "actions/delete.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_ERROR,         IconArtFile( "actions/error.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_FILE_OPEN,     IconArtFile( "actions/open.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_FILE_SAVE,     IconArtFile( "actions/save.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_FILE_SAVE_AS,  IconArtFile( "actions/save_as.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_FIND,          IconArtFile( "actions/find.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_FOLDER,        IconArtFile( "filesystem/folder.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_HELP,          IconArtFile( "status/unknown.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_HELP_FOLDER,   IconArtFile( "filesystem/folder.png" ).AddOverlay( "status/unknown.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_HELP_SETTINGS, IconArtFile( "actions/prefernces.png" ).AddOverlay( "status/unknown.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_INFORMATION,   IconArtFile( "status/info.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_NEW,           IconArtFile( "actions/create.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_NEW_DIR,       IconArtFile( "filesystem/folder.png" ).AddOverlay( "actions/create.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_NORMAL_FILE,   IconArtFile( "filesystem/file.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_PASTE,         IconArtFile( "actions/paste.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_QUESTION,      IconArtFile( "status/unknown.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_QUIT,          IconArtFile( "status/error.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_REDO,          IconArtFile( "actions/redo.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_TIP,           IconArtFile( "status/info.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_UNDO,          IconArtFile( "actions/undo.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_WARNING,       IconArtFile( "actions/warning.png" ) ) );


    //
    // Actions
    //
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Actions::Add,        IconArtFile( "actions/add.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Actions::Copy,       IconArtFile( "actions/copy.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Actions::Create,     IconArtFile( "actions/create.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Actions::Cut,        IconArtFile( "actions/cut.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Actions::Delete,     IconArtFile( "actions/delete.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Actions::Down,       IconArtFile( "actions/arrow_down.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Actions::Edit,       IconArtFile( "actions/edit.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Actions::FileAdd,    IconArtFile( "filesystem/file.png" ).AddOverlay( "actions/add.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Actions::FileDelete, IconArtFile( "filesystem/file.png" ).AddOverlay( "actions/delete.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Actions::Find,       IconArtFile( "actions/find.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Actions::Next,       IconArtFile( "editor/view_next.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Actions::Open,       IconArtFile( "actions/open.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Actions::Options,    IconArtFile( "actions/options.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Actions::Paste,      IconArtFile( "actions/paste.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Actions::Pause,      IconArtFile( "actions/pause.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Actions::Play,       IconArtFile( "actions/play.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Actions::Previous,   IconArtFile( "editor/view_previous.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Actions::Redo,       IconArtFile( "actions/redo.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Actions::Refresh,    IconArtFile( "actions/refresh.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Actions::Save,       IconArtFile( "actions/save.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Actions::SaveAll,    IconArtFile( "actions/save_all.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Actions::SaveAs,     IconArtFile( "actions/save_as.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Actions::Select,     IconArtFile( "actions/select.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Actions::SelectAll,  IconArtFile( "actions/select_all.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Actions::Settings,   IconArtFile( "actions/preferences.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Actions::Stop,       IconArtFile( "actions/stop.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Actions::Undo,       IconArtFile( "actions/undo.png" ) ) );


    //
    // Editor
    //
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::BackfaceCulling,             IconArtFile( "editor/backculling.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::Camera,                      IconArtFile( "editor/camera.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::ColorModeLayer,              IconArtFile( "editor/color_mode_layers.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::ColorModeNodeType,           IconArtFile( "editor/color_mode_type.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::ColorModeScene,              IconArtFile( "editor/color_mode_scene.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::CreateNewLayer,              IconArtFile( "editor/layers.png" ).AddOverlay( "actions/create.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::CreateNewLayerFromSelection, IconArtFile( "editor/layer_new_from_selection.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::Curve,                       IconArtFile( "status/unknown.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::CurveEdit,                   IconArtFile( "status/unknown.png" ).AddOverlay( "actions/edit.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::DeleteSelectedLayers,        IconArtFile( "editor/layers.png" ).AddOverlay( "actions/delete.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::DuplicateTool,               IconArtFile( "editor/duplicate.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::EditorIcon,                  IconArtFile( "editor/editor.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::Entity,                      IconArtFile( "editor/entity.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::EntityPlacementTool,         IconArtFile( "editor/entity.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::FrameOrigin,                 IconArtFile( "editor/frame_origin.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::FrameSelected,               IconArtFile( "editor/frame_selected.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::FrontOrthoCamera,            IconArtFile( "editor/view_front.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::FrustumCulling,              IconArtFile( "editor/frustum_culling.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::Helium,                      IconArtFile( "editor/editor.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::HighlightMode,               IconArtFile( "editor/highlight.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::Locator,                     IconArtFile( "editor/locator.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::NewProject,                  IconArtFile( "editor/project.png" ).AddOverlay( "actions/create.png", OverlayQuadrants::TopRight ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::NextView,                    IconArtFile( "editor/view_next.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::PerspectiveCamera,           IconArtFile( "editor/view_perspective.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::PreviousView,                IconArtFile( "editor/view_previous.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::ProjectFile,                 IconArtFile( "editor/project.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::ProjectFolder,               IconArtFile( "editor/project_folder.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::RotateTool,                  IconArtFile( "editor/rotate.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::ScaleTool,                   IconArtFile( "editor/scale.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::Scene,                       IconArtFile( "editor/scene.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::SelectLayerMembers,          IconArtFile( "editor/layer_member.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::SelectLayers,                IconArtFile( "editor/layers.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::SelectTool,                  IconArtFile( "actions/select.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::ShadingMaterial,             IconArtFile( "editor/materials.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::ShadingWireframe,            IconArtFile( "editor/wire.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::ShowAxes,                    IconArtFile( "editor/axes.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::ShowBounds,                  IconArtFile( "editor/bounds.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::ShowGrid,                    IconArtFile( "editor/grid.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::ShowStatistics,              IconArtFile( "editor/stats.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::SideOrthoCamera,             IconArtFile( "editor/view_side.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::TaskWindow,                  IconArtFile( "editor/task.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::TopOrthoCamera,              IconArtFile( "editor/view_top.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::TranslateTool,               IconArtFile( "editor/translate.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::Vault,                       IconArtFile( "editor/vault.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::Volume,                      IconArtFile( "editor/volume.png" ) ) );

    //
    // Devices
    //


    //
    // FileSystem
    //
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::FileSystem::File,       IconArtFile( "filesystem/file.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::FileSystem::FindFolder, IconArtFile( "filesystem/folder.png" ).AddOverlay( "actions/find.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::FileSystem::Folder,     IconArtFile( "filesystem/folder.png" ) ) );


    //
    // MimeTypes
    //
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::MimeTypes::Binary,        IconArtFile( "mimetypes/binary.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::MimeTypes::Entity,        IconArtFile( "editor/entity.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::MimeTypes::Project,       IconArtFile( "editor/project.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::MimeTypes::ReflectBinary, IconArtFile( "editor/editor.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::MimeTypes::Scene,         IconArtFile( "editor/scene.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::MimeTypes::Text,          IconArtFile( "mimetypes/txt.png" ) ) );


    //
    // RevisionControl
    //
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::Active,                  IconArtFile( "rcs/active.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::Changelist,              IconArtFile( "rcs/changelist.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::CheckedOutByMe,          IconArtFile( "rcs/checked_out.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::CheckedOutBySomeoneElse, IconArtFile( "rcs/checked_out_by_someone_else.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::CheckIn,                 IconArtFile( "rcs/check_in.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::CheckOut,                IconArtFile( "rcs/check_out.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::CreateChangelist,        IconArtFile( "rcs/changelist.png" ).AddOverlay( "actions/create.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::File,                    IconArtFile( "filesystem/file.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::History,                 IconArtFile( "rcs/history.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::Merge,                   IconArtFile( "rcs/merge.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::OutOfDate,               IconArtFile( "rcs/outofdate.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::Revert,                  IconArtFile( "rcs/revert.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::Submit,                  IconArtFile( "rcs/submit.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::Unknown,                 IconArtFile( "status/unknown.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::User,                    IconArtFile( "rcs/user.png" ) ) );


    //
    // Status
    //
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Status::Busy,        IconArtFile( "status/busy.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Status::Error,       IconArtFile( "status/error.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Status::Information, IconArtFile( "status/info.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Status::Lockable,    IconArtFile( "status/lockable.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Status::Locked,      IconArtFile( "status/locked.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Status::Unknown,     IconArtFile( "status/unknown.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Status::Warning,     IconArtFile( "status/warning.png" ) ) );
}

wxBitmap ArtProvider::CreateBitmap( const wxArtID& artId, const wxArtClient& artClient, const wxSize& size )
{
    wxLogNull nullLog; // http://trac.wxwidgets.org/ticket/15331

    if ( !m_ArtProviderCache )
    {
        Create();
    }

    if ( artId == ArtIDs::Null )
    {
        return wxNullBitmap;
    }

    wxString hashId = ArtProviderCache::ConstructHashID( artId, artClient, size );

    wxBitmap bitmap = wxNullBitmap;
    if ( !m_ArtProviderCache->GetBitmap( hashId, &bitmap ) )
    {
        M_ArtIDToFilename::iterator findFilename = m_ArtIDToFilename.find( artId );
        if ( findFilename == m_ArtIDToFilename.end() || findFilename->second.m_Filename.empty() )
        {
            findFilename = m_ArtIDToFilename.find( ArtIDs::Status::Unknown );
        }

        if ( findFilename != m_ArtIDToFilename.end() && !findFilename->second.m_Filename.empty() )
        {
            const std::string& icon = findFilename->second.m_Filename;

            int width = size.GetWidth() > 0 ? size.GetWidth() : DefaultIconSize.GetWidth();
            int height = size.GetHeight() > 0 ? size.GetHeight() : DefaultIconSize.GetHeight();

            Helium::FilePath exePath( std::string( wxStandardPaths::Get().GetExecutablePath().c_str() ) );

#if HELIUM_OS_MAC
            const char* iconsDir = "../Resources/Icons/";
#else
            const char* iconsDir = "Icons/";
#endif
            std::stringstream strm;
            strm << exePath.Directory().Get() << iconsDir << width << 'x' << height << '/' << icon;

            std::string imageFile( strm.str() );

			Status status;
			bool exists = status.Read( imageFile.c_str() );
			if ( !exists || status.m_Size <= 0 )
            {
                std::stringstream strm2;
                strm2 << exePath.Directory().Get() << iconsDir << width << 'x' << height << '/' << "status/unknown.png";
                imageFile = strm2.str();
            }

            wxImage image( imageFile.c_str(), wxBITMAP_TYPE_PNG );
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
                    std::stringstream overlayStrm;
                    overlayStrm << exePath.Directory().Get() << iconsDir << overlayWidth << 'x' << overlayHeight << '/' << itr->second;
                    std::string overlayImageFile( overlayStrm.str() );

					exists = status.Read( overlayImageFile.c_str() );
					if ( !exists || status.m_Size <= 0 )
                    {
                        std::stringstream strm2;
                        strm2 << exePath.Directory().Get() << iconsDir << width << 'x' << height << '/' << itr->second;
                        overlayImageFile = strm2.str();

						exists = status.Read( overlayImageFile.c_str() );
						if ( !exists || status.m_Size <= 0 )
                        {
                            HELIUM_BREAK();
                        }
                    }

                    wxImage overlayImage( overlayImageFile.c_str(), wxBITMAP_TYPE_PNG );
                    HELIUM_ASSERT( overlayImage.Ok() );
                    if ( overlayImage.GetWidth() != overlayWidth || overlayImage.GetHeight() != overlayHeight )
                    {
                        overlayImage.Rescale( overlayWidth, overlayHeight );
                    }

                    int x = 0;
                    int y = 0;
                    IconArtFile::CalculatePlacement( image, overlayImage, itr->first, x, y );
                    image.Paste( overlayImage, x, y );
                }
            }

            bitmap = wxBitmap( image );

            m_ArtProviderCache->PutBitmap( hashId, bitmap );
        }
    }

    return bitmap;
}