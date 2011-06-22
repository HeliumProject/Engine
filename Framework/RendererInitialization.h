//----------------------------------------------------------------------------------------------------------------------
// RendererInitialization.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_FRAMEWORK_RENDERER_INITIALIZATION_H
#define HELIUM_FRAMEWORK_RENDERER_INITIALIZATION_H

#include "Framework/Framework.h"

namespace Helium
{
    class Renderer;

    /// Interface for Renderer creation during System initialization.
    class HELIUM_FRAMEWORK_API RendererInitialization
    {
    public:
        /// @name Construction/Destruction
        //@{
        virtual ~RendererInitialization();
        //@}

        /// @name Renderer Initialization
        //@{
        virtual bool Initialize() = 0;
        //@}
    };
}

#endif  // HELIUM_FRAMEWORK_RENDERER_INITIALIZATION_H
