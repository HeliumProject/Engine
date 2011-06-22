//----------------------------------------------------------------------------------------------------------------------
// RendererInitializationWin.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_FRAMEWORK_WIN_RENDERER_INITIALIZATION_WIN_H
#define HELIUM_FRAMEWORK_WIN_RENDERER_INITIALIZATION_WIN_H

#include "FrameworkWin/FrameworkWin.h"
#include "Framework/RendererInitialization.h"

namespace Helium
{
    /// Renderer factory implementation for Windows.
    class HELIUM_FRAMEWORK_WIN_API RendererInitializationWin : public RendererInitialization
    {
    public:
        /// @name Renderer Initialization
        //@{
        virtual bool Initialize();
        //@}
    };
}

#endif  // HELIUM_FRAMEWORK_WIN_RENDERER_INITIALIZATION_WIN_H
