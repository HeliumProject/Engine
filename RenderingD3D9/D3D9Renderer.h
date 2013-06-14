#pragma once

#include "RenderingD3D9/RenderingD3D9.h"
#include "Rendering/Renderer.h"

#include "Platform/Assert.h"
#include "Foundation/DynamicArray.h"

#include <d3d9.h>

/// @defgroup d3d9error Direct3D 9 Error Handling
//@{

/// Non-zero to enable Direct3D debug error handling.
#ifdef NDEBUG
#define L_ENABLE_D3D9_DEBUG_VERIFICATION 0
#else
#define L_ENABLE_D3D9_DEBUG_VERIFICATION 1
#endif

#if L_ENABLE_D3D9_DEBUG_VERIFICATION

/// Assert that a Direct3D 9 result code is a success code.
///
/// @param[in] X  Result code to verify.
#define L_D3D9_ASSERT( X ) \
    { \
        HRESULT d3dResult = X; \
        if( FAILED( d3dResult ) ) \
        { \
            char messageBuffer[ 512 ]; \
            Helium::StringPrint( \
                messageBuffer, \
                TXT( "Direct3D result failed (0x%x)" ), \
                d3dResult ); \
            messageBuffer[ HELIUM_ARRAY_COUNT( messageBuffer ) - 1 ] = TXT( '\0' ); \
            HELIUM_TRIGGER_ASSERT_HANDLER( TXT( #X ), messageBuffer ) \
        } \
    }

/// Verify that a Direct3D 9 call returns a successful result code.
///
/// @param[in] X  Call to verify.
#define L_D3D9_VERIFY( X ) L_D3D9_ASSERT( X )

#else  // L_ENABLE_D3D9_DEBUG_VERIFICATION

/// Assert that a Direct3D 9 result code is a success code.
///
/// @param[in] X  Result code to verify.
#define L_D3D9_ASSERT( X )

/// Verify that a Direct3D 9 call returns a successful result code.
///
/// @param[in] X  Expression to verify.
#define L_D3D9_VERIFY( X ) X

#endif  // L_ENABLE_D3D9_DEBUG_VERIFICATION

//@}

namespace Helium
{
    class D3D9DeviceResetListener;

    HELIUM_DECLARE_RPTR( D3D9ImmediateCommandProxy );
    HELIUM_DECLARE_RPTR( D3D9MainContext );

    /// Direct3D 9 renderer implementation.
    class D3D9Renderer : public Renderer
    {
    public:
        /// Maximum resolution exponent (2^x) for pooled map target textures for static texture loading.
        static const uint32_t STATIC_TEXTURE_MAP_TARGET_LOG2_MAX = 12;  // 4096x4096

        /// @name Initialization
        //@{
        bool Initialize();
        void Shutdown();
        //@}

        /// @name Display Initialization
        //@{
        bool CreateMainContext( const ContextInitParameters& rInitParameters );
        bool ResetMainContext( const ContextInitParameters& rInitParameters );
        RRenderContext* GetMainContext();

        RRenderContext* CreateSubContext( const ContextInitParameters& rInitParameters );

        EStatus GetStatus();
        EStatus Reset();
        //@}

        /// @name State Object Creation
        //@{
        RRasterizerState* CreateRasterizerState( const RRasterizerState::Description& rDescription );
        RBlendState* CreateBlendState( const RBlendState::Description& rDescription );
        RDepthStencilState* CreateDepthStencilState( const RDepthStencilState::Description& rDescription );
        RSamplerState* CreateSamplerState( const RSamplerState::Description& rDescription );
        //@}

        /// @name Resource Allocation
        //@{
        RSurface* CreateDepthStencilSurface(
            uint32_t width, uint32_t height, ERendererSurfaceFormat format, uint32_t multisampleCount );

        RVertexShader* CreateVertexShader( size_t size, const void* pData );
        RPixelShader* CreatePixelShader( size_t size, const void* pData );

        RVertexBuffer* CreateVertexBuffer( size_t size, ERendererBufferUsage usage, const void* pData );
        RIndexBuffer* CreateIndexBuffer(
            size_t size, ERendererBufferUsage usage, ERendererIndexFormat format, const void* pData );
        RConstantBuffer* CreateConstantBuffer( size_t size, ERendererBufferUsage usage, const void* pData );

        RVertexDescription* CreateVertexDescription( const RVertexDescription::Element* pElements, size_t elementCount );
        RVertexInputLayout* CreateVertexInputLayout( RVertexDescription* pDescription, RVertexShader* pShader );

        RTexture2d* CreateTexture2d(
            uint32_t width, uint32_t height, uint32_t mipCount, ERendererPixelFormat format, ERendererBufferUsage usage,
            const RTexture2d::CreateData* pData );
        //@}

        /// @name Deferred Query Allocation
        //@{
        RFence* CreateFence();
        void SyncFence( RFence* pFence );
        bool TrySyncFence( RFence* pFence );
        //@}

        /// @name Command Interfaces
        //@{
        RRenderCommandProxy* GetImmediateCommandProxy();
        RRenderCommandProxy* CreateDeferredCommandProxy();

        void Flush();
        //@}

        /// @name Data Access
        //@{
        inline IDirect3D9* GetD3D() const;
        inline IDirect3DDevice9* GetD3DDevice() const;
        inline bool IsExDevice() const;

        inline D3DFORMAT GetDepthTextureFormat() const;

        void NotifyLost();
        //@}

        /// @name Device Reset Listening
        //@{
        void RegisterDeviceResetListener( D3D9DeviceResetListener* pListener );
        void UnregisterDeviceResetListener( D3D9DeviceResetListener* pListener );
        //@}

        /// @name Static Texture Mapping Support
        //@{
        IDirect3DTexture9* GetPooledStaticTextureMapTarget(
            uint32_t width, uint32_t height, ERendererPixelFormat format, uint32_t& rStartMipLevel );
        void ReleasePooledStaticTextureMapTarget( IDirect3DTexture9* pTexture, bool bSrgb );
        //@}

        /// @name Utility Functions
        //@{
        bool GetPresentParameters(
            const ContextInitParameters& rContextInitParameters, D3DPRESENT_PARAMETERS& rParameters,
            D3DDISPLAYMODEEX& rFullscreenDisplayMode ) const;
        D3DFORMAT PixelFormatToD3DFormat( ERendererPixelFormat format ) const;
        //@}

        /// @name Static Initialization
        //@{
        HELIUM_RENDERING_D3D9_API static bool CreateStaticInstance();
        //@}

        /// @name Static Data Access
        //@{
        inline static const GUID& GetPrivateDataGuid();
        //@}

        /// @name Static Utility Functions
        //@{
        static ERendererPixelFormat D3DFormatToPixelFormat( D3DFORMAT d3dFormat, bool bSrgb );
        //@}

    private:
        /// Direct3D interface.
        IDirect3D9* m_pD3D;
        /// Direct3D device.
        IDirect3DDevice9* m_pD3DDevice;
        /// True if the Direct3D interface and device are a Direct3D 9Ex interface and device.
        bool m_bExDevice;

        /// True if the device has been signaled that it has been lost (NotifyLost() has been called).
        bool m_bLost;

        /// Immediate render command proxy.
        D3D9ImmediateCommandProxyPtr m_spImmediateCommandProxy;
        /// Main rendering context.
        D3D9MainContextPtr m_spMainContext;

        /// Cached presentation parameters for the main context.
        D3DPRESENT_PARAMETERS m_presentParameters;
        /// Cached fullscreen display mode information for the main context (9Ex only).
        D3DDISPLAYMODEEX m_fullscreenDisplayMode;

        /// Static texture map target pools for each supported texture pixel format at varying resolutions.
        DynamicArray< IDirect3DTexture9* >
            m_staticTextureMapTargetPools[ 2 * STATIC_TEXTURE_MAP_TARGET_LOG2_MAX + 1 ][ RENDERER_PIXEL_FORMAT_MAX ];

        /// Depth texture format (for shadow mapping support).
        D3DFORMAT m_depthTextureFormat;

        /// Device reset listener list head.
        D3D9DeviceResetListener* m_pDeviceResetListenerHead;

        /// GUID associated with engine-specific private data stored in Direct3D resources.
        static const GUID sm_privateDataGuid;

        /// @name Construction/Destruction
        //@{
        D3D9Renderer();
        virtual ~D3D9Renderer();
        //@}

        /// @name Private Utility Functions
        //@{
        HRESULT ResetDevice( D3DPRESENT_PARAMETERS& rPresentParameters, D3DDISPLAYMODEEX& rFullscreenDisplayMode );
        //@}

        /// @name Private Static Utility Functions
        //@{
        static void GetStaticTextureMapTargetPool(
            size_t widthLog2, size_t heightLog2, size_t& rPoolIndex, uint32_t& rPoolTextureWidth,
            uint32_t& rPoolTextureHeight, uint32_t& rStartMipLevel );
        //@}
    };
}

#include "RenderingD3D9/D3D9Renderer.inl"
