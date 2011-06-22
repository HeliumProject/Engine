//----------------------------------------------------------------------------------------------------------------------
// RendererInitializationWin.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "FrameworkWinPch.h"
#include "FrameworkWin/RendererInitializationWin.h"

#include "RenderingD3D9/D3D9Renderer.h"

using namespace Helium;

/// @copydoc RendererInitialization::Initialize()
bool RendererInitializationWin::Initialize()
{
    if( !D3D9Renderer::CreateStaticInstance() )
    {
        return false;
    }

    Renderer* pRenderer = D3D9Renderer::GetStaticInstance();
    HELIUM_ASSERT( pRenderer );
    if( !pRenderer->Initialize() )
    {
        Renderer::DestroyStaticInstance();

        return false;
    }

    return true;
}
