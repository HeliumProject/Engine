#pragma once

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

	HELIUM_DECLARE_RPTR( RConstantBuffer );

	/// Material resource type.
	class HELIUM_GRAPHICS_API Material : public Resource
	{
		HELIUM_DECLARE_ASSET( Material, Resource );

	public:
		/// Scalar floating-point parameter.
		struct HELIUM_GRAPHICS_API Float1Parameter : Reflect::Struct
		{
			HELIUM_DECLARE_BASE_STRUCT(Material::Float1Parameter);
			static void PopulateMetaType( Reflect::MetaStruct& comp );

			inline bool operator==( const Float1Parameter& _rhs ) const;
			inline bool operator!=( const Float1Parameter& _rhs ) const;

			/// Parameter name.
			Name name;
			/// Parameter value.
			float32_t value;
		};

		/// Two-component floating-point vector parameter.
		struct HELIUM_GRAPHICS_API Float2Parameter : Reflect::Struct
		{
			HELIUM_DECLARE_BASE_STRUCT(Material::Float2Parameter);
			static void PopulateMetaType( Reflect::MetaStruct& comp );

			inline bool operator==( const Float2Parameter& _rhs ) const;
			inline bool operator!=( const Float2Parameter& _rhs ) const;

			/// Parameter name.
			Name name;
			/// Parameter value.
			Simd::Vector2 value;
		};

		/// Three-component floating-point vector parameter.
		struct HELIUM_GRAPHICS_API Float3Parameter : Reflect::Struct
		{
			HELIUM_DECLARE_BASE_STRUCT(Material::Float3Parameter);
			static void PopulateMetaType( Reflect::MetaStruct& comp );

			inline bool operator==( const Float3Parameter& _rhs ) const;
			inline bool operator!=( const Float3Parameter& _rhs ) const;

			/// Parameter name.
			Name name;
			/// Parameter value.
			Simd::Vector3 value;
		};

		/// Four-component floating-point vector parameter.
		struct HELIUM_GRAPHICS_API Float4Parameter : Reflect::Struct
		{
			HELIUM_DECLARE_BASE_STRUCT(Material::Float4Parameter);
			static void PopulateMetaType( Reflect::MetaStruct& comp );

			inline bool operator==( const Float4Parameter& _rhs ) const;
			inline bool operator!=( const Float4Parameter& _rhs ) const;

			/// Parameter name.
			Name name;
			/// Parameter value.
			Simd::Vector4 value;
		};

		/// Shader texture sampler parameter.
		struct HELIUM_GRAPHICS_API TextureParameter : Reflect::Struct
		{
			HELIUM_DECLARE_BASE_STRUCT(Material::TextureParameter);
			static void PopulateMetaType( Reflect::MetaStruct& comp );

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
			HELIUM_DECLARE_CLASS(Material::PersistentResourceData, Reflect::Object);
			static void PopulateMetaType( Reflect::MetaStruct& comp );

			inline bool operator==( const PersistentResourceData& _rhs ) const;
			inline bool operator!=( const PersistentResourceData& _rhs ) const;

			/// Shader variant indices.
			uint32_t m_shaderVariantIndices[ RShader::TYPE_MAX ];
		};

		/// @name Construction/Destruction
		//@{
		Material();
		virtual ~Material();
		//@}

		/// @name Serialization
		//@{
		static void PopulateMetaType( Reflect::MetaStruct& comp );

#if HELIUM_TOOLS
		void PreSerialize( const Reflect::Field* field ) override;
		void PostDeserialize( const Reflect::Field* field ) override;
#endif

		virtual bool NeedsPrecacheResourceData() const override;
		virtual bool BeginPrecacheResourceData() override;
		virtual bool TryFinishPrecacheResourceData() override;
		//@}

		/// @name Resource Serialization
		//@{
		virtual bool LoadPersistentResourceObject(Reflect::ObjectPtr &_object) override;
		//@}

		/// @name Resource Caching Support
		//@{
		virtual Name GetCacheName() const override;
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
