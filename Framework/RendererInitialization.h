//----------------------------------------------------------------------------------------------------------------------
// RendererInitialization.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_FRAMEWORK_RENDERER_INITIALIZATION_H
#define LUNAR_FRAMEWORK_RENDERER_INITIALIZATION_H

#include "Framework/Framework.h"

namespace Lunar
{
    class Renderer;

    /// Interface for Renderer creation during System initialization.
    class LUNAR_FRAMEWORK_API RendererInitialization
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

#endif  // LUNAR_FRAMEWORK_RENDERER_INITIALIZATION_H
