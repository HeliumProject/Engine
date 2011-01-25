//----------------------------------------------------------------------------------------------------------------------
// Renderer.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "RenderingPch.h"
#include "Rendering/Renderer.h"

using namespace Lunar;

Renderer* Renderer::sm_pInstance = NULL;

/// Constructor.
Renderer::Renderer()
    : m_featureFlags( 0 )
{
}

/// Destructor.
Renderer::~Renderer()
{
}

/// @fn bool Renderer::Initialize()
/// Perform primary initialization of this renderer for use in the application.
///
/// @return  True if initialization is successful, false if not.
///
/// @see Shutdown()

/// @fn void Renderer::Shutdown()
/// Shut down this renderer.  The renderer cannot be reused unless Initialize() is called again.
///
/// @see Initialize()

/// @fn bool Renderer::CreateMainContext( const ContextInitParameters& rInitParameters )
/// Create a primary display context for this device.
///
/// @param[in] rInitParameters  Context initialization parameters.
///
/// @return  True if the context was created successfully, false if not.
///
/// @see ResetMainContext(), GetMainContext(), CreateSubContext(), GetStatus()

/// @fn bool Renderer::ResetMainContext( const ContextInitParameters& rInitParameters )
/// Reset the settings of the primary display context for this device.
///
/// Note that on certain platforms (namely Direct3D 9 on Windows), various rendering resources will need to be
/// reacquired after resetting the main context.
///
/// The main context may also need to be reset if the device has been lost on certain platforms (again, namely Direct3D
/// 9 on Windows).
///
/// @param[in] rInitParameters  Context initialization parameters.
///
/// @return  True if the context was reset successfully, false if not.
///
/// @see CreateMainContext(), GetMainContext(), GetStatus()

/// @fn RRenderContext* Renderer::GetMainContext()
/// Get a reference to the main rendering context.
///
/// @return  Main rendering context.
///
/// @see CreateMainContext(), ResetMainContext()

/// @fn RRenderContext* Renderer::CreateSubContext( const ContextInitParameters& rInitParameters )
/// Create a child rendering context for rendering to multiple windows or displays.
///
/// @param[in] rInitParameters  Context initialization parameters.
///
/// @return  True if the context was created successfully, false if not.
///
/// @see CreateMainContext(), ResetMainContext()

/// @fn Renderer::EStatus Renderer::GetStatus()
/// Get the current status of the rendering device.
///
/// This should be called at the start of each frame to determine whether rendering can be performed.
///
/// On certain platforms (namely Direct3D 9), the rendering device may be placed in a "lost" state under certain
/// conditions, such as when a focus is lost from the main window while running a fullscreen application.  When such a
/// situation arises, this function will return STATUS_LOST while the device is lost and not ready to be reacquired.
/// Once the device is ready again, this will return STATUS_NOT_READY, in which case the main context will need to be
/// reset using ResetMainContext() and various resources will need to be recreated before they can be used again.
///
/// @return  Identifier specifying the current device status.
///
/// @see CreateMainContext(), ResetMainContext()

/// @fn RRasterizerState* Renderer::CreateRasterizerState( const RRasterizerState::Description& rDescription )
/// Create a rasterizer state object based on the given description.
///
/// @param[in] rDescription  Description of the rasterizer state settings.
///
/// @return  Rasterizer state object, or null if state object creation failed.
///
/// @see CreateBlendState(), CreateDepthStencilState(), CreateSamplerState()

/// @fn RBlendState* Renderer::CreateBlendState( const RBlendState::Description& rDescription )
/// Create a blend state object based on the given description.
///
/// @param[in] rDescription  Description of the blend state settings.
///
/// @return  Blend state object, or null if state object creation failed.
///
/// @see CreateRasterizerState(), CreateDepthStencilState(), CreateSamplerState()

/// @fn RDepthStencilState* Renderer::CreateDepthStencilState( const RDepthStencilState::Description& rDescription )
/// Create a depth-stencil state object based on the given description.
///
/// @param[in] rDescription  Description of the depth-stencil state settings.
///
/// @return  Depth-stencil state object, or null if state object creation failed.
///
/// @see CreateRasterizerState(), CreateBlendState(), CreateSamplerState()

