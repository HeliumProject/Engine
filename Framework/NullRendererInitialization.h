//----------------------------------------------------------------------------------------------------------------------
// NullRendererInitialization.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_FRAMEWORK_NULL_RENDERER_INITIALIZATION_H
#define HELIUM_FRAMEWORK_NULL_RENDERER_INITIALIZATION_H

#include "Framework/RendererInitialization.h"

namespace Helium
{
    /// Renderer initializer that creates a null renderer.
    class HELIUM_FRAMEWORK_API NullRendererInitialization : public RendererInitialization
    {
    public:
        /// @name Renderer Initialization
        //@{
        bool Initialize();
        //@}
    };
}

#endif  // HELIUM_FRAMEWORK_NULL_RENDERER_INITIALIZATION_H
