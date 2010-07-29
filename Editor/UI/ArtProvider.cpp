#include "Precompile.h"

#include "ArtProvider.h"

using namespace Editor;

ArtProvider::ArtProvider()
: Helium::ArtProvider()
{
}

void ArtProvider::Create()
{
    __super::Create();

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Editor::ArtIDs::PerspectiveCamera, TXT( "devices/camera.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Editor::ArtIDs::FrontOrthoCamera, TXT( "devices/camera.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Editor::ArtIDs::SideOrthoCamera, TXT( "devices/camera.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Editor::ArtIDs::TopOrthoCamera, TXT( "devices/camera.png" ) ) );

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Editor::ArtIDs::SelectTool, TXT( "nocturnal/unknown.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Editor::ArtIDs::TranslateTool, TXT( "apps/amsn4.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Editor::ArtIDs::RotateTool, TXT( "nocturnal/unknown.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Editor::ArtIDs::ScaleTool, TXT( "apps/amsn8.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Editor::ArtIDs::DuplicateTool, TXT( "nocturnal/unknown.png" ) ) );

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Editor::ArtIDs::Locator, TXT( "apps/galeon.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Editor::ArtIDs::Volume, TXT( "apps/3d.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Editor::ArtIDs::Entity, TXT( "apps/katuberling.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Editor::ArtIDs::Curve, TXT( "nocturnal/unknown.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Editor::ArtIDs::CurveEdit, TXT( "nocturnal/unknown.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Editor::ArtIDs::NavMesh, TXT( "apps/gnome_apps.png" ) ) );

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Editor::ArtIDs::NewCollection, TXT( "actions/folder_new.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Editor::ArtIDs::DeleteCollection, TXT( "actions/remove.png" ) ) );

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Editor::ArtIDs::TaskWindow, TXT( "actions/openterm.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Editor::ArtIDs::EditorIcon, TXT( "apps/browser.png" ) ) );

}