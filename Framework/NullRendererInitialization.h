//----------------------------------------------------------------------------------------------------------------------
// NullRendererInitialization.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_FRAMEWORK_NULL_RENDERER_INITIALIZATION_H
#define LUNAR_FRAMEWORK_NULL_RENDERER_INITIALIZATION_H

#include "Framework/RendererInitialization.h"

namespace Helium
{
    /// Renderer initializer that creates a null renderer.
    class LUNAR_FRAMEWORK_API NullRendererInitialization : public RendererInitialization
    {
    public:
        /// @name Renderer Initialization
        //@{
        bool Initialize();
        //@}
    };
}

#endif  // LUNAR_FRAMEWORK_NULL_RENDERER_INITIALIZATION_H
