namespace Helium
{
    bool CompiledShaderData::operator==( const CompiledShaderData& _rhs ) const
    {
        return ( 
            compiledCodeBuffer == _rhs.compiledCodeBuffer &&
            constantBuffers == _rhs.constantBuffers &&
            samplerInputs == _rhs.samplerInputs &&
            textureInputs == _rhs.textureInputs
            );
    }

    bool CompiledShaderData::operator!=( const CompiledShaderData& _rhs ) const
    {
        return !( *this == _rhs );
    }

    bool ShaderConstantInfo::operator==( const ShaderConstantInfo& _rhs ) const
    {
        return ( 
            name == _rhs.name &&
            offset == _rhs.offset &&
            size == _rhs.size &&
            usedSize == _rhs.usedSize
            );
    }

    bool ShaderConstantInfo::operator!=( const ShaderConstantInfo& _rhs ) const
    {
        return !( *this == _rhs );
    }
        
    bool ShaderConstantBufferInfo::operator==( const ShaderConstantBufferInfo& _rhs ) const
    {
        return ( 
            name == _rhs.name &&
            constants == _rhs.constants &&
            index == _rhs.index &&
            size == _rhs.size
            );
    }

    bool ShaderConstantBufferInfo::operator!=( const ShaderConstantBufferInfo& _rhs ) const
    {
        return !( *this == _rhs );
    }
      
    bool ShaderSamplerInfo::operator==( const ShaderSamplerInfo& _rhs ) const
    {
        return ( 
            name == _rhs.name &&
            bindIndex == _rhs.bindIndex
            );
    }

    bool ShaderSamplerInfo::operator!=( const ShaderSamplerInfo& _rhs ) const
    {
        return !( *this == _rhs );
    }

    bool ShaderTextureInfo::operator==( const ShaderTextureInfo& _rhs ) const
    {
        return ( 
            name == _rhs.name &&
            bindIndex == _rhs.bindIndex
            );
    }

    bool ShaderTextureInfo::operator!=( const ShaderTextureInfo& _rhs ) const
    {
        return !( *this == _rhs );
    }
        
    bool ShaderConstantBufferInfoSet::operator==( const ShaderConstantBufferInfoSet& _rhs ) const
    {
        return ( 
            buffers == _rhs.buffers
            );
    }

    bool ShaderConstantBufferInfoSet::operator!=( const ShaderConstantBufferInfoSet& _rhs ) const
    {
        return !( *this == _rhs );
    }
            
    bool ShaderSamplerInfoSet::operator==( const ShaderSamplerInfoSet& _rhs ) const
    {
        return ( 
            inputs == _rhs.inputs
            );
    }

    bool ShaderSamplerInfoSet::operator!=( const ShaderSamplerInfoSet& _rhs ) const
    {
        return !( *this == _rhs );
    }     

    bool ShaderTextureInfoSet::operator==( const ShaderTextureInfoSet& _rhs ) const
    {
        return ( 
            inputs == _rhs.inputs
            );
    }

    bool ShaderTextureInfoSet::operator!=( const ShaderTextureInfoSet& _rhs ) const
    {
        return !( *this == _rhs );
    }    
    ///////////////////////
    bool Shader::Toggle::operator==( const Shader::Toggle& _rhs ) const
    {
        return ( 
            name == _rhs.name &&
            shaderTypeFlags == _rhs.shaderTypeFlags
            );
    }

    bool Shader::Toggle::operator!=( const Shader::Toggle& _rhs ) const
    {
        return !( *this == _rhs );
    }    

    bool Shader::Select::operator==( const Shader::Select& _rhs ) const
    {
        return ( 
            name == _rhs.name &&
            choices == _rhs.choices &&
            allFlags == _rhs.allFlags
            );
    }

    bool Shader::Select::operator!=( const Shader::Select& _rhs ) const
    {
        return !( *this == _rhs );
    }    

    bool Shader::SelectPair::operator==( const Shader::SelectPair& _rhs ) const
    {
        return ( 
            name == _rhs.name &&
            choice == _rhs.choice
            );
    }

    bool Shader::SelectPair::operator!=( const Shader::SelectPair& _rhs ) const
    {
        return !( *this == _rhs );
    }    

