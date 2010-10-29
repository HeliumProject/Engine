#pragma once

#include "Foundation/Automation/Event.h"
#include "Core/API.h"

#include <d3d9.h>
#include <d3dx9.h>

namespace Helium
{
    namespace Render
    {
        enum
        {
            INIT_FLAG_REFRAST = 0x00000001,
        };

        enum
        {
            VERTEX_SHADER_SCREENSPACE = 0,
            VERTEX_SHADER_WORLD_SPACE,
            VERTEX_SHADER_OBJECT_SPACE,
            VERTEX_SHADER_MESH_NORMAL,
            VERTEX_SHADER_MESH_DEBUG_COLOR,
            VERTEX_SHADER_MESH_DEBUG_CONSTCOLOR,
            VERTEX_SHADER_MESH_DEBUG_NORMAL,
            VERTEX_SHADER_MESH_DEBUG_TANGENT,
            VERTEX_SHADER_MESH_DEBUG_UV,

            __VERTEX_SHADER_LAST__
        };

        enum
        {
            PIXEL_SHADER_DIFFUSE = 0,
            PIXEL_SHADER_DIFFUSE_GPI,
            PIXEL_SHADER_COLOR,
            PIXEL_SHADER_TEXTURE,
            PIXEL_SHADER_TEXTURE_GREEN,
            PIXEL_SHADER_TEXTURE_ALPHA,
            PIXEL_SHADER_SKY,

            __PIXEL_SHADER_LAST__
        };

        enum
        {
            VERTEX_DECL_DEBUG = 0,
            VERTEX_DECL_MESH,
            VERTEX_DECL_SCREENSPACE,

            __VERTEX_DECL_LAST__
        };

        enum
        {
            DEFPOOL_RELEASE,
            DEFPOOL_CREATE,
        };

#define __MAX_CLIENTS__ 32

        namespace DeviceStates
        {
            enum DeviceState
            {
                Found,
                Lost
            };
        }
        typedef DeviceStates::DeviceState DeviceState;

        struct DeviceStateArgs
        {
            DeviceState m_DeviceState;

            DeviceStateArgs( DeviceState state )
                : m_DeviceState( state )
            {
            }
        };
        typedef Helium::Signature< const DeviceStateArgs& > DeviceStateSignature;

        // all rendering classes should be derived this
        class CORE_API DeviceManager
        {
        public:
            DeviceManager();
            virtual ~DeviceManager();

            static void SetUnique(); // call before init
            HRESULT Init(HWND hwnd,uint32_t back_buffer_width, uint32_t back_buffer_height, uint32_t init_flags=0);

        private:
            HRESULT ResizeSwapChain(uint32_t width, uint32_t height);
            HRESULT ResizeDevice(uint32_t width, uint32_t height);
        public:
            HRESULT Resize(uint32_t width, uint32_t height);
            HRESULT Display(HWND target,RECT* src=0, RECT* dst=0);      // if display fails, call reset
            HRESULT Reset();

            bool TestDeviceReady();

            inline bool IsDeviceLost() const
            {
                return m_IsLost;
            }

            inline void SetDeviceLost( bool lost = true )
            {
                m_IsLost = lost;
            }

            IDirect3DSurface9* GetBufferData();

            bool SaveTGA(const tchar* fname);

            inline IDirect3D9* GetD3D()
            {
                return m_d3d;
            }

            inline IDirect3DDevice9* GetD3DDevice() const
            {
                return m_device;
            }

            inline IDirect3DSurface9* GetBackBuffer()
            {
                return m_back_buffer;
            }

            inline IDirect3DSurface9* GetDepthBuffer()
            {
                return m_depth_buffer;
            }

            inline IDirect3DVertexShader9* GetStockVS(uint32_t idx)
            {
                return m_vertex_shaders[idx];
            }

            inline IDirect3DPixelShader9* GetStockPS(uint32_t idx)
            {
                return m_pixel_shaders[idx];
            }

            inline IDirect3DVertexDeclaration9* GetStockDecl(uint32_t idx)
            {
                return m_vertex_dec[idx];
            }

            inline uint32_t GetWidth()
            {
                return m_width;
            }

            inline uint32_t GetHeight()
            {
                return m_height;
            }

            // call up to the parent class to handle their default pool (this will be called for every client)
            // this is called for a number of reasons
            HRESULT HandleClientDefaultPool(uint32_t reason)
            {
                if ( reason == DEFPOOL_RELEASE )
                {
                    m_Lost.Raise( DeviceStates::Lost );
                }
                else if ( reason == DEFPOOL_CREATE )
                {
                    m_Found.Raise( DeviceStates::Found );
                }
                return S_OK;
            }

        private:
            bool m_IsLost;

        private:
            DeviceStateSignature::Event m_Found;
        public:
            void AddDeviceFoundListener( const DeviceStateSignature::Delegate& listener )
            {
                m_Found.Add( listener );
            }
            void RemoveDeviceFoundListener( const DeviceStateSignature::Delegate& listener )
            {
                m_Found.Remove( listener );
            }
        private:
            DeviceStateSignature::Event m_Lost;
        public:
            void AddDeviceLostListener( const DeviceStateSignature::Delegate& listener )
            {
                m_Lost.Add( listener );
            }
            void RemoveDeviceLostListener( const DeviceStateSignature::Delegate& listener )
            {
                m_Lost.Remove( listener );
            }

        private:
            void CreateBaseResources();
            void FreeBaseResources();

            IDirect3D9*                           m_d3d;    
            IDirect3DDevice9*                     m_device; 
            IDirect3DSurface9*                    m_back_buffer;
            IDirect3DSurface9*                    m_depth_buffer;
            IDirect3DSwapChain9*                  m_swapchain;
            bool                                  m_using_swapchain;
            D3DPRESENT_PARAMETERS                 m_d3dpp;
            uint32_t                                   m_width;
            uint32_t                                   m_height;

            static bool                           m_unique;
            static uint32_t                            m_master_count;
            static IDirect3D9*                    m_master_d3d;                   // Used to create the D3DDevice
            static IDirect3DDevice9*              m_master_device;                // rendering device 
            static D3DFORMAT                      m_back_buffer_format;
            static D3DPRESENT_PARAMETERS          m_master_pp;
            static IDirect3DVertexShader9*        m_vertex_shaders[__VERTEX_SHADER_LAST__];
            static IDirect3DPixelShader9*         m_pixel_shaders[__PIXEL_SHADER_LAST__];
            static IDirect3DVertexDeclaration9*   m_vertex_dec[__VERTEX_DECL_LAST__];
            static DeviceManager*                 m_clients[__MAX_CLIENTS__];
        };

        // a predefined structure for use with vertex decl VERTEX_DECL_DEBUG
        struct VertexDebug
        {
            D3DXVECTOR3 m_pos;
            uint32_t         m_color;
        }; //size 16

        // a basic screen space vertex with a few texture params
        struct VertexScreen
        {
            D3DXVECTOR4 m_pos;
            D3DXVECTOR4 m_tex0;
            D3DXVECTOR4 m_tex1;
            D3DXVECTOR4 m_tex2;
        };

        // the main mesh vertex format is full expanded floats for easy access
        struct MeshVertex
        {
            D3DXVECTOR3 m_pos;           // offset 0
            D3DXVECTOR3 m_normal;        // offset 12
            D3DXVECTOR4 m_tangent;       // offset 24
            D3DXVECTOR2 m_uv;            // offset 40  
            D3DXVECTOR4 m_color  ;       // offset 48
        }; // size 64
    }
}