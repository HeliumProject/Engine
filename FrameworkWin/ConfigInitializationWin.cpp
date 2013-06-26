#include "FrameworkWinPch.h"
#include "FrameworkWin/ConfigInitializationWin.h"

#include "PcSupport/ConfigPc.h"

using namespace Helium;

/// @copydoc ConfigInitialization::Initialize()
bool ConfigInitializationWin::Initialize()
{
    if( !ConfigInitialization::Initialize() )
    {
        return false;
    }

#if HELIUM_TOOLS
    HELIUM_TRACE( TraceLevels::Info, TXT( "Saving user configuration.\n" ) );
    ConfigPc::SaveUserConfig();
    HELIUM_TRACE( TraceLevels::Info, TXT( "User configuration saved.\n" ) );
#endif

    return true;
}
