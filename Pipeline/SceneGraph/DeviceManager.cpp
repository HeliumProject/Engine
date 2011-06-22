#include "PipelinePch.h"
#include "DeviceManager.h"

#include "Foundation/Log.h"
#include "Rendering/RRenderContext.h"
#include "RenderingD3D9/D3D9Renderer.h"
#include "Pipeline/Image/Formats/TGA.h"

using namespace Helium;

bool                        DeviceManager::m_unique = false;
uint32_t                    DeviceManager::m_master_count = 0;
DeviceManager*              DeviceManager::m_clients[__MAX_CLIENTS__] = {0};
Helium::RRenderContextPtr    DeviceManager::sm_spMainRenderContext;
HWND                        DeviceManager::sm_hMainRenderContextWnd;
uint32_t                    DeviceManager::sm_mainRenderContextWidth;
uint32_t                    DeviceManager::sm_mainRenderContextHeight;

///////////////////////////////////////////////////////////////////////////////////////////////////
DeviceManager::DeviceManager()
: m_hWnd( NULL )
{
    m_width = 0;
    m_height = 0;
    m_using_swapchain = false;

    //record the this pointer in the client array so we can call back to free/recreate default pool resources
    // first look for empty entries in the client array
    size_t clientIndex;
    for ( clientIndex = 0; clientIndex < HELIUM_ARRAY_COUNT( m_clients ); ++clientIndex )
    {
        if ( !m_clients[ clientIndex ] )
        {
            m_clients[ clientIndex ] = this;
            ++m_master_count;

            break;
        }
    }

    HELIUM_ASSERT( clientIndex < HELIUM_ARRAY_COUNT( m_clients ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
DeviceManager::~DeviceManager()
{
    --m_master_count;

    // this is the last client, we need to free up the global resources before we free the device
    if ( m_master_count == 0 )
    {
        sm_spMainRenderContext.Release();
    }

    m_spRenderContext.Release();

    // go through all the clients and remove ourself
    for ( size_t clientIndex = 0; clientIndex < HELIUM_ARRAY_COUNT( m_clients ); ++clientIndex )
    {
        if ( m_clients[ clientIndex ] == this )
        {
            m_clients[ clientIndex ] = NULL;
            break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DeviceManager::SetUnique()
{
    if ( !sm_spMainRenderContext )
    {
        m_unique = true;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DeviceManager::Init( HWND hwnd, uint32_t back_buffer_width, uint32_t back_buffer_height, uint32_t /*init_flags*/ )
{
    Helium::Renderer* pRenderer = NULL;
    if ( !sm_spMainRenderContext )
    {
        bool bCreatedRenderer = Helium::D3D9Renderer::CreateStaticInstance();
        HELIUM_ASSERT( bCreatedRenderer );
        if ( !bCreatedRenderer )
        {
            return false;
        }

        if ( m_unique )
        {
            sm_hMainRenderContextWnd = hwnd;
            sm_mainRenderContextWidth = back_buffer_width;
            sm_mainRenderContextHeight = back_buffer_height;
        }
        else
        {
            // if we are not running in unique mode then everything if a flip chain, the default back buffer is 64x64
            sm_mainRenderContextWidth = 64;
            sm_mainRenderContextHeight = 64;
        }

        pRenderer = Helium::Renderer::GetStaticInstance();
        HELIUM_ASSERT( pRenderer );
        pRenderer->Initialize();

        Helium::Renderer::ContextInitParameters initParameters;
        initParameters.pWindow = hwnd;
        initParameters.bFullscreen = false;
        initParameters.bVsync = false;
        initParameters.displayWidth = sm_mainRenderContextWidth;
        initParameters.displayHeight = sm_mainRenderContextHeight;

        bool bCreateResult = pRenderer->CreateMainContext( initParameters );
        HELIUM_ASSERT( bCreateResult );
        if ( !bCreateResult )
        {
            return false;
        }

        sm_spMainRenderContext = pRenderer->GetMainContext();
        HELIUM_ASSERT( sm_spMainRenderContext );

        if ( m_unique )
        {
            m_spRenderContext = sm_spMainRenderContext;
        }
    }
    else
    {
        // if we get to here it must be a second instance and when running unique that is not allowed.
        HELIUM_ASSERT( !m_unique );
        if ( m_unique )
        {
            return false;
        }

        pRenderer = Helium::Renderer::GetStaticInstance();
        HELIUM_ASSERT( pRenderer );
    }

    if ( !m_unique )
    {
        // Create an additional render context.
        Helium::Renderer::ContextInitParameters initParameters;
        initParameters.pWindow = hwnd;
        initParameters.bFullscreen = false;
        initParameters.bVsync = false;
        initParameters.displayWidth = back_buffer_width;
        initParameters.displayHeight = back_buffer_height;

        m_spRenderContext = pRenderer->CreateSubContext( initParameters );
        HELIUM_ASSERT( m_spRenderContext );
        if ( !m_spRenderContext )
        {
            return false;
        }

        m_using_swapchain = true;
    }

    m_hWnd = hwnd;

    m_width = back_buffer_width;
    m_height = back_buffer_height;

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DeviceManager::ResizeSwapChain( uint32_t width, uint32_t height )
{
    m_spRenderContext.Release();

    Helium::Renderer::ContextInitParameters initParameters;
    initParameters.pWindow = m_hWnd;
    initParameters.bFullscreen = false;
    initParameters.bVsync = false;
    initParameters.displayWidth = width;
    initParameters.displayHeight = height;

    Helium::Renderer* pRenderer = Helium::Renderer::GetStaticInstance();
    HELIUM_ASSERT( pRenderer );
    m_spRenderContext = pRenderer->CreateSubContext( initParameters );

    return ( m_spRenderContext != NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DeviceManager::ResizeDevice( uint32_t width, uint32_t height )
{
    m_spRenderContext.Release();
    sm_spMainRenderContext.Release();

    Helium::Renderer::ContextInitParameters initParameters;
    initParameters.pWindow = m_hWnd;
    initParameters.bFullscreen = false;
    initParameters.bVsync = false;
    initParameters.displayWidth = width;
    initParameters.displayHeight = height;

    Helium::Renderer* pRenderer = Helium::Renderer::GetStaticInstance();
    HELIUM_ASSERT( pRenderer );
    pRenderer->ResetMainContext( initParameters );

    m_spRenderContext = pRenderer->GetMainContext();
    sm_spMainRenderContext = m_spRenderContext;

    return ( m_spRenderContext != NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DeviceManager::Resize( uint32_t width, uint32_t height )
{
    if ( m_using_swapchain )
    {
        return ResizeSwapChain( width, height );
    }

    // this will currently only be used in 'unique' mode
    HELIUM_ASSERT( m_unique );
    return ResizeDevice( width, height );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DeviceManager::Swap()
{
    m_spRenderContext->Swap();

    return true;
}

bool DeviceManager::TestDeviceReady()
{
    Helium::Renderer* pRenderer = Helium::Renderer::GetStaticInstance();
    HELIUM_ASSERT( pRenderer );

    Helium::Renderer::EStatus rendererStatus = pRenderer->GetStatus();
    if ( rendererStatus == Helium::Renderer::STATUS_READY )
    {
        return true;
    }

    if ( rendererStatus == Helium::Renderer::STATUS_NOT_RESET )
    {
        rendererStatus = pRenderer->Reset();
        if ( rendererStatus == Helium::Renderer::STATUS_READY )
        {
            return true;
        }
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DeviceManager::SaveTGA(const tchar_t* fname)
{
#ifdef VIEWPORT_REFACTOR
    IDirect3DSurface9* surface = GetBufferData();
    if (surface==0)
        return 0;

    D3DSURFACE_DESC desc;
    surface->GetDesc(&desc);

    // check if the surface is 32bit and only write if it is, print an error if it isn't, we really should handle other formats.
    if ((desc.Format!=D3DFMT_A8R8G8B8) && (desc.Format!=D3DFMT_X8R8G8B8))
    {
        Log::Error( TXT( "failed to write TGA, surface format not compatible [surface is format d3dformat %x]\n" ),desc.Format);
        return false;
    }

    D3DLOCKED_RECT lr;

    surface->LockRect(&lr,0,0);

    uint8_t* pixels = (uint8_t*)lr.pBits;
    uint32_t stride = lr.Pitch;

    FILE * texture_file = _tfopen(fname, TXT( "wb" ) );
    if (!texture_file)
    {
        surface->UnlockRect();
        surface->Release();
        return false;
    }

    Helium::TGAHeader tga;
    memset(&tga,0,sizeof(tga));
    tga.imageType = 0x02;
    tga.imageDescriptor = 0x28;
    tga.pixelDepth = 24;
    tga.colorMapIndex = 0;
    tga.colorMapLength = 0;
    tga.colorMapBits = 0;
    tga.xOrigin = 0;
    tga.yOrigin = 0;
    tga.width = (uint16_t)desc.Width;
    tga.height = (uint16_t)desc.Height;

    fwrite(&tga,sizeof(tga),1,texture_file);

    for (uint32_t y=0;y<desc.Height;y++)
    {
        for (uint32_t x=0;x<desc.Width;x++)
        {
            // write RGB24 bits
            fwrite(pixels + (4*x),3,1,texture_file);
        }

        // move to the next scanline
        pixels+=stride;
    }
    fclose(texture_file);

    surface->UnlockRect();
    surface->Release();
#endif

    return true;
}
