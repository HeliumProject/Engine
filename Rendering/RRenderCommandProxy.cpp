#include "RenderingPch.h"
#include "Rendering/RRenderCommandProxy.h"

using namespace Helium;

/// Destructor.
RRenderCommandProxy::~RRenderCommandProxy()
{
}

/// @fn void RRenderCommandProxy::SetRasterizerState( RRasterizerState* pState )
/// Set the rasterizer state.
///
/// @param[in] pState  State settings to apply.
///
/// @see Renderer::CreateRasterizerState(), SetBlendState(), SetDepthStencilState(), SetSamplerStates()

/// @fn void RRenderCommandProxy::SetBlendState( RBlendState* pState )
/// Set the blend state.
///
/// @param[in] pState  State settings to apply.
///
/// @see Renderer::CreateBlendState(), SetRasterizerState(), SetDepthStencilState(), SetSamplerStates()

/// @fn void RRenderCommandProxy::SetDepthStencilState( RDepthStencilState* pState )
/// Set the depth-stencil state.
///
/// @param[in] pState                 State settings to apply.
/// @param[in] stencilReferenceValue  Reference value used for stencil operations (when
///                                   RENDERER_STENCIL_OPERATION_REPLACE is set as a stencil operation).
///
/// @see Renderer::CreateDepthStencilState(), SetRasterizerState(), SetBlendState(), SetSamplerStates()

/// @fn void RRenderCommandProxy::SetSamplerStates( size_t startIndex, size_t samplerCount, RSamplerState* const* ppStates )
/// Set the sampler states for a series of sampler slots.
///
/// @param[in] startIndex    Index of the first sampler to set.
/// @param[in] samplerCount  Number of sampler states in the given array.
/// @param[in] ppStates      Array of state settings to apply.
///
/// @see Renderer::CreateSamplerState(), SetRasterizerState(), SetBlendState(), SetDepthStencilState()

/// @fn void RRenderCommandProxy::SetRenderSurfaces( RSurface* pRenderTargetSurface, RSurface* pDepthStencilSurface )
/// Set the render target and depth-stencil surfaces.
///
/// @param[in] pRenderTargetSurface  Render target surface.
/// @param[in] pDepthStencilSurface  Depth-stencil surface.

/// @fn void RRenderCommandProxy::SetViewport( uint32_t x, uint32_t y, uint32_t width, uint32_t height )
/// Set the render viewport parameters.
///
/// @param[in] x       Horizontal pixel coordinate of the top-left corner of the viewport.
/// @param[in] y       Vertical pixel coordinate of the top-left corner of the viewport.
/// @param[in] width   Viewport width, in pixels.
/// @param[in] height  Viewport height, in pixels.

/// @fn void RRenderCommandProxy::BeginScene()
/// Begin rendering a scene to the current render context.
///
/// This must be called prior to issuing any draw calls for rendering to a given render context.  Once all draw
/// calls for a given frame have been issued, EndScene() should be subsequently called prior to calling
/// RRenderContext::Swap() to update the display contents.
///
/// @see EndScene(), RRenderContext::Swap()

/// @fn void RRenderCommandProxy::EndScene()
/// Finish rendering a scene to the current render context prior to swapping display buffers.
///
/// This must be called prior to calling RRenderContext::Swap() on a given render context once all draw commands have
/// been issued.
///
/// @see BeginScene(), RRenderContext::Swap()

/// @fn void RRenderCommandProxy::Clear( uint32_t clearFlags, const Color& rColor, float32_t depth, uint8_t stencil )
/// Clear one or more render surfaces.
///
/// @param[in] clearFlags  Combination of ERendererClearFlag flags specifying which surfaces to clear.
/// @param[in] rColor      Color to which the render target should be cleared.
/// @param[in] depth       Value to which the depth buffer should be cleared.
/// @param[in] stencil     Value to which the stencil buffer should be cleared.

/// @fn void RRenderCommandProxy::SetIndexBuffer( RIndexBuffer* pBuffer )
/// Set the index buffer to use for rendering.
///
/// @param[in] pBuffer  Index buffer to set.
///
/// @see SetVertexBuffers(), SetVertexInputLayout()

/// @fn void RRenderCommandProxy::SetVertexBuffers( size_t startIndex, size_t bufferCount, RVertexBuffer* const* ppBuffers, uint32_t* pStrides, uint32_t* pOffsets )
/// Set the vertex buffers to use for vertex input.
///
/// Note that a VertexInputDescription must also be set to define how the vertex buffer data should be interpreted.
///
/// @param[in] startIndex   Starting vertex buffer index to set.
/// @param[in] bufferCount  Number of consecutive vertex buffers to set.
/// @param[in] ppBuffers    Array of vertex buffers to set.
/// @param[in] pStrides     Array specifying the byte stride between each consecutive vertex in each vertex buffer.
/// @param[in] pOffsets     Array specifying the byte offsets within each vertex buffer from which to read the first
///                         vertices to use for rendering.
///
/// @see SetVertexInputLayout(), SetIndexBuffer()

/// @fn void RRenderCommandProxy::SetVertexInputLayout( RVertexInputLayout* pLayout )
/// Set the layout of the vertex input.
///
/// @param[in] pLayout  Vertex input layout to set.
///
/// @see SetVertexBuffers(), SetIndexBuffer()

/// @fn void RRenderCommandProxy::SetVertexShader( RVertexShader* pShader )
/// Set the vertex shader to use for rendering.
///
/// @param[in] pShader  Vertex shader to set.
///
/// @see SetPixelShader()

