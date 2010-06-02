#include "Precompiled.h"
#include "Application.h"
#include "MainFrame.h"
#include "resource.h"

#include "AppUtils/AppUtils.h"
#include "Common/File/Path.h"
#include "Reflect/Registry.h"
#include "Inspect/Controls.h"
#include "Inspect/InspectInit.h"
#include "InspectReflect/InspectReflectInit.h"
#include "Finder/Finder.h"

IMPLEMENT_APP_NO_MAIN(Application);

int main(int argc, const char** argv)
{
    return AppUtils::StandardWinMain( &wxEntry );
}

int Application::OnRun()
{
    Reflect::Initialize();

    char executablePathBuffer[ MAX_PATH ];
    HANDLE hProcess = GetCurrentProcess();
    if ( GetModuleFileName( NULL, executablePathBuffer, MAX_PATH ) == 0 )
    {
        throw Nocturnal::Exception( "Could not determine executable filename to locate Reflect modules." );
    }

    Nocturnal::Path executablePath( executablePathBuffer );
    Reflect::InitializeModules( executablePath.Directory() );

    InspectReflect::Initialize();

    HANDLE handle;
    wxIcon icon;
    wxIconBundle bundle;

    handle = ::LoadImage( wxGetInstance(), MAKEINTRESOURCE( IDI_ICON1 ), IMAGE_ICON, 16, 16, 0 );
    icon.SetHICON( handle );
    icon.SetWidth( 16 );
    icon.SetHeight( 16 );
    bundle.AddIcon( icon );

    handle = ::LoadImage( wxGetInstance(), MAKEINTRESOURCE( IDI_ICON1 ), IMAGE_ICON, 32, 32, 0 );
    icon.SetHICON( handle );
    icon.SetWidth( 32 );
    icon.SetHeight( 32 );
    bundle.AddIcon( icon );

    handle = ::LoadImage( wxGetInstance(), MAKEINTRESOURCE( IDI_ICON1 ), IMAGE_ICON, 64, 64, 0 );
    icon.SetHICON( handle );
    icon.SetWidth( 64 );
    icon.SetHeight( 64 );
    bundle.AddIcon( icon );

    handle = ::LoadImage( wxGetInstance(), MAKEINTRESOURCE( IDI_ICON1 ), IMAGE_ICON, 128, 128, 0 );
    icon.SetHICON( handle );
    icon.SetWidth( 128 );
    icon.SetHeight( 128 );
    bundle.AddIcon( icon );

    MainFrame* frame = new MainFrame ();
    frame->SetIcons(bundle);
    frame->Show();

    return __super::OnRun();
}

int Application::OnExit()
{
    InspectReflect::Cleanup();
    Reflect::CleanupModules();
    Reflect::Cleanup();

    return __super::OnExit();
}