    bool Shader::Options::operator==( const Shader::Options& _rhs ) const
    {
        return ( 
            m_toggles == _rhs.m_toggles &&
            m_selects == _rhs.m_selects
            );
    }

    bool Shader::Options::operator!=( const Shader::Options& _rhs ) const
    {
        return !( *this == _rhs );
    }

    /// Get whether all variants of this shader should be precached by the resource preprocessing.
    ///
    /// @return  True if all variants of this shader should be precached, false if not.
    bool Shader::GetPrecacheAllVariants() const
    {
        return m_bPrecacheAllVariants;
    }

    /// Get the set of system options associated with this shader.
    ///
    /// @return  System shader options.
    ///
    /// @see GetUserOptions()
    const Shader::Options& Shader::GetSystemOptions() const
    {
        return m_persistentResourceData.GetSystemOptions();
    }

    /// Get the set of user options associated with this shader.
    ///
    /// @return  User shader options.
    ///
    /// @see GetSystemOptions()
    const Shader::Options& Shader::GetUserOptions() const
    {
        return m_persistentResourceData.GetUserOptions();
    }

    /// Get the currently registered override callback for handling shader variant begin-load calls.
    ///
    /// @return  Pointer to the currently registered begin-load override callback.
    ///
    /// @see SetVariantLoadOverride(), GetTryFinishLoadVariantOverride(), GetVariantLoadOverrideData()
    Shader::BEGIN_LOAD_VARIANT_FUNC* Shader::GetBeginLoadVariantOverride()
    {
        return sm_pBeginLoadVariantOverride;
    }

    /// Get the currently registered override callback for handling shader variant try-finish-load calls.
    ///
    /// @return  Pointer to the currently registered try-finish-load override callback.
    ///
    /// @see SetVariantLoadOverride(), GetBeginLoadVariantOverride(), GetVariantLoadOverrideData()
    Shader::TRY_FINISH_LOAD_VARIANT_FUNC* Shader::GetTryFinishLoadVariantOverride()
    {
        return sm_pTryFinishLoadVariantOverride;
    }

    /// Get the currently registered data passed to the variant load override callbacks.
    ///
    /// @return  Variant load override callback data.
    ///
    /// @see SetVariantLoadOverride(), GetBeginLoadVariantOverride(), GetTryFinishLoadVariantOverride()
    void* Shader::GetVariantLoadOverrideData()
    {
        return sm_pVariantLoadOverrideData;
    }

    /// Get the list of toggles in this set of shader preprocessor options.
    ///
    /// @return  Reference to the array of preprocessor toggles.
    ///
    /// @see GetSelects()
    DynamicArray< Shader::Toggle >& Shader::Options::GetToggles()
    {
        return m_toggles;
    }

    /// Get the list of toggles in this set of shader preprocessor options.
    ///
    /// @return  Constant reference to the array of preprocessor toggles.
    ///
    /// @see GetSelects()
    const DynamicArray< Shader::Toggle >& Shader::Options::GetToggles() const
    {
        return m_toggles;
    }

    /// Get the list of selections in this set of shader preprocessor options.
    ///
    /// @return  Reference to the array of preprocessor selections.
    ///
    /// @see GetToggles()
    DynamicArray< Shader::Select >& Shader::Options::GetSelects()
    {
        return m_selects;
    }

    /// Get the list of selections in this set of shader preprocessor options.
    ///
    /// @return  Constant reference to the array of preprocessor selections.
    ///
    /// @see GetToggles()
    const DynamicArray< Shader::Select >& Shader::Options::GetSelects() const
    {
        return m_selects;
    }

    /// Get the set of available system shader preprocessor options.
    ///
    /// @return  Reference to the system preprocessor options.
    ///
    /// @see GetUserOptions()
    Shader::Options& Shader::PersistentResourceData::GetSystemOptions()
    {
        return m_systemOptions;
    }

    /// Get the set of available system shader preprocessor options.
    ///
    /// @return  Constant reference to the system preprocessor options.
    ///
    /// @see GetUserOptions()
    const Shader::Options& Shader::PersistentResourceData::GetSystemOptions() const
    {
        return m_systemOptions;
    }

