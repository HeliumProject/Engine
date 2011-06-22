//----------------------------------------------------------------------------------------------------------------------
// RVertexShader.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "RenderingPch.h"
#include "Rendering/RVertexShader.h"

#include "Rendering/Renderer.h"
#include "Rendering/RVertexDescription.h"
#include "Rendering/RVertexInputLayout.h"

using namespace Helium;

/// Constructor.
RVertexShader::RVertexShader()
{
}

/// Destructor.
RVertexShader::~RVertexShader()
{
}

/// @copydoc RShader::GetType()
RShader::EType RVertexShader::GetType() const
{
    return TYPE_VERTEX;
}

/// Cache the input layout for the specified description.
///
/// @param[in] pRenderer     Renderer instance.
/// @param[in] pDescription  Vertex description.
///
/// @see GetCachedInputLayout()
void RVertexShader::CacheDescription( Renderer* pRenderer, RVertexDescription* pDescription )
{
    if( m_spCachedDescription != pDescription )
    {
        m_spCachedDescription = pDescription;
        m_spCachedInputLayout.Release();

        if( pDescription )
        {
            HELIUM_ASSERT( pRenderer );
            m_spCachedInputLayout = pRenderer->CreateVertexInputLayout( pDescription, this );
            HELIUM_ASSERT( m_spCachedInputLayout );
        }
    }
}

/// Get the most recently cached input layout for this shader.
///
/// @return  Cached input layout.
///
/// @see CacheDescription()
RVertexInputLayout* RVertexShader::GetCachedInputLayout() const
{
    return m_spCachedInputLayout;
}
