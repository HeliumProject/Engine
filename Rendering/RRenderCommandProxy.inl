namespace Helium
{
    /// Set the sampler states for a series of sampler slots.
    ///
    /// @param[in] startIndex    Index of the first sampler to set.
    /// @param[in] samplerCount  Number of sampler states in the given array.
    /// @param[in] pspStates     Array of state settings to apply.
    ///
    /// @see Renderer::CreateSamplerState(), SetRasterizerState(), SetBlendState(), SetDepthStencilState()
    void RRenderCommandProxy::SetSamplerStates(
        size_t startIndex,
        size_t samplerCount,
        RSamplerStatePtr const* pspStates )
    {
        SetSamplerStates( startIndex, samplerCount, &static_cast< RSamplerState* const& >( pspStates[ 0 ] ) );
    }

    /// Set the vertex buffers to use for vertex input.
    ///
    /// Note that a VertexInputDescription must also be set to define how the vertex buffer data should be interpreted.
    ///
    /// @param[in] startIndex   Starting vertex buffer index to set.
    /// @param[in] bufferCount  Number of consecutive vertex buffers to set.
    /// @param[in] pspBuffers   Array of vertex buffers to set.
    /// @param[in] pStrides     Array specifying the byte stride between each consecutive vertex in each vertex buffer.
    /// @param[in] pOffsets     Array specifying the byte offsets within each vertex buffer from which to read the first
    ///                         vertices to use for rendering.
    ///
    /// @see SetVertexInputLayout(), SetIndexBuffer()
    void RRenderCommandProxy::SetVertexBuffers(
        size_t startIndex,
        size_t bufferCount,
        RVertexBufferPtr const* pspBuffers,
        uint32_t* pStrides,
        uint32_t* pOffsets )
    {
        SetVertexBuffers(
            startIndex,
            bufferCount,
            &static_cast< RVertexBuffer* const& >( pspBuffers[ 0 ] ),
            pStrides,
            pOffsets );
    }

    /// Set a range of vertex shader constant buffers to use for rendering.
    ///
    /// @param[in] startIndex   Starting vertex shader constant buffer index to set.
    /// @param[in] bufferCount  Number of consecutive constant buffers to set.
    /// @param[in] pspBuffers   Array of constant buffers to set.
    /// @param[in] pLimitSizes  Optional array of sizes (in bytes) to limit update ranges for each constant buffer.
    ///                         Only up to the specified number of bytes will be guaranteed to be loaded into the shader
    ///                         for each buffer.  Invalid index values can be used to specify that the entire buffer
    ///                         should be updated.  On platforms that don't support storage of constant buffers on the
    ///                         GPU (i.e. Direct3D 9 and such, where shader constants must be passed in the command
    ///                         buffer when changing), this can provide a significant performance improvement.
    ///
    /// @see SetPixelConstantBuffers()
    void RRenderCommandProxy::SetVertexConstantBuffers(
        size_t startIndex,
        size_t bufferCount,
        RConstantBufferPtr const* pspBuffers,
        const size_t* pLimitSizes )
    {
        SetVertexConstantBuffers(
            startIndex,
            bufferCount,
            &static_cast< RConstantBuffer* const& >( pspBuffers[ 0 ] ),
            pLimitSizes );
    }

    /// Set a range of pixel shader constant buffers to use for rendering.
    ///
    /// @param[in] startIndex   Starting pixel shader constant buffer index to set.
    /// @param[in] bufferCount  Number of consecutive constant buffers to set.
    /// @param[in] pspBuffers   Array of constant buffers to set.
    /// @param[in] pLimitSizes  Optional array of sizes (in bytes) to limit update ranges for each constant buffer.
    ///                         Only up to the specified number of bytes will be guaranteed to be loaded into the shader
    ///                         for each buffer.  Invalid index values can be used to specify that the entire buffer
    ///                         should be updated.  On platforms that don't support storage of constant buffers on the
    ///                         GPU (i.e. Direct3D 9 and such, where shader constants must be passed in the command
    ///                         buffer when changing), this can provide a significant performance improvement.
    ///
    /// @see SetVertexConstantBuffers()
    void RRenderCommandProxy::SetPixelConstantBuffers(
        size_t startIndex,
        size_t bufferCount,
        RConstantBufferPtr const* pspBuffers,
        const size_t* pLimitSizes )
    {
        SetPixelConstantBuffers(
            startIndex,
            bufferCount,
            &static_cast< RConstantBuffer* const& >( pspBuffers[ 0 ] ),
            pLimitSizes );
    }
}
