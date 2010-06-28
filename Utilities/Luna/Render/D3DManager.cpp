#include "Precompile.h"
#include "Renderer.h"
#include "TGAHeader.h"

#include "Foundation/Log.h"

bool                           Render::D3DManager::m_unique = false;
u32                            Render::D3DManager::m_master_count = 0;
IDirect3D9*                    Render::D3DManager::m_master_d3d = 0;
IDirect3DDevice9*              Render::D3DManager::m_master_device = 0;
D3DFORMAT                      Render::D3DManager::m_back_buffer_format = D3DFMT_UNKNOWN;
D3DPRESENT_PARAMETERS          Render::D3DManager::m_master_pp = {0};
IDirect3DVertexShader9*        Render::D3DManager::m_vertex_shaders[__VERTEX_SHADER_LAST__] = {0};
IDirect3DPixelShader9*         Render::D3DManager::m_pixel_shaders[__PIXEL_SHADER_LAST__] = {0};
IDirect3DVertexDeclaration9*   Render::D3DManager::m_vertex_dec[__VERTEX_DECL_LAST__] = {0};
Render::D3DManager*        Render::D3DManager::m_clients[__MAX_CLIENTS__] = {0};

///////////////////////////////////////////////////////////////////////////////////////////////////
Render::D3DManager::D3DManager()
{
    m_d3d=0;
    m_device=0; 
    m_swapchain=0;
    m_back_buffer=0;
    m_depth_buffer=0;
    m_width=0;
    m_height=0;
    m_using_swapchain=false;
    m_back_buffer_format = D3DFMT_UNKNOWN;

    //record the this pointer in the client array so we can call back to free/recreate default pool resources
    // first look for empty entries in the client array
    bool done = false;
    for (u32 c=0;c<m_master_count;c++)
    {
        if (m_clients[c]==0)
        {
            m_clients[c]=this;
            done=true;
            break;
        }
    }

    if (!done)
    {
        m_clients[m_master_count]=this;
        m_master_count++;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
Render::D3DManager::~D3DManager()
{
    m_master_count--;  

    // this is the last client, we need to free up the global resources before we free the device
    if (m_master_count==0)
    {
        // free the shaders and declarations
        FreeBaseResources();

        m_master_d3d=0;
        m_master_device=0;
    }

    if (m_back_buffer)
    {
        m_back_buffer->Release();
        m_back_buffer=0;
    }

    if (m_depth_buffer)
    {
        m_depth_buffer->Release();
        m_depth_buffer=0;
    }

    if (m_swapchain)
    {
        // free the swap chain
        m_swapchain->Release();
        m_swapchain=0;
        m_using_swapchain=false;
    }

    if (m_device)
    {
        m_device->Release();
        m_device=0;
    }

    if (m_d3d)
    {
        m_d3d->Release();
        m_d3d=0;
    }

    // go through all the clients and remove ourself
    for (u32 c=0;c<m_master_count;c++)
    {
        if (m_clients[c]==this)
        {
            m_clients[c]=0;
            break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Render::D3DManager::SetUnique()
{
    if (m_master_d3d==0)
    {
        m_unique = true;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT Render::D3DManager::InitD3D(HWND hwnd,u32 back_buffer_width, u32 back_buffer_height, u32 init_flags)
{
    HRESULT hr;

    // NOTE: Unknown always works for rendering but it will make messed up screen shots when the desktop is not in 32bpp
    //       By default we will use that mode but if we can get a conversion from ARGB 32bit to the current mode then we
    //       shall force 32bit. This will allow the screen shots to work. 
    //
    // robw [nov 2009] - We should modify the screen shot code to convert from 16bit to 32bit or dump a 16bit tga

    if (m_master_d3d==0)
    {
        if( NULL == ( m_master_d3d = Direct3DCreate9( D3D_SDK_VERSION ) ) )
        {
            return E_FAIL;
        }
        m_d3d = m_master_d3d;

        // fill out the display params for thw default back buffer (which we don't use)   
        ZeroMemory( &m_master_pp, sizeof( m_master_pp ) );

        if (m_unique==false)
        {
            // if we are not running in unique mode then everything if a flip chain, the default back buffer is 64x64
            m_master_pp.BackBufferWidth  = 64;
            m_master_pp.BackBufferHeight = 64;
        }
        else
        {
            m_master_pp.BackBufferWidth = back_buffer_width;
            m_master_pp.BackBufferHeight = back_buffer_height;
        }

        D3DDISPLAYMODE mode;
        m_master_d3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&mode);
        HRESULT fmt_res = m_master_d3d->CheckDeviceFormatConversion(D3DADAPTER_DEFAULT,(init_flags & INIT_FLAG_REFRAST)?D3DDEVTYPE_REF:D3DDEVTYPE_HAL,D3DFMT_A8R8G8B8,mode.Format);
        if (fmt_res==S_OK)
        {
            Log::Print( TXT( "A8R8G8B8 to display format conversion available, forcing A8R8G8B8\n" ) );
            m_back_buffer_format = D3DFMT_A8R8G8B8;
        }
        else
        {
            Log::Print( TXT( "A8R8G8B8 not available, using current display format [hr=%x]\n" ),fmt_res);
        }
        // add other display conversion checks here if needed.

        m_master_pp.BackBufferFormat = m_back_buffer_format;
        m_master_pp.Windowed = true;
        m_master_pp.SwapEffect = D3DSWAPEFFECT_COPY; 
        m_master_pp.EnableAutoDepthStencil = true;
        m_master_pp.AutoDepthStencilFormat = D3DFMT_D24S8;
        m_master_pp.hDeviceWindow = hwnd;

        if (init_flags & INIT_FLAG_REFRAST)
        {
            Log::Print( TXT( "Using ref rast forcing render buffer to A8R8G8B8\n" ) );

            // if refrast force the back buffer to ARGB 32bit
            m_back_buffer_format = D3DFMT_A8R8G8B8;
            m_master_pp.BackBufferFormat = m_back_buffer_format;

            hr = m_d3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_REF , 0, D3DCREATE_MULTITHREADED| D3DCREATE_SOFTWARE_VERTEXPROCESSING, &m_master_pp, &m_master_device );
        }
        else
        {
            hr = m_d3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, 0, D3DCREATE_MULTITHREADED | D3DCREATE_HARDWARE_VERTEXPROCESSING, &m_master_pp, &m_master_device );
        }    
        if (FAILED(hr))
        {
            m_d3d=0;
            m_master_d3d->Release();
            m_master_d3d=0;
            return hr;
        }
        m_device = m_master_device;

        // create the build in verts and pixel shaders
        CreateBaseResources();

        if (m_unique)
        {
            m_d3dpp = m_master_pp;
        }
    }
    else
    {
        // if we get to here it must be a second instance and when running unique that is not allowed.
        if (m_unique)
            return E_FAIL;

        // replicate the existing device
        m_device = m_master_device;
        m_d3d = m_master_d3d;
        m_device->AddRef();
        m_d3d->AddRef();
    }

    if (!m_unique)
    {
        // create the back buffer for the flip chain (note there is no depth buffer)
        ZeroMemory( &m_d3dpp, sizeof( m_d3dpp ) );
        m_d3dpp.BackBufferWidth  = back_buffer_width;
        m_d3dpp.BackBufferHeight = back_buffer_height;
        m_d3dpp.BackBufferFormat = m_back_buffer_format;
        m_d3dpp.Windowed = true;
        m_d3dpp.SwapEffect = D3DSWAPEFFECT_COPY; 
        m_d3dpp.EnableAutoDepthStencil = false;
        m_d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
        m_d3dpp.hDeviceWindow = hwnd;

        // create a flip chain
        m_device->CreateAdditionalSwapChain(&m_d3dpp,&m_swapchain);

        // get the back buffer pointers
        m_swapchain->GetBackBuffer(0,D3DBACKBUFFER_TYPE_MONO,&m_back_buffer);
        m_using_swapchain = true;

        // create a depth buffer as the flip chain does not contain a depth buffer
        m_device->CreateDepthStencilSurface(back_buffer_width,back_buffer_height,D3DFMT_D24S8, D3DMULTISAMPLE_NONE,0,true,&m_depth_buffer,0);
    }
    else
    {
        // if we are running unique get the back buffer pointers from the device
        m_device->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&m_back_buffer);
        m_device->GetDepthStencilSurface(&m_depth_buffer);
    }

    //record the width and height
    D3DSURFACE_DESC desc;
    m_back_buffer->GetDesc(&desc);
    m_width = desc.Width;
    m_height = desc.Height;

    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT Render::D3DManager::ResizeSwapChain(u32 width, u32 height)
{
    // we always have a back buffer pointer, release it
    if (m_back_buffer)
    {
        m_back_buffer->Release();
        m_back_buffer=0;
    }

    // we always have a back buffer pointer, release it
    if (m_depth_buffer)
    {
        m_depth_buffer->Release();
        m_depth_buffer=0;
    }

    // if there is a swap chain delete it too
    if (m_swapchain)
    {
        // free the swap chain
        m_swapchain->Release();
        m_swapchain=0;
    }

    m_d3dpp.BackBufferWidth = width;
    m_d3dpp.BackBufferHeight = height;

    // create a flip chain
    m_device->CreateAdditionalSwapChain(&m_d3dpp,&m_swapchain);

    // get the back buffer pointers
    m_swapchain->GetBackBuffer(0,D3DBACKBUFFER_TYPE_MONO,&m_back_buffer);
    m_using_swapchain = true;  

    D3DSURFACE_DESC desc;
    m_back_buffer->GetDesc(&desc);
    m_width = desc.Width;
    m_height = desc.Height;

    // create a depth buffer as the flip chain does not contain a depth buffer
    m_device->CreateDepthStencilSurface(width,height,D3DFMT_D24S8, D3DMULTISAMPLE_NONE,0,true,&m_depth_buffer,0);

    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT Render::D3DManager::ResizeDevice(u32 width, u32 height)
{
    // release the default pool
    HandleClientDefaultPool(DEFPOOL_RELEASE);

    if (m_back_buffer)
    {
        m_back_buffer->Release();
        m_back_buffer=0;
    }

    if (m_depth_buffer)
    {
        m_depth_buffer->Release();
        m_depth_buffer=0;
    }

    // if there is a swap chain delete it too (should never happen)
    if (m_swapchain)
    {
        // free the swap chain
        m_swapchain->Release();
        m_swapchain=0;
    }

    m_d3dpp.BackBufferWidth = width;
    m_d3dpp.BackBufferHeight = height;
    m_master_pp.BackBufferWidth = width;
    m_master_pp.BackBufferHeight = height;

    m_device->Reset(&m_master_pp);

    // get the new back buffer and new depth buffer pointers
    m_device->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&m_back_buffer);
    m_device->GetDepthStencilSurface(&m_depth_buffer);

    // copy the width and height
    D3DSURFACE_DESC desc;
    m_back_buffer->GetDesc(&desc);
    m_width = desc.Width;
    m_height = desc.Height;

    // recreate the default pool
    HandleClientDefaultPool(DEFPOOL_CREATE);

    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT Render::D3DManager::Resize(u32 width, u32 height)
{
    if (m_swapchain)
    {
        return ResizeSwapChain(width,height);
    }
    else
    {
        // this will currently only be used in 'unique' mode
        return ResizeDevice(width,height);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT Render::D3DManager::Display(HWND target,RECT* src, RECT* dst)
{  
    HRESULT hr;

    // if this was a normal render then present the scene otherwise copy it to the offscreen
    if (m_swapchain)
    {
        // this is an additional swapchain surface
        hr = m_swapchain->Present(src,dst,target,0,0);
    }
    else
    {
        // base surface
        hr = m_device->Present(src,dst,target,0);
    }

    return hr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT Render::D3DManager::Reset()
{
    HRESULT hr = S_OK;

    // step 1, go through all the clients and release their default pool resources
    for (u32 c=0;c<m_master_count;c++)
    {
        if (m_clients[c])
        {
            m_clients[c]->HandleClientDefaultPool(DEFPOOL_RELEASE);
        }
    }

    // step 2, go through all the clients and destroy their depth buffers and swap chains
    for (u32 c=0;c<m_master_count;c++)
    {
        if (m_clients[c])
        {
            if (m_clients[c]->m_back_buffer)
            {
                m_clients[c]->m_back_buffer->Release();
                m_clients[c]->m_back_buffer=0;
            }

            if (m_clients[c]->m_depth_buffer)
            {
                m_clients[c]->m_depth_buffer->Release();
                m_clients[c]->m_depth_buffer=0;
            }

            if (m_clients[c]->m_swapchain)
            {
                m_clients[c]->m_swapchain->Release();
                m_clients[c]->m_swapchain=0;
            }      
        }
    }

    // step 3, reset the device with the default device display parameters
    m_device->Reset(&m_master_pp);

    // step 4, go through all the clients and recreate the depth buffer and swap chains based on the local parameters
    for (u32 c=0;c<m_master_count;c++)
    {
        if (m_clients[c])
        {
            if (m_clients[c]->m_using_swapchain)
            {
                // create a flip chain
                m_device->CreateAdditionalSwapChain(&m_clients[c]->m_d3dpp,&m_clients[c]->m_swapchain);

                // get the back buffer pointers
                m_clients[c]->m_swapchain->GetBackBuffer(0,D3DBACKBUFFER_TYPE_MONO,&m_clients[c]->m_back_buffer);

                // create a depth buffer as the flip chain does not contain a depth buffer
                m_device->CreateDepthStencilSurface(m_clients[c]->m_width,m_clients[c]->m_height,D3DFMT_D24S8, D3DMULTISAMPLE_NONE,0,true,&m_clients[c]->m_depth_buffer,0);
            }
            else
            {
                m_clients[c]->m_swapchain = 0;

                // get the back buffer pointers and depth buffer pointers
                m_device->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&m_clients[c]->m_back_buffer);
                m_device->GetDepthStencilSurface(&m_clients[c]->m_depth_buffer);
            }
        }
    }

    // step 5, go through all the clients and create their default pool resources
    for (u32 c=0;c<m_master_count;c++)
    {
        if (m_clients[c])
        {
            m_clients[c]->HandleClientDefaultPool(DEFPOOL_CREATE);
        }
    }

    return hr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
IDirect3DSurface9* Render::D3DManager::GetBufferData()
{
    D3DSURFACE_DESC desc;
    m_back_buffer->GetDesc(&desc);

    IDirect3DSurface9* surface;

    if ( FAILED(m_device->CreateOffscreenPlainSurface(desc.Width,desc.Height,desc.Format,D3DPOOL_SYSTEMMEM,&surface,0)))
    {
        return 0;
    }

    if (FAILED(m_device->GetRenderTargetData(m_back_buffer,surface)))
    {
        surface->Release();
        return 0;
    }

    return surface;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
bool Render::D3DManager::SaveTGA(const tchar* fname)
{
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

    u8* pixels = (u8*)lr.pBits;
    u32 stride = lr.Pitch;

    FILE * texture_file = _tfopen(fname, TXT( "wb" ) );
    if (!texture_file)
    {
        surface->UnlockRect();
        surface->Release();
        return false;
    }

    TGAHeader tga;
    memset(&tga,0,sizeof(tga));
    tga.imageType = 0x02;
    tga.imageDescriptor = 0x28;
    tga.pixelDepth = 24;
    tga.colorMapIndex = 0;
    tga.colorMapLength = 0;
    tga.colorMapBits = 0;
    tga.xOrigin = 0;
    tga.yOrigin = 0;
    tga.width = (u16)desc.Width;
    tga.height = (u16)desc.Height;

    fwrite(&tga,sizeof(tga),1,texture_file);

    for (u32 y=0;y<desc.Height;y++)
    {
        for (u32 x=0;x<desc.Width;x++)
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

    return true;
}

// vertex shaders
#include "screenspace_vs.h"
#include "basicworldspace_vs.h"
#include "basicobjspace_vs.h"
#include "mesh_normal_vs.h"
#include "mesh_debug_color_vs.h"
#include "mesh_debug_constcolor_vs.h"
#include "mesh_debug_vertnormal_vs.h"
#include "mesh_debug_verttangent_vs.h"
#include "mesh_debug_uv_vs.h"

// pixel shaders
#include "diffuse_ps.h"
#include "diffuse_gpi_ps.h"
#include "color_ps.h"
#include "texture_ps.h"
#include "texture_g_ps.h"
#include "texture_a_ps.h"
#include "sky_ps.h"

static const BYTE* g_compiled_vertex_shaders[Render::__VERTEX_SHADER_LAST__] = 
{
    g_screenspace_vs,
    g_basicworldspace_vs,
    g_basicobjspace_vs,
    g_mesh_normal_vs,
    g_mesh_debug_color_vs,
    g_mesh_debug_constcolor_vs,
    g_mesh_debug_vertnormal_vs,
    g_mesh_debug_verttangent_vs,
    g_mesh_debug_uv_vs,
};

static const BYTE* g_compiled_pixel_shaders[Render::__PIXEL_SHADER_LAST__] = 
{
    g_diffuse_ps,
    g_diffuse_gpi_ps,
    g_color_ps,
    g_texture_ps,
    g_texture_g_ps,
    g_texture_a_ps,
    g_sky_ps,
};

static D3DVERTEXELEMENT9 g_VertexDec_Screenspace[] = 
{
    {0,0, D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION,0},
    {0,16,D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD,0},
    {0,32,D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD,1},
    {0,48,D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD,2},
    {0xFF,0,D3DDECLTYPE_UNUSED, 0,0,0}  
};

static D3DVERTEXELEMENT9 g_VertexDec_Debug[] = 
{
    {0,0, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION,0},
    {0,12,D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD,0},

    {0xFF,0,D3DDECLTYPE_UNUSED, 0,0,0}  
};

static D3DVERTEXELEMENT9 g_VertexDec_Mesh[] =  // total size 64
{
    {0,0, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION,0},
    {0,12,D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD,0}, // NORMAL
    {0,24,D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD,1}, // TANGENT
    {0,40,D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD,2}, // UV
    {0,48,D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD,3}, // COLOR

    {0xFF,0,D3DDECLTYPE_UNUSED, 0,0,0}  
};

///////////////////////////////////////////////////////////////////////////////////////////////////
void Render::D3DManager::CreateBaseResources()
{
    for (u32 vs=0;vs<Render::__VERTEX_SHADER_LAST__;vs++)
    {
        if (FAILED(m_device->CreateVertexShader((const DWORD*)g_compiled_vertex_shaders[vs],&m_vertex_shaders[vs])))
        {
            Log::Error( TXT( "Failed to create vertex shader %d\n" ),vs);
            m_vertex_shaders[vs]=0;
        }
    }

    for (u32 ps=0;ps<__PIXEL_SHADER_LAST__;ps++)
    {
        if (FAILED(m_device->CreatePixelShader((const DWORD*)g_compiled_pixel_shaders[ps],&m_pixel_shaders[ps])))
        {
            Log::Error( TXT( "Failed to create pixel shader %d\n" ),ps);
            m_pixel_shaders[ps]=0;
        }
    }

    m_device->CreateVertexDeclaration(g_VertexDec_Debug,&m_vertex_dec[VERTEX_DECL_DEBUG]);
    m_device->CreateVertexDeclaration(g_VertexDec_Mesh,&m_vertex_dec[VERTEX_DECL_MESH]);
    m_device->CreateVertexDeclaration(g_VertexDec_Screenspace,&m_vertex_dec[VERTEX_DECL_SCREENSPACE]);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Render::D3DManager::FreeBaseResources()
{
    // delete all the shaders and vertex decls
    for (u32 s=0;s<__VERTEX_SHADER_LAST__;s++)
    {
        if (m_vertex_shaders[s])
            m_vertex_shaders[s]->Release();
    }
    for (u32 s=0;s<__PIXEL_SHADER_LAST__;s++)
    {
        if (m_pixel_shaders[s])
            m_pixel_shaders[s]->Release();
    }
    for (u32 s=0;s<__VERTEX_DECL_LAST__;s++)
    {
        if (m_vertex_dec[s])
            m_vertex_dec[s]->Release();
    }
}
