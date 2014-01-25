#pragma once

#include "PrimitiveRadius.h"

namespace Helium
{
	namespace Editor
	{
		class PrimitiveCylinder : public PrimitiveRadius
		{
		public:
			typedef PrimitiveRadius Base;

			float m_Length;
			int m_LengthSteps;

			bool m_VerticalOrientation;

		public:
			PrimitiveCylinder();

			int GetWireVertCount() const;
			int GetPolyVertCount() const;

			virtual void Update() HELIUM_OVERRIDE;
			virtual void Draw(
				BufferedDrawer* drawInterface,
				Helium::Color materialColor = Colors::WHITE,
				const Simd::Matrix44& transform = Simd::Matrix44::IDENTITY,
				const bool* solid = NULL,
				const bool* transparent = NULL ) const HELIUM_OVERRIDE;
			virtual bool Pick( PickVisitor* pick, const bool* solid = NULL ) const HELIUM_OVERRIDE;
		};
	}
}