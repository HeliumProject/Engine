#pragma once

#include "MathSimd/API.h"
#include "Foundation/Math.h"
#include "Reflect/MetaStruct.h"

namespace Helium
{
	/// 32-bit ARGB color value.
	struct HELIUM_MATH_SIMD_API Color : Reflect::Struct
	{
	public:
		HELIUM_DECLARE_BASE_STRUCT(Color);
		static void PopulateMetaType( Reflect::MetaStruct& comp );

		/// @name Construction/Destruction
		//@{
		inline Color();
		inline Color( uint8_t r, uint8_t g, uint8_t b, uint8_t a );
		inline Color( float32_t r, float32_t g, float32_t b, float32_t a );
		inline explicit Color( uint32_t argb );
		//@}

		/// @name Data Access
		//@{
		inline uint8_t GetR() const;
		inline uint8_t GetG() const;
		inline uint8_t GetB() const;
		inline uint8_t GetA() const;
		inline uint32_t GetArgb() const;
		inline void SetR( uint8_t r );
		inline void SetG( uint8_t g );
		inline void SetB( uint8_t b );
		inline void SetA( uint8_t a );
		inline void SetArgb( uint32_t argb );

		inline float32_t GetFloatR() const;
		inline float32_t GetFloatG() const;
		inline float32_t GetFloatB() const;
		inline float32_t GetFloatA() const;
		inline void SetFloatR( float32_t r );
		inline void SetFloatG( float32_t g );
		inline void SetFloatB( float32_t b );
		inline void SetFloatA( float32_t a );
		//@}

		/// @name Overloaded Operators
		//@{
		inline bool operator==( const Color& rOther ) const;
		inline bool operator!=( const Color& rOther ) const;
		//@}

	private:

		/// Color data.
		union
		{
			/// Packed color value.
			uint32_t m_color;

			/// Individual color components.
			struct
			{
#if HELIUM_ENDIAN_LITTLE
				/// Blue component.
				uint8_t b;
				/// Green component.
				uint8_t g;
				/// Red component.
				uint8_t r;
				/// Alpha component.
				uint8_t a;
#else
				/// Alpha component.
				uint8_t a;
				/// Red component.
				uint8_t r;
				/// Green component.
				uint8_t g;
				/// Blue component.
				uint8_t b;
#endif
			} m_components;
		};
	};
}

#include "MathSimd/Color.inl"