/// @fn RSamplerState* Renderer::CreateSamplerState( const RSamplerState::Description& rDescription )
/// Create a sampler state object based on the given description.
///
/// @param[in] rDescription  Description of the sampler state settings.
///
/// @return  Sampler state object, or null if state object creation failed.
///
/// @see CreateRasterizerState(), CreateBlendState(), CreateDepthStencilState()

/// @fn RSurface* Renderer::CreateDepthStencilSurface( uint32_t width, uint32_t height, ERendererSurfaceFormat format, uint32_t multisampleCount )
/// Create a depth-stencil surface.
///
/// @param[in] width             Surface width, in pixels.
/// @param[in] height            Surface height, in pixels,
/// @param[in] format            Surface format.
/// @param[in] multisampleCount  Multisampling sample count (0 or 1 to disable).
///
/// @return  Pointer to the depth-stencil surface if created successfully, null pointer if creation failed.

/// @fn RVertexShader* Renderer::CreateVertexShader( size_t size, const void* pData )
/// Create a new vertex shader.
///
/// @param[in] size   Shader size, in bytes.
/// @param[in] pData  Option pointer to data to which the vertex shader should be initialized.  If this is null, the
///                   shader will simply be created with its contents left in an uninitialized state.
///
/// @return  Pointer to the vertex shader interface if created successfully, null pointer if creation failed.
///
/// @see CreatePixelShader()

/// @fn RPixelShader* Renderer::CreatePixelShader( size_t size, const void* pData )
/// Create a new pixel shader.
///
/// @param[in] size   Shader size, in bytes.
/// @param[in] pData  Option pointer to data to which the pixel shader should be initialized.  If this is null, the
///                   shader will simply be created with its contents left in an uninitialized state.
///
/// @return  Pointer to the pixel shader interface if created successfully, null pointer if creation failed.
///
/// @see CreateVertexShader()

/// @fn RVertexBuffer* Renderer::CreateVertexBuffer( size_t size, ERendererBufferUsage usage, const void* pData )
/// Create a new vertex buffer.
///
/// @param[in] size   Buffer size, in bytes.
/// @param[in] usage  Buffer usage.
/// @param[in] pData  Optional pointer to data to which the vertex buffer should be initialized.  If this is null,
///                   the buffer will simply be created with its contents left in an uninitialized state.
///
/// @return  Pointer to the vertex buffer interface if created successfully, null pointer if creation failed.
///
/// @see CreateIndexBuffer(), CreateConstantBuffer()

/// @fn RIndexBuffer* Renderer::CreateIndexBuffer( size_t size, ERendererBufferUsage usage, const void* pData )
/// Create a new index buffer.
///
/// @param[in] size    Buffer size, in bytes.
/// @param[in] usage   Buffer usage.
/// @param[in] format  Index format.
/// @param[in] pData   Optional pointer to data to which the index buffer should be initialized.  If this is null,
///                    the buffer will simply be created with its contents left in an uninitialized state.
///
/// @return  Pointer to the index buffer interface if created successfully, null pointer if creation failed.
///
/// @see CreateVertexBuffer(), CreateConstantBuffer()

/// @fn RConstantBuffer* Renderer::CreateConstantBuffer( size_t size, ERendererBufferUsage usage, const void* pData )
/// Create a new shader constant buffer.
///
/// @param[in] size   Buffer size, in bytes.
/// @param[in] usage  Buffer usage.
/// @param[in] pData  Optional pointer to data to which the constant buffer should be initialized.  If this is null,
///                   the buffer will simply be created with its contents left in an uninitialized state.
///
/// @return  Pointer to the constant buffer interface if created successfully, null pointer if creation failed.
///
/// @see CreateVertexBuffer(), CreateIndexBuffer()

/// @fn RVertexDescription* Renderer::CreateVertexDescription( const VertexInputDescription::Element* pElements, size_t elementCount )
/// Create a vertex description object for defining the layout of a vertex type.
///
/// @param[in] pElements     Array of vertex element descriptions.
/// @param[in] elementCount  Number of vertex elements.
///
/// @return  Vertex description interface.

/// @fn RVertexInputLayout* Renderer::CreateVertexInputLayout( RVertexDescription* pDescription, RVertexShader* pShader )
/// Create an input layout object for defining the mapping between a vertex type and the input for a vertex shader.
///
/// @param[in] pDescription  Description of the vertex format.
/// @param[in] pShader       Vertex shader for which to create the input layout.
///
/// @return  Vertex input layout interface.