/// @fn void RRenderCommandProxy::SetPixelShader( RPixelShader* pShader )
/// Set the pixel shader to use for rendering.
///
/// @param[in] pShader  Pixel shader to set.
///
/// @see SetVertexShader()

/// @fn void RRenderCommandProxy::SetVertexConstantBuffers( size_t startIndex, size_t bufferCount, RConstantBuffer* const* ppBuffers, const size_t* pLimitSizes )
/// Set a range of vertex shader constant buffers to use for rendering.
///
/// @param[in] startIndex   Starting vertex shader constant buffer index to set.
/// @param[in] bufferCount  Number of consecutive constant buffers to set.
/// @param[in] ppBuffers    Array of constant buffers to set.
/// @param[in] pLimitSizes  Optional array of sizes (in bytes) to limit update ranges for each constant buffer.
///                         Only up to the specified number of bytes will be guaranteed to be loaded into the shader
///                         for each buffer.  Invalid index values can be used to specify that the entire buffer
///                         should be updated.  On platforms that don't support storage of constant buffers on the
///                         GPU (i.e. Direct3D 9 and such, where shader constants must be passed in the command
///                         buffer when changing), this can provide a significant performance improvement.
///
/// @see SetPixelConstantBuffers()

/// @fn void RRenderCommandProxy::SetPixelConstantBuffers( size_t startIndex, size_t bufferCount, RConstantBuffer* const* ppBuffers, const size_t* pLimitSizes )
/// Set a range of pixel shader constant buffers to use for rendering.
///
/// @param[in] startIndex   Starting pixel shader constant buffer index to set.
/// @param[in] bufferCount  Number of consecutive constant buffers to set.
/// @param[in] ppBuffers    Array of constant buffers to set.
/// @param[in] pLimitSizes  Optional array of sizes (in bytes) to limit update ranges for each constant buffer.
///                         Only up to the specified number of bytes will be guaranteed to be loaded into the shader
///                         for each buffer.  Invalid index values can be used to specify that the entire buffer
///                         should be updated.  On platforms that don't support storage of constant buffers on the
///                         GPU (i.e. Direct3D 9 and such, where shader constants must be passed in the command
///                         buffer when changing), this can provide a significant performance improvement.
///
/// @see SetVertexConstantBuffers()

/// @fn void RRenderCommandProxy::SetTexture( size_t samplerIndex, RTexture* pTexture )
/// Set the texture used for a given sampler.
///
/// @param[in] samplerIndex  Index of the sampler to set.
/// @param[in] pTexture      Texture to set.

/// @fn void RRenderCommandProxy::DrawIndexed( ERendererPrimitiveType primitiveType, uint32_t baseVertexIndex, uint32_t minIndex, uint32_t usedVertexCount, uint32_t startIndex, uint32_t primitiveCount )
/// Draw primitives based on a list of indexed vertices.
///
/// @param[in] primitiveType    Type of primitive to render.
/// @param[in] baseVertexIndex  Vertex offset of the first vertex to use from the start of each vertex stream.
/// @param[in] minIndex         Minimum vertex index value.
/// @param[in] usedVertexCount  Range of vertices used during this call, starting from the vertex addressed by the
///                             minimum vertex index value.
/// @param[in] startIndex       Offset of the first index within the index buffer to use for rendering.
/// @param[in] primitiveCount   Number of primitives to render.
///
/// @see DrawUnindexed()

/// @fn void RRenderCommandProxy::DrawUnindexed( ERendererPrimitiveType primitiveType, uint32_t baseVertexIndex, uint32_t primitiveCount )
/// Draw primitives based on an unindexed list of vertices.
///
/// @param[in] primitiveType    Type of primitive to render.
/// @param[in] baseVertexIndex  Vertex offset of the first vertex to use from the start of each vertex stream.
/// @param[in] primitiveCount   Number of primitives to render.
///
/// @see DrawIndexed()

/// @fn void RRenderCommandProxy::SetFence( RFence* pFence )
/// Signal a fence once all previously issued commands have been processed by the GPU.
///
/// Information about fences and their use can be found in the description for Renderer::CreateFence()
///
/// @param[in] pFence  Fence to signal.
///
/// @see Renderer::CreateFence(), Renderer::SyncFence(), Renderer::TrySyncFence()

/// @fn void RRenderCommandProxy::UnbindResources()
/// Unbind all currently bound state objects, buffers, and textures, and reset the target render surfaces to the
/// default set provided by the main render context.
///
/// This make sure any references to render resources held internally by the rendering system are released.  This is
/// useful to ensure resources can be freed safely in between frames.

/// @fn void RRenderCommandProxy::ExecuteCommandList( RRenderCommandList* pCommandList )
/// Execute a pre-recorded command list.
///
/// @param[in] pCommandList  Command list to execute.
///
/// @see FinishCommandList()

/// @fn void RRenderCommandProxy::FinishCommandList( RRenderCommandListPtr& rspCommandList )
/// Finish recording commands to the current command list and return the command list.
///
/// Note that this can only be called on deferred command proxies.  The renderer implementation may trigger an
/// assertion if this is an immediate command proxy.
///
/// When finishing the command list, the internal reference to the command list is cleared, so the command list must
/// be assigned to a smart pointer passed as a parameter.  The provided smart pointer will contain the only
/// reference to the command list immediately upon returning.
///
/// @param[out] rspCommandList  Recorded command list, or null if this is not a deferred command list.
///
/// @see ExecuteCommandList()
