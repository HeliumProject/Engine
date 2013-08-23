#pragma once

#include "MathSimd/API.h"
#include "Foundation/Math.h"
#include "Reflect/MetaStruct.h"

namespace Helium
{
	namespace Simd
	{
		/// Single-precision floating-point 2-component vector.  Note that unlike Vector3 and Vector4, this is not aligned
		/// or padded for SIMD use.
		struct HELIUM_MATH_SIMD_API Vector2 : Reflect::Struct
		{
		public:
			HELIUM_DECLARE_BASE_STRUCT(Vector2);
			static void PopulateMetaType( Reflect::MetaStruct& comp );

			const static Vector2 Zero;
			const static Vector2 Unit;
			const static Vector2 BasisX;
			const static Vector2 BasisY;

			/// @name Construction/Destruction
			//@{
			inline Vector2();
			inline Vector2( float32_t x, float32_t y );
			inline explicit Vector2( float32_t s );
			//@}

			/// @name Data Access
			//@{
			inline float32_t& GetX();
			inline float32_t GetX() const;
			inline void SetX( float32_t x );

			inline float32_t& GetY();
			inline float32_t GetY() const;
			inline void SetY( float32_t y );

			inline float32_t& GetElement( size_t index );
			inline float32_t GetElement( size_t index ) const;
			inline void SetElement( size_t index, float32_t value );
			//@}

			/// @name Math
			//@{
			inline Vector2 Add( const Vector2& rVector ) const;
			inline Vector2 Subtract( const Vector2& rVector ) const;
			inline Vector2 Multiply( const Vector2& rVector ) const;
			inline Vector2 Divide( const Vector2& rVector ) const;

			inline void AddSet( const Vector2& rVector0, const Vector2& rVector1 );
			inline void SubtractSet( const Vector2& rVector0, const Vector2& rVector1 );
			inline void MultiplySet( const Vector2& rVector0, const Vector2& rVector1 );
			inline void DivideSet( const Vector2& rVector0, const Vector2& rVector1 );

			inline void MultiplyAddSet( const Vector2& rVectorMul0, const Vector2& rVectorMul1, const Vector2& rVectorAdd );

			inline Vector2 GetScaled( float32_t scale ) const;
			inline void Scale( float32_t scale );

			inline float32_t Dot( const Vector2& rVector ) const;

			inline float32_t Cross( const Vector2& rVector ) const;

			inline float32_t GetMagnitude() const;
			inline float32_t GetMagnitudeSquared() const;

			inline Vector2 GetNormalized( float32_t epsilon = HELIUM_EPSILON ) const;
			inline void Normalize( float32_t epsilon = HELIUM_EPSILON );

			inline Vector2 GetNormalizedOrZeroed( float32_t epsilon = HELIUM_EPSILON ) const;
			inline void NormalizeOrZero( float32_t epsilon = HELIUM_EPSILON );

			inline Vector2 GetNegated() const;
			inline void GetNegated( Vector2& rResult ) const;
			inline void Negate();
			//@}

			/// @name Comparison
			//@{
			inline bool Equals( const Vector2& rVector, float32_t epsilon = HELIUM_EPSILON ) const;
			//@}

			/// @name Overloaded Operators
			//@{
			inline Vector2 operator-() const;

			inline Vector2 operator+( const Vector2& rVector ) const;
			inline Vector2 operator-( const Vector2& rVector ) const;
			inline Vector2 operator*( const Vector2& rVector ) const;
			inline Vector2 operator/( const Vector2& rVector ) const;

			inline Vector2& operator+=( const Vector2& rVector );
			inline Vector2& operator-=( const Vector2& rVector );
			inline Vector2& operator*=( const Vector2& rVector );
			inline Vector2& operator/=( const Vector2& rVector );

			inline Vector2 operator*( float32_t scale ) const;
			inline Vector2& operator*=( float32_t scale );

			inline bool operator==( const Vector2& rVector ) const;
			inline bool operator!=( const Vector2& rVector ) const;
			//@}

			/// @name Friend Functions
			//@{
			inline friend Vector2 operator*( float32_t scale, const Vector2& rVector );
			//@}

		private:
			/// Vector values.
			union
			{
				float32_t m_vector[ 2 ];
				struct
				{
					float32_t m_x;
					float32_t m_y;
				};
			};
		};
	}
}

#include "MathSimd/Vector2.inl"
