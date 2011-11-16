#include "EditorPch.h"
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
IconArtFile& IconArtFile::AddOverlay( const tchar_t* filename, OverlayQuadrants::OverlayQuadrant quadrant )
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
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_ADD_BOOKMARK, IconArtFile( TXT( "status/unknown.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_CDROM, IconArtFile( TXT( "status/unknown.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_COPY, IconArtFile( TXT( "actions/copy.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_CROSS_MARK, IconArtFile( TXT( "status/unknown.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_CUT, IconArtFile( TXT( "actions/cut.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_DELETE, IconArtFile( TXT( "actions/delete.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_DEL_BOOKMARK, IconArtFile( TXT( "status/unknown.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_ERROR, IconArtFile( TXT( "actions/error.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_EXECUTABLE_FILE, IconArtFile( TXT( "mimetypes/exe.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_FILE_OPEN, IconArtFile( TXT( "actions/open.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_FILE_SAVE, IconArtFile( TXT( "actions/save.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_FILE_SAVE_AS, IconArtFile( TXT( "actions/save_as.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_FIND, IconArtFile( TXT( "actions/find.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_FIND_AND_REPLACE, IconArtFile( TXT( "actions/find.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_FLOPPY, IconArtFile( TXT( "filesystem/floppy.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_FOLDER, IconArtFile( TXT( "filesystem/folder.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_FOLDER_OPEN, IconArtFile( TXT( "filesystem/folder_open.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_GO_BACK, IconArtFile( TXT( "actions/go_back.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_GO_DIR_UP, IconArtFile( TXT( "filesystem/folder_up.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_GO_DOWN, IconArtFile( TXT( "filesystem/go_down.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_GO_FORWARD, IconArtFile( TXT( "actions/go_forward.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_GO_HOME, IconArtFile( TXT( "actions/go_home.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_GO_TO_PARENT, IconArtFile( TXT( "actions/go_parent.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_GO_UP, IconArtFile( TXT( "actions/go_up.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_HARDDISK, IconArtFile( TXT( "filesystem/harddrive.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_HELP, IconArtFile( TXT( "status/unknown.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_HELP_BOOK, IconArtFile( TXT( "actions/book.png" ) ).AddOverlay( TXT( "status/unknown.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_HELP_FOLDER, IconArtFile( TXT( "filesystem/folder.png" ) ).AddOverlay( TXT( "status/unknown.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_HELP_PAGE, IconArtFile( TXT( "actions/page.png" ) ).AddOverlay( TXT( "status/unknown.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_HELP_SETTINGS, IconArtFile( TXT( "actions/prefernces.png" ) ).AddOverlay( TXT( "status/unknown.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_HELP_SIDE_PANEL, IconArtFile( TXT( "status/unknown.png" ) ).AddOverlay( TXT( "status/unknown.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_INFORMATION, IconArtFile( TXT( "status/info.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_LIST_VIEW, IconArtFile( TXT( "status/unknown.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_MISSING_IMAGE, IconArtFile( TXT( "status/missing_image.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_NEW, IconArtFile( TXT( "actions/create.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_NEW_DIR, IconArtFile( TXT( "filesystem/folder.png" ) ).AddOverlay( TXT( "actions/create.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_NORMAL_FILE, IconArtFile( TXT( "filesystem/file.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_PASTE, IconArtFile( TXT( "actions/paste.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_PRINT, IconArtFile( TXT( "status/unknown.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_QUESTION, IconArtFile( TXT( "status/unknown.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_QUIT, IconArtFile( TXT( "status/error.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_REDO, IconArtFile( TXT( "actions/redo.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_REMOVABLE, IconArtFile( TXT( "status/unknown.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_REPORT_VIEW, IconArtFile( TXT( "status/unknown.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_TICK_MARK, IconArtFile( TXT( "status/unknown.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_TIP, IconArtFile( TXT( "status/info.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_UNDO, IconArtFile( TXT( "actions/undo.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( wxART_WARNING, IconArtFile( TXT( "actions/warning.png" ) ) ) );


    //
    // Actions
    //
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Actions::Add, IconArtFile( TXT( "actions/add.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Actions::Copy, IconArtFile( TXT( "actions/copy.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Actions::Create, IconArtFile( TXT( "actions/create.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Actions::Cut, IconArtFile( TXT( "actions/cut.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Actions::Delete, IconArtFile( TXT( "actions/delete.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Actions::Down, IconArtFile( TXT( "actions/arrow_down.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Actions::Edit, IconArtFile( TXT( "actions/edit.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Actions::FileAdd, IconArtFile( TXT( "filesystem/file.png" ) ).AddOverlay( TXT( "actions/add.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Actions::FileDelete, IconArtFile( TXT( "filesystem/file.png" ) ).AddOverlay( TXT( "actions/delete.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Actions::Find, IconArtFile( TXT( "actions/find.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Actions::Next, IconArtFile( TXT( "editor/view_next.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Actions::Open, IconArtFile( TXT( "actions/open.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Actions::Options, IconArtFile( TXT( "actions/options.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Actions::Paste, IconArtFile( TXT( "actions/paste.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Actions::Pause, IconArtFile( TXT( "actions/pause.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Actions::Play, IconArtFile( TXT( "actions/play.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Actions::Previous, IconArtFile( TXT( "editor/view_previous.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Actions::Redo, IconArtFile( TXT( "actions/redo.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Actions::Refresh, IconArtFile( TXT( "actions/refresh.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Actions::Save, IconArtFile( TXT( "actions/save.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Actions::SaveAll, IconArtFile( TXT( "actions/save_all.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Actions::SaveAs, IconArtFile( TXT( "actions/save_as.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Actions::Select, IconArtFile( TXT( "actions/select.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Actions::SelectAll, IconArtFile( TXT( "actions/select_all.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Actions::Settings, IconArtFile( TXT( "actions/preferences.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Actions::Stop, IconArtFile( TXT( "actions/stop.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Actions::Undo, IconArtFile( TXT( "actions/undo.png" ) ) ) );


    //
    // Editor
    //
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::AddSelectionToLayers, IconArtFile( TXT( "status/unknown.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::BackfaceCulling, IconArtFile( TXT( "editor/backculling.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::Camera, IconArtFile( TXT( "editor/camera.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::ColorModeLayer, IconArtFile( TXT( "editor/color_mode_layers.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::ColorModeNodeType, IconArtFile( TXT( "editor/color_mode_type.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::ColorModeScale, IconArtFile( TXT( "status/unknown.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::ColorModeScaleGradient, IconArtFile( TXT( "status/unknown.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::ColorModeScene, IconArtFile( TXT( "editor/color_mode_scene.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::CreateNewLayer, IconArtFile( TXT( "editor/layers.png" ) ).AddOverlay( TXT( "actions/create.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::CreateNewLayerFromSelection, IconArtFile( TXT( "editor/layer_new_from_selection.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::Curve, IconArtFile( TXT( "status/unknown.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::CurveEdit, IconArtFile( TXT( "status/unknown.png" ) ).AddOverlay( TXT( "actions/edit.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::DeleteSelectedLayers, IconArtFile( TXT( "editor/layers.png" ) ).AddOverlay( TXT( "actions/delete.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::DuplicateTool, IconArtFile( TXT( "editor/duplicate.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::EditorIcon, IconArtFile( TXT( "editor/editor.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::Entity, IconArtFile( TXT( "editor/entity.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::EntityPlacementTool, IconArtFile( TXT( "editor/entity.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::FrameOrigin, IconArtFile( TXT( "editor/frame_origin.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::FrameSelected, IconArtFile( TXT( "editor/frame_selected.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::FrontOrthoCamera, IconArtFile( TXT( "editor/view_front.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::FrustumCulling, IconArtFile( TXT( "editor/frustum_culling.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::Helium, IconArtFile( TXT( "editor/editor.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::HighlightMode, IconArtFile( TXT( "editor/highlight.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::Locator, IconArtFile( TXT( "editor/locator.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::NewProject, IconArtFile( TXT( "editor/project.png" ) ).AddOverlay( TXT( "actions/create.png" ), OverlayQuadrants::TopRight ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::NextView, IconArtFile( TXT( "editor/view_next.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::PerspectiveCamera, IconArtFile( TXT( "editor/view_perspective.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::PreviousView, IconArtFile( TXT( "editor/view_previous.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::ProjectFile, IconArtFile( TXT( "editor/project.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::ProjectFolder, IconArtFile( TXT( "editor/project_folder.png" ) ) ) );
    //m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::RemoveSelectionFromLayers, IconArtFile( TXT( "status/unknown.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::RotateTool, IconArtFile( TXT( "editor/rotate.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::ScaleTool, IconArtFile( TXT( "editor/scale.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::Scene, IconArtFile( TXT( "editor/scene.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::SelectLayerMembers, IconArtFile( TXT( "editor/layer_member.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::SelectLayers, IconArtFile( TXT( "editor/layers.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::SelectTool, IconArtFile( TXT( "actions/select.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::ShadingMaterial, IconArtFile( TXT( "editor/materials.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::ShadingWireframe, IconArtFile( TXT( "editor/wire.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::ShowAxes, IconArtFile( TXT( "editor/axes.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::ShowBounds, IconArtFile( TXT( "editor/bounds.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::ShowGrid, IconArtFile( TXT( "editor/grid.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::ShowStatistics, IconArtFile( TXT( "editor/stats.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::SideOrthoCamera, IconArtFile( TXT( "editor/view_side.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::TaskWindow, IconArtFile( TXT( "editor/task.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::TopOrthoCamera, IconArtFile( TXT( "editor/view_top.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::TranslateTool, IconArtFile( TXT( "editor/translate.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::Vault, IconArtFile( TXT( "editor/vault.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Editor::Volume, IconArtFile( TXT( "editor/volume.png" ) ) ) );

    //
    // Devices
    //


    //
    // FileSystem
    //
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::FileSystem::File, IconArtFile( TXT( "filesystem/file.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::FileSystem::FindFolder, IconArtFile( TXT( "filesystem/folder.png" ) ).AddOverlay( TXT( "actions/find.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::FileSystem::Folder, IconArtFile( TXT( "filesystem/folder.png" ) ) ) );


    //
    // MimeTypes
    //
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::MimeTypes::Binary, IconArtFile( TXT( "mimetypes/binary.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::MimeTypes::Entity, IconArtFile( TXT( "editor/entity.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::MimeTypes::Project, IconArtFile( TXT( "editor/project.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::MimeTypes::ReflectBinary, IconArtFile( TXT( "editor/editor.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::MimeTypes::Scene, IconArtFile( TXT( "editor/scene.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::MimeTypes::Text, IconArtFile( TXT( "mimetypes/txt.png" ) ) ) );


    //
    // RevisionControl
    //
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::Active, IconArtFile( TXT( "rcs/active.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::Changelist, IconArtFile( TXT( "rcs/changelist.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::CheckedOutByMe, IconArtFile( TXT( "rcs/checked_out.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::CheckedOutBySomeoneElse, IconArtFile( TXT( "rcs/checked_out_by_someone_else.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::CheckIn, IconArtFile( TXT( "rcs/check_in.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::CheckOut, IconArtFile( TXT( "rcs/check_out.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::CreateChangelist, IconArtFile( TXT( "rcs/changelist.png" ) ).AddOverlay( TXT( "actions/create.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::File, IconArtFile( TXT( "filesystem/file.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::History, IconArtFile( TXT( "rcs/history.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::Merge, IconArtFile( TXT( "rcs/merge.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::OutOfDate, IconArtFile( TXT( "rcs/outofdate.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::Revert, IconArtFile( TXT( "rcs/revert.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::Submit, IconArtFile( TXT( "rcs/submit.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::Unknown, IconArtFile( TXT( "status/unknown.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::RevisionControl::User, IconArtFile( TXT( "rcs/user.png" ) ) ) );


    //
    // Status
    //
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Status::Busy, IconArtFile( TXT( "status/busy.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Status::Error, IconArtFile( TXT( "status/error.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Status::Information, IconArtFile( TXT( "status/info.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Status::Lockable, IconArtFile( TXT( "status/lockable.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Status::Locked, IconArtFile( TXT( "status/locked.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Status::Unknown, IconArtFile( TXT( "status/unknown.png" ) ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( ArtIDs::Status::Warning, IconArtFile( TXT( "status/warning.png" ) ) ) );
}

wxBitmap ArtProvider::CreateBitmap( const wxArtID& artId, const wxArtClient& artClient, const wxSize& size )
{
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

                //tstringstream strm2;
                //strm2 << exePath.Directory() << TXT( "Icons/" ) << width << TXT( 'x' ) << height << TXT( '/' ) << TXT( "status/unknown.png" );
                //imageFile.Set( strm2.str() );
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
                        tstringstream strm2;
                        strm2 << exePath.Directory() << TXT( "Icons/" ) << width << TXT( 'x' ) << height << TXT( '/' ) << itr->second;
                        overlayImageFile.Set( strm2.str() );

                        if ( !overlayImageFile.Exists() || overlayImageFile.Size() <= 0 )
                        {
                            HELIUM_BREAK();
                        }
                    }      

                    wxImage overlayImage( overlayImageFile.Get().c_str(), wxBITMAP_TYPE_PNG );
                    HELIUM_ASSERT( overlayImage.Ok() );
                    if ( overlayImage.GetWidth() != overlayWidth || overlayImage.GetHeight() != overlayHeight )
                    {
                        overlayImage.Rescale( overlayWidth, overlayHeight );
                    }

                    int x = 0;
                    int y = 0;
                    IconArtFile::CalculatePlacement( image, overlayImage, itr->first, x, y );
                    image.Paste( overlayImage, x, y, wxIMAGE_ALPHA_BLEND_COMPOSITE );
                }
            }

            bitmap = wxBitmap( image );

            m_ArtProviderCache->PutBitmap( hashId, bitmap );
        }
    }

    return bitmap;
}