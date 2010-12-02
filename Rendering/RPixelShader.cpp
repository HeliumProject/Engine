//----------------------------------------------------------------------------------------------------------------------
// RPixelShader.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "RenderingPch.h"
#include "Rendering/RPixelShader.h"

using namespace Lunar;

/// Destructor.
RPixelShader::~RPixelShader()
{
}

/// @copydoc RShader::GetType()
RShader::EType RPixelShader::GetType() const
{
    return TYPE_PIXEL;
}
