#pragma once

#include "Primitive.h"

namespace Helium
{
	namespace Editor
	{
		class PrimitiveCone : public PrimitiveTemplate< Helium::SimpleVertex >
		{
		private:
			typedef PrimitiveTemplate< Helium::SimpleVertex > Base;

			mutable int32_t m_WireVertCount;
			mutable int32_t m_PolyVertCount;

		public:
			float32_t m_Length;
			float32_t m_Radius;
			int32_t m_Steps;

		public:
			PrimitiveCone();

			uint32_t GetWireVertCount() const;
			uint32_t GetPolyVertCount() const;

			virtual void Update() override;
			virtual void Draw(
				BufferedDrawer* drawInterface,
				Helium::Color materialColor = Colors::WHITE,
				const Simd::Matrix44& transform = Simd::Matrix44::IDENTITY,
				const bool* solid = NULL,
				const bool* transparent = NULL ) const override;
			virtual bool Pick( PickVisitor* pick, const bool* solid = NULL ) const override;
		};
	}
}