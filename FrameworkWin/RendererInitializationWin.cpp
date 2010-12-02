//----------------------------------------------------------------------------------------------------------------------
// RendererInitializationWin.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "FrameworkWinPch.h"
#include "FrameworkWin/RendererInitializationWin.h"

#include "D3D9Rendering/D3D9Renderer.h"

using namespace Lunar;

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
