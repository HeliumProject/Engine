#include "RenderingD3D9Pch.h"
#include "RenderingD3D9/D3D9VertexShader.h"

using namespace Helium;

/// Constructor.
///
/// @param[in] pShaderData  Either a Direct3D 9 vertex shader to wrap, or a pointer to a preallocated staging area
///                         for shader loading.  The semantic is determined by the @c bStaging parameter.  If this
///                         is a Direct3D 9 vertex shader, its reference count will be incremented when this object
///                         is constructed and decremented back when this object is destroyed.
/// @param[in] bStaging     True if @c pShaderData points to a staging buffer for loading, false if it points to a
///                         Direct3D 9 vertex shader instance.
D3D9VertexShader::D3D9VertexShader( void* pShaderData, bool bStaging )
: m_pShaderData( pShaderData )
, m_bStaging( bStaging )
{
    HELIUM_ASSERT( pShaderData || bStaging );

    if( !bStaging )
    {
        static_cast< IDirect3DVertexShader9* >( pShaderData )->AddRef();
    }
}

/// Destructor.
D3D9VertexShader::~D3D9VertexShader()
{
    if( m_bStaging )
    {
        DefaultAllocator().Free( m_pShaderData );
    }
    else
    {
        if( m_pShaderData )
        {
            static_cast< IDirect3DVertexShader9* >( m_pShaderData )->Release();
        }
    }
}

/// @copydoc RShader::Lock()
void* D3D9VertexShader::Lock()
{
    if( !m_bStaging )
    {
        HELIUM_TRACE( TraceLevels::Error, "D3D9VertexShader::Lock(): Vertex shader has already been loaded.\n" );

        return NULL;
    }

    return m_pShaderData;
}

/// @copydoc RShader::Unlock()
bool D3D9VertexShader::Unlock()
{
    if( !m_bStaging )
    {
        HELIUM_TRACE( TraceLevels::Error, "D3D9VertexShader::Unlock(): Vertex shader has already been loaded.\n" );

        return false;
    }

    // Verify that the shader data is DWORD-aligned since we need to cast it when passing it to the shader creation
    // function.
    HELIUM_ASSERT( ( reinterpret_cast< uintptr_t >( m_pShaderData ) & ( sizeof( DWORD ) - 1 ) ) == 0 );

    D3D9Renderer* pRenderer = static_cast< D3D9Renderer* >( Renderer::GetInstance() );
    HELIUM_ASSERT( pRenderer );

    IDirect3DDevice9* pD3DDevice = pRenderer->GetD3DDevice();
    HELIUM_ASSERT( pD3DDevice );

    IDirect3DVertexShader9* pD3DShader = NULL;
    HRESULT createResult = pD3DDevice->CreateVertexShader( static_cast< DWORD* >( m_pShaderData ), &pD3DShader );
    if( FAILED( createResult ) )
    {
        HELIUM_TRACE( TraceLevels::Error, "D3D9VertexShader::Unlock(): Vertex shader creation failed.\n" );
        pD3DShader = NULL;
    }

    DefaultAllocator().Free( m_pShaderData );
    m_pShaderData = pD3DShader;
    m_bStaging = false;

    return ( pD3DShader != NULL );
}
