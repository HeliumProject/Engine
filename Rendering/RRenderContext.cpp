//----------------------------------------------------------------------------------------------------------------------
// RRenderContext.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "RenderingPch.h"
#include "Rendering/RRenderContext.h"

using namespace Lunar;

/// Destructor.
RRenderContext::~RRenderContext()
{
}

/// @fn RSurface* RRenderContext::GetBackBufferSurface()
/// Get an interface to the current back buffer surface.
///
/// @return  Back buffer surface interface.

/// @fn void RRenderContext::Swap()
/// Swap out the front buffer with the next buffer in queue, presenting the next frame of render data to the screen.
///
/// @see RRenderCommandProxy::BeginScene(), RRenderCommandProxy::EndScene()
