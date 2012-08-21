//----------------------------------------------------------------------------------------------------------------------
// Material.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Helium
{
    /// Get the shader resource assigned to this material.
    ///
    /// @return  Assigned shader resource.
    Shader* Material::GetShader() const
    {
        return m_spShader;
    }

    /// Get the cached shader variant index for the specified shader type.
    ///
    /// @param[in] shaderType  Shader type.
    ///
    /// @return  Shader variant index.
    uint32_t Material::GetShaderVariantIndex( RShader::EType shaderType ) const
    {
        HELIUM_ASSERT( static_cast< size_t >( shaderType ) < static_cast< size_t >( RShader::TYPE_MAX ) );

        return m_persistentResourceData.m_shaderVariantIndices[ shaderType ];
    }

    /// Get the shader variant resource for the specified shader type.
    ///
    /// @param[in] shaderType  Shader type.
    ///
    /// @return  Shader variant resource.
    ShaderVariant* Material::GetShaderVariant( RShader::EType shaderType ) const
    {
        HELIUM_ASSERT( static_cast< size_t >( shaderType ) < static_cast< size_t >( RShader::TYPE_MAX ) );

        return m_shaderVariants[ shaderType ];
    }

    /// Get the constant buffer render resource for the specified shader type.
    ///
    /// @param[in] shaderType  Shader type.
    ///
    /// @return  Constant buffer render resource.
    RConstantBuffer* Material::GetConstantBuffer( RShader::EType shaderType ) const
    {
        HELIUM_ASSERT( static_cast< size_t >( shaderType ) < static_cast< size_t >( RShader::TYPE_MAX ) );

        return m_constantBuffers[ shaderType ];
    }

    /// Get the number of texture parameters exposed in this material.
    ///
    /// @return  Texture parameter count.
    ///
    /// @see GetTextureParameter(), GetFloatParameterCount(), GetFloatParameter()
    size_t Material::GetTextureParameterCount() const
    {
        return m_textureParameters.GetSize();
    }

    /// Get the texture parameter information associated with the specified index.
    ///
    /// @param[in] index  Texture parameter index.
    ///
    /// @return  Reference to the specified texture parameter.
    ///
    /// @see GetTextureParameterCount(), GetFloatParameterCount(), GetFloatParameter()
    const Material::TextureParameter& Material::GetTextureParameter( size_t index ) const
    {
        HELIUM_ASSERT( index < m_textureParameters.GetSize() );

        return m_textureParameters[ index ];
    }

#if HELIUM_TOOLS
    /// Get the array of user options loaded for this material from the non-preprocessed XML data.
    ///
    /// @return  Non-preprocessed user option set.
    const DynArray< Shader::SelectPair >& Material::GetUserOptions() const
    {
        return m_userOptions;
    }

    /// Get the number of scalar float shader parameters exposed in this material.
    ///
    /// @return  Scalar float parameter count.
    ///
    /// @see GetFloat1Parameter(), GetFloat2ParameterCount(), GetFloat2Parameter(), GetFloat3ParameterCount(),
    ///      GetFloat3Parameter(), GetFloat4ParameterCount(), GetFloat4Parameter(), GetTextureParameterCount(),
    ///      GetTextureParameter()
    size_t Material::GetFloat1ParameterCount() const
    {
        return m_float1Parameters.GetSize();
    }

    /// Get the scalar float parameter information associated with the specified index.
    ///
    /// @param[in] index  Scalar float parameter index.
    ///
    /// @return  Reference to the specified scalar float parameter.
    ///
    /// @see GetFloat1ParameterCount(), GetFloat2ParameterCount(), GetFloat2Parameter(), GetFloat3ParameterCount(),
    ///      GetFloat3Parameter(), GetFloat4ParameterCount(), GetFloat4Parameter(), GetTextureParameterCount(),
    ///      GetTextureParameter()
    const Material::Float1Parameter& Material::GetFloat1Parameter( size_t index ) const
    {
        HELIUM_ASSERT( index < m_float1Parameters.GetSize() );

        return m_float1Parameters[ index ];
    }

    /// Get the number of two-component float vector shader parameters exposed in this material.
    ///
    /// @return  Two-component float vector parameter count.
    ///
    /// @see GetFloat2Parameter(), GetFloat1ParameterCount(), GetFloat1Parameter(), GetFloat3ParameterCount(),
    ///      GetFloat3Parameter(), GetFloat4ParameterCount(), GetFloat4Parameter(), GetTextureParameterCount(),
    ///      GetTextureParameter()
    size_t Material::GetFloat2ParameterCount() const
    {
        return m_float2Parameters.GetSize();
    }

    /// Get the two-component float vector parameter information associated with the specified index.
    ///
    /// @param[in] index  Two-component float vector parameter index.
    ///
    /// @return  Reference to the specified two-component float vector parameter.
    ///
    /// @see GetFloat2ParameterCount(), GetFloat1ParameterCount(), GetFloat1Parameter(), GetFloat3ParameterCount(),
    ///      GetFloat3Parameter(), GetFloat4ParameterCount(), GetFloat4Parameter(), GetTextureParameterCount(),
    ///      GetTextureParameter()
    const Material::Float2Parameter& Material::GetFloat2Parameter( size_t index ) const
    {
        HELIUM_ASSERT( index < m_float2Parameters.GetSize() );

        return m_float2Parameters[ index ];
    }

    /// Get the number of three-component float vector shader parameters exposed in this material.
    ///
    /// @return  Three-component float vector parameter count.
    ///
    /// @see GetFloat3Parameter(), GetFloat1ParameterCount(), GetFloat1Parameter(), GetFloat2ParameterCount(),
    ///      GetFloat2Parameter(), GetFloat4ParameterCount(), GetFloat4Parameter(), GetTextureParameterCount(),
    ///      GetTextureParameter()
    size_t Material::GetFloat3ParameterCount() const
    {
        return m_float3Parameters.GetSize();
    }

    /// Get the three-component float vector parameter information associated with the specified index.
    ///
    /// @param[in] index  Three-component float vector parameter index.
    ///
    /// @return  Reference to the specified three-component float vector parameter.
    ///
    /// @see GetFloat3ParameterCount(), GetFloat1ParameterCount(), GetFloat1Parameter(), GetFloat2ParameterCount(),
    ///      GetFloat2Parameter(), GetFloat4ParameterCount(), GetFloat4Parameter(), GetTextureParameterCount(),
    ///      GetTextureParameter()
    const Material::Float3Parameter& Material::GetFloat3Parameter( size_t index ) const
    {
        HELIUM_ASSERT( index < m_float3Parameters.GetSize() );

        return m_float3Parameters[ index ];
    }

    /// Get the number of four-component float vector shader parameters exposed in this material.
    ///
    /// @return  Four-component float vector parameter count.
    ///
    /// @see GetFloat4Parameter(), GetFloat1ParameterCount(), GetFloat1Parameter(), GetFloat2ParameterCount(),
    ///      GetFloat2Parameter(), GetFloat3ParameterCount(), GetFloat3Parameter(), GetTextureParameterCount(),
    ///      GetTextureParameter()
    size_t Material::GetFloat4ParameterCount() const
    {
        return m_float4Parameters.GetSize();
    }

    /// Get the four-component float vector parameter information associated with the specified index.
    ///
    /// @param[in] index  Four-component float vector parameter index.
    ///
    /// @return  Reference to the specified four-component float vector parameter.
    ///
    /// @see GetFloat4ParameterCount(), GetFloat1ParameterCount(), GetFloat1Parameter(), GetFloat2ParameterCount(),
    ///      GetFloat2Parameter(), GetFloat3ParameterCount(), GetFloat3Parameter(), GetTextureParameterCount(),
    ///      GetTextureParameter()
    const Material::Float4Parameter& Material::GetFloat4Parameter( size_t index ) const
    {
        HELIUM_ASSERT( index < m_float4Parameters.GetSize() );

        return m_float4Parameters[ index ];
    }
#endif  // HELIUM_TOOLS
    
    bool Helium::Material::Float1Parameter::operator==( const Helium::Material::Float1Parameter& _rhs ) const
    {
        return ( 
            name == _rhs.name &&
            value == _rhs.value
            );
    }

    bool Helium::Material::Float1Parameter::operator!=( const Helium::Material::Float1Parameter& _rhs ) const
    {
        return !( *this == _rhs );
    }
        
    bool Helium::Material::Float2Parameter::operator==( const Helium::Material::Float2Parameter& _rhs ) const
    {
        return ( 
            name == _rhs.name &&
            value == _rhs.value
            );
    }

    bool Helium::Material::Float2Parameter::operator!=( const Helium::Material::Float2Parameter& _rhs ) const
    {
        return !( *this == _rhs );
    }
        
    bool Helium::Material::Float3Parameter::operator==( const Helium::Material::Float3Parameter& _rhs ) const
    {
        return ( 
            name == _rhs.name &&
            value == _rhs.value
            );
    }

    bool Helium::Material::Float3Parameter::operator!=( const Helium::Material::Float3Parameter& _rhs ) const
    {
        return !( *this == _rhs );
    }
        
    bool Helium::Material::Float4Parameter::operator==( const Helium::Material::Float4Parameter& _rhs ) const
    {
        return ( 
            name == _rhs.name &&
            value == _rhs.value
            );
    }

    bool Helium::Material::Float4Parameter::operator!=( const Helium::Material::Float4Parameter& _rhs ) const
    {
        return !( *this == _rhs );
    }
    
    bool Helium::Material::TextureParameter::operator==( const Helium::Material::TextureParameter& _rhs ) const
    {
        return ( 
            name == _rhs.name &&
            value == _rhs.value
            );
    }

    bool Helium::Material::TextureParameter::operator!=( const Helium::Material::TextureParameter& _rhs ) const
    {
        return !( *this == _rhs );
    }
    
    bool Helium::Material::PersistentResourceData::operator==( const Helium::Material::PersistentResourceData& _rhs ) const
    {
        for (uint32_t i = 0; i < RShader::TYPE_MAX; ++i)
        {
            if (m_shaderVariantIndices[i] != _rhs.m_shaderVariantIndices[i])
            {
                return false;
            }
        }

        return true;
    }

    bool Helium::Material::PersistentResourceData::operator!=( const Helium::Material::PersistentResourceData& _rhs ) const
    {
        return !( *this == _rhs );
    }
}
