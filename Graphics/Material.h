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

#include "MathSimd/Vector2.h"
#include "MathSimd/Vector3.h"
#include "MathSimd/Vector4.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"

namespace Helium
{
    class Shader;
    typedef Helium::StrongPtr< Shader > ShaderPtr;
    typedef Helium::StrongPtr< const Shader > ConstShaderPtr;

    class ShaderVariant;
    typedef Helium::StrongPtr< ShaderVariant > ShaderVariantPtr;
    typedef Helium::StrongPtr< const ShaderVariant > ConstShaderVariantPtr;

    class Texture;
    typedef Helium::StrongPtr< Texture > TexturePtr;
    typedef Helium::StrongPtr< const Texture > ConstTexturePtr;

    HELIUM_DECLARE_RPTR( RConstantBuffer );

    /// Material resource type.
    class HELIUM_GRAPHICS_API Material : public Resource
    {
        HELIUM_DECLARE_OBJECT( Material, Resource );

    public:
        /// Scalar floating-point parameter.
        struct HELIUM_GRAPHICS_API Float1Parameter
        {
            REFLECT_DECLARE_BASE_STRUCTURE(Material::Float1Parameter);
            static void PopulateComposite( Reflect::Composite& comp );

            inline bool operator==( const Float1Parameter& _rhs ) const;
            inline bool operator!=( const Float1Parameter& _rhs ) const;

            /// Parameter name.
            Name name;
            /// Parameter value.
            float32_t value;
            
        };

        /// Two-component floating-point vector parameter.
        struct HELIUM_GRAPHICS_API Float2Parameter
        {
            REFLECT_DECLARE_BASE_STRUCTURE(Material::Float2Parameter);
            static void PopulateComposite( Reflect::Composite& comp );

            inline bool operator==( const Float2Parameter& _rhs ) const;
            inline bool operator!=( const Float2Parameter& _rhs ) const;

            /// Parameter name.
            Name name;
            /// Parameter value.
            Simd::Vector2 value;
        };

        /// Three-component floating-point vector parameter.
        struct HELIUM_GRAPHICS_API Float3Parameter
        {
            REFLECT_DECLARE_BASE_STRUCTURE(Material::Float3Parameter);
            static void PopulateComposite( Reflect::Composite& comp );

            inline bool operator==( const Float3Parameter& _rhs ) const;
            inline bool operator!=( const Float3Parameter& _rhs ) const;

            /// Parameter name.
            Name name;
            /// Parameter value.
            Simd::Vector3 value;
        };

        /// Four-component floating-point vector parameter.
        struct HELIUM_GRAPHICS_API Float4Parameter
        {
            REFLECT_DECLARE_BASE_STRUCTURE(Material::Float4Parameter);
            static void PopulateComposite( Reflect::Composite& comp );

            inline bool operator==( const Float4Parameter& _rhs ) const;
            inline bool operator!=( const Float4Parameter& _rhs ) const;

            /// Parameter name.
            Name name;
            /// Parameter value.
            Simd::Vector4 value;
        };

        /// Shader texture sampler parameter.
        struct HELIUM_GRAPHICS_API TextureParameter
        {
            REFLECT_DECLARE_BASE_STRUCTURE(Material::TextureParameter);
            static void PopulateComposite( Reflect::Composite& comp );

            inline bool operator==( const TextureParameter& _rhs ) const;
            inline bool operator!=( const TextureParameter& _rhs ) const;

            /// Parameter name.
            Name name;
            /// Parameter value.
            TexturePtr value;
        };
        
        /// Shader texture sampler parameter.
        struct HELIUM_GRAPHICS_API PersistentResourceData : public Reflect::Object
        {
            REFLECT_DECLARE_OBJECT(Material::PersistentResourceData, Reflect::Object);
            static void PopulateComposite( Reflect::Composite& comp );

            inline bool operator==( const PersistentResourceData& _rhs ) const;
            inline bool operator!=( const PersistentResourceData& _rhs ) const;
            
            union
            {
                /// Shader variant indices.
                uint32_t m_shaderVariantIndices[ RShader::TYPE_MAX ];
                struct
                {
                    uint32_t m_shaderVariantIndexVertex;
                    uint32_t m_shaderVariantIndexPixel;
                };
            };
        };

        /// @name Construction/Destruction
        //@{
        Material();
        virtual ~Material();
        //@}

        /// @name Serialization
        //@{
        //virtual void Serialize( Serializer& s );
        static void PopulateComposite( Reflect::Composite& comp );

#if HELIUM_TOOLS
        void Helium::Material::PreSerialize( const Reflect::Field* field );
        void Helium::Material::PostDeserialize( const Reflect::Field* field );
#endif

        virtual bool NeedsPrecacheResourceData() const;
        virtual bool BeginPrecacheResourceData();
        virtual bool TryFinishPrecacheResourceData();
        //@}

        /// @name Resource Serialization
        //@{
        //virtual void SerializePersistentResourceData( Serializer& s );
        virtual bool LoadPersistentResourceObject(Reflect::ObjectPtr &_object);
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
        inline const DynamicArray< Shader::SelectPair >& GetUserOptions() const;

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
        //AssetPtr m_spShaderAsAsset;
        ShaderPtr m_spShader;
        PersistentResourceData m_persistentResourceData;

        /// Cached references to the specific shader variants used by this material.
        ShaderVariantPtr m_shaderVariants[ RShader::TYPE_MAX ];
        /// Shader variant load IDs.
        size_t m_shaderVariantLoadIds[ RShader::TYPE_MAX ];

        /// Constant buffers for material parameters.
        RConstantBufferPtr m_constantBuffers[ RShader::TYPE_MAX ];
        /// Constant buffer load IDs.
        size_t m_constantBufferLoadIds[ RShader::TYPE_MAX ];

        /// Shader texture parameters.
        DynamicArray< TextureParameter > m_textureParameters;

#if HELIUM_TOOLS
        /// User options cached during loading.
        DynamicArray< Shader::SelectPair > m_userOptions;
        /// Shader scalar float parameters.
        DynamicArray< Float1Parameter > m_float1Parameters;
        /// Shader two-component float vector parameters.
        DynamicArray< Float2Parameter > m_float2Parameters;
        /// Shader three-component float vector parameters.
        DynamicArray< Float3Parameter > m_float3Parameters;
        /// Shader four-component float vector parameters.
        DynamicArray< Float4Parameter > m_float4Parameters;

        /// True if shader options have been loaded and need to be resolved prior to resource precaching.
        bool m_bLoadedOptions;
#endif
    };
}

#include "Graphics/Material.inl"

#endif  // HELIUM_GRAPHICS_MATERIAL_H
