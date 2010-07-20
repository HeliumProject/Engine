#include "Precompile.h"

#include "ArtProvider.h"

using namespace Luna;

ArtProvider::ArtProvider()
: Nocturnal::ArtProvider()
{
}

void ArtProvider::Create()
{
    __super::Create();

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Luna::ArtIDs::PerspectiveCamera, TXT( "devices/camera.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Luna::ArtIDs::FrontOrthoCamera, TXT( "devices/camera.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Luna::ArtIDs::SideOrthoCamera, TXT( "devices/camera.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Luna::ArtIDs::TopOrthoCamera, TXT( "devices/camera.png" ) ) );

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Luna::ArtIDs::SelectTool, TXT( "nocturnal/unknown.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Luna::ArtIDs::TranslateTool, TXT( "apps/amsn4.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Luna::ArtIDs::RotateTool, TXT( "nocturnal/unknown.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Luna::ArtIDs::ScaleTool, TXT( "apps/amsn8.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Luna::ArtIDs::DuplicateTool, TXT( "nocturnal/unknown.png" ) ) );

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Luna::ArtIDs::Locator, TXT( "apps/galeon.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Luna::ArtIDs::Volume, TXT( "apps/3d.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Luna::ArtIDs::Entity, TXT( "apps/katuberling.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Luna::ArtIDs::Curve, TXT( "nocturnal/unknown.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Luna::ArtIDs::CurveEdit, TXT( "nocturnal/unknown.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Luna::ArtIDs::NavMesh, TXT( "apps/gnome_apps.png" ) ) );

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Luna::ArtIDs::NewCollection, TXT( "actions/folder_new.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Luna::ArtIDs::DeleteCollection, TXT( "actions/remove.png" ) ) );

    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Luna::ArtIDs::TaskWindow, TXT( "actions/openterm.png" ) ) );
    m_ArtIDToFilename.insert( M_ArtIDToFilename::value_type( Luna::ArtIDs::LunaIcon, TXT( "apps/browser.png" ) ) );

}