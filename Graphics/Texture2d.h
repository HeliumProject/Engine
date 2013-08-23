#pragma once

#include "Graphics/Texture.h"

namespace Helium
{
	class Texture2d;
	typedef Helium::StrongPtr< Texture2d > Texture2dPtr;
	typedef Helium::StrongPtr< const Texture2d > ConstTexture2dPtr;

	/// 2D texture resource.
	class HELIUM_GRAPHICS_API Texture2d : public Texture
	{
		HELIUM_DECLARE_ASSET( Texture2d, Texture );

	public:
		/// @name Construction/Destruction
		//@{
		Texture2d();
		virtual ~Texture2d();
		//@}

		struct HELIUM_GRAPHICS_API PersistentResourceData : public Object
		{
			HELIUM_DECLARE_CLASS(Texture2d::PersistentResourceData, Reflect::Object);

			PersistentResourceData();
			static void PopulateMetaType( Reflect::MetaStruct& comp );

			uint32_t m_baseLevelWidth;
			uint32_t m_baseLevelHeight;
			uint32_t m_mipCount;
			int32_t m_pixelFormatIndex;
		};

		/// Persistent texture resource data.
		PersistentResourceData m_persistentResourceData;

		/// @name Serialization
		//@{
		virtual bool NeedsPrecacheResourceData() const;
		virtual bool BeginPrecacheResourceData();
		virtual bool TryFinishPrecacheResourceData();
		//@}

		inline uint32_t GetWidth() const;
		inline uint32_t GetHeight() const;

		/// @name Resource Serialization
		//@{
		virtual bool LoadPersistentResourceObject( Reflect::ObjectPtr& _object );
		//@}

		/// @name Data Access
		//@{
		RTexture2d* GetRenderResource2d() const;
		//@}

	private:
		/// Async load IDs for cached texture data.
		DynamicArray< size_t > m_renderResourceLoadIds;
	};
}

#include "Graphics/Texture2d.inl"
