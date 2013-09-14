#include "FrameworkImplPch.h"
#include "FrameworkImpl/ConfigInitializationImpl.h"

#include "PcSupport/ConfigPc.h"

using namespace Helium;

/// @copydoc ConfigInitialization::Initialize()
bool ConfigInitializationImpl::Initialize()
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
