//----------------------------------------------------------------------------------------------------------------------
// ConfigInitializationWin.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

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

    HELIUM_TRACE( TRACE_INFO, TXT( "Saving user configuration.\n" ) );
    ConfigPc::SaveUserConfig();
    HELIUM_TRACE( TRACE_INFO, TXT( "User configuration saved.\n" ) );

    return true;
}
