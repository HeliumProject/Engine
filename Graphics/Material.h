//----------------------------------------------------------------------------------------------------------------------
// Material.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_GRAPHICS_MATERIAL_H
#define HELIUM_GRAPHICS_MATERIAL_H

#include "Graphics/Graphics.h"
#include "Engine/Resource.h"

#include "Foundation/Math/SimdVector2.h"
#include "Foundation/Math/SimdVector3.h"
#include "Foundation/Math/SimdVector4.h"
#include "Graphics/Shader.h"

namespace Helium
{
    HELIUM_DECLARE_PTR( Shader );
    HELIUM_DECLARE_PTR( ShaderVariant );
    HELIUM_DECLARE_PTR( Texture );

    HELIUM_DECLARE_RPTR( RConstantBuffer );

    /// Material resource type.
    class HELIUM_GRAPHICS_API Material : public Resource
    {
        HELIUM_DECLARE_OBJECT( Material, Resource );

    public:
        /// Scalar floating-point parameter.
        struct HELIUM_GRAPHICS_API Float1Parameter
        {
            /// Parameter name.
            Name name;
            /// Parameter value.
            float32_t value;

            /// @name Serialization
            //@{
            //void Serialize( Serializer& s );
            //@}
        };

        /// Two-component floating-point vector parameter.
        struct HELIUM_GRAPHICS_API Float2Parameter
        {
            /// Parameter name.
            Name name;
            /// Parameter value.
            Simd::Vector2 value;

            /// @name Serialization
            //@{
            //void Serialize( Serializer& s );
            //@}
        };

        /// Three-component floating-point vector parameter.
        struct HELIUM_GRAPHICS_API Float3Parameter
        {
            /// Parameter name.
            Name name;
            /// Parameter value.
            Simd::Vector3 value;

            /// @name Serialization
            //@{
            //void Serialize( Serializer& s );
            //@}
        };

        /// Four-component floating-point vector parameter.
        struct HELIUM_GRAPHICS_API Float4Parameter
        {
            /// Parameter name.
            Name name;
            /// Parameter value.
            Simd::Vector4 value;

            /// @name Serialization
            //@{
            //void Serialize( Serializer& s );
            //@}
        };

        /// Shader texture sampler parameter.
        struct TextureParameter
        {
            /// Parameter name.
            Name name;
            /// Parameter value.
            TexturePtr value;

            /// @name Serialization
            //@{
            //void Serialize( Serializer& s );
            //@}
        };

        /// @name Construction/Destruction
        //@{
        Material();
        virtual ~Material();
        //@}

        /// @name Serialization
        //@{
        //virtual void Serialize( Serializer& s );

        virtual bool NeedsPrecacheResourceData() const;
        virtual bool BeginPrecacheResourceData();
        virtual bool TryFinishPrecacheResourceData();
        //@}

        /// @name Resource Serialization
        //@{
        virtual void SerializePersistentResourceData( Serializer& s );
        //@}

        /// @name Resource Caching Support
        //@{
        virtual Name GetCacheName() const;
        //@}

        /// @name Data Access
        //@{
        inline Shader* GetShader() const;
        inline uint32_t GetShaderVariantIndex( RShader::EType shaderType ) const;
        inline ShaderVariant* GetShaderVariant( RShader::EType shaderType ) const;

        inline RConstantBuffer* GetConstantBuffer( RShader::EType shaderType ) const;

        inline size_t GetTextureParameterCount() const;
        inline const TextureParameter& GetTextureParameter( size_t index ) const;

#if HELIUM_TOOLS
        inline const DynArray< Shader::SelectPair >& GetUserOptions() const;

        inline size_t GetFloat1ParameterCount() const;
        inline const Float1Parameter& GetFloat1Parameter( size_t index ) const;

        inline size_t GetFloat2ParameterCount() const;
        inline const Float2Parameter& GetFloat2Parameter( size_t index ) const;

        inline size_t GetFloat3ParameterCount() const;
        inline const Float3Parameter& GetFloat3Parameter( size_t index ) const;

        inline size_t GetFloat4ParameterCount() const;
        inline const Float4Parameter& GetFloat4Parameter( size_t index ) const;
#endif
        //@}

        /// @name Static Information
        //@{
        static Name GetParameterConstantBufferName();
        //@}

    protected:
        /// @name Protected Utility Functions
        //@{
#if HELIUM_TOOLS
        void SynchronizeShaderParameters();
#endif
        //@}

    private:
        /// Material shader.
        ShaderPtr m_spShader;
        /// Shader variant indices.
        uint32_t m_shaderVariantIndices[ RShader::TYPE_MAX ];

        /// Cached references to the specific shader variants used by this material.
        ShaderVariantPtr m_shaderVariants[ RShader::TYPE_MAX ];
        /// Shader variant load IDs.
        size_t m_shaderVariantLoadIds[ RShader::TYPE_MAX ];

        /// Constant buffers for material parameters.
        RConstantBufferPtr m_constantBuffers[ RShader::TYPE_MAX ];
        /// Constant buffer load IDs.
        size_t m_constantBufferLoadIds[ RShader::TYPE_MAX ];

        /// Shader texture parameters.
        DynArray< TextureParameter > m_textureParameters;

#if HELIUM_TOOLS
        /// User options cached during loading.
        DynArray< Shader::SelectPair > m_userOptions;
        /// Shader scalar float parameters.
        DynArray< Float1Parameter > m_float1Parameters;
        /// Shader two-component float vector parameters.
        DynArray< Float2Parameter > m_float2Parameters;
        /// Shader three-component float vector parameters.
        DynArray< Float3Parameter > m_float3Parameters;
        /// Shader four-component float vector parameters.
        DynArray< Float4Parameter > m_float4Parameters;

        /// True if shader options have been loaded and need to be resolved prior to resource precaching.
        bool m_bLoadedOptions;
#endif
    };
}

#include "Graphics/Material.inl"

#endif  // HELIUM_GRAPHICS_MATERIAL_H
