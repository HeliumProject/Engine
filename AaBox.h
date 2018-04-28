#pragma once

#include "MathSimd/API.h"
#include "MathSimd/Vector3.h"
#include "Reflect/MetaStruct.h"

namespace Helium
{
	namespace Simd
	{
		struct Matrix44;

		/// Axis-aligned bounding box.
		HELIUM_ALIGN_PRE( 16 ) struct HELIUM_MATH_SIMD_API AaBox : Reflect::Struct
		{
		public:
			HELIUM_DECLARE_BASE_STRUCT(AaBox);
			static void PopulateMetaType( Reflect::MetaStruct& comp );

			inline bool operator== (const AaBox& rhs) const;
			inline bool operator!= (const AaBox& rhs) const;

			/// @name Construction/Destruction
			//@{
			AaBox();
			AaBox( const Vector3& rMinimum, const Vector3& rMaximum );
			//@}

			/// @name Data Access
			//@{
			void Set( const Vector3& rMinimum, const Vector3& rMaximum );
			inline const Vector3& GetMinimum() const;
			inline const Vector3& GetMaximum() const;

			void Expand( const Vector3& rPoint );

			void TransformBy( const Matrix44& rTransform );
			//@}

		private:
			/// Box minimum.
			Vector3 m_minimum;
			/// Box maximum.
			Vector3 m_maximum;
		} HELIUM_ALIGN_POST( 16 );
	}
}

#include "MathSimd/AaBox.inl"
