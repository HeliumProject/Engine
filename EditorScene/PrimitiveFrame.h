#pragma once

#include "Primitive.h"

namespace Helium
{
	namespace Editor
	{
		class PrimitiveFrame : public PrimitiveTemplate< Helium::ScreenVertex >
		{
		public:
			typedef PrimitiveTemplate< Helium::ScreenVertex > Base;

			Point m_Start;
			Point m_End;

			Helium::Color m_InnerColor;
			Helium::Color m_BorderColor;

		public:
			PrimitiveFrame();

			virtual void Update() HELIUM_OVERRIDE;
			virtual void Draw(
				Helium::BufferedDrawer* drawInterface, DrawArgs* args, Helium::Color materialColor = Color::WHITE,
				const Simd::Matrix44& transform = Simd::Matrix44::IDENTITY, const bool* solid = NULL,
				const bool* transparent = NULL ) const HELIUM_OVERRIDE;
			virtual bool Pick( PickVisitor* pick, const bool* solid = NULL ) const HELIUM_OVERRIDE;
		};
	}
}