/// @fn RTexture2d* Renderer::CreateTexture2d( uint32_t width, uint32_t height, uint32_t mipCount, ERendererPixelFormat format, ERendererBufferUsage usage, const RTexture2d::CreateData* pData )
/// Create a 2D texture.
///
/// @param[in] width     Width of the top-most mip level, in texels.
/// @param[in] height    Height of the top-most mip level, in texels.
/// @param[in] mipCount  Number of mip levels.
/// @param[in] format    Pixel format.
/// @param[in] usage     Texture usage hint.
/// @param[in] pData     Array specifying the data with which to initialize this texture.  This can be null, in
///                      which case the texture will be left in an uninitialized state.  If this is not null,
///                      initialization data must be specified for all mip levels.
///
/// @return  Pointer to the texture resource if created successfully, null if not.

/// @fn RFence* Renderer::CreateFence()
/// Create a GPU command buffer fence object.
///
/// A fence object can be used to synchronize with the GPU once a certain set of commands have been completed.  A
/// command to signal a fence can be inserted into the command buffer using RRenderCommandProxy::SetFence().  The
/// fence can later be queried using SyncFence() or TrySyncFence().  Fences can only be signaled and synced once
/// within an application; the fence object must be released once it has been used.
///
/// @return  Pointer to the command fence object.
///
/// @see RRenderCommandProxy::SetFence(), SyncFence(), TrySyncFence()

/// @fn void Renderer::SyncFence( RFence* pFence )
/// Block until a fence has been signaled by the GPU.
///
/// Once this function returns, the fence should no longer be used, and any references to it should subsequently be
/// released.
///
/// More information about fences and their use can be found in the description for CreateFence().
///
/// @param[in] pFence  Fence on which to wait.
///
/// @see TrySyncFence(), CreateFence(), RRenderCommandProxy::SetFence()

/// @fn bool Renderer::TrySyncFence( RFence* pFence )
/// Check whether a fence has been signaled by the GPU without blocking.
///
/// If this function returns true (fence has been signaled), the fence should no longer be used, and any references
/// to it should subsequently be released.
///
/// More information about fences and their use can be found in the description for CreateFence().
///
/// @param[in] pFence  Fence to test.
///
/// @return  True if the fence has been signaled, false if not.
///
/// @see SyncFence(), CreateFence(), RRenderCommandProxy::SetFence()

/// @fn RRenderCommandProxy* Renderer::GetImmediateCommandProxy()
/// Get a reference to the render command proxy interface for immediate issuing of render commands.
///
/// Note that only one thread (typically the same thread used to process the target window messages) should use an
/// immediate command proxy at a time.
///
/// @return  Immediate render command proxy interface.
///
/// @see CreateDeferredCommandProxy()

/// @fn RRenderCommandProxy* Renderer::CreateDeferredCommandProxy()
/// Create a render command proxy for recording render commands for later immediate use.
///
/// Note that only one thread should use a given deferred command proxy at a given time.  Multiple threads can use
/// different command proxies concurrently, though.
///
/// @return  Deferred render command proxy interface.
///
/// @see CreateImmediateCommandProxy()

/// @fn void Renderer::Flush()
/// Flush the renderer command buffer and block until all pending commands have been executed.
///
/// This should almost never be called during normal runtime, as doing so can incur significant performance penalties.
/// It should only be reserved for cases where the normal application flow has already been interrupted (i.e. making
/// sure resources that need to be updated during a window resize are no longer being used by the GPU).

/// Get the global renderer instance.
///
/// A renderer instance must be initialized first through the interface of one of the Renderer subclasses.
///
/// @return  Renderer instance.  If an instance has not yet been initialized, this will return null.
///
/// @see DestroyStaticInstance()
Renderer* Renderer::GetStaticInstance()
{
    return sm_pInstance;
}

/// Destroy the global renderer instance if one exists.
///
/// @see GetStaticInstance()
void Renderer::DestroyStaticInstance()
{
    if( sm_pInstance )
    {
        sm_pInstance->Shutdown();
        delete sm_pInstance;
        sm_pInstance = NULL;
    }
}