    /// Get the set of available user shader preprocessor options.
    ///
    /// @return  Reference to the user preprocessor options.
    ///
    /// @see GetSystemOptions()
    Shader::Options& Shader::PersistentResourceData::GetUserOptions()
    {
        return m_userOptions;
    }

    /// Get the set of available user shader preprocessor options.
    ///
    /// @return  Constant reference to the user preprocessor options.
    ///
    /// @see GetSystemOptions()
    const Shader::Options& Shader::PersistentResourceData::GetUserOptions() const
    {
        return m_userOptions;
    }

    /// Get the render resource associated with the specified system option set index for this shader variant.
    ///
    /// @param[in] index  Index of the specific render resource to retrieve (based on system options).
    ///
    /// @return  Shader render resource, if available.
    ///
    /// @see GetConstantBufferInfoSet(), GetSamplerInfoSet(), GetTextureInfoSet()
    RShader* ShaderVariant::GetRenderResource( size_t index ) const
    {
        if( index >= m_renderResources.GetSize() )
        {
            HELIUM_TRACE(
                TraceLevels::Error,
                "ShaderVariant::GetRenderResource(): Invalid render resource index %" PRIuSZ " specified (resource count: %" PRIuSZ ").\n",
                index,
                m_renderResources.GetSize() );

            return NULL;
        }

        return m_renderResources[ index ];
    }

    /// Get information about the available shader constant buffers associated with the specified system option set
    /// index for this shader variant.
    ///
    /// @param[in] index  Index of the constant buffer set to retrieve (based on system options).
    ///
    /// @return  Pointer to the constant buffer set, if available.
    ///
    /// @see GetRenderResource(), GetSamplerInfoSet(), GetTextureInfoSet()
    const ShaderConstantBufferInfoSet* ShaderVariant::GetConstantBufferInfoSet( size_t index ) const
    {
        if( index >= m_constantBufferSets.GetSize() )
        {
            HELIUM_TRACE(
                TraceLevels::Error,
                "ShaderVariant::GetConstantBufferSet(): Invalid render resource index %" PRIuSZ " specified (resource count: %" PRIuSZ ").\n",
                index,
                m_constantBufferSets.GetSize() );

            return NULL;
        }

        return &m_constantBufferSets[ index ];
    }

    /// Get information about the available texture sampler inputs associated with the specified system option set index
    /// for this shader variant.
    ///
    /// @param[in] index  Index of the sampler input set to retrieve (based on system options).
    ///
    /// @return  Pointer to the sampler input set, if available.
    ///
    /// @see GetRenderResource(), GetConstantBufferInfoSet(), GetTextureInfoSet()
    const ShaderSamplerInfoSet* ShaderVariant::GetSamplerInfoSet( size_t index ) const
    {
        if( index >= m_samplerInputSets.GetSize() )
        {
            HELIUM_TRACE(
                TraceLevels::Error,
                "ShaderVariant::GetSamplerInfoSet(): Invalid render resource index %" PRIuSZ " specified (resource count: %" PRIuSZ ").\n",
                index,
                m_samplerInputSets.GetSize() );

            return NULL;
        }

        return &m_samplerInputSets[ index ];
    }

    /// Get information about the available texture inputs associated with the specified system option set index for
    /// this shader variant.
    ///
    /// @param[in] index  Index of the texture input set to retrieve (based on system options).
    ///
    /// @return  Pointer to the texture input set, if available.
    ///
    /// @see GetRenderResource(), GetConstantBufferInfoSet(), GetSamplerInfoSet()
    const ShaderTextureInfoSet* ShaderVariant::GetTextureInfoSet( size_t index ) const
    {
        if( index >= m_textureInputSets.GetSize() )
        {
            HELIUM_TRACE(
                TraceLevels::Error,
                "ShaderVariant::GetTextureInfoSet(): Invalid render resource index %" PRIuSZ " specified (resource count: %" PRIuSZ ").\n",
                index,
                m_textureInputSets.GetSize() );

            return NULL;
        }

        return &m_textureInputSets[ index ];
    }

    /// Get the shader resource to which this variant belongs.
    ///
    /// @return  Owner shader resource.
    Shader* ShaderVariant::GetShader() const
    {
        Asset* pOwner = GetOwner();
        Shader* pShader = Reflect::AssertCast< Shader >( pOwner );

        return pShader;
    }
}
