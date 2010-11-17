//----------------------------------------------------------------------------------------------------------------------
// CommandLineInitializationWin.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_FRAMEWORK_WIN_COMMAND_LINE_INITIALIZATION_WIN_H
#define LUNAR_FRAMEWORK_WIN_COMMAND_LINE_INITIALIZATION_WIN_H

#include "FrameworkWin/FrameworkWin.h"
#include "Framework/CommandLineInitialization.h"

namespace Lunar
{
    /// Command-line initialization for Windows.
    class LUNAR_FRAMEWORK_WIN_API CommandLineInitializationWin : public CommandLineInitialization
    {
    public:
        /// @name Command-line Initialization
        //@{
        virtual bool Initialize( String& rModuleName, DynArray< String >& rArguments );
        //@}
    };
}

#endif  // LUNAR_FRAMEWORK_WIN_COMMAND_LINE_INITIALIZATION_WIN_H
